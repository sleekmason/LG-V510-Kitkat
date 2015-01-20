/*
 * Hardware specific definitions for SL-C7xx series of PDAs
 *
 * Copyright (c) 2004-2005 Richard Purdie
 *
 * Based on Sharp's 2.4 kernel patches
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#ifndef __ASM_ARCH_CORGI_H
#define __ASM_ARCH_CORGI_H  1


/*
                                        
 */
#define CORGI_GPIO_KEY_INT			(0)	/*                    */
#define CORGI_GPIO_AC_IN			(1) /*                   */
#define CORGI_GPIO_WAKEUP			(3) /*                             */
#define CORGI_GPIO_AK_INT			(4)	/*                                  */
#define CORGI_GPIO_TP_INT			(5)	/*                       */
#define CORGI_GPIO_nSD_WP			(7) /*                   */
#define CORGI_GPIO_nSD_DETECT		(9) /*                    */
#define CORGI_GPIO_nSD_INT			(10) /*                        */
#define CORGI_GPIO_MAIN_BAT_LOW		(11) /*                               */
#define CORGI_GPIO_BAT_COVER		(11) /*                      */
#define CORGI_GPIO_LED_ORANGE		(13) /*                    */
#define CORGI_GPIO_CF_CD			(14) /*                           */
#define CORGI_GPIO_CHRG_FULL		(16) /*                                */
#define CORGI_GPIO_CF_IRQ			(17) /*                         */
#define CORGI_GPIO_LCDCON_CS		(19) /*                         */
#define CORGI_GPIO_MAX1111_CS		(20) /*                     */
#define CORGI_GPIO_ADC_TEMP_ON		(21) /*                                       */
#define CORGI_GPIO_IR_ON			(22) /*                       */
#define CORGI_GPIO_ADS7846_CS		(24) /*                     */
#define CORGI_GPIO_SD_PWR			(33) /*              */
#define CORGI_GPIO_CHRG_ON			(38) /*                         */
#define CORGI_GPIO_DISCHARGE_ON		(42) /*                          */
#define CORGI_GPIO_CHRG_UKN			(43) /*                                    */
#define CORGI_GPIO_HSYNC			(44) /*                 */
#define CORGI_GPIO_USB_PULLUP		(45) /*                           */


/*
                             
 */
#define CORGI_KEY_STROBE_NUM		(12)
#define CORGI_KEY_SENSE_NUM			(8)
#define CORGI_GPIO_ALL_STROBE_BIT	(0x00003ffc)
#define CORGI_GPIO_HIGH_SENSE_BIT	(0xfc000000)
#define CORGI_GPIO_HIGH_SENSE_RSHIFT	(26)
#define CORGI_GPIO_LOW_SENSE_BIT	(0x00000003)
#define CORGI_GPIO_LOW_SENSE_LSHIFT	(6)
#define CORGI_GPIO_STROBE_BIT(a)	GPIO_bit(66+(a))
#define CORGI_GPIO_SENSE_BIT(a)		GPIO_bit(58+(a))
#define CORGI_GAFR_ALL_STROBE_BIT	(0x0ffffff0)
#define CORGI_GAFR_HIGH_SENSE_BIT	(0xfff00000)
#define CORGI_GAFR_LOW_SENSE_BIT	(0x0000000f)
#define CORGI_GPIO_KEY_SENSE(a)		(58+(a))
#define CORGI_GPIO_KEY_STROBE(a)	(66+(a))


/*
                   
 */
#define CORGI_IRQ_GPIO_KEY_INT		PXA_GPIO_TO_IRQ(0)
#define CORGI_IRQ_GPIO_AC_IN		PXA_GPIO_TO_IRQ(1)
#define CORGI_IRQ_GPIO_WAKEUP		PXA_GPIO_TO_IRQ(3)
#define CORGI_IRQ_GPIO_AK_INT		PXA_GPIO_TO_IRQ(4)
#define CORGI_IRQ_GPIO_TP_INT		PXA_GPIO_TO_IRQ(5)
#define CORGI_IRQ_GPIO_nSD_DETECT	PXA_GPIO_TO_IRQ(9)
#define CORGI_IRQ_GPIO_nSD_INT		PXA_GPIO_TO_IRQ(10)
#define CORGI_IRQ_GPIO_MAIN_BAT_LOW	PXA_GPIO_TO_IRQ(11)
#define CORGI_IRQ_GPIO_CF_CD		PXA_GPIO_TO_IRQ(14)
#define CORGI_IRQ_GPIO_CHRG_FULL	PXA_GPIO_TO_IRQ(16)	/*                       */
#define CORGI_IRQ_GPIO_CF_IRQ		PXA_GPIO_TO_IRQ(17)
#define CORGI_IRQ_GPIO_KEY_SENSE(a)	PXA_GPIO_TO_IRQ(58+(a))	/*                      */


/*
                               
 */
#define CORGI_SCP_LED_GREEN		SCOOP_GPCR_PA11
#define CORGI_SCP_SWA			SCOOP_GPCR_PA12  /*                */
#define CORGI_SCP_SWB			SCOOP_GPCR_PA13  /*                */
#define CORGI_SCP_MUTE_L		SCOOP_GPCR_PA14
#define CORGI_SCP_MUTE_R		SCOOP_GPCR_PA15
#define CORGI_SCP_AKIN_PULLUP	SCOOP_GPCR_PA16
#define CORGI_SCP_APM_ON		SCOOP_GPCR_PA17
#define CORGI_SCP_BACKLIGHT_CONT	SCOOP_GPCR_PA18
#define CORGI_SCP_MIC_BIAS		SCOOP_GPCR_PA19

#define CORGI_SCOOP_IO_DIR	( CORGI_SCP_LED_GREEN | CORGI_SCP_MUTE_L | CORGI_SCP_MUTE_R | \
			CORGI_SCP_AKIN_PULLUP | CORGI_SCP_APM_ON | CORGI_SCP_BACKLIGHT_CONT | \
			CORGI_SCP_MIC_BIAS )
#define CORGI_SCOOP_IO_OUT	( CORGI_SCP_MUTE_L | CORGI_SCP_MUTE_R )

#define CORGI_SCOOP_GPIO_BASE		(PXA_NR_BUILTIN_GPIO)
#define CORGI_GPIO_LED_GREEN		(CORGI_SCOOP_GPIO_BASE + 0)
#define CORGI_GPIO_SWA			(CORGI_SCOOP_GPIO_BASE + 1)  /*                */
#define CORGI_GPIO_SWB			(CORGI_SCOOP_GPIO_BASE + 2)  /*                */
#define CORGI_GPIO_MUTE_L		(CORGI_SCOOP_GPIO_BASE + 3)
#define CORGI_GPIO_MUTE_R		(CORGI_SCOOP_GPIO_BASE + 4)
#define CORGI_GPIO_AKIN_PULLUP		(CORGI_SCOOP_GPIO_BASE + 5)
#define CORGI_GPIO_APM_ON		(CORGI_SCOOP_GPIO_BASE + 6)
#define CORGI_GPIO_BACKLIGHT_CONT	(CORGI_SCOOP_GPIO_BASE + 7)
#define CORGI_GPIO_MIC_BIAS		(CORGI_SCOOP_GPIO_BASE + 8)

#endif /*                     */

