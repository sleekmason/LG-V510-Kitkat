#ifndef QDSP5LPMCMDI_H
#define QDSP5LPMCMDI_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    L P M   I N T E R N A L   C O M M A N D S

GENERAL DESCRIPTION
  This file contains defintions of format blocks of commands 
  that are accepted by LPM Task

REFERENCES
  None

EXTERNALIZED FUNCTIONS
  None

Copyright (c) 1992-2009, The Linux Foundation. All rights reserved.

This software is licensed under the terms of the GNU General Public
License version 2, as published by the Free Software Foundation, and
may be copied, distributed, and modified under those terms.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*                                                                           

                                           

                                                                    
                                                              
   
  
                                                                                                                                                             
                                                             
  
                                   
                                                                             
                                  
                                                                           */


/*
                                                             
 */

#define	LPM_CMD_START		0x0000
#define	LPM_CMD_START_LEN	sizeof(lpm_cmd_start)

#define	LPM_CMD_SPATIAL_FILTER_PART_OPMODE_0	0x00000000
#define	LPM_CMD_SPATIAL_FILTER_PART_OPMODE_1	0x00010000
typedef struct {
	unsigned int	cmd_id;
	unsigned int	ip_data_cfg_part1;
	unsigned int	ip_data_cfg_part2;
	unsigned int	ip_data_cfg_part3;
	unsigned int	ip_data_cfg_part4;
	unsigned int	op_data_cfg_part1;
	unsigned int	op_data_cfg_part2;
	unsigned int	op_data_cfg_part3;
	unsigned int	spatial_filter_part[32];
} __attribute__((packed)) lpm_cmd_start;



/*
                                 
 */

#define	LPM_CMD_IDLE		0x0001
#define	LPM_CMD_IDLE_LEN	sizeof(lpm_cmd_idle)

typedef struct {
	unsigned int	cmd_id;
} __attribute__((packed)) lpm_cmd_idle;


#endif
