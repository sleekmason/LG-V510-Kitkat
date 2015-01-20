/*
 * Copyright (C) 2009 ST-Ericsson SA
 * Copyright (C) 2009 STMicroelectronics
 *
 * I2C master mode controller driver, used in Nomadik 8815
 * and Ux500 platforms.
 *
 * Author: Srinidhi Kasagar <srinidhi.kasagar@stericsson.com>
 * Author: Sachin Verma <sachin.verma@st.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2, as
 * published by the Free Software Foundation.
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/regulator/consumer.h>
#include <linux/pm_runtime.h>

#include <plat/i2c.h>

#define DRIVER_NAME "nmk-i2c"

/*                                 */
#define I2C_CR		(0x000)
#define I2C_SCR		(0x004)
#define I2C_HSMCR	(0x008)
#define I2C_MCR		(0x00C)
#define I2C_TFR		(0x010)
#define I2C_SR		(0x014)
#define I2C_RFR		(0x018)
#define I2C_TFTR	(0x01C)
#define I2C_RFTR	(0x020)
#define I2C_DMAR	(0x024)
#define I2C_BRCR	(0x028)
#define I2C_IMSCR	(0x02C)
#define I2C_RISR	(0x030)
#define I2C_MISR	(0x034)
#define I2C_ICR		(0x038)

/*                   */
#define I2C_CR_PE		(0x1 << 0)	/*                   */
#define I2C_CR_OM		(0x3 << 1)	/*                */
#define I2C_CR_SAM		(0x1 << 3)	/*                       */
#define I2C_CR_SM		(0x3 << 4)	/*            */
#define I2C_CR_SGCM		(0x1 << 6)	/*                         */
#define I2C_CR_FTX		(0x1 << 7)	/*                */
#define I2C_CR_FRX		(0x1 << 8)	/*               */
#define I2C_CR_DMA_TX_EN	(0x1 << 9)	/*               */
#define I2C_CR_DMA_RX_EN	(0x1 << 10)	/*               */
#define I2C_CR_DMA_SLE		(0x1 << 11)	/*                        */
#define I2C_CR_LM		(0x1 << 12)	/*               */
#define I2C_CR_FON		(0x3 << 13)	/*              */
#define I2C_CR_FS		(0x3 << 15)	/*                   */

/*                                  */
#define I2C_MCR_OP		(0x1 << 0)	/*           */
#define I2C_MCR_A7		(0x7f << 1)	/*               */
#define I2C_MCR_EA10		(0x7 << 8)	/*                         */
#define I2C_MCR_SB		(0x1 << 11)	/*                  */
#define I2C_MCR_AM		(0x3 << 12)	/*              */
#define I2C_MCR_STOP		(0x1 << 14)	/*                */
#define I2C_MCR_LENGTH		(0x7ff << 15)	/*                    */

/*                      */
#define I2C_SR_OP		(0x3 << 0)	/*           */
#define I2C_SR_STATUS		(0x3 << 2)	/*                   */
#define I2C_SR_CAUSE		(0x7 << 4)	/*             */
#define I2C_SR_TYPE		(0x3 << 7)	/*              */
#define I2C_SR_LENGTH		(0x7ff << 9)	/*                 */

/*                                       */
#define I2C_IT_TXFE		(0x1 << 0)
#define I2C_IT_TXFNE		(0x1 << 1)
#define I2C_IT_TXFF		(0x1 << 2)
#define I2C_IT_TXFOVR		(0x1 << 3)
#define I2C_IT_RXFE		(0x1 << 4)
#define I2C_IT_RXFNF		(0x1 << 5)
#define I2C_IT_RXFF		(0x1 << 6)
#define I2C_IT_RFSR		(0x1 << 16)
#define I2C_IT_RFSE		(0x1 << 17)
#define I2C_IT_WTSR		(0x1 << 18)
#define I2C_IT_MTD		(0x1 << 19)
#define I2C_IT_STD		(0x1 << 20)
#define I2C_IT_MAL		(0x1 << 24)
#define I2C_IT_BERR		(0x1 << 25)
#define I2C_IT_MTDWS		(0x1 << 28)

#define GEN_MASK(val, mask, sb)  (((val) << (sb)) & (mask))

/*                               */
#define I2C_CLEAR_ALL_INTS	0x131f007f

/*                                   */
#define IRQ_MASK(mask)		(mask & 0x1fffffff)

/*                         */
#define MAX_I2C_FIFO_THRESHOLD	15

enum i2c_status {
	I2C_NOP,
	I2C_ON_GOING,
	I2C_OK,
	I2C_ABORT
};

/*           */
enum i2c_operation {
	I2C_NO_OPERATION = 0xff,
	I2C_WRITE = 0x00,
	I2C_READ = 0x01
};

/* 
                                               
                                  
                                      
                              
                                          
                                    
 */
struct i2c_nmk_client {
	unsigned short		slave_adr;
	unsigned long		count;
	unsigned char		*buffer;
	unsigned long		xfer_bytes;
	enum i2c_operation	operation;
};

/* 
                                                                 
                                 
                                    
                                           
                                     
                                  
                                                   
                                        
                         
                                                            
                                         
                                        
                              
 */
struct nmk_i2c_dev {
	struct platform_device		*pdev;
	struct i2c_adapter		adap;
	int				irq;
	void __iomem			*virtbase;
	struct clk			*clk;
	struct nmk_i2c_controller	cfg;
	struct i2c_nmk_client		cli;
	int				stop;
	struct completion		xfer_complete;
	int				result;
	struct regulator		*regulator;
	bool				busy;
};

/*                           */
static const char *abort_causes[] = {
	"no ack received after address transmission",
	"no ack received during data phase",
	"ack received after xmission of master code",
	"master lost arbitration",
	"slave restarts",
	"slave reset",
	"overflow, maxsize is 2047 bytes",
};

static inline void i2c_set_bit(void __iomem *reg, u32 mask)
{
	writel(readl(reg) | mask, reg);
}

static inline void i2c_clr_bit(void __iomem *reg, u32 mask)
{
	writel(readl(reg) & ~mask, reg);
}

/* 
                                                        
                                   
  
                                                            
                                   
 */
static int flush_i2c_fifo(struct nmk_i2c_dev *dev)
{
#define LOOP_ATTEMPTS 10
	int i;
	unsigned long timeout;

	/*
                                                     
                                                          
                                                          
                                                       
                                                         
  */
	writel((I2C_CR_FTX | I2C_CR_FRX), dev->virtbase + I2C_CR);

	for (i = 0; i < LOOP_ATTEMPTS; i++) {
		timeout = jiffies + dev->adap.timeout;

		while (!time_after(jiffies, timeout)) {
			if ((readl(dev->virtbase + I2C_CR) &
				(I2C_CR_FTX | I2C_CR_FRX)) == 0)
					return 0;
		}
	}

	dev_err(&dev->pdev->dev,
		"flushing operation timed out giving up after %d attempts",
		LOOP_ATTEMPTS);

	return -ETIMEDOUT;
}

/* 
                                                                    
                                   
 */
static void disable_all_interrupts(struct nmk_i2c_dev *dev)
{
	u32 mask = IRQ_MASK(0);
	writel(mask, dev->virtbase + I2C_IMSCR);
}

/* 
                                                                  
                                   
 */
static void clear_all_interrupts(struct nmk_i2c_dev *dev)
{
	u32 mask;
	mask = IRQ_MASK(I2C_CLEAR_ALL_INTS);
	writel(mask, dev->virtbase + I2C_ICR);
}

/* 
                                          
                                   
 */
static int init_hw(struct nmk_i2c_dev *dev)
{
	int stat;

	stat = flush_i2c_fifo(dev);
	if (stat)
		goto exit;

	/*                        */
	i2c_clr_bit(dev->virtbase + I2C_CR , I2C_CR_PE);

	disable_all_interrupts(dev);

	clear_all_interrupts(dev);

	dev->cli.operation = I2C_NO_OPERATION;

exit:
	return stat;
}

/*                                          */
#define DEFAULT_I2C_REG_CR	((1 << 1) | I2C_CR_PE)

/* 
                                             
                                   
 */
static u32 load_i2c_mcr_reg(struct nmk_i2c_dev *dev)
{
	u32 mcr = 0;

	/*                           */
	mcr |= GEN_MASK(1, I2C_MCR_AM, 12);
	mcr |= GEN_MASK(dev->cli.slave_adr, I2C_MCR_A7, 1);

	/*                                  */
	mcr |= GEN_MASK(0, I2C_MCR_SB, 11);

	/*                                         */
	if (dev->cli.operation == I2C_WRITE)
		mcr |= GEN_MASK(I2C_WRITE, I2C_MCR_OP, 0);
	else
		mcr |= GEN_MASK(I2C_READ, I2C_MCR_OP, 0);

	/*                         */
	if (dev->stop)
		mcr |= GEN_MASK(1, I2C_MCR_STOP, 14);
	else
		mcr &= ~(GEN_MASK(1, I2C_MCR_STOP, 14));

	mcr |= GEN_MASK(dev->cli.count, I2C_MCR_LENGTH, 15);

	return mcr;
}

/* 
                                                
                                   
 */
static void setup_i2c_controller(struct nmk_i2c_dev *dev)
{
	u32 brcr1, brcr2;
	u32 i2c_clk, div;

	writel(0x0, dev->virtbase + I2C_CR);
	writel(0x0, dev->virtbase + I2C_HSMCR);
	writel(0x0, dev->virtbase + I2C_TFTR);
	writel(0x0, dev->virtbase + I2C_RFTR);
	writel(0x0, dev->virtbase + I2C_DMAR);

	/*
                 
   
                                                    
                                              
                                                    
                                                       
                                     
  */
	writel(dev->cfg.slsu << 16, dev->virtbase + I2C_SCR);

	i2c_clk = clk_get_rate(dev->clk);

	/*                                                      */
	if (dev->cfg.clk_freq == 0)
		dev->cfg.clk_freq = 100000;

	/*
                                                      
                                                   
                                         
  */
	div = (dev->cfg.clk_freq > 100000) ? 3 : 2;

	/*
                                                            
                                                          
                                                             
                                                              
                      
  */
	brcr1 = 0 << 16;
	brcr2 = (i2c_clk/(dev->cfg.clk_freq * div)) & 0xffff;

	/*                                    */
	writel((brcr1 | brcr2), dev->virtbase + I2C_BRCR);

	/*
                                            
                                            
                                                   
                                   
  */
	if (dev->cfg.sm > I2C_FREQ_MODE_FAST) {
		dev_err(&dev->pdev->dev,
			"do not support this mode defaulting to std. mode\n");
		brcr2 = i2c_clk/(100000 * 2) & 0xffff;
		writel((brcr1 | brcr2), dev->virtbase + I2C_BRCR);
		writel(I2C_FREQ_MODE_STANDARD << 4,
				dev->virtbase + I2C_CR);
	}
	writel(dev->cfg.sm << 4, dev->virtbase + I2C_CR);

	/*                                  */
	writel(dev->cfg.tft, dev->virtbase + I2C_TFTR);
	writel(dev->cfg.rft, dev->virtbase + I2C_RFTR);
}

/* 
                                           
                                   
  
                                                                   
                                                                      
                                    
 */
static int read_i2c(struct nmk_i2c_dev *dev)
{
	u32 status = 0;
	u32 mcr;
	u32 irq_mask = 0;
	int timeout;

	mcr = load_i2c_mcr_reg(dev);
	writel(mcr, dev->virtbase + I2C_MCR);

	/*                           */
	writel(readl(dev->virtbase + I2C_CR) | DEFAULT_I2C_REG_CR,
			dev->virtbase + I2C_CR);

	/*                       */
	i2c_set_bit(dev->virtbase + I2C_CR, I2C_CR_PE);

	init_completion(&dev->xfer_complete);

	/*                                       */
	irq_mask = (I2C_IT_RXFNF | I2C_IT_RXFF |
			I2C_IT_MAL | I2C_IT_BERR);

	if (dev->stop)
		irq_mask |= I2C_IT_MTD;
	else
		irq_mask |= I2C_IT_MTDWS;

	irq_mask = I2C_CLEAR_ALL_INTS & IRQ_MASK(irq_mask);

	writel(readl(dev->virtbase + I2C_IMSCR) | irq_mask,
			dev->virtbase + I2C_IMSCR);

	timeout = wait_for_completion_timeout(
		&dev->xfer_complete, dev->adap.timeout);

	if (timeout < 0) {
		dev_err(&dev->pdev->dev,
			"wait_for_completion_timeout "
			"returned %d waiting for event\n", timeout);
		status = timeout;
	}

	if (timeout == 0) {
		/*                      */
		dev_err(&dev->pdev->dev, "read from slave 0x%x timed out\n",
				dev->cli.slave_adr);
		status = -ETIMEDOUT;
	}
	return status;
}

static void fill_tx_fifo(struct nmk_i2c_dev *dev, int no_bytes)
{
	int count;

	for (count = (no_bytes - 2);
			(count > 0) &&
			(dev->cli.count != 0);
			count--) {
		/*                      */
		writeb(*dev->cli.buffer,
			dev->virtbase + I2C_TFR);
		dev->cli.buffer++;
		dev->cli.count--;
		dev->cli.xfer_bytes++;
	}

}

/* 
                                          
                                   
  
                                          
 */
static int write_i2c(struct nmk_i2c_dev *dev)
{
	u32 status = 0;
	u32 mcr;
	u32 irq_mask = 0;
	int timeout;

	mcr = load_i2c_mcr_reg(dev);

	writel(mcr, dev->virtbase + I2C_MCR);

	/*                           */
	writel(readl(dev->virtbase + I2C_CR) | DEFAULT_I2C_REG_CR,
			dev->virtbase + I2C_CR);

	/*                       */
	i2c_set_bit(dev->virtbase + I2C_CR , I2C_CR_PE);

	init_completion(&dev->xfer_complete);

	/*                                         */
	irq_mask = (I2C_IT_TXFOVR | I2C_IT_MAL | I2C_IT_BERR);

	/*                                     */
	fill_tx_fifo(dev, MAX_I2C_FIFO_THRESHOLD);

	if (dev->cli.count != 0)
		irq_mask |= I2C_IT_TXFNE;

	/*
                                                                  
                                                            
                                     
  */
	if (dev->stop)
		irq_mask |= I2C_IT_MTD;
	else
		irq_mask |= I2C_IT_MTDWS;

	irq_mask = I2C_CLEAR_ALL_INTS & IRQ_MASK(irq_mask);

	writel(readl(dev->virtbase + I2C_IMSCR) | irq_mask,
			dev->virtbase + I2C_IMSCR);

	timeout = wait_for_completion_timeout(
		&dev->xfer_complete, dev->adap.timeout);

	if (timeout < 0) {
		dev_err(&dev->pdev->dev,
			"wait_for_completion_timeout "
			"returned %d waiting for event\n", timeout);
		status = timeout;
	}

	if (timeout == 0) {
		/*                      */
		dev_err(&dev->pdev->dev, "write to slave 0x%x timed out\n",
				dev->cli.slave_adr);
		status = -ETIMEDOUT;
	}

	return status;
}

/* 
                                                     
                                              
                        
 */
static int nmk_i2c_xfer_one(struct nmk_i2c_dev *dev, u16 flags)
{
	int status;

	if (flags & I2C_M_RD) {
		/*                */
		dev->cli.operation = I2C_READ;
		status = read_i2c(dev);
	} else {
		/*                 */
		dev->cli.operation = I2C_WRITE;
		status = write_i2c(dev);
	}

	if (status || (dev->result)) {
		u32 i2c_sr;
		u32 cause;

		i2c_sr = readl(dev->virtbase + I2C_SR);
		/*
                                                 
                          
   */
		if (((i2c_sr >> 2) & 0x3) == 0x3) {
			/*                     */
			cause =	(i2c_sr >> 4) & 0x7;
			dev_err(&dev->pdev->dev, "%s\n",
				cause >= ARRAY_SIZE(abort_causes) ?
				"unknown reason" :
				abort_causes[cause]);
		}

		(void) init_hw(dev);

		status = status ? status : dev->result;
	}

	return status;
}

/* 
                                                                  
                                               
                                        
                                               
  
                                                                   
                                                                       
                                                       
  
        
                                                                         
                                           
                                 
                                            
                                          
                                                              
                               
                                                     
             
                                
                        
                    
                       
  
                                
                             
                    
                         
                                
  
                                                                              
                                                             
                                                            
                                                      
                      
                       
                       
                        
                    
                          
                                
  
                                                                          
                                                 
                                          
 */
static int nmk_i2c_xfer(struct i2c_adapter *i2c_adap,
		struct i2c_msg msgs[], int num_msgs)
{
	int status;
	int i;
	struct nmk_i2c_dev *dev = i2c_get_adapdata(i2c_adap);
	int j;

	dev->busy = true;

	if (dev->regulator)
		regulator_enable(dev->regulator);
	pm_runtime_get_sync(&dev->pdev->dev);

	clk_enable(dev->clk);

	status = init_hw(dev);
	if (status)
		goto out;

	/*                                               */
	for (j = 0; j < 3; j++) {
		/*                          */
		setup_i2c_controller(dev);

		for (i = 0; i < num_msgs; i++) {
			if (unlikely(msgs[i].flags & I2C_M_TEN)) {
				dev_err(&dev->pdev->dev,
					"10 bit addressing not supported\n");

				status = -EINVAL;
				goto out;
			}
			dev->cli.slave_adr	= msgs[i].addr;
			dev->cli.buffer		= msgs[i].buf;
			dev->cli.count		= msgs[i].len;
			dev->stop = (i < (num_msgs - 1)) ? 0 : 1;
			dev->result = 0;

			status = nmk_i2c_xfer_one(dev, msgs[i].flags);
			if (status != 0)
				break;
		}
		if (status == 0)
			break;
	}

out:
	clk_disable(dev->clk);
	pm_runtime_put_sync(&dev->pdev->dev);
	if (dev->regulator)
		regulator_disable(dev->regulator);

	dev->busy = false;

	/*                                   */
	if (status)
		return status;
	else
		return num_msgs;
}

/* 
                                                
                                   
                         
 */
static int disable_interrupts(struct nmk_i2c_dev *dev, u32 irq)
{
	irq = IRQ_MASK(irq);
	writel(readl(dev->virtbase + I2C_IMSCR) & ~(I2C_CLEAR_ALL_INTS & irq),
			dev->virtbase + I2C_IMSCR);
	return 0;
}

/* 
                                        
                         
                                   
  
                                                              
                                                               
                                                             
                                                                  
             
 */
static irqreturn_t i2c_irq_handler(int irq, void *arg)
{
	struct nmk_i2c_dev *dev = arg;
	u32 tft, rft;
	u32 count;
	u32 misr;
	u32 src = 0;

	/*                                           */
	tft = readl(dev->virtbase + I2C_TFTR);
	rft = readl(dev->virtbase + I2C_RFTR);

	/*                                */
	misr = readl(dev->virtbase + I2C_MISR);

	src = __ffs(misr);
	switch ((1 << src)) {

	/*                                      */
	case I2C_IT_TXFNE:
	{
		if (dev->cli.operation == I2C_READ) {
			/*
                                                   
                                            
    */
			disable_interrupts(dev, I2C_IT_TXFNE);
		} else {
			fill_tx_fifo(dev, (MAX_I2C_FIFO_THRESHOLD - tft));
			/*
                                                  
                                   
    */
			if (dev->cli.count == 0)
				disable_interrupts(dev,	I2C_IT_TXFNE);
		}
	}
	break;

	/*
                                  
                                                       
                                                        
          
  */
	case I2C_IT_RXFNF:
		for (count = rft; count > 0; count--) {
			/*                  */
			*dev->cli.buffer = readb(dev->virtbase + I2C_RFR);
			dev->cli.buffer++;
		}
		dev->cli.count -= rft;
		dev->cli.xfer_bytes += rft;
		break;

	/*              */
	case I2C_IT_RXFF:
		for (count = MAX_I2C_FIFO_THRESHOLD; count > 0; count--) {
			*dev->cli.buffer = readb(dev->virtbase + I2C_RFR);
			dev->cli.buffer++;
		}
		dev->cli.count -= MAX_I2C_FIFO_THRESHOLD;
		dev->cli.xfer_bytes += MAX_I2C_FIFO_THRESHOLD;
		break;

	/*                                           */
	case I2C_IT_MTD:
	case I2C_IT_MTDWS:
		if (dev->cli.operation == I2C_READ) {
			while (!(readl(dev->virtbase + I2C_RISR)
				 & I2C_IT_RXFE)) {
				if (dev->cli.count == 0)
					break;
				*dev->cli.buffer =
					readb(dev->virtbase + I2C_RFR);
				dev->cli.buffer++;
				dev->cli.count--;
				dev->cli.xfer_bytes++;
			}
		}

		disable_all_interrupts(dev);
		clear_all_interrupts(dev);

		if (dev->cli.count) {
			dev->result = -EIO;
			dev_err(&dev->pdev->dev,
				"%lu bytes still remain to be xfered\n",
				dev->cli.count);
			(void) init_hw(dev);
		}
		complete(&dev->xfer_complete);

		break;

	/*                                   */
	case I2C_IT_MAL:
		dev->result = -EIO;
		(void) init_hw(dev);

		i2c_set_bit(dev->virtbase + I2C_ICR, I2C_IT_MAL);
		complete(&dev->xfer_complete);

		break;

	/*
                        
                                                               
                           
  */
	case I2C_IT_BERR:
		dev->result = -EIO;
		/*                */
		if (((readl(dev->virtbase + I2C_SR) >> 2) & 0x3) == I2C_ABORT)
			(void) init_hw(dev);

		i2c_set_bit(dev->virtbase + I2C_ICR, I2C_IT_BERR);
		complete(&dev->xfer_complete);

		break;

	/*
                              
                                                                  
                        
  */
	case I2C_IT_TXFOVR:
		dev->result = -EIO;
		(void) init_hw(dev);

		dev_err(&dev->pdev->dev, "Tx Fifo Over run\n");
		complete(&dev->xfer_complete);

		break;

	/*                                           */
	case I2C_IT_TXFE:
	case I2C_IT_TXFF:
	case I2C_IT_RXFE:
	case I2C_IT_RFSR:
	case I2C_IT_RFSE:
	case I2C_IT_WTSR:
	case I2C_IT_STD:
		dev_err(&dev->pdev->dev, "unhandled Interrupt\n");
		break;
	default:
		dev_err(&dev->pdev->dev, "spurious Interrupt..\n");
		break;
	}

	return IRQ_HANDLED;
}


#ifdef CONFIG_PM
static int nmk_i2c_suspend(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct nmk_i2c_dev *nmk_i2c = platform_get_drvdata(pdev);

	if (nmk_i2c->busy)
		return -EBUSY;

	return 0;
}

static int nmk_i2c_resume(struct device *dev)
{
	return 0;
}
#else
#define nmk_i2c_suspend	NULL
#define nmk_i2c_resume	NULL
#endif

/*
                                                                              
                                                                          
                                                                          
 */
static const struct dev_pm_ops nmk_i2c_pm = {
	.suspend_noirq	= nmk_i2c_suspend,
	.resume_noirq	= nmk_i2c_resume,
};

static unsigned int nmk_i2c_functionality(struct i2c_adapter *adap)
{
	return I2C_FUNC_I2C | I2C_FUNC_SMBUS_EMUL;
}

static const struct i2c_algorithm nmk_i2c_algo = {
	.master_xfer	= nmk_i2c_xfer,
	.functionality	= nmk_i2c_functionality
};

static int __devinit nmk_i2c_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct resource *res;
	struct nmk_i2c_controller *pdata =
			pdev->dev.platform_data;
	struct nmk_i2c_dev	*dev;
	struct i2c_adapter *adap;

	dev = kzalloc(sizeof(struct nmk_i2c_dev), GFP_KERNEL);
	if (!dev) {
		dev_err(&pdev->dev, "cannot allocate memory\n");
		ret = -ENOMEM;
		goto err_no_mem;
	}
	dev->busy = false;
	dev->pdev = pdev;
	platform_set_drvdata(pdev, dev);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		ret = -ENOENT;
		goto err_no_resource;
	}

	if (request_mem_region(res->start, resource_size(res),
		DRIVER_NAME "I/O region") == NULL) {
		ret = -EBUSY;
		goto err_no_region;
	}

	dev->virtbase = ioremap(res->start, resource_size(res));
	if (!dev->virtbase) {
		ret = -ENOMEM;
		goto err_no_ioremap;
	}

	dev->irq = platform_get_irq(pdev, 0);
	ret = request_irq(dev->irq, i2c_irq_handler, 0,
				DRIVER_NAME, dev);
	if (ret) {
		dev_err(&pdev->dev, "cannot claim the irq %d\n", dev->irq);
		goto err_irq;
	}

	dev->regulator = regulator_get(&pdev->dev, "v-i2c");
	if (IS_ERR(dev->regulator)) {
		dev_warn(&pdev->dev, "could not get i2c regulator\n");
		dev->regulator = NULL;
	}

	pm_suspend_ignore_children(&pdev->dev, true);
	pm_runtime_enable(&pdev->dev);

	dev->clk = clk_get(&pdev->dev, NULL);
	if (IS_ERR(dev->clk)) {
		dev_err(&pdev->dev, "could not get i2c clock\n");
		ret = PTR_ERR(dev->clk);
		goto err_no_clk;
	}

	adap = &dev->adap;
	adap->dev.parent = &pdev->dev;
	adap->owner	= THIS_MODULE;
	adap->class	= I2C_CLASS_HWMON | I2C_CLASS_SPD;
	adap->algo	= &nmk_i2c_algo;
	adap->timeout	= pdata->timeout ? msecs_to_jiffies(pdata->timeout) :
		msecs_to_jiffies(20000);
	snprintf(adap->name, sizeof(adap->name),
		 "Nomadik I2C%d at %lx", pdev->id, (unsigned long)res->start);

	/*                         */
	adap->nr	= pdev->id;

	/*                                                 */
	dev->cfg.clk_freq = pdata->clk_freq;
	dev->cfg.slsu	= pdata->slsu;
	dev->cfg.tft	= pdata->tft;
	dev->cfg.rft	= pdata->rft;
	dev->cfg.sm	= pdata->sm;

	i2c_set_adapdata(adap, dev);

	dev_info(&pdev->dev,
		 "initialize %s on virtual base %p\n",
		 adap->name, dev->virtbase);

	ret = i2c_add_numbered_adapter(adap);
	if (ret) {
		dev_err(&pdev->dev, "failed to add adapter\n");
		goto err_add_adap;
	}

	return 0;

 err_add_adap:
	clk_put(dev->clk);
 err_no_clk:
	if (dev->regulator)
		regulator_put(dev->regulator);
	pm_runtime_disable(&pdev->dev);
	free_irq(dev->irq, dev);
 err_irq:
	iounmap(dev->virtbase);
 err_no_ioremap:
	release_mem_region(res->start, resource_size(res));
 err_no_region:
	platform_set_drvdata(pdev, NULL);
 err_no_resource:
	kfree(dev);
 err_no_mem:

	return ret;
}

static int __devexit nmk_i2c_remove(struct platform_device *pdev)
{
	struct resource *res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	struct nmk_i2c_dev *dev = platform_get_drvdata(pdev);

	i2c_del_adapter(&dev->adap);
	flush_i2c_fifo(dev);
	disable_all_interrupts(dev);
	clear_all_interrupts(dev);
	/*                        */
	i2c_clr_bit(dev->virtbase + I2C_CR, I2C_CR_PE);
	free_irq(dev->irq, dev);
	iounmap(dev->virtbase);
	if (res)
		release_mem_region(res->start, resource_size(res));
	clk_put(dev->clk);
	if (dev->regulator)
		regulator_put(dev->regulator);
	pm_runtime_disable(&pdev->dev);
	platform_set_drvdata(pdev, NULL);
	kfree(dev);

	return 0;
}

static struct platform_driver nmk_i2c_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name = DRIVER_NAME,
		.pm = &nmk_i2c_pm,
	},
	.probe = nmk_i2c_probe,
	.remove = __devexit_p(nmk_i2c_remove),
};

static int __init nmk_i2c_init(void)
{
	return platform_driver_register(&nmk_i2c_driver);
}

static void __exit nmk_i2c_exit(void)
{
	platform_driver_unregister(&nmk_i2c_driver);
}

subsys_initcall(nmk_i2c_init);
module_exit(nmk_i2c_exit);

MODULE_AUTHOR("Sachin Verma, Srinidhi KASAGAR");
MODULE_DESCRIPTION("Nomadik/Ux500 I2C driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:" DRIVER_NAME);