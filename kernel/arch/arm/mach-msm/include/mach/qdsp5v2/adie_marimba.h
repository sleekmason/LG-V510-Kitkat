/* Copyright (c) 2009-2010, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#ifndef __MACH_QDSP5_V2_ADIE_MARIMBA_H
#define __MACH_QDSP5_V2_ADIE_MARIMBA_H

#include <linux/types.h>

/*                          */
#define ADIE_CODEC_ACTION_ENTRY       0x1
/*                                 */
#define ADIE_CODEC_ACTION_DELAY_WAIT      0x2
/*                                    */
#define ADIE_CODEC_ACTION_STAGE_REACHED   0x3

/*                                                        */
#define ADIE_CODEC_PATH_OFF                                        0x0050

/*                                                              
                                           
 */
#define ADIE_CODEC_DIGITAL_READY                                   0x0100

/*                                                              
                                                       
 */
#define ADIE_CODEC_DIGITAL_ANALOG_READY                            0x1000


/*                                                          
                                                  
 */
#define ADIE_CODEC_ANALOG_OFF                                      0x0750


/*                                                          
                                                               
  
                                                  
  
 */
#define ADIE_CODEC_DIGITAL_OFF                                     0x0600

/*                                                               
                    */
#define ADIE_CODEC_FLASH_IMAGE 					   0x0001

/*           */
#define ADIE_CODEC_RX 0
#define ADIE_CODEC_TX 1
#define ADIE_CODEC_LB 3
#define ADIE_CODEC_MAX 4

#define ADIE_CODEC_PACK_ENTRY(reg, mask, val) ((val)|(mask << 8)|(reg << 16))

#define ADIE_CODEC_UNPACK_ENTRY(packed, reg, mask, val) \
	do { \
		((reg) = ((packed >> 16) & (0xff))); \
		((mask) = ((packed >> 8) & (0xff))); \
		((val) = ((packed) & (0xff))); \
	} while (0);

struct adie_codec_action_unit {
	u32 type;
	u32 action;
};

struct adie_codec_hwsetting_entry{
	struct adie_codec_action_unit *actions;
	u32 action_sz;
	u32 freq_plan;
	u32 osr;
	/*              
                      
  */
};

struct adie_codec_dev_profile {
	u32 path_type; /*          */
	u32 setting_sz;
	struct adie_codec_hwsetting_entry *settings;
};

#endif
