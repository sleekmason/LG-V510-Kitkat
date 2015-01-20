/* linux/arch/arm/mach-s3c2410/mach-otom.c
 *
 * Copyright (c) 2004 Nex Vision
 *   Guillaume GOURAT <guillaume.gourat@nexvision.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/serial_core.h>
#include <linux/platform_device.h>
#include <linux/io.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/irq.h>

#include <mach/otom-map.h>

#include <mach/hardware.h>
#include <asm/irq.h>
#include <asm/mach-types.h>

#include <plat/regs-serial.h>
#include <mach/regs-gpio.h>

#include <plat/s3c2410.h>
#include <plat/clock.h>
#include <plat/devs.h>
#include <plat/iic.h>
#include <plat/cpu.h>

#include "common.h"

static struct map_desc otom11_iodesc[] __initdata = {
  /*             */
	{ (u32)OTOM_VA_CS8900A_BASE, OTOM_PA_CS8900A_BASE, SZ_16M, MT_DEVICE },
};

#define UCON S3C2410_UCON_DEFAULT
#define ULCON S3C2410_LCON_CS8 | S3C2410_LCON_PNONE | S3C2410_LCON_STOPB
#define UFCON S3C2410_UFCON_RXTRIG12 | S3C2410_UFCON_FIFOMODE

static struct s3c2410_uartcfg otom11_uartcfgs[] __initdata = {
	[0] = {
		.hwport	     = 0,
		.flags	     = 0,
		.ucon	     = UCON,
		.ulcon	     = ULCON,
		.ufcon	     = UFCON,
	},
	[1] = {
		.hwport	     = 1,
		.flags	     = 0,
		.ucon	     = UCON,
		.ulcon	     = ULCON,
		.ufcon	     = UFCON,
	},
	/*                             */
	[2] = {
		.hwport	     = 2,
		.flags	     = 0,
		.ucon	     = UCON,
		.ulcon	     = ULCON,
		.ufcon	     = UFCON,
	}
};

/*                                   */

static struct resource otom_nor_resource[] = {
	[0] = {
		.start = S3C2410_CS0,
		.end   = S3C2410_CS0 + (4*1024*1024) - 1,
		.flags = IORESOURCE_MEM,
	}
};

static struct platform_device otom_device_nor = {
	.name		= "mtd-flash",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(otom_nor_resource),
	.resource	= otom_nor_resource,
};

/*                       */

static struct platform_device *otom11_devices[] __initdata = {
	&s3c_device_ohci,
	&s3c_device_lcd,
	&s3c_device_wdt,
	&s3c_device_i2c0,
	&s3c_device_iis,
 	&s3c_device_rtc,
	&otom_device_nor,
};

static void __init otom11_map_io(void)
{
	s3c24xx_init_io(otom11_iodesc, ARRAY_SIZE(otom11_iodesc));
	s3c24xx_init_clocks(0);
	s3c24xx_init_uarts(otom11_uartcfgs, ARRAY_SIZE(otom11_uartcfgs));
}

static void __init otom11_init(void)
{
	s3c_i2c0_set_platdata(NULL);
	platform_add_devices(otom11_devices, ARRAY_SIZE(otom11_devices));
}

MACHINE_START(OTOM, "Nex Vision - Otom 1.1")
	/*                                                              */
	.atag_offset	= 0x100,
	.map_io		= otom11_map_io,
	.init_machine	= otom11_init,
	.init_irq	= s3c24xx_init_irq,
	.timer		= &s3c24xx_timer,
	.restart	= s3c2410_restart,
MACHINE_END
