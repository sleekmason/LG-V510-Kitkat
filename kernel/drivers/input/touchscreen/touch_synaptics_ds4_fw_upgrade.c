/*
   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   Copyright (c) 2011 Synaptics, Inc.

   Permission is hereby granted, free of charge, to any person obtaining a copy of
   this software and associated documentation files (the "Software"), to deal in
   the Software without restriction, including without limitation the rights to use,
   copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
   Software, and to permit persons to whom the Software is furnished to do so,
   subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.


   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/hrtimer.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <mach/gpio.h>
#include <linux/slab.h>
#include <linux/jiffies.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/earlysuspend.h>
#include <linux/input/lge_touch_core.h>
#include <linux/input/touch_synaptics.h>


/*                                 */
unsigned short SynaF34DataBase;
unsigned short SynaF34QueryBase;
unsigned short SynaF01DataBase;
unsigned short SynaF01CommandBase;
unsigned short SynaF01ControlBase;
unsigned short SynaF01QueryBase;

unsigned short SynaF34Reflash_BlockNum;
unsigned short SynaF34Reflash_BlockData;
unsigned short SynaF34ReflashQuery_BootID;
unsigned short SynaF34ReflashQuery_FlashPropertyQuery;
unsigned short SynaF34ReflashQuery_FirmwareBlockSize;
unsigned short SynaF34ReflashQuery_FirmwareBlockCount;
unsigned short SynaF34ReflashQuery_ConfigBlockSize;
unsigned short SynaF34ReflashQuery_ConfigBlockCount;

unsigned short SynaFirmwareBlockSize;
unsigned short SynaFirmwareBlockCount;
unsigned long SynaImageSize;

unsigned short SynaConfigBlockSize;
unsigned short SynaConfigBlockCount;
unsigned long SynaConfigImageSize;

unsigned short SynaBootloadID;

unsigned short SynaF34_FlashControl;

unsigned char *SynafirmwareImgData;
unsigned char *SynaconfigImgData;
unsigned char *SynalockImgData;
unsigned int SynafirmwareImgVersion;

//                            
//                                                                
//                                                              

unsigned char *my_image_bin;
unsigned long my_image_size;
u8 fw_image_config_id[5];

void CompleteReflash(struct synaptics_ts_data *ts);
void ConfigBlockReflash(struct synaptics_ts_data *ts);
void CompleteReflash_Lockdown(struct synaptics_ts_data *ts);
void SynaInitialize(struct synaptics_ts_data *ts);
void SynaReadConfigInfo(struct synaptics_ts_data *ts);
void SynaReadFirmwareInfo(struct synaptics_ts_data *ts);
void SynaEnableFlashing(struct synaptics_ts_data *ts);
void SynaProgramFirmware(struct synaptics_ts_data *ts);
void SynaProgramConfiguration(struct synaptics_ts_data *ts);
void SynaFinalizeReflash(struct synaptics_ts_data *ts);
void SynaWaitForATTN(int time);
//                    

//                                    
//     
/*                                                  
                     

          
 */
int FirmwareUpgrade(struct synaptics_ts_data *ts, const char* fw_path){

	int ret = 0;
	int fd = -1;
	mm_segment_t old_fs = 0;
	struct stat fw_bin_stat;
	unsigned long read_bytes;

	if (unlikely(fw_path[0] != 0)) {
		old_fs = get_fs();
		set_fs(get_ds());

		if ((fd = sys_open((const char __user *) fw_path, O_RDONLY, 0)) < 0) {
			TOUCH_ERR_MSG("Can not read FW binary from %s\n", fw_path);
			ret = -EEXIST;
			goto read_fail;
		}

		if ((ret = sys_newstat((char __user *) fw_path,
					(struct stat *)&fw_bin_stat)) < 0) {
			TOUCH_ERR_MSG("Can not read FW binary stat from %s\n", fw_path);
			goto fw_mem_alloc_fail;
		}

		my_image_size = fw_bin_stat.st_size;
		my_image_bin =
			kzalloc(sizeof(char) * (my_image_size+1), GFP_KERNEL);
		if (my_image_bin == NULL) {
			TOUCH_ERR_MSG("Can not allocate  memory\n");
			ret = -ENOMEM;
			goto fw_mem_alloc_fail;
		}

		//                              
		read_bytes = sys_read(fd,
				(char __user *)my_image_bin, my_image_size);

		/*              */
		*(my_image_bin+my_image_size) = 0xFF;

		TOUCH_INFO_MSG("Touch FW image read %ld bytes from %s\n",
							read_bytes, fw_path);

	} else {
		my_image_size = ts->fw_info->fw_size-1;
		my_image_bin = (unsigned char *)(&ts->fw_info->fw_start[0]);
	}

	strncpy(fw_image_config_id, &my_image_bin[0xb100], 4);

	TOUCH_INFO_MSG("fw_image_confid_id = %s\n", fw_image_config_id);

	//                    
	CompleteReflash_Lockdown(ts);

	if (unlikely(fw_path[0] != 0))
		kfree(my_image_bin);

fw_mem_alloc_fail:
	sys_close(fd);
read_fail:
	set_fs(old_fs);

	return ret;
	//         
}
//                                   

static int writeRMI(struct i2c_client *client, u8 uRmiAddress, u8 *data, unsigned int length)
{
	//                                                             
	return touch_i2c_write(client, uRmiAddress, length, data);
}

static int readRMI(struct i2c_client *client, u8 uRmiAddress, u8 *data, unsigned int length)
{
	//                                                            
	return touch_i2c_read(client, uRmiAddress, length, data);
}

/*                       
               
            
 */
//                                                 

/*                                      */

/*                                                                                     
                                                                                         
                                                           
 */
void SynaSetup(struct synaptics_ts_data *ts)
{

	unsigned char address;
	unsigned char buffer[6];

	TOUCH_INFO_MSG("%s", __FUNCTION__);

	for (address = 0xe9; address > 0xd0; address = address - 6) {
		readRMI(ts->client, address, &buffer[0], 6);

		if (!buffer[5])
			break;

		switch (buffer[5]) {
		case 0x34:
			SynaF34DataBase = buffer[3];
			SynaF34QueryBase = buffer[0];
			break;
		case 0x01:
			SynaF01DataBase = buffer[3];
			SynaF01CommandBase = buffer[1];
			SynaF01ControlBase = buffer[2];
			SynaF01QueryBase = buffer[0];
			break;
		}
	}

	SynaF34Reflash_BlockNum = SynaF34DataBase;
	SynaF34Reflash_BlockData = SynaF34DataBase + 2;
	SynaF34ReflashQuery_BootID = SynaF34QueryBase;
	SynaF34ReflashQuery_FlashPropertyQuery = SynaF34QueryBase + 2;
	SynaF34ReflashQuery_FirmwareBlockSize = SynaF34QueryBase + 3;
	SynaF34ReflashQuery_FirmwareBlockCount = SynaF34QueryBase +5;
	SynaF34ReflashQuery_ConfigBlockSize = SynaF34QueryBase + 3;
	SynaF34ReflashQuery_ConfigBlockCount = SynaF34QueryBase + 7;

	//                                                                  
	//                                                                           
	SynafirmwareImgData = (unsigned char *)((&my_image_bin[0])+0x100);
	SynaconfigImgData   = (unsigned char *)(SynafirmwareImgData+SynaImageSize);
	//                                                         
	//                                                                  
	SynafirmwareImgVersion = (unsigned int)(my_image_bin[7]);

	switch (SynafirmwareImgVersion) {
	case 2:
		//                                                               
		//                                                                        
		SynalockImgData = (unsigned char *)((&my_image_bin[0]) + 0xD0);
		break;
	case 3:
	case 4:
		//                                                               
		//                                                                        
		SynalockImgData = (unsigned char *)((&my_image_bin[0]) + 0xC0);
		break;
	case 5:
		//                                                               
		//                                                                        
		SynalockImgData = (unsigned char *)((&my_image_bin[0]) + 0xB0);
		break;
	default: break;
	}
}

/*                                           
 */
void SynaInitialize(struct synaptics_ts_data *ts)
{
	unsigned char uData[2];
	//                      
	TOUCH_INFO_MSG("%s", __FUNCTION__);

	TOUCH_INFO_MSG("\nInitializing Reflash Process...");
	uData[0] = 0x00;
	writeRMI(ts->client, 0xff, &uData[0], 1);

	SynaSetup(ts);

	//                        
	uData[0] = 0x0f;
	writeRMI(ts->client, SynaF01ControlBase+1, &uData[0], 1);

	//                        
	//                       
	//                                                                                   
	//                                                                                 

	readRMI(ts->client, SynaF34ReflashQuery_FirmwareBlockSize, &uData[0], 2);

	SynaFirmwareBlockSize = uData[0] | (uData[1] << 8);
}

/*                                                                                          
                                                       
 */
void SynaReadFirmwareInfo(struct synaptics_ts_data *ts)
{
	unsigned char uData[2];
	uData[0] = 0;
	uData[1] = 0;

	TOUCH_INFO_MSG("%s", __FUNCTION__);

	TOUCH_INFO_MSG("\nRead Firmware Info");

	readRMI(ts->client, SynaF34ReflashQuery_FirmwareBlockSize, &uData[0], 2);
	SynaFirmwareBlockSize = uData[0] | (uData[1] << 8);

	readRMI(ts->client, SynaF34ReflashQuery_FirmwareBlockCount, &uData[0], 2);
	SynaFirmwareBlockCount = uData[0] | (uData[1] << 8);
	SynaImageSize = SynaFirmwareBlockCount * SynaFirmwareBlockSize;
}

/*                                                                                        
                                                            
 */
void SynaReadConfigInfo(struct synaptics_ts_data *ts)
{
	unsigned char uData[2];
	TOUCH_INFO_MSG("%s", __FUNCTION__);

	TOUCH_INFO_MSG("\nRead Config Info");

	readRMI(ts->client, SynaF34ReflashQuery_ConfigBlockSize, &uData[0], 2);
	SynaConfigBlockSize = uData[0] | (uData[1] << 8);

	readRMI(ts->client, SynaF34ReflashQuery_ConfigBlockCount, &uData[0], 2);
	SynaConfigBlockCount = uData[0] | (uData[1] << 8);
	SynaConfigImageSize = SynaConfigBlockCount * SynaConfigBlockSize;
}


/*                                                                                                 
 */
void SynaReadBootloadID(struct synaptics_ts_data *ts)
{
	unsigned char uData[2];
	TOUCH_INFO_MSG("%s", __FUNCTION__);

	readRMI(ts->client, SynaF34ReflashQuery_BootID, &uData[0], 2);
	SynaBootloadID = uData[0] + uData[1] * 0x100;
}

/*                                                                                                    
 */
void SynaWriteBootloadID(struct synaptics_ts_data *ts)
{
	unsigned char uData[2];
	TOUCH_INFO_MSG("%s", __FUNCTION__);

	uData[0] = SynaBootloadID % 0x100;
	uData[1] = SynaBootloadID / 0x100;

	writeRMI(ts->client, SynaF34Reflash_BlockData, &uData[0], 2);
}

/*                                                                               
 */
void SynaWaitForATTN(int time)
{
	//                   

	//                          
	//               
	mdelay(time);
}

/*                                                                            
                                                                                         
 */
void SynaWaitATTN(struct synaptics_ts_data *ts)
{
	unsigned char uData;
	unsigned char uStatus;
//                    

	//                                         
//                             
	SynaWaitForATTN(10);

	do {
 		readRMI(ts->client, SynaF34_FlashControl, &uData, 1);
		readRMI(ts->client, (SynaF01DataBase + 1), &uStatus, 1);
	} while (uData!= 0x80);
}



/*                                                 
 */
void SynaEnableFlashing(struct synaptics_ts_data *ts)
{
	unsigned char uData;
	unsigned char uStatus;
	TOUCH_INFO_MSG("%s", __FUNCTION__);

	TOUCH_INFO_MSG("\nEnable Reflash...");

	//                                                                                          
	SynaReadBootloadID(ts);
	SynaWriteBootloadID(ts);

	//                                         
	do {
		readRMI(ts->client, SynaF34_FlashControl, &uData, 1);
	} while (((uData & 0x0f) != 0x00));

	//           
	readRMI (ts->client, SynaF01DataBase, &uStatus, 1);

	if ((uStatus &0x40) == 0) {
		//                                                                    
		//                                       
		uData = 0x0f;
		writeRMI(ts->client, SynaF34_FlashControl, &uData, 1);
		SynaWaitForATTN(100);
		readRMI(ts->client, (SynaF01DataBase + 1), &uStatus, 1);
		//                                                              
		SynaSetup(ts);
		/*                                                            
                                         */
		do {
			readRMI(ts->client, SynaF34_FlashControl, &uData, 1);
			/*                                                         
                           */
			//                                                 
		} while (uData != 0x80);
	}

}

/*                                                                                      
 */
void SynaProgramConfiguration(struct synaptics_ts_data *ts)
{
	unsigned char uData[2];
	//                                        
	//                                                               
	//                                                                        
	unsigned char *puData = (unsigned char *)&my_image_bin[0xb100];

	unsigned short blockNum;
	TOUCH_INFO_MSG("%s", __FUNCTION__);

	TOUCH_INFO_MSG("\nProgram Configuration Section...");

	for (blockNum = 0; blockNum < SynaConfigBlockCount; blockNum++) {
		uData[0] = blockNum & 0xff;
		uData[1] = (blockNum & 0xff00) >> 8;

		//                                                                                       
		writeRMI(ts->client, SynaF34Reflash_BlockNum, &uData[0], 2);
		writeRMI(ts->client, SynaF34Reflash_BlockData, puData, SynaConfigBlockSize);
		puData += SynaConfigBlockSize;

		//                                              
		uData[0] = 0x06;
		writeRMI(ts->client, SynaF34_FlashControl, &uData[0], 1);
		SynaWaitATTN(ts);
		TOUCH_INFO_MSG(".");
	}
}

/*                                                  
 */
void SynaFinalizeReflash(struct synaptics_ts_data *ts)
{
	unsigned char uData;
	unsigned char uStatus;
	TOUCH_INFO_MSG("%s", __FUNCTION__);

	TOUCH_INFO_MSG("\nFinalizing Reflash...");

	//                                                                    
	//                                                                             
	uData = 1;
	writeRMI(ts->client, SynaF01CommandBase, &uData, 1);

	SynaWaitForATTN(100);
	readRMI(ts->client, SynaF01DataBase, &uData, 1);

	//                                                       
	do {
		readRMI(ts->client, SynaF34_FlashControl, &uStatus, 1);
	} while ((uStatus & 0x0f) != 0x00);
	readRMI(ts->client, (SynaF01DataBase + 1), &uStatus, 1);

	SynaSetup(ts);
	uData = 0;

	//                                                                   
	//                                                                           
	do {
		readRMI(ts->client, SynaF01DataBase, &uData, 1);
	} while ((uData & 0x40) != 0);

	//                                                   
	SynaSetup(ts);

	TOUCH_INFO_MSG("\nReflash Completed. Please reboot.");
}

/*                                                                               
 */
void SynaFlashFirmwareWrite(struct synaptics_ts_data *ts)
{
	//                                                    
	//                                                                      
	//                                                                               
	unsigned char *puFirmwareData = (unsigned char *)&my_image_bin[0x100];
	unsigned char uData[2];
	unsigned short blockNum;
	TOUCH_INFO_MSG("%s", __FUNCTION__);

	for (blockNum = 0; blockNum < SynaFirmwareBlockCount; ++blockNum) {
		if (blockNum%100 == 0)
			TOUCH_INFO_MSG("blockNum = [%d], (SynaFirmwareBlockCount=%d)\n", blockNum, SynaFirmwareBlockCount);
		//                                                                                       
		uData[0] = blockNum & 0xff;
		uData[1] = (blockNum & 0xff00) >> 8;
		writeRMI(ts->client, SynaF34Reflash_BlockNum, &uData[0], 2);

		writeRMI(ts->client, SynaF34Reflash_BlockData, puFirmwareData,
					SynaFirmwareBlockSize);
		puFirmwareData += SynaFirmwareBlockSize;

		//                                         
		uData[0] = 2;
		writeRMI(ts->client, SynaF34_FlashControl, &uData[0], 1);

		SynaWaitATTN(ts);
	}

}

/*                                                          
 */
void SynaProgramFirmware(struct synaptics_ts_data *ts)
{
	unsigned char uData;
	TOUCH_INFO_MSG("%s", __FUNCTION__);

	TOUCH_INFO_MSG("\nProgram Firmware Section...");

	//                       

	SynaWriteBootloadID(ts);

	uData = 3;
	writeRMI(ts->client, SynaF34_FlashControl, &uData, 1);

	msleep(1000);

	SynaWaitATTN(ts);

	SynaFlashFirmwareWrite(ts);
}


/*                                         
*/
void eraseConfigBlock(struct synaptics_ts_data *ts)
{
	unsigned char uData;
	TOUCH_INFO_MSG("%s", __FUNCTION__);

	//                                                                     
	SynaReadBootloadID(ts);
	SynaWriteBootloadID(ts);

	//                                
	uData = 7;
	writeRMI(ts->client, SynaF34_FlashControl, &uData, 1);

	SynaWaitATTN(ts);
}

#if 0
//                                                                                                                 
//                                                 
//                                                                                             
void convertConfigBlockData()
{
	for (int i = 0; value[i]!=NULL; i++)
	{
		ConfigBlock[i] = value[i].Value;
	}
}
#endif

//                                                                                 
//                                                                                 
//                                    
//                                                                                    
//                                                         
void CRC_Calculate(unsigned short * data, unsigned short len)
{
	short i;
	unsigned long Data_CRC = 0xffffffff;
	unsigned long sum1 = (unsigned long)(Data_CRC & 0xFFFF);
	unsigned long sum2 = (unsigned long)(Data_CRC >> 16);
	TOUCH_INFO_MSG("%s", __FUNCTION__);

	for (i = 0; i < len; i++) {
		unsigned long temp = data[i];
		sum1 += temp;
		sum2 += sum1;
		sum1 = (unsigned long)((sum1 & 0xffff) + (sum1 >> 16));
		sum2 = (unsigned long)((sum2 & 0xffff) + (sum2 >> 16));
	}

	Data_CRC = (unsigned long)(sum2 << 16 | sum1);
	//                                 
}


void SynaBootloaderLock(struct synaptics_ts_data *ts)
{
	unsigned short lockBlockCount;
	unsigned char *puFirmwareData = SynalockImgData;
	unsigned char uData[2];
	unsigned short uBlockNum;

	//                                     
	readRMI(ts->client, (SynaF34QueryBase+ 2), &uData[0], 1);

	//                  
	if (uData[0] & 0x02) {
		TOUCH_INFO_MSG("Device unlocked. Lock it first...\n");
		//                                                                             
		//                                                                         
		switch (SynafirmwareImgVersion) {
		case 2:
			lockBlockCount = 3;
			break;
		case 3:
		case 4:
			lockBlockCount = 4;
			break;
		case 5:
			lockBlockCount = 5;
			break;
		default:
			lockBlockCount = 0;
			break;
		}

		//                                       
		//                                                                              
		//                                                                                
		//                                                                                  
		//                                      
		for (uBlockNum = 0; uBlockNum < lockBlockCount; ++uBlockNum) {
			uData[0] = uBlockNum & 0xff;
			uData[1] = (uBlockNum & 0xff00) >> 8;

			/*                    */
			readRMI(ts->client, SynaF34Reflash_BlockNum, &uData[0], 2);

			/*                  */
			writeRMI(ts->client, SynaF34Reflash_BlockData,
					puFirmwareData, SynaFirmwareBlockSize);

			/*                         */
			puFirmwareData += SynaFirmwareBlockSize;

			/*                                    */
			uData[0] = 4;
			writeRMI(ts->client, SynaF34_FlashControl, &uData[0], 1);

			/*                                                                             */
			SynaWaitATTN(ts);
		}
		TOUCH_INFO_MSG("Device locking done.\n");

		//                                                     
		//                                                                            
		//                                                                                  
		SynaEnableFlashing(ts);
	} else {
		TOUCH_INFO_MSG("Device already locked.\n");
	}
}



/*                                                   
*/
void ConfigBlockReflash(struct synaptics_ts_data *ts)
{
	unsigned char uData[2];
	TOUCH_INFO_MSG("%s", __FUNCTION__);


	SynaInitialize(ts);

	SynaReadConfigInfo(ts);

	SynaReadFirmwareInfo(ts);

	SynaF34_FlashControl = SynaF34DataBase + SynaFirmwareBlockSize + 2;

	SynaEnableFlashing(ts);

       //                                     
	readRMI(ts->client, (SynaF34QueryBase + 2), &uData[0], 1);

	//                  
	if (uData[0] & 0x02) {
	   SynaFinalizeReflash(ts);
	   return;
	   //                                           
	}

	eraseConfigBlock(ts);
	//                                                     

	SynaProgramConfiguration(ts);

	SynaFinalizeReflash(ts);
}

/*                                                                                                
*/
void CompleteReflash(struct synaptics_ts_data *ts)
{
	TOUCH_INFO_MSG("%s", __FUNCTION__);
	SynaInitialize(ts);

	SynaReadConfigInfo(ts);

	SynaReadFirmwareInfo(ts);

	SynaF34_FlashControl = SynaF34DataBase + SynaFirmwareBlockSize + 2;

	SynaEnableFlashing(ts);

	SynaProgramFirmware(ts);

	SynaProgramConfiguration(ts);

	SynaFinalizeReflash(ts);
}

void CompleteReflash_Lockdown(struct synaptics_ts_data *ts)
{
	TOUCH_INFO_MSG("%s", __FUNCTION__);

	SynaInitialize(ts);

	SynaReadConfigInfo(ts);

	SynaReadFirmwareInfo(ts);

	SynaF34_FlashControl = SynaF34DataBase + SynaFirmwareBlockSize + 2;

	SynaEnableFlashing(ts);

	SynaBootloaderLock(ts);

	SynaProgramFirmware(ts);

	SynaProgramConfiguration(ts);

	SynaFinalizeReflash(ts);
}


