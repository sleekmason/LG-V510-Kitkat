/* Copyright (c) 2009, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef __DAL_AUDIO_H__
#define __DAL_AUDIO_H__

#include "../dal.h"
#include "dal_audio_format.h"

#define AUDIO_DAL_DEVICE 0x02000028
#define AUDIO_DAL_PORT "DAL_AQ_AUD"
#define AUDIO_DAL_VERSION	0x00030001

enum {
	AUDIO_OP_CONTROL = DAL_OP_FIRST_DEVICE_API,
	AUDIO_OP_DATA,
	AUDIO_OP_INIT,
};

/*                                   */

/*                                                                   */
/*                                                                 */
/*                                                                   */
/*                         */
#define ADSP_AUDIO_BUFFER_FLAG_SYNC_POINT        0x01

/*                                                                    */
/*                                               */
#define ADSP_AUDIO_BUFFER_FLAG_PHYS_ADDR         0x04

/*                                                                    */
/*                   */
#define ADSP_AUDIO_BUFFER_FLAG_START_SET         0x08

/*                                                                       */
/*               */
#define ADSP_AUDIO_BUFFER_FLAG_STOP_SET          0x10

/*                                                                    */
/*               */
#define ADSP_AUDIO_BUFFER_FLAG_PREROLL_SET       0x20

/*                                                                           */
/*                                                                       */
/*                         */
#define ADSP_AUDIO_BUFFER_FLAG_CONTINUATION      0x40

struct adsp_audio_buffer {
	u32 addr;		/*                            */
	u32 max_size;		/*                        */
	u32 actual_size;	/*                                         */
	u32 offset;		/*                                */
	u32 flags;		/*                                           */
	s64 start;		/*                         */
	s64 stop;		/*                        */
	s64 preroll;		/*                           */
} __attribute__ ((packed));



/*                          */

/*                              */
#define ADSP_AUDIO_RESPONSE_COMMAND   0
#define ADSP_AUDIO_RESPONSE_ASYNC     1

struct adsp_command_hdr {
	u32 size;		/*                           */

	u32 dest;
	u32 src;
	u32 opcode;
	u32 response_type;
	u32 seq_number;

	u32 context;		/*               */
	u32 data;
	u32 padding;
} __attribute__ ((packed));


#define DOMAIN_APP	0
#define DOMAIN_MODEM	1
#define DOMAIN_DSP	2


/*                                                            */
#define AUDIO_ADDR(dmn, maj, min) (((maj & 0xff) << 16) \
		| ((min & 0xff) << 24) | (dmn & 0xff))

/*                   */
#define ADSP_AUDIO_ENC_AAC_LC_ONLY_MODE		0
#define ADSP_AUDIO_ENC_AAC_PLUS_MODE		1
#define ADSP_AUDIO_ENC_ENHANCED_AAC_PLUS_MODE	2

struct adsp_audio_aac_enc_cfg {
	u32 bit_rate;		/*                 */
	u32 encoder_mode;	/*                  */
} __attribute__ ((packed));

#define ADSP_AUDIO_ENC_SBC_ALLOCATION_METHOD_LOUNDNESS     0
#define ADSP_AUDIO_ENC_SBC_ALLOCATION_METHOD_SNR           1

#define ADSP_AUDIO_ENC_SBC_CHANNEL_MODE_MONO                1
#define ADSP_AUDIO_ENC_SBC_CHANNEL_MODE_STEREO              2
#define ADSP_AUDIO_ENC_SBC_CHANNEL_MODE_DUAL                8
#define ADSP_AUDIO_ENC_SBC_CHANNEL_MODE_JOINT_STEREO        9

struct adsp_audio_sbc_encoder_cfg {
	u32 num_subbands;
	u32 block_len;
	u32 channel_mode;
	u32 allocation_method;
	u32 bit_rate;
} __attribute__ ((packed));

/*                      */
#define ADSP_AUDIO_AMR_MR475	0
#define ADSP_AUDIO_AMR_MR515	1
#define ADSP_AUDIO_AMR_MMR59	2
#define ADSP_AUDIO_AMR_MMR67	3
#define ADSP_AUDIO_AMR_MMR74	4
#define ADSP_AUDIO_AMR_MMR795	5
#define ADSP_AUDIO_AMR_MMR102	6
#define ADSP_AUDIO_AMR_MMR122	7

/*                                          */
#define ADSP_AUDIO_AMR_DTX_MODE_OFF		0
#define ADSP_AUDIO_AMR_DTX_MODE_ON_VAD1		1
#define ADSP_AUDIO_AMR_DTX_MODE_ON_VAD2		2
#define ADSP_AUDIO_AMR_DTX_MODE_ON_AUTO		3

/*                           */
struct adsp_audio_amr_enc_cfg {
	u32	mode;		/*                    */
	u32	dtx_mode;	/*                          */
	u32	enable;		/*                         */
} __attribute__ ((packed));

struct adsp_audio_qcelp13k_enc_cfg {
	u16	min_rate;
	u16	max_rate;
} __attribute__ ((packed));

struct adsp_audio_evrc_enc_cfg {
	u16	min_rate;
	u16	max_rate;
} __attribute__ ((packed));

union adsp_audio_codec_config {
	struct adsp_audio_amr_enc_cfg amr;
	struct adsp_audio_aac_enc_cfg aac;
	struct adsp_audio_qcelp13k_enc_cfg qcelp13k;
	struct adsp_audio_evrc_enc_cfg evrc;
	struct adsp_audio_sbc_encoder_cfg sbc;
} __attribute__ ((packed));


/*                            */
#define ADSP_AUDIO_OPEN_STREAM_MODE_NONE		0x0000

/*                                                                */
#define ADSP_AUDIO_OPEN_STREAM_MODE_AVSYNC		0x0001

/*                                                               */
/*                                        */
#define ADSP_AUDIO_OPEN_STREAM_MODE_SR_CM_NOTIFY	0x0002

#define  ADSP_AUDIO_OPEN_STREAM_MODE_ENABLE_SYNC_CLOCK	0x0004

#define ADSP_AUDIO_MAX_DEVICES 1

struct adsp_open_command {
	struct adsp_command_hdr hdr;
	u32 device;
	u32 end_point;
	u32 stream_context;
	u32 mode;
	u32 buf_max_size;
	union adsp_audio_format format_block;
	union adsp_audio_codec_config config;

} __attribute__ ((packed));


/*                                                  */

/*                                                         */
#define ADSP_AUDIO_IOCTL_CMD_OPEN_READ			0x0108dd79

/*                                                        */
#define ADSP_AUDIO_IOCTL_CMD_OPEN_WRITE			0x0108dd7a

/*                                                     */
#define ADSP_AUDIO_IOCTL_CMD_OPEN_DEVICE		0x0108dd7b

/*                                    */
#define ADSP_AUDIO_IOCTL_CMD_CLOSE			0x0108d8bc



/*                                      */
/*                                                              */

/*                                                          */
/*                       */

/*                                                      */
/*                                      */
#define ADSP_AUDIO_IOCTL_CMD_DEVICE_SWITCH_PREPARE	0x010815c4

/*                                                     */
/*                                                            */

/*                                                         */
/*                                                          */
/*                                           */
#define ADSP_AUDIO_IOCTL_CMD_DEVICE_SWITCH_STANDBY	0x010815c5

/*                                                                       */
#define ADSP_AUDIO_IOCTL_CMD_DEVICE_SWITCH_COMMIT	0x01075ee7

struct adsp_device_switch_command {
	struct adsp_command_hdr hdr;
	u32 old_device;
	u32 new_device;
	u8 device_class; /*                                        */
	u8 device_type; /*                          */
} __attribute__ ((packed));



/*                                       */

#define ADSP_PATH_RX	0
#define ADSP_PATH_TX	1
#define ADSP_PATH_BOTH	2

/*                                                                    */
/*          */


/*                    */
#define ADSP_AUDIO_IOCTL_CMD_SET_DEVICE_VOL		0x0107605c

struct adsp_set_dev_volume_command {
	struct adsp_command_hdr hdr;
	u32 device_id;
	u32 path; /*                          */
	s32 volume;
} __attribute__ ((packed));

/*                                                          */
/*                                                  */
#define ADSP_AUDIO_IOCTL_SET_DEVICE_STEREO_VOL		0x0108df3e

/*                                                            */
/*                                                              */
#define ADSP_AUDIO_IOCTL_SET_DEVICE_XCHAN_GAIN		0x0108df40

/*                        */
#define ADSP_AUDIO_IOCTL_CMD_SET_DEVICE_MUTE		0x0107605f

struct adsp_set_dev_mute_command {
	struct adsp_command_hdr hdr;
	u32 device_id;
	u32 path; /*                          */
	u32 mute; /*          */
} __attribute__ ((packed));

/*                                   */
/*                                                                       */
#define ADSP_AUDIO_IOCTL_CMD_SET_DEVICE_EQ_CONFIG	0x0108b10e

/*                                                             */
/*                                                                 */
#define ADSP_AUDIO_IOCTL_SET_DEVICE_CONFIG		0x0108b6cb

struct adsp_set_dev_cfg_command {
	struct adsp_command_hdr hdr;
	u32 device_id;
	u32 block_id;
	u32 interface_id;
	u32 phys_addr;
	u32 phys_size;
	u32 phys_used;
} __attribute__ ((packed));

/*                                                        */
#define ADSP_AUDIO_IOCTL_SET_DEVICE_CONFIG_TABLE	0x0108b6bf

struct adsp_set_dev_cfg_table_command {
	struct adsp_command_hdr hdr;
	u32 device_id;
	u32 phys_addr;
	u32 phys_size;
	u32 phys_used;
} __attribute__ ((packed));

/*                                      */

#define ADSP_AUDIO_IOCTL_CMD_DATA_TX			0x0108dd7f
#define ADSP_AUDIO_IOCTL_CMD_DATA_RX			0x0108dd80

struct adsp_buffer_command {
	struct adsp_command_hdr hdr;
	struct adsp_audio_buffer buffer;
} __attribute__ ((packed));



/*                                                                          */

/*                               */
#define ADSP_AUDIO_IOCTL_CMD_STREAM_STOP		0x01075c54

/*                                                            */
#define ADSP_AUDIO_IOCTL_CMD_STREAM_EOS			0x0108b150

/*                                                            */
/*                                                        */
#define ADSP_AUDIO_IOCTL_CMD_STREAM_SLIPSAMPLE		0x0108d40e

/*                    */
/*                                                                      */
#define ADSP_AUDIO_IOCTL_CMD_SET_STREAM_VOL		0x0108c0de

/*                                                          */
/*                                              */
#define ADSP_AUDIO_IOCTL_SET_STREAM_STEREO_VOL		0x0108dd7c

/*                                                            */
/*                                                          */
#define ADSP_AUDIO_IOCTL_SET_STREAM_XCHAN_GAIN		0x0108dd7d

/*                        */
/*                                                                   */
#define ADSP_AUDIO_IOCTL_CMD_SET_STREAM_MUTE		0x0108c0df

/*                                                         */
/*                                                 */
#define ADSP_AUDIO_IOCTL_SET_STREAM_BITRATE		0x0108ccf1

/*                                                            */
/*                                                                          */
#define ADSP_AUDIO_IOCTL_SET_STREAM_CHANNELMAP		0x0108d32a

/*                             */
/*                                                                 */
#define ADSP_AUDIO_IOCTL_SET_STREAM_SBR			0x0108d416

/*                                                                    */
/*                                           */
#define ADSP_AUDIO_IOCTL_SET_STREAM_WMAPRO		0x0108d417


/*                                                                 */

/*                                */
#define ADSP_AUDIO_IOCTL_CMD_SESSION_START		0x010815c6

/*                                                                */
#define ADSP_AUDIO_IOCTL_CMD_SESSION_STOP		0x0108dd7e

/*                                                             */
#define ADSP_AUDIO_IOCTL_CMD_SESSION_PAUSE		0x01075ee8

/*                                                              */
#define ADSP_AUDIO_IOCTL_CMD_SESSION_RESUME		0x01075ee9

/*                                                                           */
#define ADSP_AUDIO_IOCTL_CMD_SESSION_FLUSH		0x01075eea

/*                        */
#define ADSP_AUDIO_IOCTL_CMD_SESSION_DTMF_START		0x0108c0dd

/*                       */
#define ADSP_AUDIO_IOCTL_CMD_SESSION_DTMF_STOP		0x01087554

/*                     */
/*                                                                      */
#define ADSP_AUDIO_IOCTL_SET_SESSION_VOL		0x0108d8bd

/*                                                           */
/*                                              */
#define ADSP_AUDIO_IOCTL_SET_SESSION_STEREO_VOL		0x0108df3d

/*                                                             */
/*                                                          */
#define ADSP_AUDIO_IOCTL_SET_SESSION_XCHAN_GAIN		0x0108df3f

/*                         */
/*                                                                    */
#define ADSP_AUDIO_IOCTL_SET_SESSION_MUTE		0x0108d8be

/*                                   */
/*                                                                   */
#define ADSP_AUDIO_IOCTL_SET_SESSION_EQ_CONFIG		0x0108c0e0

/*                                   */
/*                                                                       */
#define ADSP_AUDIO_IOCTL_SET_SESSION_AVSYNC		0x0108d1e2

/*                               */
/*                                                                   */
#define ADSP_AUDIO_IOCTL_CMD_GET_AUDIO_TIME		0x0108c26c


/*                                                                      */

struct adsp_set_volume_command {
	struct adsp_command_hdr hdr;
	s32 volume;
} __attribute__ ((packed));

struct adsp_set_mute_command {
	struct adsp_command_hdr hdr;
	u32 mute; /*           */
} __attribute__ ((packed));



/*                        */

/*                                                                   */
/*                                                     */

/*                                                                 */
#define ADSP_AUDIO_EVT_STATUS_OPEN				0x0108c0d6

/*                                                                  */
#define ADSP_AUDIO_EVT_STATUS_CLOSE				0x0108c0d7

/*                                                                 */
/*                                                               */
/*                    */
#define ADSP_AUDIO_EVT_STATUS_BUF_DONE				0x0108c0d8

/*                                                                  */
/*                                                                  */
/*                                                                     */
#define ADSP_AUDIO_EVT_STATUS_BUF_UNDERRUN			0x0108c0d9

/*                                                                    */
/*                                                   */
#define ADSP_AUDIO_EVT_STATUS_BUF_OVERFLOW			0x0108c0da

/*                                                              */
/*                                                               */
/*                                                         */
/*                                                           */
#define ADSP_AUDIO_EVT_SR_CM_CHANGE				0x0108d329

struct adsp_event_hdr {
	u32 evt_handle;		/*                   */
	u32 evt_cookie;
	u32 evt_length;

	u32 dest;
	u32 src;

	u32 event_id;
	u32 response_type;
	u32 seq_number;

	u32 context;		/*               */
	u32 data;

	u32 status;
} __attribute__ ((packed));

struct adsp_buffer_event {
	struct adsp_event_hdr hdr;
	struct adsp_audio_buffer buffer;
} __attribute__ ((packed));


/*                            */

/*                             */
#define ADSP_AUDIO_RX_DEVICE		0x00
#define ADSP_AUDIO_TX_DEVICE		0x01

#define ADSP_AUDIO_DEVICE_ID_DEFAULT		0x1081679

/*                         */

#define ADSP_AUDIO_DEVICE_ID_HANDSET_MIC	0x107ac8d
#define ADSP_AUDIO_DEVICE_ID_HANDSET_DUAL_MIC		0x108f9c3
#define ADSP_AUDIO_DEVICE_ID_HEADSET_MIC	0x1081510
#define ADSP_AUDIO_DEVICE_ID_SPKR_PHONE_MIC	0x1081512
#define ADSP_AUDIO_DEVICE_ID_SPKR_PHONE_DUAL_MIC	0x108f9c5
#define ADSP_AUDIO_DEVICE_ID_BT_SCO_MIC		0x1081518
#define ADSP_AUDIO_DEVICE_ID_TTY_HEADSET_MIC	0x108151b
#define ADSP_AUDIO_DEVICE_ID_I2S_MIC		0x1089bf3

/*                                                               */
/*                                                       */
#define ADSP_AUDIO_DEVICE_ID_MIXED_PCM_LOOPBACK_TX	0x1089bf2

/*                   */
#define ADSP_AUDIO_DEVICE_ID_HANDSET_SPKR			0x107ac88
#define ADSP_AUDIO_DEVICE_ID_HEADSET_SPKR_MONO			0x1081511
#define ADSP_AUDIO_DEVICE_ID_HEADSET_SPKR_STEREO		0x107ac8a
#define ADSP_AUDIO_DEVICE_ID_SPKR_PHONE_MONO			0x1081513
#define ADSP_AUDIO_DEVICE_ID_SPKR_PHONE_MONO_W_MONO_HEADSET     0x108c508
#define ADSP_AUDIO_DEVICE_ID_SPKR_PHONE_MONO_W_STEREO_HEADSET   0x108c894
#define ADSP_AUDIO_DEVICE_ID_SPKR_PHONE_STEREO			0x1081514
#define ADSP_AUDIO_DEVICE_ID_SPKR_PHONE_STEREO_W_MONO_HEADSET   0x108c895
#define ADSP_AUDIO_DEVICE_ID_SPKR_PHONE_STEREO_W_STEREO_HEADSET	0x108c509
#define ADSP_AUDIO_DEVICE_ID_BT_SCO_SPKR			0x1081519
#define ADSP_AUDIO_DEVICE_ID_TTY_HEADSET_SPKR			0x108151c
#define ADSP_AUDIO_DEVICE_ID_I2S_SPKR				0x1089bf4
#define ADSP_AUDIO_DEVICE_ID_NULL_SINK				0x108e512

/*                          */
/*                                 */
/*                                                   */
/*                                                 */
#define ADSP_AUDIO_DEVICE_ID_BT_A2DP_SPKR	0x108151a

/*                                                      */
/*                                                          */
#define ADSP_AUDIO_DEVICE_ID_VOICE		0x0108df3c

/*                            */
/*                                                               */
/*                */
#define ADSP_AUDIO_DEVICE_CONTEXT_VOICE			0x01
#define ADSP_AUDIO_DEVICE_CONTEXT_PLAYBACK		0x02
#define ADSP_AUDIO_DEVICE_CONTEXT_MIXED_RECORD		0x10
#define ADSP_AUDIO_DEVICE_CONTEXT_RECORD		0x20
#define ADSP_AUDIO_DEVICE_CONTEXT_PCM_LOOPBACK		0x40

#endif
