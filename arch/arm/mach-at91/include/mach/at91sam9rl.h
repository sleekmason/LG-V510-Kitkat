/*
 * arch/arm/mach-at91/include/mach/at91sam9260.h
 *
 *  Copyright (C) 2007 Atmel Corporation
 *
 * Common definitions.
 * Based on AT91SAM9RL datasheet revision A. (Preliminary)
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive for
 * more details.
 */

#ifndef AT91SAM9RL_H
#define AT91SAM9RL_H

/*
                                     
 */
#define AT91SAM9RL_ID_PIOA	2	/*                          */
#define AT91SAM9RL_ID_PIOB	3	/*                          */
#define AT91SAM9RL_ID_PIOC	4	/*                          */
#define AT91SAM9RL_ID_PIOD	5	/*                          */
#define AT91SAM9RL_ID_US0	6	/*         */
#define AT91SAM9RL_ID_US1	7	/*         */
#define AT91SAM9RL_ID_US2	8	/*         */
#define AT91SAM9RL_ID_US3	9	/*         */
#define AT91SAM9RL_ID_MCI	10	/*                           */
#define AT91SAM9RL_ID_TWI0	11	/*       */
#define AT91SAM9RL_ID_TWI1	12	/*       */
#define AT91SAM9RL_ID_SPI	13	/*                             */
#define AT91SAM9RL_ID_SSC0	14	/*                                 */
#define AT91SAM9RL_ID_SSC1	15	/*                                 */
#define AT91SAM9RL_ID_TC0	16	/*                 */
#define AT91SAM9RL_ID_TC1	17	/*                 */
#define AT91SAM9RL_ID_TC2	18	/*                 */
#define AT91SAM9RL_ID_PWMC	19	/*                                   */
#define AT91SAM9RL_ID_TSC	20	/*                         */
#define AT91SAM9RL_ID_DMA	21	/*                */
#define AT91SAM9RL_ID_UDPHS	22	/*               */
#define AT91SAM9RL_ID_LCDC	23	/*                */
#define AT91SAM9RL_ID_AC97C	24	/*                 */
#define AT91SAM9RL_ID_IRQ0	31	/*                                      */


/*
                                           
 */
#define AT91SAM9RL_BASE_TCB0	0xfffa0000
#define AT91SAM9RL_BASE_TC0	0xfffa0000
#define AT91SAM9RL_BASE_TC1	0xfffa0040
#define AT91SAM9RL_BASE_TC2	0xfffa0080
#define AT91SAM9RL_BASE_MCI	0xfffa4000
#define AT91SAM9RL_BASE_TWI0	0xfffa8000
#define AT91SAM9RL_BASE_TWI1	0xfffac000
#define AT91SAM9RL_BASE_US0	0xfffb0000
#define AT91SAM9RL_BASE_US1	0xfffb4000
#define AT91SAM9RL_BASE_US2	0xfffb8000
#define AT91SAM9RL_BASE_US3	0xfffbc000
#define AT91SAM9RL_BASE_SSC0	0xfffc0000
#define AT91SAM9RL_BASE_SSC1	0xfffc4000
#define AT91SAM9RL_BASE_PWMC	0xfffc8000
#define AT91SAM9RL_BASE_SPI	0xfffcc000
#define AT91SAM9RL_BASE_TSC	0xfffd0000
#define AT91SAM9RL_BASE_UDPHS	0xfffd4000
#define AT91SAM9RL_BASE_AC97C	0xfffd8000


/*
                                                 
 */
#define AT91_SCKCR	(0xfffffd50 - AT91_BASE_SYS)

#define AT91SAM9RL_BASE_DMA	0xffffe600
#define AT91SAM9RL_BASE_ECC	0xffffe800
#define AT91SAM9RL_BASE_SDRAMC	0xffffea00
#define AT91SAM9RL_BASE_SMC	0xffffec00
#define AT91SAM9RL_BASE_MATRIX	0xffffee00
#define AT91SAM9RL_BASE_DBGU	AT91_BASE_DBGU0
#define AT91SAM9RL_BASE_PIOA	0xfffff400
#define AT91SAM9RL_BASE_PIOB	0xfffff600
#define AT91SAM9RL_BASE_PIOC	0xfffff800
#define AT91SAM9RL_BASE_PIOD	0xfffffa00
#define AT91SAM9RL_BASE_RSTC	0xfffffd00
#define AT91SAM9RL_BASE_SHDWC	0xfffffd10
#define AT91SAM9RL_BASE_RTT	0xfffffd20
#define AT91SAM9RL_BASE_PIT	0xfffffd30
#define AT91SAM9RL_BASE_WDT	0xfffffd40
#define AT91SAM9RL_BASE_GPBR	0xfffffd60
#define AT91SAM9RL_BASE_RTC	0xfffffe00

#define AT91_USART0	AT91SAM9RL_BASE_US0
#define AT91_USART1	AT91SAM9RL_BASE_US1
#define AT91_USART2	AT91SAM9RL_BASE_US2
#define AT91_USART3	AT91SAM9RL_BASE_US3


/*
                   
 */
#define AT91SAM9RL_SRAM_BASE	0x00300000	/*                            */
#define AT91SAM9RL_SRAM_SIZE	SZ_16K		/*                           */

#define AT91SAM9RL_ROM_BASE	0x00400000	/*                           */
#define AT91SAM9RL_ROM_SIZE	(2 * SZ_16K)	/*                          */

#define AT91SAM9RL_LCDC_BASE	0x00500000	/*                */
#define AT91SAM9RL_UDPHS_FIFO	0x00600000	/*                          */

#endif
