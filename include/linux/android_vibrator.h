/*
  * Copyright (C) 2011, 2012 LGE, Inc.
  *
  * This software is licensed under the terms of the GNU General Public
  * License version 2, as published by the Free Software Foundation, and
  * may be copied, distributed, and modified under those terms.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  */

#ifndef __LINUX_ANDROID_VIBRATOR_H
#define __LINUX_ANDROID_VIBRATOR_H

/*                                */
struct android_vibrator_platform_data {
	int enable_status;
	int amp;
	int vibe_n_value;
	int vibe_warmup_delay; /*       */
	int (*power_set)(int enable); /*                        */
	int (*pwm_set)(int enable, int gain, int n_value); /*                  */
	int (*ic_enable_set)(int enable); /*                       */
	int (*vibrator_init)(void);
};

#endif

