/*
 * Samsung HDMI interface driver
 *
 * Copyright (c) 2010-2011 Samsung Electronics Co., Ltd.
 *
 * Tomasz Stanislawski, <t.stanislaws@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundiation. either version 2 of the License,
 * or (at your option) any later version
 */

#ifdef CONFIG_VIDEO_SAMSUNG_S5P_HDMI_DEBUG
#define DEBUG
#endif

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <media/v4l2-subdev.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/delay.h>
#include <linux/bug.h>
#include <linux/pm_runtime.h>
#include <linux/clk.h>
#include <linux/regulator/consumer.h>

#include <media/s5p_hdmi.h>
#include <media/v4l2-common.h>
#include <media/v4l2-dev.h>
#include <media/v4l2-device.h>

#include "regs-hdmi.h"

MODULE_AUTHOR("Tomasz Stanislawski, <t.stanislaws@samsung.com>");
MODULE_DESCRIPTION("Samsung HDMI");
MODULE_LICENSE("GPL");

/*                                    */
#define HDMI_DEFAULT_PRESET V4L2_DV_1080P60

struct hdmi_resources {
	struct clk *hdmi;
	struct clk *sclk_hdmi;
	struct clk *sclk_pixel;
	struct clk *sclk_hdmiphy;
	struct clk *hdmiphy;
	struct regulator_bulk_data *regul_bulk;
	int regul_count;
};

struct hdmi_device {
	/*                                 */
	void __iomem *regs;
	/*                 */
	unsigned int irq;
	/*                           */
	struct device *dev;
	/*                                  */
	struct v4l2_subdev sd;
	/*                        */
	struct v4l2_device v4l2_dev;
	/*                              */
	struct v4l2_subdev *phy_sd;
	/*                          */
	struct v4l2_subdev *mhl_sd;
	/*                                        */
	const struct hdmi_preset_conf *cur_conf;
	/*                 */
	u32 cur_preset;
	/*                  */
	struct hdmi_resources res;
};

struct hdmi_tg_regs {
	u8 cmd;
	u8 h_fsz_l;
	u8 h_fsz_h;
	u8 hact_st_l;
	u8 hact_st_h;
	u8 hact_sz_l;
	u8 hact_sz_h;
	u8 v_fsz_l;
	u8 v_fsz_h;
	u8 vsync_l;
	u8 vsync_h;
	u8 vsync2_l;
	u8 vsync2_h;
	u8 vact_st_l;
	u8 vact_st_h;
	u8 vact_sz_l;
	u8 vact_sz_h;
	u8 field_chg_l;
	u8 field_chg_h;
	u8 vact_st2_l;
	u8 vact_st2_h;
	u8 vsync_top_hdmi_l;
	u8 vsync_top_hdmi_h;
	u8 vsync_bot_hdmi_l;
	u8 vsync_bot_hdmi_h;
	u8 field_top_hdmi_l;
	u8 field_top_hdmi_h;
	u8 field_bot_hdmi_l;
	u8 field_bot_hdmi_h;
};

struct hdmi_core_regs {
	u8 h_blank[2];
	u8 v_blank[3];
	u8 h_v_line[3];
	u8 vsync_pol[1];
	u8 int_pro_mode[1];
	u8 v_blank_f[3];
	u8 h_sync_gen[3];
	u8 v_sync_gen1[3];
	u8 v_sync_gen2[3];
	u8 v_sync_gen3[3];
};

struct hdmi_preset_conf {
	struct hdmi_core_regs core;
	struct hdmi_tg_regs tg;
	struct v4l2_mbus_framefmt mbus_fmt;
};

static struct platform_device_id hdmi_driver_types[] = {
	{
		.name		= "s5pv210-hdmi",
	}, {
		.name		= "exynos4-hdmi",
	}, {
		/*          */
	}
};

static const struct v4l2_subdev_ops hdmi_sd_ops;

static struct hdmi_device *sd_to_hdmi_dev(struct v4l2_subdev *sd)
{
	return container_of(sd, struct hdmi_device, sd);
}

static inline
void hdmi_write(struct hdmi_device *hdev, u32 reg_id, u32 value)
{
	writel(value, hdev->regs + reg_id);
}

static inline
void hdmi_write_mask(struct hdmi_device *hdev, u32 reg_id, u32 value, u32 mask)
{
	u32 old = readl(hdev->regs + reg_id);
	value = (value & mask) | (old & ~mask);
	writel(value, hdev->regs + reg_id);
}

static inline
void hdmi_writeb(struct hdmi_device *hdev, u32 reg_id, u8 value)
{
	writeb(value, hdev->regs + reg_id);
}

static inline u32 hdmi_read(struct hdmi_device *hdev, u32 reg_id)
{
	return readl(hdev->regs + reg_id);
}

static irqreturn_t hdmi_irq_handler(int irq, void *dev_data)
{
	struct hdmi_device *hdev = dev_data;
	u32 intc_flag;

	(void)irq;
	intc_flag = hdmi_read(hdev, HDMI_INTC_FLAG);
	/*                                    */
	if (intc_flag & HDMI_INTC_FLAG_HPD_UNPLUG) {
		printk(KERN_INFO "unplugged\n");
		hdmi_write_mask(hdev, HDMI_INTC_FLAG, ~0,
			HDMI_INTC_FLAG_HPD_UNPLUG);
	}
	if (intc_flag & HDMI_INTC_FLAG_HPD_PLUG) {
		printk(KERN_INFO "plugged\n");
		hdmi_write_mask(hdev, HDMI_INTC_FLAG, ~0,
			HDMI_INTC_FLAG_HPD_PLUG);
	}

	return IRQ_HANDLED;
}

static void hdmi_reg_init(struct hdmi_device *hdev)
{
	/*                       */
	hdmi_write_mask(hdev, HDMI_INTC_CON, ~0, HDMI_INTC_EN_GLOBAL |
		HDMI_INTC_EN_HPD_PLUG | HDMI_INTC_EN_HPD_UNPLUG);
	/*                 */
	hdmi_write_mask(hdev, HDMI_MODE_SEL,
		HDMI_MODE_DVI_EN, HDMI_MODE_MASK);
	hdmi_write_mask(hdev, HDMI_CON_2, ~0,
		HDMI_DVI_PERAMBLE_EN | HDMI_DVI_BAND_EN);
	/*                    */
	hdmi_write_mask(hdev, HDMI_CON_0, 0, HDMI_BLUE_SCR_EN);
	/*                                 */
	hdmi_writeb(hdev, HDMI_BLUE_SCREEN_0, 0x12);
	hdmi_writeb(hdev, HDMI_BLUE_SCREEN_1, 0x34);
	hdmi_writeb(hdev, HDMI_BLUE_SCREEN_2, 0x56);
}

static void hdmi_timing_apply(struct hdmi_device *hdev,
	const struct hdmi_preset_conf *conf)
{
	const struct hdmi_core_regs *core = &conf->core;
	const struct hdmi_tg_regs *tg = &conf->tg;

	/*                        */
	hdmi_writeb(hdev, HDMI_H_BLANK_0, core->h_blank[0]);
	hdmi_writeb(hdev, HDMI_H_BLANK_1, core->h_blank[1]);
	hdmi_writeb(hdev, HDMI_V_BLANK_0, core->v_blank[0]);
	hdmi_writeb(hdev, HDMI_V_BLANK_1, core->v_blank[1]);
	hdmi_writeb(hdev, HDMI_V_BLANK_2, core->v_blank[2]);
	hdmi_writeb(hdev, HDMI_H_V_LINE_0, core->h_v_line[0]);
	hdmi_writeb(hdev, HDMI_H_V_LINE_1, core->h_v_line[1]);
	hdmi_writeb(hdev, HDMI_H_V_LINE_2, core->h_v_line[2]);
	hdmi_writeb(hdev, HDMI_VSYNC_POL, core->vsync_pol[0]);
	hdmi_writeb(hdev, HDMI_INT_PRO_MODE, core->int_pro_mode[0]);
	hdmi_writeb(hdev, HDMI_V_BLANK_F_0, core->v_blank_f[0]);
	hdmi_writeb(hdev, HDMI_V_BLANK_F_1, core->v_blank_f[1]);
	hdmi_writeb(hdev, HDMI_V_BLANK_F_2, core->v_blank_f[2]);
	hdmi_writeb(hdev, HDMI_H_SYNC_GEN_0, core->h_sync_gen[0]);
	hdmi_writeb(hdev, HDMI_H_SYNC_GEN_1, core->h_sync_gen[1]);
	hdmi_writeb(hdev, HDMI_H_SYNC_GEN_2, core->h_sync_gen[2]);
	hdmi_writeb(hdev, HDMI_V_SYNC_GEN_1_0, core->v_sync_gen1[0]);
	hdmi_writeb(hdev, HDMI_V_SYNC_GEN_1_1, core->v_sync_gen1[1]);
	hdmi_writeb(hdev, HDMI_V_SYNC_GEN_1_2, core->v_sync_gen1[2]);
	hdmi_writeb(hdev, HDMI_V_SYNC_GEN_2_0, core->v_sync_gen2[0]);
	hdmi_writeb(hdev, HDMI_V_SYNC_GEN_2_1, core->v_sync_gen2[1]);
	hdmi_writeb(hdev, HDMI_V_SYNC_GEN_2_2, core->v_sync_gen2[2]);
	hdmi_writeb(hdev, HDMI_V_SYNC_GEN_3_0, core->v_sync_gen3[0]);
	hdmi_writeb(hdev, HDMI_V_SYNC_GEN_3_1, core->v_sync_gen3[1]);
	hdmi_writeb(hdev, HDMI_V_SYNC_GEN_3_2, core->v_sync_gen3[2]);
	/*                            */
	hdmi_writeb(hdev, HDMI_TG_H_FSZ_L, tg->h_fsz_l);
	hdmi_writeb(hdev, HDMI_TG_H_FSZ_H, tg->h_fsz_h);
	hdmi_writeb(hdev, HDMI_TG_HACT_ST_L, tg->hact_st_l);
	hdmi_writeb(hdev, HDMI_TG_HACT_ST_H, tg->hact_st_h);
	hdmi_writeb(hdev, HDMI_TG_HACT_SZ_L, tg->hact_sz_l);
	hdmi_writeb(hdev, HDMI_TG_HACT_SZ_H, tg->hact_sz_h);
	hdmi_writeb(hdev, HDMI_TG_V_FSZ_L, tg->v_fsz_l);
	hdmi_writeb(hdev, HDMI_TG_V_FSZ_H, tg->v_fsz_h);
	hdmi_writeb(hdev, HDMI_TG_VSYNC_L, tg->vsync_l);
	hdmi_writeb(hdev, HDMI_TG_VSYNC_H, tg->vsync_h);
	hdmi_writeb(hdev, HDMI_TG_VSYNC2_L, tg->vsync2_l);
	hdmi_writeb(hdev, HDMI_TG_VSYNC2_H, tg->vsync2_h);
	hdmi_writeb(hdev, HDMI_TG_VACT_ST_L, tg->vact_st_l);
	hdmi_writeb(hdev, HDMI_TG_VACT_ST_H, tg->vact_st_h);
	hdmi_writeb(hdev, HDMI_TG_VACT_SZ_L, tg->vact_sz_l);
	hdmi_writeb(hdev, HDMI_TG_VACT_SZ_H, tg->vact_sz_h);
	hdmi_writeb(hdev, HDMI_TG_FIELD_CHG_L, tg->field_chg_l);
	hdmi_writeb(hdev, HDMI_TG_FIELD_CHG_H, tg->field_chg_h);
	hdmi_writeb(hdev, HDMI_TG_VACT_ST2_L, tg->vact_st2_l);
	hdmi_writeb(hdev, HDMI_TG_VACT_ST2_H, tg->vact_st2_h);
	hdmi_writeb(hdev, HDMI_TG_VSYNC_TOP_HDMI_L, tg->vsync_top_hdmi_l);
	hdmi_writeb(hdev, HDMI_TG_VSYNC_TOP_HDMI_H, tg->vsync_top_hdmi_h);
	hdmi_writeb(hdev, HDMI_TG_VSYNC_BOT_HDMI_L, tg->vsync_bot_hdmi_l);
	hdmi_writeb(hdev, HDMI_TG_VSYNC_BOT_HDMI_H, tg->vsync_bot_hdmi_h);
	hdmi_writeb(hdev, HDMI_TG_FIELD_TOP_HDMI_L, tg->field_top_hdmi_l);
	hdmi_writeb(hdev, HDMI_TG_FIELD_TOP_HDMI_H, tg->field_top_hdmi_h);
	hdmi_writeb(hdev, HDMI_TG_FIELD_BOT_HDMI_L, tg->field_bot_hdmi_l);
	hdmi_writeb(hdev, HDMI_TG_FIELD_BOT_HDMI_H, tg->field_bot_hdmi_h);
}

static int hdmi_conf_apply(struct hdmi_device *hdmi_dev)
{
	struct device *dev = hdmi_dev->dev;
	const struct hdmi_preset_conf *conf = hdmi_dev->cur_conf;
	struct v4l2_dv_preset preset;
	int ret;

	dev_dbg(dev, "%s\n", __func__);

	/*               */
	hdmi_write_mask(hdmi_dev, HDMI_PHY_RSTOUT, ~0, HDMI_PHY_SW_RSTOUT);
	mdelay(10);
	hdmi_write_mask(hdmi_dev, HDMI_PHY_RSTOUT,  0, HDMI_PHY_SW_RSTOUT);
	mdelay(10);

	/*                   */
	preset.preset = hdmi_dev->cur_preset;
	ret = v4l2_subdev_call(hdmi_dev->phy_sd, video, s_dv_preset, &preset);
	if (ret) {
		dev_err(dev, "failed to set preset (%u)\n", preset.preset);
		return ret;
	}

	/*                     */
	hdmi_write_mask(hdmi_dev, HDMI_CORE_RSTOUT,  0, HDMI_CORE_SW_RSTOUT);
	mdelay(10);
	hdmi_write_mask(hdmi_dev, HDMI_CORE_RSTOUT, ~0, HDMI_CORE_SW_RSTOUT);
	mdelay(10);

	hdmi_reg_init(hdmi_dev);

	/*                        */
	hdmi_timing_apply(hdmi_dev, conf);

	return 0;
}

static void hdmi_dumpregs(struct hdmi_device *hdev, char *prefix)
{
#define DUMPREG(reg_id) \
	dev_dbg(hdev->dev, "%s:" #reg_id " = %08x\n", prefix, \
		readl(hdev->regs + reg_id))

	dev_dbg(hdev->dev, "%s: ---- CONTROL REGISTERS ----\n", prefix);
	DUMPREG(HDMI_INTC_FLAG);
	DUMPREG(HDMI_INTC_CON);
	DUMPREG(HDMI_HPD_STATUS);
	DUMPREG(HDMI_PHY_RSTOUT);
	DUMPREG(HDMI_PHY_VPLL);
	DUMPREG(HDMI_PHY_CMU);
	DUMPREG(HDMI_CORE_RSTOUT);

	dev_dbg(hdev->dev, "%s: ---- CORE REGISTERS ----\n", prefix);
	DUMPREG(HDMI_CON_0);
	DUMPREG(HDMI_CON_1);
	DUMPREG(HDMI_CON_2);
	DUMPREG(HDMI_SYS_STATUS);
	DUMPREG(HDMI_PHY_STATUS);
	DUMPREG(HDMI_STATUS_EN);
	DUMPREG(HDMI_HPD);
	DUMPREG(HDMI_MODE_SEL);
	DUMPREG(HDMI_HPD_GEN);
	DUMPREG(HDMI_DC_CONTROL);
	DUMPREG(HDMI_VIDEO_PATTERN_GEN);

	dev_dbg(hdev->dev, "%s: ---- CORE SYNC REGISTERS ----\n", prefix);
	DUMPREG(HDMI_H_BLANK_0);
	DUMPREG(HDMI_H_BLANK_1);
	DUMPREG(HDMI_V_BLANK_0);
	DUMPREG(HDMI_V_BLANK_1);
	DUMPREG(HDMI_V_BLANK_2);
	DUMPREG(HDMI_H_V_LINE_0);
	DUMPREG(HDMI_H_V_LINE_1);
	DUMPREG(HDMI_H_V_LINE_2);
	DUMPREG(HDMI_VSYNC_POL);
	DUMPREG(HDMI_INT_PRO_MODE);
	DUMPREG(HDMI_V_BLANK_F_0);
	DUMPREG(HDMI_V_BLANK_F_1);
	DUMPREG(HDMI_V_BLANK_F_2);
	DUMPREG(HDMI_H_SYNC_GEN_0);
	DUMPREG(HDMI_H_SYNC_GEN_1);
	DUMPREG(HDMI_H_SYNC_GEN_2);
	DUMPREG(HDMI_V_SYNC_GEN_1_0);
	DUMPREG(HDMI_V_SYNC_GEN_1_1);
	DUMPREG(HDMI_V_SYNC_GEN_1_2);
	DUMPREG(HDMI_V_SYNC_GEN_2_0);
	DUMPREG(HDMI_V_SYNC_GEN_2_1);
	DUMPREG(HDMI_V_SYNC_GEN_2_2);
	DUMPREG(HDMI_V_SYNC_GEN_3_0);
	DUMPREG(HDMI_V_SYNC_GEN_3_1);
	DUMPREG(HDMI_V_SYNC_GEN_3_2);

	dev_dbg(hdev->dev, "%s: ---- TG REGISTERS ----\n", prefix);
	DUMPREG(HDMI_TG_CMD);
	DUMPREG(HDMI_TG_H_FSZ_L);
	DUMPREG(HDMI_TG_H_FSZ_H);
	DUMPREG(HDMI_TG_HACT_ST_L);
	DUMPREG(HDMI_TG_HACT_ST_H);
	DUMPREG(HDMI_TG_HACT_SZ_L);
	DUMPREG(HDMI_TG_HACT_SZ_H);
	DUMPREG(HDMI_TG_V_FSZ_L);
	DUMPREG(HDMI_TG_V_FSZ_H);
	DUMPREG(HDMI_TG_VSYNC_L);
	DUMPREG(HDMI_TG_VSYNC_H);
	DUMPREG(HDMI_TG_VSYNC2_L);
	DUMPREG(HDMI_TG_VSYNC2_H);
	DUMPREG(HDMI_TG_VACT_ST_L);
	DUMPREG(HDMI_TG_VACT_ST_H);
	DUMPREG(HDMI_TG_VACT_SZ_L);
	DUMPREG(HDMI_TG_VACT_SZ_H);
	DUMPREG(HDMI_TG_FIELD_CHG_L);
	DUMPREG(HDMI_TG_FIELD_CHG_H);
	DUMPREG(HDMI_TG_VACT_ST2_L);
	DUMPREG(HDMI_TG_VACT_ST2_H);
	DUMPREG(HDMI_TG_VSYNC_TOP_HDMI_L);
	DUMPREG(HDMI_TG_VSYNC_TOP_HDMI_H);
	DUMPREG(HDMI_TG_VSYNC_BOT_HDMI_L);
	DUMPREG(HDMI_TG_VSYNC_BOT_HDMI_H);
	DUMPREG(HDMI_TG_FIELD_TOP_HDMI_L);
	DUMPREG(HDMI_TG_FIELD_TOP_HDMI_H);
	DUMPREG(HDMI_TG_FIELD_BOT_HDMI_L);
	DUMPREG(HDMI_TG_FIELD_BOT_HDMI_H);
#undef DUMPREG
}

static const struct hdmi_preset_conf hdmi_conf_480p = {
	.core = {
		.h_blank = {0x8a, 0x00},
		.v_blank = {0x0d, 0x6a, 0x01},
		.h_v_line = {0x0d, 0xa2, 0x35},
		.vsync_pol = {0x01},
		.int_pro_mode = {0x00},
		.v_blank_f = {0x00, 0x00, 0x00},
		.h_sync_gen = {0x0e, 0x30, 0x11},
		.v_sync_gen1 = {0x0f, 0x90, 0x00},
		/*                  */
	},
	.tg = {
		0x00, /*     */
		0x5a, 0x03, /*       */
		0x8a, 0x00, 0xd0, 0x02, /*      */
		0x0d, 0x02, /*       */
		0x01, 0x00, 0x33, 0x02, /*       */
		0x2d, 0x00, 0xe0, 0x01, /*      */
		0x33, 0x02, /*           */
		0x49, 0x02, /*          */
		0x01, 0x00, 0x33, 0x02, /*               */
		0x01, 0x00, 0x33, 0x02, /*               */
	},
	.mbus_fmt = {
		.width = 720,
		.height = 480,
		.code = V4L2_MBUS_FMT_FIXED, /*              */
		.field = V4L2_FIELD_NONE,
		.colorspace = V4L2_COLORSPACE_SRGB,
	},
};

static const struct hdmi_preset_conf hdmi_conf_720p60 = {
	.core = {
		.h_blank = {0x72, 0x01},
		.v_blank = {0xee, 0xf2, 0x00},
		.h_v_line = {0xee, 0x22, 0x67},
		.vsync_pol = {0x00},
		.int_pro_mode = {0x00},
		.v_blank_f = {0x00, 0x00, 0x00}, /*            */
		.h_sync_gen = {0x6c, 0x50, 0x02},
		.v_sync_gen1 = {0x0a, 0x50, 0x00},
		/*                  */
	},
	.tg = {
		0x00, /*     */
		0x72, 0x06, /*       */
		0x72, 0x01, 0x00, 0x05, /*      */
		0xee, 0x02, /*       */
		0x01, 0x00, 0x33, 0x02, /*       */
		0x1e, 0x00, 0xd0, 0x02, /*      */
		0x33, 0x02, /*           */
		0x49, 0x02, /*          */
		0x01, 0x00, 0x33, 0x02, /*               */
		0x01, 0x00, 0x33, 0x02, /*               */
	},
	.mbus_fmt = {
		.width = 1280,
		.height = 720,
		.code = V4L2_MBUS_FMT_FIXED, /*              */
		.field = V4L2_FIELD_NONE,
		.colorspace = V4L2_COLORSPACE_SRGB,
	},
};

static const struct hdmi_preset_conf hdmi_conf_1080p50 = {
	.core = {
		.h_blank = {0xd0, 0x02},
		.v_blank = {0x65, 0x6c, 0x01},
		.h_v_line = {0x65, 0x04, 0xa5},
		.vsync_pol = {0x00},
		.int_pro_mode = {0x00},
		.v_blank_f = {0x00, 0x00, 0x00}, /*            */
		.h_sync_gen = {0x0e, 0xea, 0x08},
		.v_sync_gen1 = {0x09, 0x40, 0x00},
		/*                  */
	},
	.tg = {
		0x00, /*     */
		0x98, 0x08, /*       */
		0x18, 0x01, 0x80, 0x07, /*      */
		0x65, 0x04, /*       */
		0x01, 0x00, 0x33, 0x02, /*       */
		0x2d, 0x00, 0x38, 0x04, /*      */
		0x33, 0x02, /*           */
		0x49, 0x02, /*          */
		0x01, 0x00, 0x33, 0x02, /*               */
		0x01, 0x00, 0x33, 0x02, /*               */
	},
	.mbus_fmt = {
		.width = 1920,
		.height = 1080,
		.code = V4L2_MBUS_FMT_FIXED, /*              */
		.field = V4L2_FIELD_NONE,
		.colorspace = V4L2_COLORSPACE_SRGB,
	},
};

static const struct hdmi_preset_conf hdmi_conf_1080p60 = {
	.core = {
		.h_blank = {0x18, 0x01},
		.v_blank = {0x65, 0x6c, 0x01},
		.h_v_line = {0x65, 0x84, 0x89},
		.vsync_pol = {0x00},
		.int_pro_mode = {0x00},
		.v_blank_f = {0x00, 0x00, 0x00}, /*            */
		.h_sync_gen = {0x56, 0x08, 0x02},
		.v_sync_gen1 = {0x09, 0x40, 0x00},
		/*                  */
	},
	.tg = {
		0x00, /*     */
		0x98, 0x08, /*       */
		0x18, 0x01, 0x80, 0x07, /*      */
		0x65, 0x04, /*       */
		0x01, 0x00, 0x33, 0x02, /*       */
		0x2d, 0x00, 0x38, 0x04, /*      */
		0x33, 0x02, /*           */
		0x48, 0x02, /*          */
		0x01, 0x00, 0x01, 0x00, /*               */
		0x01, 0x00, 0x33, 0x02, /*               */
	},
	.mbus_fmt = {
		.width = 1920,
		.height = 1080,
		.code = V4L2_MBUS_FMT_FIXED, /*              */
		.field = V4L2_FIELD_NONE,
		.colorspace = V4L2_COLORSPACE_SRGB,
	},
};

static const struct {
	u32 preset;
	const struct hdmi_preset_conf *conf;
} hdmi_conf[] = {
	{ V4L2_DV_480P59_94, &hdmi_conf_480p },
	{ V4L2_DV_720P59_94, &hdmi_conf_720p60 },
	{ V4L2_DV_1080P50, &hdmi_conf_1080p50 },
	{ V4L2_DV_1080P30, &hdmi_conf_1080p60 },
	{ V4L2_DV_1080P60, &hdmi_conf_1080p60 },
};

static const struct hdmi_preset_conf *hdmi_preset2conf(u32 preset)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(hdmi_conf); ++i)
		if (hdmi_conf[i].preset == preset)
			return  hdmi_conf[i].conf;
	return NULL;
}

static int hdmi_streamon(struct hdmi_device *hdev)
{
	struct device *dev = hdev->dev;
	struct hdmi_resources *res = &hdev->res;
	int ret, tries;

	dev_dbg(dev, "%s\n", __func__);

	ret = v4l2_subdev_call(hdev->phy_sd, video, s_stream, 1);
	if (ret)
		return ret;

	/*                                                  */
	for (tries = 100; tries; --tries) {
		u32 val = hdmi_read(hdev, HDMI_PHY_STATUS);
		if (val & HDMI_PHY_STATUS_READY)
			break;
		mdelay(1);
	}
	/*                           */
	if (tries == 0) {
		dev_err(dev, "hdmiphy's pll could not reach steady state.\n");
		v4l2_subdev_call(hdev->phy_sd, video, s_stream, 0);
		hdmi_dumpregs(hdev, "hdmiphy - s_stream");
		return -EIO;
	}

	/*              */
	ret = v4l2_subdev_call(hdev->mhl_sd, video, s_stream, 1);
	if (hdev->mhl_sd && ret) {
		v4l2_subdev_call(hdev->phy_sd, video, s_stream, 0);
		hdmi_dumpregs(hdev, "mhl - s_stream");
		return -EIO;
	}

	/*                                                  */
	clk_disable(res->sclk_hdmi);
	clk_set_parent(res->sclk_hdmi, res->sclk_hdmiphy);
	clk_enable(res->sclk_hdmi);

	/*                                  */
	hdmi_write_mask(hdev, HDMI_CON_0, ~0, HDMI_EN);
	hdmi_write_mask(hdev, HDMI_TG_CMD, ~0, HDMI_TG_EN);
	hdmi_dumpregs(hdev, "streamon");
	return 0;
}

static int hdmi_streamoff(struct hdmi_device *hdev)
{
	struct device *dev = hdev->dev;
	struct hdmi_resources *res = &hdev->res;

	dev_dbg(dev, "%s\n", __func__);

	hdmi_write_mask(hdev, HDMI_CON_0, 0, HDMI_EN);
	hdmi_write_mask(hdev, HDMI_TG_CMD, 0, HDMI_TG_EN);

	/*                                                   */
	clk_disable(res->sclk_hdmi);
	clk_set_parent(res->sclk_hdmi, res->sclk_pixel);
	clk_enable(res->sclk_hdmi);

	v4l2_subdev_call(hdev->mhl_sd, video, s_stream, 0);
	v4l2_subdev_call(hdev->phy_sd, video, s_stream, 0);

	hdmi_dumpregs(hdev, "streamoff");
	return 0;
}

static int hdmi_s_stream(struct v4l2_subdev *sd, int enable)
{
	struct hdmi_device *hdev = sd_to_hdmi_dev(sd);
	struct device *dev = hdev->dev;

	dev_dbg(dev, "%s(%d)\n", __func__, enable);
	if (enable)
		return hdmi_streamon(hdev);
	return hdmi_streamoff(hdev);
}

static void hdmi_resource_poweron(struct hdmi_resources *res)
{
	/*                    */
	regulator_bulk_enable(res->regul_count, res->regul_bulk);
	/*                                  */
	clk_enable(res->hdmiphy);
	/*                                                     */
	clk_set_parent(res->sclk_hdmi, res->sclk_pixel);
	/*                */
	clk_enable(res->sclk_hdmi);
}

static void hdmi_resource_poweroff(struct hdmi_resources *res)
{
	/*                 */
	clk_disable(res->sclk_hdmi);
	/*                   */
	clk_disable(res->hdmiphy);
	/*                     */
	regulator_bulk_disable(res->regul_count, res->regul_bulk);
}

static int hdmi_s_power(struct v4l2_subdev *sd, int on)
{
	struct hdmi_device *hdev = sd_to_hdmi_dev(sd);
	int ret;

	if (on)
		ret = pm_runtime_get_sync(hdev->dev);
	else
		ret = pm_runtime_put_sync(hdev->dev);
	/*                                 */
	return IS_ERR_VALUE(ret) ? ret : 0;
}

static int hdmi_s_dv_preset(struct v4l2_subdev *sd,
	struct v4l2_dv_preset *preset)
{
	struct hdmi_device *hdev = sd_to_hdmi_dev(sd);
	struct device *dev = hdev->dev;
	const struct hdmi_preset_conf *conf;

	conf = hdmi_preset2conf(preset->preset);
	if (conf == NULL) {
		dev_err(dev, "preset (%u) not supported\n", preset->preset);
		return -EINVAL;
	}
	hdev->cur_conf = conf;
	hdev->cur_preset = preset->preset;
	return 0;
}

static int hdmi_g_dv_preset(struct v4l2_subdev *sd,
	struct v4l2_dv_preset *preset)
{
	memset(preset, 0, sizeof(*preset));
	preset->preset = sd_to_hdmi_dev(sd)->cur_preset;
	return 0;
}

static int hdmi_g_mbus_fmt(struct v4l2_subdev *sd,
	  struct v4l2_mbus_framefmt *fmt)
{
	struct hdmi_device *hdev = sd_to_hdmi_dev(sd);
	struct device *dev = hdev->dev;

	dev_dbg(dev, "%s\n", __func__);
	if (!hdev->cur_conf)
		return -EINVAL;
	*fmt = hdev->cur_conf->mbus_fmt;
	return 0;
}

static int hdmi_enum_dv_presets(struct v4l2_subdev *sd,
	struct v4l2_dv_enum_preset *preset)
{
	if (preset->index >= ARRAY_SIZE(hdmi_conf))
		return -EINVAL;
	return v4l_fill_dv_preset_info(hdmi_conf[preset->index].preset, preset);
}

static const struct v4l2_subdev_core_ops hdmi_sd_core_ops = {
	.s_power = hdmi_s_power,
};

static const struct v4l2_subdev_video_ops hdmi_sd_video_ops = {
	.s_dv_preset = hdmi_s_dv_preset,
	.g_dv_preset = hdmi_g_dv_preset,
	.enum_dv_presets = hdmi_enum_dv_presets,
	.g_mbus_fmt = hdmi_g_mbus_fmt,
	.s_stream = hdmi_s_stream,
};

static const struct v4l2_subdev_ops hdmi_sd_ops = {
	.core = &hdmi_sd_core_ops,
	.video = &hdmi_sd_video_ops,
};

static int hdmi_runtime_suspend(struct device *dev)
{
	struct v4l2_subdev *sd = dev_get_drvdata(dev);
	struct hdmi_device *hdev = sd_to_hdmi_dev(sd);

	dev_dbg(dev, "%s\n", __func__);
	v4l2_subdev_call(hdev->mhl_sd, core, s_power, 0);
	hdmi_resource_poweroff(&hdev->res);
	return 0;
}

static int hdmi_runtime_resume(struct device *dev)
{
	struct v4l2_subdev *sd = dev_get_drvdata(dev);
	struct hdmi_device *hdev = sd_to_hdmi_dev(sd);
	int ret = 0;

	dev_dbg(dev, "%s\n", __func__);

	hdmi_resource_poweron(&hdev->res);

	ret = hdmi_conf_apply(hdev);
	if (ret)
		goto fail;

	/*              */
	ret = v4l2_subdev_call(hdev->mhl_sd, core, s_power, 1);
	if (hdev->mhl_sd && ret)
		goto fail;

	dev_dbg(dev, "poweron succeed\n");

	return 0;

fail:
	hdmi_resource_poweroff(&hdev->res);
	dev_err(dev, "poweron failed\n");

	return ret;
}

static const struct dev_pm_ops hdmi_pm_ops = {
	.runtime_suspend = hdmi_runtime_suspend,
	.runtime_resume	 = hdmi_runtime_resume,
};

static void hdmi_resources_cleanup(struct hdmi_device *hdev)
{
	struct hdmi_resources *res = &hdev->res;

	dev_dbg(hdev->dev, "HDMI resource cleanup\n");
	/*                   */
	if (res->regul_count)
		regulator_bulk_free(res->regul_count, res->regul_bulk);
	/*                    */
	kfree(res->regul_bulk);
	if (!IS_ERR_OR_NULL(res->hdmiphy))
		clk_put(res->hdmiphy);
	if (!IS_ERR_OR_NULL(res->sclk_hdmiphy))
		clk_put(res->sclk_hdmiphy);
	if (!IS_ERR_OR_NULL(res->sclk_pixel))
		clk_put(res->sclk_pixel);
	if (!IS_ERR_OR_NULL(res->sclk_hdmi))
		clk_put(res->sclk_hdmi);
	if (!IS_ERR_OR_NULL(res->hdmi))
		clk_put(res->hdmi);
	memset(res, 0, sizeof *res);
}

static int hdmi_resources_init(struct hdmi_device *hdev)
{
	struct device *dev = hdev->dev;
	struct hdmi_resources *res = &hdev->res;
	static char *supply[] = {
		"hdmi-en",
		"vdd",
		"vdd_osc",
		"vdd_pll",
	};
	int i, ret;

	dev_dbg(dev, "HDMI resource init\n");

	memset(res, 0, sizeof *res);
	/*                   */

	res->hdmi = clk_get(dev, "hdmi");
	if (IS_ERR_OR_NULL(res->hdmi)) {
		dev_err(dev, "failed to get clock 'hdmi'\n");
		goto fail;
	}
	res->sclk_hdmi = clk_get(dev, "sclk_hdmi");
	if (IS_ERR_OR_NULL(res->sclk_hdmi)) {
		dev_err(dev, "failed to get clock 'sclk_hdmi'\n");
		goto fail;
	}
	res->sclk_pixel = clk_get(dev, "sclk_pixel");
	if (IS_ERR_OR_NULL(res->sclk_pixel)) {
		dev_err(dev, "failed to get clock 'sclk_pixel'\n");
		goto fail;
	}
	res->sclk_hdmiphy = clk_get(dev, "sclk_hdmiphy");
	if (IS_ERR_OR_NULL(res->sclk_hdmiphy)) {
		dev_err(dev, "failed to get clock 'sclk_hdmiphy'\n");
		goto fail;
	}
	res->hdmiphy = clk_get(dev, "hdmiphy");
	if (IS_ERR_OR_NULL(res->hdmiphy)) {
		dev_err(dev, "failed to get clock 'hdmiphy'\n");
		goto fail;
	}
	res->regul_bulk = kcalloc(ARRAY_SIZE(supply),
				  sizeof(res->regul_bulk[0]), GFP_KERNEL);
	if (!res->regul_bulk) {
		dev_err(dev, "failed to get memory for regulators\n");
		goto fail;
	}
	for (i = 0; i < ARRAY_SIZE(supply); ++i) {
		res->regul_bulk[i].supply = supply[i];
		res->regul_bulk[i].consumer = NULL;
	}

	ret = regulator_bulk_get(dev, ARRAY_SIZE(supply), res->regul_bulk);
	if (ret) {
		dev_err(dev, "failed to get regulators\n");
		goto fail;
	}
	res->regul_count = ARRAY_SIZE(supply);

	return 0;
fail:
	dev_err(dev, "HDMI resource init - failed\n");
	hdmi_resources_cleanup(hdev);
	return -ENODEV;
}

static int __devinit hdmi_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct resource *res;
	struct i2c_adapter *adapter;
	struct v4l2_subdev *sd;
	struct hdmi_device *hdmi_dev = NULL;
	struct s5p_hdmi_platform_data *pdata = dev->platform_data;
	int ret;

	dev_dbg(dev, "probe start\n");

	if (!pdata) {
		dev_err(dev, "platform data is missing\n");
		ret = -ENODEV;
		goto fail;
	}

	hdmi_dev = devm_kzalloc(&pdev->dev, sizeof(*hdmi_dev), GFP_KERNEL);
	if (!hdmi_dev) {
		dev_err(dev, "out of memory\n");
		ret = -ENOMEM;
		goto fail;
	}

	hdmi_dev->dev = dev;

	ret = hdmi_resources_init(hdmi_dev);
	if (ret)
		goto fail;

	/*                        */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		dev_err(dev, "get memory resource failed.\n");
		ret = -ENXIO;
		goto fail_init;
	}

	hdmi_dev->regs = devm_ioremap(&pdev->dev, res->start,
				      resource_size(res));
	if (hdmi_dev->regs == NULL) {
		dev_err(dev, "register mapping failed.\n");
		ret = -ENXIO;
		goto fail_init;
	}

	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (res == NULL) {
		dev_err(dev, "get interrupt resource failed.\n");
		ret = -ENXIO;
		goto fail_init;
	}

	ret = devm_request_irq(&pdev->dev, res->start, hdmi_irq_handler, 0,
			       "hdmi", hdmi_dev);
	if (ret) {
		dev_err(dev, "request interrupt failed.\n");
		goto fail_init;
	}
	hdmi_dev->irq = res->start;

	/*                                                              */
	strlcpy(hdmi_dev->v4l2_dev.name, dev_name(dev),
		sizeof(hdmi_dev->v4l2_dev.name));
	/*                                                         */
	ret = v4l2_device_register(NULL, &hdmi_dev->v4l2_dev);
	if (ret) {
		dev_err(dev, "could not register v4l2 device.\n");
		goto fail_init;
	}

	/*                                    */
	if (!pdata->hdmiphy_info) {
		dev_err(dev, "hdmiphy info is missing in platform data\n");
		ret = -ENXIO;
		goto fail_vdev;
	}

	adapter = i2c_get_adapter(pdata->hdmiphy_bus);
	if (adapter == NULL) {
		dev_err(dev, "hdmiphy adapter request failed\n");
		ret = -ENXIO;
		goto fail_vdev;
	}

	hdmi_dev->phy_sd = v4l2_i2c_new_subdev_board(&hdmi_dev->v4l2_dev,
		adapter, pdata->hdmiphy_info, NULL);
	/*                                               */
	i2c_put_adapter(adapter);
	if (hdmi_dev->phy_sd == NULL) {
		dev_err(dev, "missing subdev for hdmiphy\n");
		ret = -ENODEV;
		goto fail_vdev;
	}

	/*                                            */
	if (pdata->mhl_info) {
		adapter = i2c_get_adapter(pdata->mhl_bus);
		if (adapter == NULL) {
			dev_err(dev, "MHL adapter request failed\n");
			ret = -ENXIO;
			goto fail_vdev;
		}

		hdmi_dev->mhl_sd = v4l2_i2c_new_subdev_board(
			&hdmi_dev->v4l2_dev, adapter,
			pdata->mhl_info, NULL);
		/*                                               */
		i2c_put_adapter(adapter);
		if (hdmi_dev->mhl_sd == NULL) {
			dev_err(dev, "missing subdev for MHL\n");
			ret = -ENODEV;
			goto fail_vdev;
		}
	}

	clk_enable(hdmi_dev->res.hdmi);

	pm_runtime_enable(dev);

	sd = &hdmi_dev->sd;
	v4l2_subdev_init(sd, &hdmi_sd_ops);
	sd->owner = THIS_MODULE;

	strlcpy(sd->name, "s5p-hdmi", sizeof sd->name);
	hdmi_dev->cur_preset = HDMI_DEFAULT_PRESET;
	/*                                             */
	hdmi_dev->cur_conf = hdmi_preset2conf(hdmi_dev->cur_preset);

	/*                                                                */
	dev_set_drvdata(dev, sd);

	dev_info(dev, "probe successful\n");

	return 0;

fail_vdev:
	v4l2_device_unregister(&hdmi_dev->v4l2_dev);

fail_init:
	hdmi_resources_cleanup(hdmi_dev);

fail:
	dev_err(dev, "probe failed\n");
	return ret;
}

static int __devexit hdmi_remove(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct v4l2_subdev *sd = dev_get_drvdata(dev);
	struct hdmi_device *hdmi_dev = sd_to_hdmi_dev(sd);

	pm_runtime_disable(dev);
	clk_disable(hdmi_dev->res.hdmi);
	v4l2_device_unregister(&hdmi_dev->v4l2_dev);
	disable_irq(hdmi_dev->irq);
	hdmi_resources_cleanup(hdmi_dev);
	dev_info(dev, "remove successful\n");

	return 0;
}

static struct platform_driver hdmi_driver __refdata = {
	.probe = hdmi_probe,
	.remove = __devexit_p(hdmi_remove),
	.id_table = hdmi_driver_types,
	.driver = {
		.name = "s5p-hdmi",
		.owner = THIS_MODULE,
		.pm = &hdmi_pm_ops,
	}
};

module_platform_driver(hdmi_driver);
