/* Copyright (c) 2010-2011 The Linux Foundation. All rights reserved.
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

#ifndef __ASM_ARCH_MSM_IRQS_8X60_H
#define __ASM_ARCH_MSM_IRQS_8X60_H

/*                            */

/*                                                         
                                             
                                             */

#define GIC_PPI_START 16
#define GIC_SPI_START 32

#define INT_DEBUG_TIMER_EXP			(GIC_PPI_START + 0)
#define INT_GP_TIMER_EXP			(GIC_PPI_START + 1)
#define INT_GP_TIMER2_EXP			(GIC_PPI_START + 2)
#define WDT0_ACCSCSSNBARK_INT			(GIC_PPI_START + 3)
#define WDT1_ACCSCSSNBARK_INT			(GIC_PPI_START + 4)
#define AVS_SVICINT				(GIC_PPI_START + 5)
#define AVS_SVICINTSWDONE			(GIC_PPI_START + 6)
#define CPU_DBGCPUXCOMMRXFULL			(GIC_PPI_START + 7)
#define CPU_DBGCPUXCOMMTXEMPTY			(GIC_PPI_START + 8)
#define INT_ARMQC_PERFMON			(GIC_PPI_START + 9)
#define SC_AVSCPUXDOWN				(GIC_PPI_START + 10)
#define SC_AVSCPUXUP				(GIC_PPI_START + 11)
#define SC_SICCPUXACGIRPTREQ			(GIC_PPI_START + 12)
/*                         */


#define SC_SICMPUIRPTREQ			(GIC_SPI_START + 0)
#define SC_SICL2IRPTREQ				(GIC_SPI_START + 1)
#define SC_SICL2PERFMONIRPTREQ			(GIC_SPI_START + 2)
#define NC					(GIC_SPI_START + 3)
#define TLMM_MSM_DIR_CONN_IRQ_0			(GIC_SPI_START + 4)
#define TLMM_MSM_DIR_CONN_IRQ_1			(GIC_SPI_START + 5)
#define TLMM_MSM_DIR_CONN_IRQ_2			(GIC_SPI_START + 6)
#define TLMM_MSM_DIR_CONN_IRQ_3			(GIC_SPI_START + 7)
#define TLMM_MSM_DIR_CONN_IRQ_4			(GIC_SPI_START + 8)
#define TLMM_MSM_DIR_CONN_IRQ_5			(GIC_SPI_START + 9)
#define TLMM_MSM_DIR_CONN_IRQ_6			(GIC_SPI_START + 10)
#define TLMM_MSM_DIR_CONN_IRQ_7			(GIC_SPI_START + 11)
#define TLMM_MSM_DIR_CONN_IRQ_8			(GIC_SPI_START + 12)
#define TLMM_MSM_DIR_CONN_IRQ_9			(GIC_SPI_START + 13)
#define PM8058_SEC_IRQ_N			(GIC_SPI_START + 14)
#define PM8901_SEC_IRQ_N			(GIC_SPI_START + 15)
#define TLMM_MSM_SUMMARY_IRQ			(GIC_SPI_START + 16)
#define SPDM_RT_1_IRQ				(GIC_SPI_START + 17)
#define SPDM_DIAG_IRQ				(GIC_SPI_START + 18)
#define RPM_SCSS_CPU0_GP_HIGH_IRQ		(GIC_SPI_START + 19)
#define RPM_SCSS_CPU0_GP_MEDIUM_IRQ		(GIC_SPI_START + 20)
#define RPM_SCSS_CPU0_GP_LOW_IRQ		(GIC_SPI_START + 21)
#define RPM_SCSS_CPU0_WAKE_UP_IRQ		(GIC_SPI_START + 22)
#define RPM_SCSS_CPU1_GP_HIGH_IRQ		(GIC_SPI_START + 23)
#define RPM_SCSS_CPU1_GP_MEDIUM_IRQ		(GIC_SPI_START + 24)
#define RPM_SCSS_CPU1_GP_LOW_IRQ		(GIC_SPI_START + 25)
#define RPM_SCSS_CPU1_WAKE_UP_IRQ		(GIC_SPI_START + 26)
#define SSBI2_2_SC_CPU0_SECURE_INT		(GIC_SPI_START + 27)
#define SSBI2_2_SC_CPU0_NON_SECURE_INT		(GIC_SPI_START + 28)
#define SSBI2_1_SC_CPU0_SECURE_INT		(GIC_SPI_START + 29)
#define SSBI2_1_SC_CPU0_NON_SECURE_INT		(GIC_SPI_START + 30)
#define MSMC_SC_SEC_CE_IRQ			(GIC_SPI_START + 31)
#define MSMC_SC_PRI_CE_IRQ			(GIC_SPI_START + 32)
#define MARM_FIQ				(GIC_SPI_START + 33)
#define MARM_IRQ				(GIC_SPI_START + 34)
#define MARM_L2CC_IRQ				(GIC_SPI_START + 35)
#define MARM_WDOG_EXPIRED			(GIC_SPI_START + 36)
#define MARM_SCSS_GP_IRQ_0			(GIC_SPI_START + 37)
#define MARM_SCSS_GP_IRQ_1			(GIC_SPI_START + 38)
#define MARM_SCSS_GP_IRQ_2			(GIC_SPI_START + 39)
#define MARM_SCSS_GP_IRQ_3			(GIC_SPI_START + 40)
#define MARM_SCSS_GP_IRQ_4			(GIC_SPI_START + 41)
#define MARM_SCSS_GP_IRQ_5			(GIC_SPI_START + 42)
#define MARM_SCSS_GP_IRQ_6			(GIC_SPI_START + 43)
#define MARM_SCSS_GP_IRQ_7			(GIC_SPI_START + 44)
#define MARM_SCSS_GP_IRQ_8			(GIC_SPI_START + 45)
#define MARM_SCSS_GP_IRQ_9			(GIC_SPI_START + 46)
#define INT_VPE					(GIC_SPI_START + 47)
#define VFE_IRQ					(GIC_SPI_START + 48)
#define VCODEC_IRQ				(GIC_SPI_START + 49)
#define TV_ENC_IRQ				(GIC_SPI_START + 50)
#define SMMU_VPE_CB_SC_SECURE_IRQ		(GIC_SPI_START + 51)
#define SMMU_VPE_CB_SC_NON_SECURE_IRQ		(GIC_SPI_START + 52)
#define SMMU_VFE_CB_SC_SECURE_IRQ		(GIC_SPI_START + 53)
#define SMMU_VFE_CB_SC_NON_SECURE_IRQ		(GIC_SPI_START + 54)
#define SMMU_VCODEC_B_CB_SC_SECURE_IRQ		(GIC_SPI_START + 55)
#define SMMU_VCODEC_B_CB_SC_NON_SECURE_IRQ	(GIC_SPI_START + 56)
#define SMMU_VCODEC_A_CB_SC_SECURE_IRQ		(GIC_SPI_START + 57)
#define SMMU_VCODEC_A_CB_SC_NON_SECURE_IRQ	(GIC_SPI_START + 58)
#define SMMU_ROT_CB_SC_SECURE_IRQ		(GIC_SPI_START + 59)
#define SMMU_ROT_CB_SC_NON_SECURE_IRQ		(GIC_SPI_START + 60)
#define SMMU_MDP1_CB_SC_SECURE_IRQ		(GIC_SPI_START + 61)
#define SMMU_MDP1_CB_SC_NON_SECURE_IRQ		(GIC_SPI_START + 62)
#define SMMU_MDP0_CB_SC_SECURE_IRQ		(GIC_SPI_START + 63)
#define SMMU_MDP0_CB_SC_NON_SECURE_IRQ		(GIC_SPI_START + 64)
#define SMMU_JPEGD_CB_SC_SECURE_IRQ		(GIC_SPI_START + 65)
#define SMMU_JPEGD_CB_SC_NON_SECURE_IRQ		(GIC_SPI_START + 66)
#define SMMU_IJPEG_CB_SC_SECURE_IRQ		(GIC_SPI_START + 67)
#define SMMU_IJPEG_CB_SC_NON_SECURE_IRQ		(GIC_SPI_START + 68)
#define SMMU_GFX3D_CB_SC_SECURE_IRQ		(GIC_SPI_START + 69)
#define SMMU_GFX3D_CB_SC_NON_SECURE_IRQ		(GIC_SPI_START + 70)
#define SMMU_GFX2D0_CB_SC_SECURE_IRQ		(GIC_SPI_START + 71)
#define SMMU_GFX2D0_CB_SC_NON_SECURE_IRQ	(GIC_SPI_START + 72)
#define ROT_IRQ					(GIC_SPI_START + 73)
#define MMSS_FABRIC_IRQ				(GIC_SPI_START + 74)
#define INT_MDP					(GIC_SPI_START + 75)
#define JPEGD_IRQ				(GIC_SPI_START + 76)
#define INT_JPEG				(GIC_SPI_START + 77)
#define MMSS_IMEM_IRQ				(GIC_SPI_START + 78)
#define HDMI_IRQ				(GIC_SPI_START + 79)
#define GFX3D_IRQ				(GIC_SPI_START + 80)
#define GFX2D0_IRQ				(GIC_SPI_START + 81)
#define DSI_IRQ					(GIC_SPI_START + 82)
#define CSI_1_IRQ				(GIC_SPI_START + 83)
#define CSI_0_IRQ				(GIC_SPI_START + 84)
#define LPASS_SCSS_AUDIO_IF_OUT0_IRQ		(GIC_SPI_START + 85)
#define LPASS_SCSS_MIDI_IRQ			(GIC_SPI_START + 86)
#define LPASS_Q6SS_WDOG_EXPIRED			(GIC_SPI_START + 87)
#define LPASS_SCSS_GP_LOW_IRQ			(GIC_SPI_START + 88)
#define LPASS_SCSS_GP_MEDIUM_IRQ		(GIC_SPI_START + 89)
#define LPASS_SCSS_GP_HIGH_IRQ			(GIC_SPI_START + 90)
#define TOP_IMEM_IRQ				(GIC_SPI_START + 91)
#define FABRIC_SYS_IRQ				(GIC_SPI_START + 92)
#define FABRIC_APPS_IRQ				(GIC_SPI_START + 93)
#define USB1_HS_BAM_IRQ				(GIC_SPI_START + 94)
#define SDC4_BAM_IRQ				(GIC_SPI_START + 95)
#define SDC3_BAM_IRQ				(GIC_SPI_START + 96)
#define SDC2_BAM_IRQ				(GIC_SPI_START + 97)
#define SDC1_BAM_IRQ				(GIC_SPI_START + 98)
#define FABRIC_SPS_IRQ				(GIC_SPI_START + 99)
#define USB1_HS_IRQ				(GIC_SPI_START + 100)
#define SDC4_IRQ_0				(GIC_SPI_START + 101)
#define SDC3_IRQ_0				(GIC_SPI_START + 102)
#define SDC2_IRQ_0				(GIC_SPI_START + 103)
#define SDC1_IRQ_0				(GIC_SPI_START + 104)
#define SPS_BAM_DMA_IRQ				(GIC_SPI_START + 105)
#define SPS_SEC_VIOL_IRQ			(GIC_SPI_START + 106)
#define SPS_MTI_0				(GIC_SPI_START + 107)
#define SPS_MTI_1				(GIC_SPI_START + 108)
#define SPS_MTI_2				(GIC_SPI_START + 109)
#define SPS_MTI_3				(GIC_SPI_START + 110)
#define SPS_MTI_4				(GIC_SPI_START + 111)
#define SPS_MTI_5				(GIC_SPI_START + 112)
#define SPS_MTI_6				(GIC_SPI_START + 113)
#define SPS_MTI_7				(GIC_SPI_START + 114)
#define SPS_MTI_8				(GIC_SPI_START + 115)
#define SPS_MTI_9				(GIC_SPI_START + 116)
#define SPS_MTI_10				(GIC_SPI_START + 117)
#define SPS_MTI_11				(GIC_SPI_START + 118)
#define SPS_MTI_12				(GIC_SPI_START + 119)
#define SPS_MTI_13				(GIC_SPI_START + 120)
#define SPS_MTI_14				(GIC_SPI_START + 121)
#define SPS_MTI_15				(GIC_SPI_START + 122)
#define SPS_MTI_16				(GIC_SPI_START + 123)
#define SPS_MTI_17				(GIC_SPI_START + 124)
#define SPS_MTI_18				(GIC_SPI_START + 125)
#define SPS_MTI_19				(GIC_SPI_START + 126)
#define SPS_MTI_20				(GIC_SPI_START + 127)
#define SPS_MTI_21				(GIC_SPI_START + 128)
#define SPS_MTI_22				(GIC_SPI_START + 129)
#define SPS_MTI_23				(GIC_SPI_START + 130)
#define SPS_MTI_24				(GIC_SPI_START + 131)
#define SPS_MTI_25				(GIC_SPI_START + 132)
#define SPS_MTI_26				(GIC_SPI_START + 133)
#define SPS_MTI_27				(GIC_SPI_START + 134)
#define SPS_MTI_28				(GIC_SPI_START + 135)
#define SPS_MTI_29				(GIC_SPI_START + 136)
#define SPS_MTI_30				(GIC_SPI_START + 137)
#define SPS_MTI_31				(GIC_SPI_START + 138)
#define UXMC_EBI2_WR_ER_DONE_IRQ		(GIC_SPI_START + 139)
#define UXMC_EBI2_OP_DONE_IRQ			(GIC_SPI_START + 140)
#define USB2_IRQ				(GIC_SPI_START + 141)
#define USB1_IRQ				(GIC_SPI_START + 142)
#define TSSC_SSBI_IRQ				(GIC_SPI_START + 143)
#define TSSC_SAMPLE_IRQ				(GIC_SPI_START + 144)
#define TSSC_PENUP_IRQ				(GIC_SPI_START + 145)
#define GSBI1_UARTDM_IRQ			(GIC_SPI_START + 146)
#define GSBI1_QUP_IRQ		         	(GIC_SPI_START + 147)
#define GSBI2_UARTDM_IRQ			(GIC_SPI_START + 148)
#define GSBI2_QUP_IRQ			        (GIC_SPI_START + 149)
#define GSBI3_UARTDM_IRQ			(GIC_SPI_START + 150)
#define GSBI3_QUP_IRQ				(GIC_SPI_START + 151)
#define GSBI4_UARTDM_IRQ			(GIC_SPI_START + 152)
#define GSBI4_QUP_IRQ				(GIC_SPI_START + 153)
#define GSBI5_UARTDM_IRQ			(GIC_SPI_START + 154)
#define GSBI5_QUP_IRQ				(GIC_SPI_START + 155)
#define GSBI6_UARTDM_IRQ			(GIC_SPI_START + 156)
#define GSBI6_QUP_IRQ				(GIC_SPI_START + 157)
#define GSBI7_UARTDM_IRQ			(GIC_SPI_START + 158)
#define GSBI7_QUP_IRQ				(GIC_SPI_START + 159)
#define GSBI8_UARTDM_IRQ			(GIC_SPI_START + 160)
#define GSBI8_QUP_IRQ				(GIC_SPI_START + 161)
#define TSIF_TSPP_IRQ				(GIC_SPI_START + 162)
#define TSIF_BAM_IRQ				(GIC_SPI_START + 163)
#define TSIF2_IRQ				(GIC_SPI_START + 164)
#define TSIF1_IRQ				(GIC_SPI_START + 165)
#define INT_ADM1_MASTER				(GIC_SPI_START + 166)
#define INT_ADM1_AARM				(GIC_SPI_START + 167)
#define INT_ADM1_SD2				(GIC_SPI_START + 168)
#define INT_ADM1_SD3				(GIC_SPI_START + 169)
#define INT_ADM0_MASTER				(GIC_SPI_START + 170)
#define INT_ADM0_AARM				(GIC_SPI_START + 171)
#define INT_ADM0_SD2				(GIC_SPI_START + 172)
#define INT_ADM0_SD3				(GIC_SPI_START + 173)
#define CC_SCSS_WDT1CPU1BITEEXPIRED		(GIC_SPI_START + 174)
#define CC_SCSS_WDT1CPU0BITEEXPIRED		(GIC_SPI_START + 175)
#define CC_SCSS_WDT0CPU1BITEEXPIRED		(GIC_SPI_START + 176)
#define CC_SCSS_WDT0CPU0BITEEXPIRED		(GIC_SPI_START + 177)
#define TSENS_UPPER_LOWER_INT			(GIC_SPI_START + 178)
#define SSBI2_2_SC_CPU1_SECURE_INT		(GIC_SPI_START + 179)
#define SSBI2_2_SC_CPU1_NON_SECURE_INT		(GIC_SPI_START + 180)
#define SSBI2_1_SC_CPU1_SECURE_INT		(GIC_SPI_START + 181)
#define SSBI2_1_SC_CPU1_NON_SECURE_INT		(GIC_SPI_START + 182)
#define XPU_SUMMARY_IRQ				(GIC_SPI_START + 183)
#define BUS_EXCEPTION_SUMMARY_IRQ		(GIC_SPI_START + 184)
#define HSDDRX_SMICH0_IRQ			(GIC_SPI_START + 185)
#define HSDDRX_EBI1_IRQ				(GIC_SPI_START + 186)
#define SDC5_BAM_IRQ				(GIC_SPI_START + 187)
#define SDC5_IRQ_0				(GIC_SPI_START + 188)
#define GSBI9_UARTDM_IRQ			(GIC_SPI_START + 189)
#define GSBI9_QUP_IRQ				(GIC_SPI_START + 190)
#define GSBI10_UARTDM_IRQ			(GIC_SPI_START + 191)
#define GSBI10_QUP_IRQ				(GIC_SPI_START + 192)
#define GSBI11_UARTDM_IRQ			(GIC_SPI_START + 193)
#define GSBI11_QUP_IRQ				(GIC_SPI_START + 194)
#define GSBI12_UARTDM_IRQ			(GIC_SPI_START + 195)
#define GSBI12_QUP_IRQ				(GIC_SPI_START + 196)

#define SMMU_GFX2D1_CB_SC_SECURE_IRQ		(GIC_SPI_START + 210)
#define SMMU_GFX2D1_CB_SC_NON_SECURE_IRQ	(GIC_SPI_START + 211)
#define GFX2D1_IRQ				(GIC_SPI_START + 212)

#define SMPSS_SPARE_1				(GIC_SPI_START + 217)
#define SMPSS_SPARE_2				(GIC_SPI_START + 218)
#define SMPSS_SPARE_3				(GIC_SPI_START + 219)
#define SMPSS_SPARE_4				(GIC_SPI_START + 220)
#define SMPSS_SPARE_5				(GIC_SPI_START + 221)
#define SMPSS_SPARE_6				(GIC_SPI_START + 222)
#define SMPSS_SPARE_7				(GIC_SPI_START + 223)

#define NR_TLMM_MSM_DIR_CONN_IRQ 10
#define NR_GPIO_IRQS 173
#define NR_MSM_GPIOS NR_GPIO_IRQS
#define NR_MSM_IRQS 256
#define NR_PMIC8058_IRQS 256
#define NR_PMIC8901_IRQS 72
#define NR_GPIO_EXPANDER_IRQS 98
#define NR_BOARD_IRQS (NR_PMIC8058_IRQS + NR_PMIC8901_IRQS +\
		NR_GPIO_EXPANDER_IRQS)

/*                     */
#define INT_A9_M2A_0                    MARM_SCSS_GP_IRQ_0
#define INT_A9_M2A_5                    MARM_SCSS_GP_IRQ_1
#define INT_ADSP_A11                    LPASS_SCSS_GP_HIGH_IRQ
#define INT_ADSP_A11_SMSM               LPASS_SCSS_GP_MEDIUM_IRQ
#define INT_DSPS_A11                    SPS_MTI_31

#endif
