/* Copyright (c) 2011, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _LINUX_MSM_GPIOMUX_H
#define _LINUX_MSM_GPIOMUX_H

#ifdef CONFIG_MSM_GPIOMUX

/*                                                                   */
int __must_check msm_gpiomux_get(unsigned gpio);

/*                                                                   */
int msm_gpiomux_put(unsigned gpio);

#else

static inline int __must_check msm_gpiomux_get(unsigned gpio)
{
	return -ENOSYS;
}

static inline int msm_gpiomux_put(unsigned gpio)
{
	return -ENOSYS;
}

#endif

#endif /*                      */
