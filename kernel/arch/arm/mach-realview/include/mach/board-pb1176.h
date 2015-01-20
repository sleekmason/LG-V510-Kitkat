/*
 * arch/arm/mach-realview/include/mach/board-pb1176.h
 *
 * Copyright (C) 2008 ARM Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#ifndef __ASM_ARCH_BOARD_PB1176_H
#define __ASM_ARCH_BOARD_PB1176_H

#include <mach/platform.h>

/*
                       
 */
#define REALVIEW_PB1176_UART4_BASE		0x10009000 /*        */
#define REALVIEW_PB1176_SCTL_BASE		0x10100000 /*                   */
#define REALVIEW_PB1176_SMC_BASE		0x10111000 /*     */
#define REALVIEW_PB1176_DMC_BASE		0x10109000 /*                   */
#define REALVIEW_PB1176_SDRAM67_BASE		0x70000000 /*                     */
#define REALVIEW_PB1176_FLASH_BASE		0x30000000
#define REALVIEW_PB1176_FLASH_SIZE		SZ_64M
#define REALVIEW_PB1176_SEC_FLASH_BASE		0x3C000000 /*              */
#define REALVIEW_PB1176_SEC_FLASH_SIZE		SZ_64M

#define REALVIEW_PB1176_TIMER0_1_BASE		0x10104000 /*               */
#define REALVIEW_PB1176_TIMER2_3_BASE		0x10105000 /*               */
#define REALVIEW_PB1176_TIMER4_5_BASE		0x10106000 /*               */
#define REALVIEW_PB1176_WATCHDOG_BASE		0x10107000 /*                    */
#define REALVIEW_PB1176_RTC_BASE		0x10108000 /*                 */
#define REALVIEW_PB1176_GPIO0_BASE		0x1010A000 /*             */
#define REALVIEW_PB1176_SSP_BASE		0x1010B000 /*                         */
#define REALVIEW_PB1176_UART0_BASE		0x1010C000 /*        */
#define REALVIEW_PB1176_UART1_BASE		0x1010D000 /*        */
#define REALVIEW_PB1176_UART2_BASE		0x1010E000 /*        */
#define REALVIEW_PB1176_UART3_BASE		0x1010F000 /*        */
#define REALVIEW_PB1176_CLCD_BASE		0x10112000 /*      */
#define REALVIEW_PB1176_ETH_BASE		0x3A000000 /*          */
#define REALVIEW_PB1176_USB_BASE		0x3B000000 /*     */

/*
              
 */
#define REALVIEW_PB1176_PCI_BASE		0x60000000 /*                 */
#define REALVIEW_PB1176_PCI_CFG_BASE		0x61000000 /*            */
#define REALVIEW_PB1176_PCI_IO_BASE0		0x62000000 /*               */
#define REALVIEW_PB1176_PCI_MEM_BASE0		0x63000000 /*                 */
#define REALVIEW_PB1176_PCI_MEM_BASE1		0x64000000 /*                 */
#define REALVIEW_PB1176_PCI_MEM_BASE2		0x68000000 /*                 */

#define REALVIEW_PB1176_PCI_BASE_SIZE		0x01000000 /*      */
#define REALVIEW_PB1176_PCI_CFG_BASE_SIZE	0x01000000 /*      */
#define REALVIEW_PB1176_PCI_IO_BASE0_SIZE	0x01000000 /*      */
#define REALVIEW_PB1176_PCI_MEM_BASE0_SIZE	0x01000000 /*      */
#define REALVIEW_PB1176_PCI_MEM_BASE1_SIZE	0x04000000 /*      */
#define REALVIEW_PB1176_PCI_MEM_BASE2_SIZE	0x08000000 /*       */

#define REALVIEW_DC1176_GIC_CPU_BASE		0x10120000 /*                               */
#define REALVIEW_DC1176_GIC_DIST_BASE		0x10121000 /*                             */
#define REALVIEW_DC1176_ROM_BASE		0x10200000 /*                                   */
#define REALVIEW_PB1176_GIC_CPU_BASE		0x10040000 /*                            */
#define REALVIEW_PB1176_GIC_DIST_BASE		0x10041000 /*                          */
#define REALVIEW_PB1176_L220_BASE		0x10110000 /*                */

/*
                                                                        
 */
#define REALVIEW_PB1176_SYS_SOFT_RESET    0x0100

#endif	/*                           */