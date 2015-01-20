/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
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

#include "adreno.h"
#include "a4xx_reg.h"
#include "adreno_a3xx.h"

/*
                                                                
                                                               
                                 
 */

const unsigned int a4xx_registers[] = {
	0x0000, 0x0002, /*                                         */
	0x0020, 0x0020, /*                */
	0x0021, 0x0021, /*                  */
	0x0023, 0x0024, /*                               */
	0x0026, 0x0026, /*                           */
	0x0028, 0x0034, /*                                                    */
	0x0037, 0x003f, /*                                      */
	0x0041, 0x0045, /*                                          */
	0x0047, 0x0049, /*                                            */
	0x009c, 0x0170, /*                                         */
	0x0174, 0x0182, /*                                                */
	0x0189, 0x019f, /*                                               */

	0x0206, 0x0217, /*                                  */
	0x0219, 0x0219, /*                  */
	0x021b, 0x021b, /*                */
	0x0228, 0x0229, /*                                    */
	0x022a, 0x022c, /*                      */
	0x022e, 0x022e, /*          */
	0x0231, 0x0232, /*                                           */
	0x0240, 0x0250, /*                                    */
	0x04c0, 0x04ce, /*                           */
	0x04d0, 0x04d0, /*                */
	0x04d2, 0x04dd, /*                                       */
	0x0500, 0x050b, /*                                              */
	0x0578, 0x058f, /*                                    */

	0x0c00, 0x0c03, /*                                      */
	0x0c08, 0x0c41, /*                                             */
	0x0c50, 0x0c51, /*                                               */

	0x0e64, 0x0e68, /*                                               */
	0x2140, 0x216e, /*                                        */
	0x2540, 0x256e, /*                                        */

	0x0f00, 0x0f0b, /*                                                */
	/*                                                            */
	0x2380, 0x23a6,
	/*                                                            */
	0x2780, 0x27a6,

	0x0ec0, 0x0ecf, /*                                     */
	0x22c0, 0x22c1, /*                                         */
	0x22c4, 0x2360, /*                                    */
	0x26c0, 0x26c1, /*                                         */
	0x26c4, 0x2760, /*                                    */

	0x0cc0, 0x0cd2, /*                                          */
	0x20a0, 0x213f, /*                                                  */
	0x24a0, 0x253f, /*                                                  */

	0x0e40, 0x0e4a, /*                                           */
	0x2200, 0x2204, /*                                       */
	0x2208, 0x22a9, /*                                                */
	0x2600, 0x2604, /*                                       */
	0x2608, 0x26a9, /*                                                */

	0x0c80, 0x0c81, /*                                          */
	0x0c88, 0x0c8b, /*                                                 */
	0x2000, 0x2004, /*                                                 */
	/*                                                            */
	0x2008, 0x209f,
	0x2400, 0x2404, /*                                                 */
	/*                                                            */
	0x2408, 0x249f,

	0x0e80, 0x0e84, /*                                             */
	0x0e88, 0x0e95, /*                                             */

	0x0e00, 0x0e00, /*                                                 */
	0x0e04, 0x0e0e, /*                                              */
	0x23c0, 0x23db, /*                                              */
	0x27c0, 0x27db, /*                                              */

	0x0d00, 0x0d0c, /*                                                 */
	0x0d10, 0x0d17, /*                                           */
	0x21c0, 0x21c6, /*                                        */
	0x21e5, 0x21e7, /*                                   */
	0x25c0, 0x25c6, /*                                        */
	0x25e5, 0x25e7, /*                                   */
};

const unsigned int a4xx_registers_count = ARRAY_SIZE(a4xx_registers) / 2;

static const struct adreno_vbif_data a420_vbif[] = {
	{ A4XX_VBIF_ABIT_SORT, 0x0001001F },
	{ A4XX_VBIF_ABIT_SORT_CONF, 0x000000A4 },
	{ A4XX_VBIF_GATE_OFF_WRREQ_EN, 0x00000001 },
	{ A4XX_VBIF_IN_RD_LIM_CONF0, 0x18181818 },
	{ A4XX_VBIF_IN_RD_LIM_CONF1, 0x00000018 },
	{ A4XX_VBIF_IN_WR_LIM_CONF0, 0x18181818 },
	{ A4XX_VBIF_IN_WR_LIM_CONF1, 0x00000018 },
	{ A4XX_VBIF_ROUND_ROBIN_QOS_ARB, 0x00000003 },
	{0, 0},
};

const struct adreno_vbif_platform a4xx_vbif_platforms[] = {
	{ adreno_is_a420, a420_vbif },
};

static void a4xx_start(struct adreno_device *adreno_dev)
{
	struct kgsl_device *device = &adreno_dev->dev;

	adreno_vbif_start(device, a4xx_vbif_platforms,
			ARRAY_SIZE(a4xx_vbif_platforms));
	/*                                                         */
	kgsl_regwrite(device, A4XX_RBBM_GPU_BUSY_MASKED, 0xFFFFFFFF);

	/*                                                          */
	kgsl_regwrite(device, A4XX_RBBM_SP_HYST_CNT, 0x10);
	kgsl_regwrite(device, A4XX_RBBM_WAIT_IDLE_CLOCKS_CTL, 0x10);

	/*
                                                           
                                 
  */

	kgsl_regwrite(device, A4XX_RBBM_AHB_CTL0, 0x00000001);

	/*                            */
	kgsl_regwrite(device, A4XX_RBBM_AHB_CTL1, 0xA6FFFFFF);

	/*
                                                                   
                                     
  */

	kgsl_regwrite(device, A4XX_RBBM_INTERFACE_HANG_INT_CTL,
			(1 << 16) | 0xFFF);

	/*                                     */
	kgsl_regwrite(device, A4XX_RB_GMEM_BASE_ADDR,
			(unsigned int)(adreno_dev->ocmem_base >> 14));
}

/*                                                                */
static unsigned int a4xx_register_offsets[ADRENO_REG_REGISTER_MAX] = {
	ADRENO_REG_DEFINE(ADRENO_REG_CP_DEBUG, A4XX_CP_DEBUG),
	ADRENO_REG_DEFINE(ADRENO_REG_CP_ME_RAM_WADDR, A4XX_CP_ME_RAM_WADDR),
	ADRENO_REG_DEFINE(ADRENO_REG_CP_ME_RAM_DATA, A4XX_CP_ME_RAM_DATA),
	ADRENO_REG_DEFINE(ADRENO_REG_CP_PFP_UCODE_DATA, A4XX_CP_PFP_UCODE_DATA),
	ADRENO_REG_DEFINE(ADRENO_REG_CP_PFP_UCODE_ADDR, A4XX_CP_PFP_UCODE_ADDR),
	ADRENO_REG_DEFINE(ADRENO_REG_CP_WFI_PEND_CTR, A4XX_CP_WFI_PEND_CTR),
	ADRENO_REG_DEFINE(ADRENO_REG_CP_RB_BASE, A4XX_CP_RB_BASE),
	ADRENO_REG_DEFINE(ADRENO_REG_CP_RB_RPTR_ADDR, A4XX_CP_RB_RPTR_ADDR),
	ADRENO_REG_DEFINE(ADRENO_REG_CP_RB_RPTR, A4XX_CP_RB_RPTR),
	ADRENO_REG_DEFINE(ADRENO_REG_CP_RB_WPTR, A4XX_CP_RB_WPTR),
	ADRENO_REG_DEFINE(ADRENO_REG_CP_PROTECT_CTRL, A4XX_CP_PROTECT_CTRL),
	ADRENO_REG_DEFINE(ADRENO_REG_CP_ME_CNTL, A4XX_CP_ME_CNTL),
	ADRENO_REG_DEFINE(ADRENO_REG_CP_RB_CNTL, A4XX_CP_RB_CNTL),
	ADRENO_REG_DEFINE(ADRENO_REG_CP_IB1_BASE, A4XX_CP_IB1_BASE),
	ADRENO_REG_DEFINE(ADRENO_REG_CP_IB1_BUFSZ, A4XX_CP_IB1_BUFSZ),
	ADRENO_REG_DEFINE(ADRENO_REG_CP_IB2_BASE, A4XX_CP_IB2_BASE),
	ADRENO_REG_DEFINE(ADRENO_REG_CP_IB2_BUFSZ, A4XX_CP_IB2_BUFSZ),
	ADRENO_REG_DEFINE(ADRENO_REG_CP_TIMESTAMP, A4XX_CP_SCRATCH_REG0),
	ADRENO_REG_DEFINE(ADRENO_REG_CP_ME_RAM_RADDR, A4XX_CP_ME_RAM_RADDR),
	ADRENO_REG_DEFINE(ADRENO_REG_SCRATCH_ADDR, A4XX_CP_SCRATCH_ADDR),
	ADRENO_REG_DEFINE(ADRENO_REG_SCRATCH_UMSK, A4XX_CP_SCRATCH_UMASK),
	ADRENO_REG_DEFINE(ADRENO_REG_RBBM_STATUS, A4XX_RBBM_STATUS),
	ADRENO_REG_DEFINE(ADRENO_REG_RBBM_PERFCTR_CTL, A4XX_RBBM_PERFCTR_CTL),
	ADRENO_REG_DEFINE(ADRENO_REG_RBBM_PERFCTR_LOAD_CMD0,
					A4XX_RBBM_PERFCTR_LOAD_CMD0),
	ADRENO_REG_DEFINE(ADRENO_REG_RBBM_PERFCTR_LOAD_CMD1,
					A4XX_RBBM_PERFCTR_LOAD_CMD1),
	ADRENO_REG_DEFINE(ADRENO_REG_RBBM_PERFCTR_LOAD_CMD2,
				A4XX_RBBM_PERFCTR_LOAD_CMD2),
	ADRENO_REG_DEFINE(ADRENO_REG_RBBM_PERFCTR_PWR_1_LO,
					A4XX_RBBM_PERFCTR_PWR_1_LO),
	ADRENO_REG_DEFINE(ADRENO_REG_RBBM_INT_0_MASK, A4XX_RBBM_INT_0_MASK),
	ADRENO_REG_DEFINE(ADRENO_REG_RBBM_INT_0_STATUS, A4XX_RBBM_INT_0_STATUS),
	ADRENO_REG_DEFINE(ADRENO_REG_RBBM_AHB_ERROR_STATUS,
					A4XX_RBBM_AHB_ERROR_STATUS),
	ADRENO_REG_DEFINE(ADRENO_REG_RBBM_AHB_CMD, A4XX_RBBM_AHB_CMD),
	ADRENO_REG_DEFINE(ADRENO_REG_VPC_DEBUG_RAM_SEL,
					A4XX_VPC_DEBUG_RAM_SEL),
	ADRENO_REG_DEFINE(ADRENO_REG_VPC_DEBUG_RAM_READ,
					A4XX_VPC_DEBUG_RAM_READ),
	ADRENO_REG_DEFINE(ADRENO_REG_RBBM_INT_CLEAR_CMD,
				A4XX_RBBM_INT_CLEAR_CMD),
	ADRENO_REG_DEFINE(ADRENO_REG_VSC_PIPE_DATA_ADDRESS_0,
				A4XX_VSC_PIPE_DATA_ADDRESS_0),
	ADRENO_REG_DEFINE(ADRENO_REG_VSC_PIPE_DATA_LENGTH_7,
					A4XX_VSC_PIPE_DATA_LENGTH_7),
	ADRENO_REG_DEFINE(ADRENO_REG_VSC_SIZE_ADDRESS, A4XX_VSC_SIZE_ADDRESS),
	ADRENO_REG_DEFINE(ADRENO_REG_VFD_CONTROL_0, A4XX_VFD_CONTROL_0),
	ADRENO_REG_DEFINE(ADRENO_REG_VFD_FETCH_INSTR_0_0,
					A4XX_VFD_FETCH_INSTR_0_0),
	ADRENO_REG_DEFINE(ADRENO_REG_VFD_FETCH_INSTR_1_F,
					A4XX_VFD_FETCH_INSTR_1_31),
	ADRENO_REG_DEFINE(ADRENO_REG_SP_VS_PVT_MEM_ADDR_REG,
				A4XX_SP_VS_PVT_MEM_ADDR),
	ADRENO_REG_DEFINE(ADRENO_REG_SP_FS_PVT_MEM_ADDR_REG,
				A4XX_SP_FS_PVT_MEM_ADDR),
	ADRENO_REG_DEFINE(ADRENO_REG_SP_VS_OBJ_START_REG,
				A4XX_SP_VS_OBJ_START),
	ADRENO_REG_DEFINE(ADRENO_REG_SP_FS_OBJ_START_REG,
				A4XX_SP_FS_OBJ_START),
};

const struct adreno_reg_offsets a4xx_reg_offsets = {
	.offsets = a4xx_register_offsets,
	.offset_0 = ADRENO_REG_REGISTER_MAX,
};

struct adreno_gpudev adreno_a4xx_gpudev = {
	.reg_offsets = &a4xx_reg_offsets,

	.rb_init = a3xx_rb_init,
	.irq_control = a3xx_irq_control,
	.irq_handler = a3xx_irq_handler,
	.irq_pending = a3xx_irq_pending,
	.busy_cycles = a3xx_busy_cycles,
	.start = a4xx_start,
};
