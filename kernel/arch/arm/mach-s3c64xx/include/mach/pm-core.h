/* linux/arch/arm/mach-s3c64xx/include/mach/pm-core.h
 *
 * Copyright 2008 Openmoko, Inc.
 * Copyright 2008 Simtec Electronics
 *      Ben Dooks <ben@simtec.co.uk>
 *      http://armlinux.simtec.co.uk/
 *
 * S3C64XX - PM core support for arch/arm/plat-s3c/pm.c
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <mach/regs-gpio.h>

static inline void s3c_pm_debug_init_uart(void)
{
	u32 tmp = __raw_readl(S3C_PCLK_GATE);

	/*                                                           
                                                               
                                                              
                  
  */

	tmp |= S3C_CLKCON_PCLK_UART0;
	tmp |= S3C_CLKCON_PCLK_UART1;
	tmp |= S3C_CLKCON_PCLK_UART2;
	tmp |= S3C_CLKCON_PCLK_UART3;

	__raw_writel(tmp, S3C_PCLK_GATE);
	udelay(10);
}

static inline void s3c_pm_arch_prepare_irqs(void)
{
	/*                                            */

	/*                                    */
	__raw_writel(__raw_readl(S3C64XX_EINT0PEND), S3C64XX_EINT0PEND);
}

static inline void s3c_pm_arch_stop_clocks(void)
{
}

static inline void s3c_pm_arch_show_resume_irqs(void)
{
}

/*                                                                
                                                                */

#define s3c_irqwake_eintallow	((1 << 28) - 1)
#define s3c_irqwake_intallow	(~0)

static inline void s3c_pm_arch_update_uart(void __iomem *regs,
					   struct pm_uart_save *save)
{
	u32 ucon = __raw_readl(regs + S3C2410_UCON);
	u32 ucon_clk = ucon & S3C6400_UCON_CLKMASK;
	u32 save_clk = save->ucon & S3C6400_UCON_CLKMASK;
	u32 new_ucon;
	u32 delta;

	/*                                                                  
                                                                   
             */
	save->ucon |= S3C2410_UCON_TXILEVEL | S3C2410_UCON_RXILEVEL;

	/*                                                            
                                                               
                                                              
                         
  */
	if (ucon_clk != save_clk) {
		new_ucon = save->ucon;
		delta = ucon_clk ^ save_clk;

		/*                                 
                                                
                */
		if (ucon_clk & S3C6400_UCON_UCLK0 &&
		    !(save_clk & S3C6400_UCON_UCLK0) &&
		    delta & S3C6400_UCON_PCLK2) {
			new_ucon &= ~S3C6400_UCON_UCLK0;
		} else if (delta == S3C6400_UCON_PCLK2) {
			/*                                    
                                  */
			new_ucon ^= S3C6400_UCON_PCLK2;
		}

		S3C_PMDBG("ucon change %04x => %04x (save=%04x)\n",
			  ucon, new_ucon, save->ucon);
		save->ucon = new_ucon;
	}
}

static inline void s3c_pm_restored_gpios(void)
{
	/*                                                    */

	__raw_writel(0, S3C64XX_SLPEN);
}

static inline void samsung_pm_saved_gpios(void)
{
	/*                                                                  
                                                                      
                                                 
  */

	__raw_writel(S3C64XX_SLPEN_USE_xSLP, S3C64XX_SLPEN);
}
