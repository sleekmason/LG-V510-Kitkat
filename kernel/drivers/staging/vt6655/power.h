/*
 * Copyright (c) 1996, 2003 VIA Networking Technologies, Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * File: power.h
 *
 * Purpose: Handles 802.11 power management  functions
 *
 * Author: Lyndon Chen
 *
 * Date: July 17, 2002
 *
 */

#ifndef __POWER_H__
#define __POWER_H__


/*                                                                   */
#define     C_PWBT                   1000      //                                
#define     PS_FAST_INTERVAL         1         //                                  
#define     PS_MAX_INTERVAL          4         //                                 

/*                                                                   */

/*                                                                   */


/*                                                                   */


/*                                                                   */

//                 
//                        

bool
PSbConsiderPowerDown(
    void *hDeviceContext,
    bool bCheckRxDMA,
    bool bCheckCountToWakeUp
    );

void
PSvDisablePowerSaving(
    void *hDeviceContext
    );

void
PSvEnablePowerSaving(
    void *hDeviceContext,
    unsigned short wListenInterval
    );

void
PSvSendPSPOLL(
    void *hDeviceContext
    );

bool
PSbSendNullPacket(
    void *hDeviceContext
    );

bool
PSbIsNextTBTTWakeUp(
    void *hDeviceContext
    );

#endif //           
