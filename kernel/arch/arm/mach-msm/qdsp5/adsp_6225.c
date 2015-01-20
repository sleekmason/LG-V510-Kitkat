/* arch/arm/mach-msm/qdsp5/adsp_6225.h
 *
 * Copyright (c) 2008-2009, The Linux Foundation. All rights reserved.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "adsp.h"

/*                  */
typedef enum {
	QDSP_MODULE_KERNEL,
	QDSP_MODULE_AFETASK,
	QDSP_MODULE_AUDPLAY0TASK,
	QDSP_MODULE_AUDPLAY1TASK,
	QDSP_MODULE_AUDPPTASK,
	QDSP_MODULE_VIDEOTASK,
	QDSP_MODULE_VIDEO_AAC_VOC,
	QDSP_MODULE_PCM_DEC,
	QDSP_MODULE_AUDIO_DEC_MP3,
	QDSP_MODULE_AUDIO_DEC_AAC,
	QDSP_MODULE_AUDIO_DEC_WMA,
	QDSP_MODULE_HOSTPCM,
	QDSP_MODULE_DTMF,
	QDSP_MODULE_AUDRECTASK,
	QDSP_MODULE_AUDPREPROCTASK,
	QDSP_MODULE_SBC_ENC,
	QDSP_MODULE_VOC_UMTS,
	QDSP_MODULE_VOC_CDMA,
	QDSP_MODULE_VOC_PCM,
	QDSP_MODULE_VOCENCTASK,
	QDSP_MODULE_VOCDECTASK,
	QDSP_MODULE_VOICEPROCTASK,
	QDSP_MODULE_VIDEOENCTASK,
	QDSP_MODULE_VFETASK,
	QDSP_MODULE_WAV_ENC,
	QDSP_MODULE_AACLC_ENC,
	QDSP_MODULE_VIDEO_AMR,
	QDSP_MODULE_VOC_AMR,
	QDSP_MODULE_VOC_EVRC,
	QDSP_MODULE_VOC_13K,
	QDSP_MODULE_VOC_FGV,
	QDSP_MODULE_DIAGTASK,
	QDSP_MODULE_JPEGTASK,
	QDSP_MODULE_LPMTASK,
	QDSP_MODULE_QCAMTASK,
	QDSP_MODULE_MODMATHTASK,
	QDSP_MODULE_AUDPLAY2TASK,
	QDSP_MODULE_AUDPLAY3TASK,
	QDSP_MODULE_AUDPLAY4TASK,
	QDSP_MODULE_GRAPHICSTASK,
	QDSP_MODULE_MIDI,
	QDSP_MODULE_GAUDIO,
	QDSP_MODULE_VDEC_LP_MODE,
	QDSP_MODULE_MAX,
} qdsp_module_type;

#define QDSP_RTOS_MAX_TASK_ID  30U

/*                                                          */
static qdsp_module_type qdsp_gaudio_task_to_module_table[] = {
	QDSP_MODULE_KERNEL,
	QDSP_MODULE_AFETASK,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_AUDPPTASK,
	QDSP_MODULE_AUDPLAY0TASK,
	QDSP_MODULE_AUDPLAY1TASK,
	QDSP_MODULE_AUDPLAY2TASK,
	QDSP_MODULE_AUDPLAY3TASK,
	QDSP_MODULE_AUDPLAY4TASK,
	QDSP_MODULE_MAX,
	QDSP_MODULE_AUDRECTASK,
	QDSP_MODULE_AUDPREPROCTASK,
	QDSP_MODULE_MAX,
	QDSP_MODULE_GRAPHICSTASK,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
};

/*                                                             */
static uint32_t qdsp_gaudio_queue_offset_table[] = {
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	0x3f0,               /*                                   */
	0x420,               /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	0x3f4,               /*                                   */
	0x3f8,               /*                                   */
	0x3fc,               /*                                   */
	0x40c,               /*                                   */
	0x410,               /*                                   */
	0x414,               /*                                   */
	0x418,               /*                                   */
	0x41c,               /*                                   */
	0x400,               /*                                   */
	0x408,               /*                                   */
	0x404,               /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
};

/*                                                         */
static qdsp_module_type qdsp_combo_task_to_module_table[] = {
	QDSP_MODULE_KERNEL,
	QDSP_MODULE_AFETASK,
	QDSP_MODULE_VOCDECTASK,
	QDSP_MODULE_VOCENCTASK,
	QDSP_MODULE_VIDEOTASK,
	QDSP_MODULE_VIDEOENCTASK,
	QDSP_MODULE_VOICEPROCTASK,
	QDSP_MODULE_VFETASK,
	QDSP_MODULE_JPEGTASK,
	QDSP_MODULE_AUDPPTASK,
	QDSP_MODULE_AUDPLAY0TASK,
	QDSP_MODULE_AUDPLAY1TASK,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_LPMTASK,
	QDSP_MODULE_AUDRECTASK,
	QDSP_MODULE_AUDPREPROCTASK,
	QDSP_MODULE_MODMATHTASK,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_DIAGTASK,
	QDSP_MODULE_MAX,
};

/*                                                            */
static uint32_t qdsp_combo_queue_offset_table[] = {
	0x714,               /*                                   */
	0x6bc,               /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	0x6d0,               /*                                   */
	0x6e8,               /*                                   */
	0x6ec,               /*                                   */
	0x6e4,               /*                                   */
	0x6c4,               /*                                   */
	0x6c8,               /*                                   */
	0x6cc,               /*                                   */
	0x6f0,               /*                                   */
	0x6f4,               /*                                   */
	0x6f8,               /*                                   */
	0x708,               /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	0x6fc,               /*                                   */
	0x704,               /*                                   */
	0x700,               /*                                   */
	0x710,               /*                                   */
	0x70c,               /*                                   */
	0x6c0,               /*                                   */
	0x6d8,               /*                                   */
	0x6e0,               /*                                   */
	0x6dc,               /*                                   */
	0x6d4,               /*                                   */
};

/*                                                          */
static qdsp_module_type qdsp_qtv_lp_task_to_module_table[] = {
	QDSP_MODULE_KERNEL,
	QDSP_MODULE_AFETASK,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_VIDEOTASK,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_AUDPPTASK,
	QDSP_MODULE_AUDPLAY0TASK,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_AUDRECTASK,
	QDSP_MODULE_AUDPREPROCTASK,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
	QDSP_MODULE_MAX,
};

/*                                                             */
static uint32_t qdsp_qtv_lp_queue_offset_table[] = {
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	0x3fe,               /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	0x402,               /*                                   */
	0x406,               /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	0x40e,               /*                                   */
	0x412,               /*                                   */
	0x416,               /*                                   */
	0x422,               /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	0x40a,               /*                                   */
	0x41e,               /*                                   */
	0x41a,               /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
};

/*                                    */
static qdsp_module_type *qdsp_task_to_module[] = {
	qdsp_combo_task_to_module_table,
	qdsp_gaudio_task_to_module_table,
	qdsp_qtv_lp_task_to_module_table,
};

/*                                  */
static uint32_t *qdsp_queue_offset_table[] = {
	qdsp_combo_queue_offset_table,
	qdsp_gaudio_queue_offset_table,
	qdsp_qtv_lp_queue_offset_table,
};

#define QDSP_MODULE(n, clkname, clkrate, verify_cmd_func, patch_event_func) \
	{ .name = #n, .pdev_name = "adsp_" #n, .id = QDSP_MODULE_##n, \
	  .clk_name = clkname, .clk_rate = clkrate, \
	  .verify_cmd = verify_cmd_func, .patch_event = patch_event_func }

static struct adsp_module_info module_info[] = {
	QDSP_MODULE(AUDPLAY0TASK, NULL, 0, NULL, NULL),
	QDSP_MODULE(AUDPPTASK, NULL, 0, NULL, NULL),
	QDSP_MODULE(AUDRECTASK, NULL, 0, NULL, NULL),
	QDSP_MODULE(AUDPREPROCTASK, NULL, 0, NULL, NULL),
	QDSP_MODULE(VFETASK, "vfe_clk", 0, adsp_vfe_verify_cmd,
		adsp_vfe_patch_event),
	QDSP_MODULE(QCAMTASK, NULL, 0, NULL, NULL),
	QDSP_MODULE(LPMTASK, NULL, 0, adsp_lpm_verify_cmd, NULL),
	QDSP_MODULE(JPEGTASK, "vdc_clk", 0, adsp_jpeg_verify_cmd,
		adsp_jpeg_patch_event),
	QDSP_MODULE(VIDEOTASK, "vdc_clk", 96000000,
		adsp_video_verify_cmd, NULL),
	QDSP_MODULE(VDEC_LP_MODE, NULL, 0, NULL, NULL),
	QDSP_MODULE(VIDEOENCTASK, "vdc_clk", 96000000,
		adsp_videoenc_verify_cmd, NULL),
};

int adsp_init_info(struct adsp_info *info)
{
	info->send_irq =   0x00c00200;
	info->read_ctrl =  0x00400038;
	info->write_ctrl = 0x00400034;

	info->max_msg16_size = 193;
	info->max_msg32_size = 8;

	info->max_task_id = 16;
	info->max_module_id = QDSP_MODULE_MAX - 1;
	info->max_queue_id = QDSP_QUEUE_MAX;
	info->max_image_id = 2;
	info->queue_offset = qdsp_queue_offset_table;
	info->task_to_module = qdsp_task_to_module;

	info->module_count = ARRAY_SIZE(module_info);
	info->module = module_info;
	return 0;
}
