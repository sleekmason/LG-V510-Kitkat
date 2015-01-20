/* genius-tvgo-a11mce.h - Keytable for genius_tvgo_a11mce Remote Controller
 *
 * keymap imported from ir-keymaps.c
 *
 * Copyright (c) 2010 by Mauro Carvalho Chehab <mchehab@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <media/rc-map.h>
#include <linux/module.h>

/*
                                            
                                        
 */

static struct rc_map_table genius_tvgo_a11mce[] = {
	/*             */
	{ 0x48, KEY_0 },
	{ 0x09, KEY_1 },
	{ 0x1d, KEY_2 },
	{ 0x1f, KEY_3 },
	{ 0x19, KEY_4 },
	{ 0x1b, KEY_5 },
	{ 0x11, KEY_6 },
	{ 0x17, KEY_7 },
	{ 0x12, KEY_8 },
	{ 0x16, KEY_9 },

	{ 0x54, KEY_RECORD },		/*           */
	{ 0x06, KEY_MUTE },		/*      */
	{ 0x10, KEY_POWER },
	{ 0x40, KEY_LAST },		/*        */
	{ 0x4c, KEY_CHANNELUP },	/*                     */
	{ 0x00, KEY_CHANNELDOWN },	/*                     */
	{ 0x0d, KEY_VOLUMEUP },
	{ 0x15, KEY_VOLUMEDOWN },
	{ 0x4d, KEY_OK },		/*                       */
	{ 0x1c, KEY_ZOOM },		/*                     */
	{ 0x02, KEY_MODE },		/*                    */
	{ 0x04, KEY_LIST },		/*      */
	/*                            */
	{ 0x1a, KEY_NEXT },		/*                   */
	{ 0x0e, KEY_PREVIOUS },		/*             */
	/*                                                   
                           */
	{ 0x1e, KEY_UP },		/*               */
	{ 0x0a, KEY_DOWN },		/*               */
	{ 0x05, KEY_CAMERA },		/*          */
	{ 0x0c, KEY_RIGHT },		/*            */
	/*                            */
	{ 0x49, KEY_RED },
	{ 0x0b, KEY_GREEN },
	{ 0x13, KEY_YELLOW },
	{ 0x50, KEY_BLUE },
};

static struct rc_map_list genius_tvgo_a11mce_map = {
	.map = {
		.scan    = genius_tvgo_a11mce,
		.size    = ARRAY_SIZE(genius_tvgo_a11mce),
		.rc_type = RC_TYPE_UNKNOWN,	/*                */
		.name    = RC_MAP_GENIUS_TVGO_A11MCE,
	}
};

static int __init init_rc_map_genius_tvgo_a11mce(void)
{
	return rc_map_register(&genius_tvgo_a11mce_map);
}

static void __exit exit_rc_map_genius_tvgo_a11mce(void)
{
	rc_map_unregister(&genius_tvgo_a11mce_map);
}

module_init(init_rc_map_genius_tvgo_a11mce)
module_exit(exit_rc_map_genius_tvgo_a11mce)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mauro Carvalho Chehab <mchehab@redhat.com>");