#ifndef QDSP5LPMMSGI_H
#define QDSP5LPMMSGI_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    L P M   I N T E R N A L   M E S S A G E S

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

#define	LPM_MSG_IDLE_ACK	0x0000
#define	LPM_MSG_IDLE_ACK_LEN	sizeof(lpm_msg_idle_ack)

typedef struct {
} __attribute__((packed)) lpm_msg_idle_ack;


/*
                                               
 */


#define	LPM_MSG_START_ACK	0x0001
#define	LPM_MSG_START_ACK_LEN	sizeof(lpm_msg_start_ack)


typedef struct {
} __attribute__((packed)) lpm_msg_start_ack;


/*
                                                            
 */

#define	LPM_MSG_DONE		0x0002
#define	LPM_MSG_DONE_LEN	sizeof(lpm_msg_done)

typedef struct {
} __attribute__((packed)) lpm_msg_done;


#endif
