/*
 * SBE 2T3E3 synchronous serial card driver for Linux
 *
 * Copyright (C) 2009-2010 Krzysztof Halasa <khc@pm.waw.pl>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License
 * as published by the Free Software Foundation.
 *
 * This code is based on a driver written by SBE Inc.
 */

#include <linux/kernel.h>
#include "2t3e3.h"

const u32 cpld_reg_map[][2] =
{
	{ 0x0000, 0x0080 }, /*                                    */
	{ 0x0004, 0x0084 }, /*                                    */
	{ 0x0008, 0x0088 }, /*                               */
	{ 0x000c, 0x008c }, /*                                   */
	{ 0x0010, 0x0090 }, /*                                  */
	{ 0x0200, 0x0200 }, /*                                                 */
	{ 0x0204, 0x0204 }, /*                           */
	{ 0x0800, 0x1000 }, /*                                   */
	{ 0x2000, 0x2000 }, /*                                 */
	{ 0x2004, 0x2004 }, /*                           */
	{ 0x2008, 0x2008 }, /*                           */
	{ 0x200c, 0x200c }, /*                                    */
	{ 0x2010, 0x2014 }, /*                          */
	{ 0x2018, 0x201c }, /*                                              */
	{ 0x2020, 0x2024 }, /*                                       */
	{ 0x0068, 0x00e8 }, /*                                     */
	{ 0x006c, 0x00ec }, /*                                  */
	{ 0x0070, 0x00f0 }, /*                                 */
};

const u32 cpld_val_map[][2] =
{
	{ 0x01, 0x02 }, /*                                           */
	{ 0x04, 0x08 }, /*                                           */
	{ 0x00, 0x04 }, /*                                              */
	{ 0x01, 0x02 }  /*                                                 */
};

const u32 t3e3_framer_reg_map[] = {
	0x00, /*                    */
	0x01, /*                */
	0x04, /*                            */
	0x05, /*                            */
	0x10, /*                                                              */
	0x11, /*                                                */
	0x12, /*                                                      */
	0x13, /*                                                      */
	0x14, /*                                                        */
	0x15, /*                              */
	0x16, /*                 */
	0x17, /*                                         */
	0x18, /*                                             */
	0x19, /*                                           */
	0x1a, /*                                        */
	0x1b, /*                    */
	0x30, /*                                               */
	0x31, /*                                      */
	0x32, /*                 */
	0x33, /*                                                         */
	0x34, /*                                                     */
	0x35, /*                                                         */
	0x36, /*                                     */
	0x37, /*                                       */
	0x38, /*                        */
	0x48, /*                                                       */
	0x49, /*                                                       */
	0x4a, /*                                       */
	0x50, /*                               */
	0x51, /*                               */
	0x52, /*                                             */
	0x53, /*                                             */
	0x54, /*                                        */
	0x55, /*                                        */
	0x56, /*                                */
	0x57, /*                                */
	0x58, /*                                        */
	0x59, /*                                        */
	0x6c, /*                            */
	0x6d, /*                              */
	0x6e, /*                                     */
	0x6f, /*                                     */
	0x70, /*                                                    */
	0x71, /*                                                    */
	0x72, /*                                                    */
	0x73, /*                                                    */
	0x80, /*                           */
	0x81  /*                          */
};

const u32 t3e3_liu_reg_map[] =
{
	0x00, /*      */
	0x01, /*      */
	0x02, /*      */
	0x03, /*      */
	0x04 /*      */
};
