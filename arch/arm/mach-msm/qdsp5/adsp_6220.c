/* arch/arm/mach-msm/qdsp5/adsp_6220.h
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
	QDSP_MODULE_VOC,
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

#define QDSP_RTOS_MAX_TASK_ID  19U

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
	QDSP_MODULE_MAX
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
	QDSP_RTOS_NO_QUEUE   /*                                   */
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
	QDSP_MODULE_MAX
};

/*                                                            */
static uint32_t qdsp_combo_queue_offset_table[] = {
	0x6f2,               /*                                   */
	0x69e,               /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	0x6b2,               /*                                   */
	0x6c6,               /*                                   */
	0x6ca,               /*                                   */
	0x6c2,               /*                                   */
	0x6a6,               /*                                   */
	0x6aa,               /*                                   */
	0x6ae,               /*                                   */
	0x6ce,               /*                                   */
	0x6d2,               /*                                   */
	0x6d6,               /*                                   */
	0x6e6,               /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	0x6da,               /*                                   */
	0x6e2,               /*                                   */
	0x6de,               /*                                   */
	0x6ee,               /*                                   */
	0x6ea,               /*                                   */
	0x6a2,               /*                                   */
	0x6b6,               /*                                   */
	0x6be,               /*                                   */
	0x6ba                /*                                   */
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
	QDSP_MODULE_MAX
};

/*                                                             */
static uint32_t qdsp_qtv_lp_queue_offset_table[] = {
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	0x430,               /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	0x434,               /*                                   */
	0x438,               /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	0x440,               /*                                   */
	0x444,               /*                                   */
	0x448,               /*                                   */
	0x454,               /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	0x43c,               /*                                   */
	0x450,               /*                                   */
	0x44c,               /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE,  /*                                   */
	QDSP_RTOS_NO_QUEUE   /*                                   */
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

#define QDSP_MODULE(n) \
	{ .name = #n, .pdev_name = "adsp_" #n, .id = QDSP_MODULE_##n }

static struct adsp_module_info module_info[] = {
	QDSP_MODULE(AUDPLAY0TASK),
	QDSP_MODULE(AUDPPTASK),
	QDSP_MODULE(AUDPREPROCTASK),
	QDSP_MODULE(AUDRECTASK),
	QDSP_MODULE(VFETASK),
	QDSP_MODULE(QCAMTASK),
	QDSP_MODULE(LPMTASK),
	QDSP_MODULE(JPEGTASK),
	QDSP_MODULE(VIDEOTASK),
	QDSP_MODULE(VDEC_LP_MODE),
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
