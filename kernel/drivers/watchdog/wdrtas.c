/*
                                                                     
                           
 */

/*
 * RTAS watchdog driver
 *
 * (C) Copyright IBM Corp. 2005
 * device driver to exploit watchdog RTAS functions
 *
 * Authors : Utz Bacher <utz.bacher@de.ibm.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/notifier.h>
#include <linux/reboot.h>
#include <linux/types.h>
#include <linux/watchdog.h>
#include <linux/uaccess.h>

#include <asm/rtas.h>

#define WDRTAS_MAGIC_CHAR		42
#define WDRTAS_SUPPORTED_MASK		(WDIOF_SETTIMEOUT | \
					 WDIOF_MAGICCLOSE)

MODULE_AUTHOR("Utz Bacher <utz.bacher@de.ibm.com>");
MODULE_DESCRIPTION("RTAS watchdog driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS_MISCDEV(WATCHDOG_MINOR);
MODULE_ALIAS_MISCDEV(TEMP_MINOR);

static bool wdrtas_nowayout = WATCHDOG_NOWAYOUT;
static atomic_t wdrtas_miscdev_open = ATOMIC_INIT(0);
static char wdrtas_expect_close;

static int wdrtas_interval;

#define WDRTAS_THERMAL_SENSOR		3
static int wdrtas_token_get_sensor_state;
#define WDRTAS_SURVEILLANCE_IND		9000
static int wdrtas_token_set_indicator;
#define WDRTAS_SP_SPI			28
static int wdrtas_token_get_sp;
static int wdrtas_token_event_scan;

#define WDRTAS_DEFAULT_INTERVAL		300

#define WDRTAS_LOGBUFFER_LEN		128
static char wdrtas_logbuffer[WDRTAS_LOGBUFFER_LEN];


/*                             */

/* 
                                                   
                          
  
                                       
  
                                                                          
                                                                      
           
 */

static int wdrtas_set_interval(int interval)
{
	long result;
	static int print_msg = 10;

	/*                   */
	interval = (interval + 59) / 60;

	result = rtas_call(wdrtas_token_set_indicator, 3, 1, NULL,
			   WDRTAS_SURVEILLANCE_IND, 0, interval);
	if (result < 0 && print_msg) {
		pr_err("setting the watchdog to %i timeout failed: %li\n",
		       interval, result);
		print_msg--;
	}

	return result;
}

#define WDRTAS_SP_SPI_LEN 4

/* 
                                                              
                                                                     
  
                       
  
                                                                      
                                                                      
                                  
 */
static int wdrtas_get_interval(int fallback_value)
{
	long result;
	char value[WDRTAS_SP_SPI_LEN];

	spin_lock(&rtas_data_buf_lock);
	memset(rtas_data_buf, 0, WDRTAS_SP_SPI_LEN);
	result = rtas_call(wdrtas_token_get_sp, 3, 1, NULL,
			   WDRTAS_SP_SPI, __pa(rtas_data_buf),
			   WDRTAS_SP_SPI_LEN);

	memcpy(value, rtas_data_buf, WDRTAS_SP_SPI_LEN);
	spin_unlock(&rtas_data_buf_lock);

	if (value[0] != 0 || value[1] != 2 || value[3] != 0 || result < 0) {
		pr_warn("could not get sp_spi watchdog timeout (%li). Continuing\n",
			result);
		return fallback_value;
	}

	/*                   */
	return ((int)value[2]) * 60;
}

/* 
                                       
  
                                                                      
                              
 */
static void wdrtas_timer_start(void)
{
	wdrtas_set_interval(wdrtas_interval);
}

/* 
                                     
  
                                                                          
                              
 */
static void wdrtas_timer_stop(void)
{
	wdrtas_set_interval(0);
}

/* 
                                                                        
  
                                                                      
                                          
 */
static void wdrtas_log_scanned_event(void)
{
	int i;

	for (i = 0; i < WDRTAS_LOGBUFFER_LEN; i += 16)
		pr_info("dumping event (line %i/%i), data = "
			"%02x %02x %02x %02x  %02x %02x %02x %02x   "
			"%02x %02x %02x %02x  %02x %02x %02x %02x\n",
			(i / 16) + 1, (WDRTAS_LOGBUFFER_LEN / 16),
			wdrtas_logbuffer[i + 0], wdrtas_logbuffer[i + 1],
			wdrtas_logbuffer[i + 2], wdrtas_logbuffer[i + 3],
			wdrtas_logbuffer[i + 4], wdrtas_logbuffer[i + 5],
			wdrtas_logbuffer[i + 6], wdrtas_logbuffer[i + 7],
			wdrtas_logbuffer[i + 8], wdrtas_logbuffer[i + 9],
			wdrtas_logbuffer[i + 10], wdrtas_logbuffer[i + 11],
			wdrtas_logbuffer[i + 12], wdrtas_logbuffer[i + 13],
			wdrtas_logbuffer[i + 14], wdrtas_logbuffer[i + 15]);
}

/* 
                                                                      
  
                                                                    
                                                                        
                                               
 */
static void wdrtas_timer_keepalive(void)
{
	long result;

	do {
		result = rtas_call(wdrtas_token_event_scan, 4, 1, NULL,
				   RTAS_EVENT_SCAN_ALL_EVENTS, 0,
				   (void *)__pa(wdrtas_logbuffer),
				   WDRTAS_LOGBUFFER_LEN);
		if (result < 0)
			pr_err("event-scan failed: %li\n", result);
		if (result == 0)
			wdrtas_log_scanned_event();
	} while (result == 0);
}

/* 
                                                       
  
                                        
  
                                                                           
                                                        
 */
static int wdrtas_get_temperature(void)
{
	int result;
	int temperature = 0;

	result = rtas_get_sensor(WDRTAS_THERMAL_SENSOR, 0, &temperature);

	if (result < 0)
		pr_warn("reading the thermal sensor failed: %i\n", result);
	else
		temperature = ((temperature * 9) / 5) + 32; /*            */

	return temperature;
}

/* 
                                                         
  
                                                       
                           
 */
static int wdrtas_get_status(void)
{
	return 0; /*      */
}

/* 
                                                                
  
                                                       
                                                                            
 */
static int wdrtas_get_boot_status(void)
{
	return 0; /*      */
}

/*                                     */

/*                                                         
                        
                              
                               
                          
  
                                                                           
                                              
  
                                                                          
                                                                        
            
 */
static ssize_t wdrtas_write(struct file *file, const char __user *buf,
	     size_t len, loff_t *ppos)
{
	int i;
	char c;

	if (!len)
		goto out;

	if (!wdrtas_nowayout) {
		wdrtas_expect_close = 0;
		/*              */
		for (i = 0; i < len; i++) {
			if (get_user(c, buf + i))
				return -EFAULT;
			/*                       */
			if (c == 'V')
				wdrtas_expect_close = WDRTAS_MAGIC_CHAR;
		}
	}

	wdrtas_timer_keepalive();

out:
	return len;
}

/* 
                                                        
                        
                          
                         
  
                                      
  
                                                  
 */

static long wdrtas_ioctl(struct file *file, unsigned int cmd,
							unsigned long arg)
{
	int __user *argp = (void __user *)arg;
	int i;
	static const struct watchdog_info wdinfo = {
		.options = WDRTAS_SUPPORTED_MASK,
		.firmware_version = 0,
		.identity = "wdrtas",
	};

	switch (cmd) {
	case WDIOC_GETSUPPORT:
		if (copy_to_user(argp, &wdinfo, sizeof(wdinfo)))
			return -EFAULT;
		return 0;

	case WDIOC_GETSTATUS:
		i = wdrtas_get_status();
		return put_user(i, argp);

	case WDIOC_GETBOOTSTATUS:
		i = wdrtas_get_boot_status();
		return put_user(i, argp);

	case WDIOC_GETTEMP:
		if (wdrtas_token_get_sensor_state == RTAS_UNKNOWN_SERVICE)
			return -EOPNOTSUPP;

		i = wdrtas_get_temperature();
		return put_user(i, argp);

	case WDIOC_SETOPTIONS:
		if (get_user(i, argp))
			return -EFAULT;
		if (i & WDIOS_DISABLECARD)
			wdrtas_timer_stop();
		if (i & WDIOS_ENABLECARD) {
			wdrtas_timer_keepalive();
			wdrtas_timer_start();
		}
		/*                            
                            
    */
		return 0;

	case WDIOC_KEEPALIVE:
		wdrtas_timer_keepalive();
		return 0;

	case WDIOC_SETTIMEOUT:
		if (get_user(i, argp))
			return -EFAULT;

		if (wdrtas_set_interval(i))
			return -EINVAL;

		wdrtas_timer_keepalive();

		if (wdrtas_token_get_sp == RTAS_UNKNOWN_SERVICE)
			wdrtas_interval = i;
		else
			wdrtas_interval = wdrtas_get_interval(i);
		/*             */

	case WDIOC_GETTIMEOUT:
		return put_user(wdrtas_interval, argp);

	default:
		return -ENOTTY;
	}
}

/* 
                                                 
                          
                        
  
                                                                          
                 
  
                                                 
 */
static int wdrtas_open(struct inode *inode, struct file *file)
{
	/*                */
	if (atomic_inc_return(&wdrtas_miscdev_open) > 1) {
		atomic_dec(&wdrtas_miscdev_open);
		return -EBUSY;
	}

	wdrtas_timer_start();
	wdrtas_timer_keepalive();

	return nonseekable_open(inode, file);
}

/* 
                                                   
                          
                        
  
                       
  
                                  
 */
static int wdrtas_close(struct inode *inode, struct file *file)
{
	/*                                                            */
	if (wdrtas_expect_close == WDRTAS_MAGIC_CHAR)
		wdrtas_timer_stop();
	else {
		pr_warn("got unexpected close. Watchdog not stopped.\n");
		wdrtas_timer_keepalive();
	}

	wdrtas_expect_close = 0;
	atomic_dec(&wdrtas_miscdev_open);
	return 0;
}

/* 
                                                              
                        
                    
                                     
                          
  
                                                                         
                 
  
                                                                      
                                                                          
 */
static ssize_t wdrtas_temp_read(struct file *file, char __user *buf,
		 size_t count, loff_t *ppos)
{
	int temperature = 0;

	temperature = wdrtas_get_temperature();
	if (temperature < 0)
		return temperature;

	if (copy_to_user(buf, &temperature, 1))
		return -EFAULT;

	return 1;
}

/* 
                                                         
                          
                        
  
                                      
  
                                                    
 */
static int wdrtas_temp_open(struct inode *inode, struct file *file)
{
	return nonseekable_open(inode, file);
}

/* 
                                                           
                          
                        
  
                       
  
                                  
 */
static int wdrtas_temp_close(struct inode *inode, struct file *file)
{
	return 0;
}

/* 
                                           
                                
                     
               
  
                      
  
                                                       
 */
static int wdrtas_reboot(struct notifier_block *this,
					unsigned long code, void *ptr)
{
	if (code == SYS_DOWN || code == SYS_HALT)
		wdrtas_timer_stop();

	return NOTIFY_DONE;
}

/*                        */

static const struct file_operations wdrtas_fops = {
	.owner		= THIS_MODULE,
	.llseek		= no_llseek,
	.write		= wdrtas_write,
	.unlocked_ioctl	= wdrtas_ioctl,
	.open		= wdrtas_open,
	.release	= wdrtas_close,
};

static struct miscdevice wdrtas_miscdev = {
	.minor =	WATCHDOG_MINOR,
	.name =		"watchdog",
	.fops =		&wdrtas_fops,
};

static const struct file_operations wdrtas_temp_fops = {
	.owner		= THIS_MODULE,
	.llseek		= no_llseek,
	.read		= wdrtas_temp_read,
	.open		= wdrtas_temp_open,
	.release	= wdrtas_temp_close,
};

static struct miscdevice wdrtas_tempdev = {
	.minor =	TEMP_MINOR,
	.name =		"temperature",
	.fops =		&wdrtas_temp_fops,
};

static struct notifier_block wdrtas_notifier = {
	.notifier_call =	wdrtas_reboot,
};

/* 
                                           
  
                                      
  
                                                                   
                                                                
                                                
 */
static int wdrtas_get_tokens(void)
{
	wdrtas_token_get_sensor_state = rtas_token("get-sensor-state");
	if (wdrtas_token_get_sensor_state == RTAS_UNKNOWN_SERVICE) {
		pr_warn("couldn't get token for get-sensor-state. Trying to continue without temperature support.\n");
	}

	wdrtas_token_get_sp = rtas_token("ibm,get-system-parameter");
	if (wdrtas_token_get_sp == RTAS_UNKNOWN_SERVICE) {
		pr_warn("couldn't get token for ibm,get-system-parameter. Trying to continue with a default timeout value of %i seconds.\n",
			WDRTAS_DEFAULT_INTERVAL);
	}

	wdrtas_token_set_indicator = rtas_token("set-indicator");
	if (wdrtas_token_set_indicator == RTAS_UNKNOWN_SERVICE) {
		pr_err("couldn't get token for set-indicator. Terminating watchdog code.\n");
		return -EIO;
	}

	wdrtas_token_event_scan = rtas_token("event-scan");
	if (wdrtas_token_event_scan == RTAS_UNKNOWN_SERVICE) {
		pr_err("couldn't get token for event-scan. Terminating watchdog code.\n");
		return -EIO;
	}

	return 0;
}

/* 
                                                             
  
                                                                         
            
 */
static void wdrtas_unregister_devs(void)
{
	misc_deregister(&wdrtas_miscdev);
	if (wdrtas_token_get_sensor_state != RTAS_UNKNOWN_SERVICE)
		misc_deregister(&wdrtas_tempdev);
}

/* 
                                                         
  
                                      
  
                                                                       
            
 */
static int wdrtas_register_devs(void)
{
	int result;

	result = misc_register(&wdrtas_miscdev);
	if (result) {
		pr_err("couldn't register watchdog misc device. Terminating watchdog code.\n");
		return result;
	}

	if (wdrtas_token_get_sensor_state != RTAS_UNKNOWN_SERVICE) {
		result = misc_register(&wdrtas_tempdev);
		if (result) {
			pr_warn("couldn't register watchdog temperature misc device. Continuing without temperature support.\n");
			wdrtas_token_get_sensor_state = RTAS_UNKNOWN_SERVICE;
		}
	}

	return 0;
}

/* 
                                                     
  
                                      
  
                                                      
 */
static int __init wdrtas_init(void)
{
	if (wdrtas_get_tokens())
		return -ENODEV;

	if (wdrtas_register_devs())
		return -ENODEV;

	if (register_reboot_notifier(&wdrtas_notifier)) {
		pr_err("could not register reboot notifier. Terminating watchdog code.\n");
		wdrtas_unregister_devs();
		return -ENODEV;
	}

	if (wdrtas_token_get_sp == RTAS_UNKNOWN_SERVICE)
		wdrtas_interval = WDRTAS_DEFAULT_INTERVAL;
	else
		wdrtas_interval = wdrtas_get_interval(WDRTAS_DEFAULT_INTERVAL);

	return 0;
}

/* 
                                                     
  
                                                        
 */
static void __exit wdrtas_exit(void)
{
	if (!wdrtas_nowayout)
		wdrtas_timer_stop();

	wdrtas_unregister_devs();

	unregister_reboot_notifier(&wdrtas_notifier);
}

module_init(wdrtas_init);
module_exit(wdrtas_exit);
