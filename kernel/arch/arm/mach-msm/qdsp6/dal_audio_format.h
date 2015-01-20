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

#ifndef __ADSP_AUDIO_MEDIA_FORMAT_H
#define __ADSP_AUDIO_MEDIA_FORMAT_H



/*                               */

/*                       */
#define ADSP_AUDIO_FORMAT_SHAREDMEMORY	0x01091a78
/*                                */
#define ADSP_AUDIO_FORMAT_PCM		0x0103d2fd
/*                                */
#define ADSP_AUDIO_FORMAT_DTMF		0x01087725
/*                              */
#define ADSP_AUDIO_FORMAT_ADPCM		0x0103d2ff
/*                   */
#define ADSP_AUDIO_FORMAT_YADPCM	0x0108dc07
/*               */
#define ADSP_AUDIO_FORMAT_MP3		0x0103d308
/*               */
#define ADSP_AUDIO_FORMAT_MPEG4_AAC	0x010422f1
/*                           */
#define ADSP_AUDIO_FORMAT_AMRNB_FS	0x0105c16c
/*                           */
#define ADSP_AUDIO_FORMAT_AMRWB_FS	0x0105c16e
/*                  */
#define ADSP_AUDIO_FORMAT_V13K_FS	0x01080b8a
/*                  */
#define ADSP_AUDIO_FORMAT_EVRC_FS	0x01080b89
/*                  */
#define ADSP_AUDIO_FORMAT_EVRCB_FS	0x0108f2a3
/*                     */
#define ADSP_AUDIO_FORMAT_MIDI		0x0103d300
/*                 */
#define ADSP_AUDIO_FORMAT_SBC		0x0108c4d8
/*                         */
#define ADSP_AUDIO_FORMAT_WMA_V10PRO	0x0108aa92
/*                     */
#define ADSP_AUDIO_FORMAT_WMA_V9	0x0108d430
/*                   */
#define ADSP_AUDIO_FORMAT_AMR_WB_PLUS	0x0108f3da
/*             */
#define ADSP_AUDIO_FORMAT_AC3_DECODER	0x0108d5f9


/*                                       */



/*               */
#define ADSP_AUDIO_FORMAT_MPEG2_AAC	0x0103d309
/*                        */
#define ADSP_AUDIO_FORMAT_AMRNB_IF1	0x0103d305
/*                        */
#define ADSP_AUDIO_FORMAT_AMRNB_IF2	0x01057b31
/*                */
#define ADSP_AUDIO_FORMAT_AMRWB_IF1	0x0103d306
/*                */
#define ADSP_AUDIO_FORMAT_AMRWB_IF2	0x0105c16d
/*       */
#define ADSP_AUDIO_FORMAT_G711		0x0106201d
/*                  */
#define ADSP_AUDIO_FORMAT_V8K_FS	0x01081d29
/*                 */
#define ADSP_AUDIO_FORMAT_WMA_V1	0x01055b2b
/*                        */
#define ADSP_AUDIO_FORMAT_WMA_V8	0x01055b2c
/*                              */
#define ADSP_AUDIO_FORMAT_WMA_V9PRO	0x01055b2d
/*                       */
#define ADSP_AUDIO_FORMAT_WMA_SP1	0x01055b2e
/*                          */
#define ADSP_AUDIO_FORMAT_WMA_LOSSLESS	0x01055b2f
/*                                 */
#define ADSP_AUDIO_FORMAT_RA_SIPR	0x01042a0f
/*                    */
#define ADSP_AUDIO_FORMAT_RA_COOK	0x01042a0e


/*                                                           */
/*                      */
/*                                                                    */
/*                                                                */
/*             */



/*                   */

/*                                                             */
/*                                                            */

/*                                                    */
#define ADSP_AUDIO_AAC_ADTS		0x010619cf
#define ADSP_AUDIO_AAC_MPEG4_ADTS	0x010619d0
#define ADSP_AUDIO_AAC_LOAS		0x010619d1
#define ADSP_AUDIO_AAC_ADIF		0x010619d2
#define ADSP_AUDIO_AAC_RAW		0x010619d3
#define ADSP_AUDIO_AAC_FRAMED_RAW	0x0108c1fb


#define ADSP_AUDIO_COMPANDING_ALAW	0x10619cd
#define ADSP_AUDIO_COMPANDING_MLAW	0x10619ce

/*                                                  */
#define ADSP_AUDIO_FORMAT_DATA_MAX 16


struct adsp_audio_no_payload_format {
	/*                                                  */
	u32		format;

	/*                                 */
} __attribute__ ((packed));


/*                                                        */
/*                                                               */
/*                     */
struct adsp_audio_standard_format {
	/*                                                  */
	u32		format;

	/*         */
	u16		channels;
	u16		bits_per_sample;
	u32		sampling_rate;
	u8		is_signed;
	u8		is_interleaved;
} __attribute__ ((packed));



/*                    */
struct adsp_audio_adpcm_format {
	/*                                                  */
	u32		format;

	/*         */
	u16		channels;
	u16		bits_per_sample;
	u32		sampling_rate;
	u8		is_signed;
	u8		is_interleaved;
	u32		block_size;
} __attribute__ ((packed));


/*                   */
struct adsp_audio_midi_format {
	/*                                                  */
	u32		format;

	/*         */
	u32		sampling_rate;
	u16		channels;
	u16		mode;
} __attribute__ ((packed));


/*                   */
struct adsp_audio_g711_format {
	/*                                                  */
	u32		format;

	/*         */
	u32		companding;
} __attribute__ ((packed));


struct adsp_audio_wma_pro_format {
	/*                                                  */
	u32		format;

	/*         */
	u16		format_tag;
	u16		channels;
	u32		samples_per_sec;
	u32		avg_bytes_per_sec;
	u16		block_align;
	u16		valid_bits_per_sample;
	u32		channel_mask;
	u16		encode_opt;
	u16		advanced_encode_opt;
	u32		advanced_encode_opt2;
	u32		drc_peak_reference;
	u32		drc_peak_target;
	u32		drc_average_reference;
	u32		drc_average_target;
} __attribute__ ((packed));


struct adsp_audio_amrwb_plus_format {
	/*                                                  */
	u32		format;

	/*         */
	u32		size;
	u32		version;
	u32		channels;
	u32		amr_band_mode;
	u32		amr_dtx_mode;
	u32		amr_frame_format;
	u32		amr_isf_index;
} __attribute__ ((packed));


/*                           */
/*                                                */
/*                                             */
struct adsp_audio_binary_format {
	/*                                                  */
	u32		format;

	/*         */
	/*                                    */
	u32		num_bytes;
	/*                         */
	u8		data[ADSP_AUDIO_FORMAT_DATA_MAX];
} __attribute__ ((packed));


struct adsp_audio_shared_memory_format {
	/*                                                  */
	u32		format;

	/*                                  */
	u32		len;
	/*                                            */
	u32		address;
} __attribute__ ((packed));


/*                           */
union adsp_audio_format {
	/*                                    */
	struct adsp_audio_no_payload_format	no_payload;
	/*                                */
	struct adsp_audio_standard_format	standard;
	/*                    */
	struct adsp_audio_adpcm_format		adpcm;
	/*                   */
	struct adsp_audio_midi_format		midi;
	/*                   */
	struct adsp_audio_g711_format		g711;
	/*                     */
	struct adsp_audio_wma_pro_format	wma_pro;
	/*                     */
	struct adsp_audio_amrwb_plus_format	amrwb_plus;
	/*                                                 */
	struct adsp_audio_binary_format		binary;
	/*                               */
	struct adsp_audio_shared_memory_format	shared_mem;
};

#endif

