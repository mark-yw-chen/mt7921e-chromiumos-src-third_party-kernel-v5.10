// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2015 MediaTek Inc.
 */

#include <linux/clk.h>
#include <linux/iopoll.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/soc/mediatek/mtk-mmsys.h>
#include <linux/soc/mediatek/mtk-mutex.h>

#define MT2701_MUTEX0_MOD0			0x2c
#define MT2701_MUTEX0_SOF0			0x30
#define MT8183_MUTEX0_MOD0			0x30
#define MT8183_MUTEX0_SOF0			0x2c

#define MT8195_DISP_MUTEX0_MOD0			0x30
#define MT8195_DISP_MUTEX0_SOF			0x2c

#define DISP_REG_MUTEX_EN(n)			(0x20 + 0x20 * (n))
#define DISP_REG_MUTEX(n)			(0x24 + 0x20 * (n))
#define DISP_REG_MUTEX_RST(n)			(0x28 + 0x20 * (n))
#define DISP_REG_MUTEX_MOD(mutex_mod_reg, n)	(mutex_mod_reg + 0x20 * (n))
#define DISP_REG_MUTEX_SOF(mutex_sof_reg, n)	(mutex_sof_reg + 0x20 * (n))
#define DISP_REG_MUTEX_MOD2(n)			(0x34 + 0x20 * (n))

#define INT_MUTEX				BIT(1)

#define MT8167_MUTEX_MOD_DISP_PWM		BIT(1)
#define MT8167_MUTEX_MOD_DISP_OVL0		BIT(6)
#define MT8167_MUTEX_MOD_DISP_OVL1		BIT(7)
#define MT8167_MUTEX_MOD_DISP_RDMA0		BIT(8)
#define MT8167_MUTEX_MOD_DISP_RDMA1		BIT(9)
#define MT8167_MUTEX_MOD_DISP_WDMA0		BIT(10)
#define MT8167_MUTEX_MOD_DISP_CCORR		BIT(11)
#define MT8167_MUTEX_MOD_DISP_COLOR		BIT(12)
#define MT8167_MUTEX_MOD_DISP_AAL		BIT(13)
#define MT8167_MUTEX_MOD_DISP_GAMMA		BIT(14)
#define MT8167_MUTEX_MOD_DISP_DITHER	BIT(15)
#define MT8167_MUTEX_MOD_DISP_UFOE		BIT(16)

#define MT8183_MUTEX_MOD_DISP_RDMA0		BIT(0)
#define MT8183_MUTEX_MOD_DISP_RDMA1		BIT(1)
#define MT8183_MUTEX_MOD_DISP_OVL0		BIT(9)
#define MT8183_MUTEX_MOD_DISP_OVL0_2L		BIT(10)
#define MT8183_MUTEX_MOD_DISP_OVL1_2L		BIT(11)
#define MT8183_MUTEX_MOD_DISP_WDMA0		BIT(12)
#define MT8183_MUTEX_MOD_DISP_COLOR0		BIT(13)
#define MT8183_MUTEX_MOD_DISP_CCORR0		BIT(14)
#define MT8183_MUTEX_MOD_DISP_AAL0		BIT(15)
#define MT8183_MUTEX_MOD_DISP_GAMMA0		BIT(16)
#define MT8183_MUTEX_MOD_DISP_DITHER0		BIT(17)

#define MT8173_MUTEX_MOD_DISP_OVL0		BIT(11)
#define MT8173_MUTEX_MOD_DISP_OVL1		BIT(12)
#define MT8173_MUTEX_MOD_DISP_RDMA0		BIT(13)
#define MT8173_MUTEX_MOD_DISP_RDMA1		BIT(14)
#define MT8173_MUTEX_MOD_DISP_RDMA2		BIT(15)
#define MT8173_MUTEX_MOD_DISP_WDMA0		BIT(16)
#define MT8173_MUTEX_MOD_DISP_WDMA1		BIT(17)
#define MT8173_MUTEX_MOD_DISP_COLOR0		BIT(18)
#define MT8173_MUTEX_MOD_DISP_COLOR1		BIT(19)
#define MT8173_MUTEX_MOD_DISP_AAL		BIT(20)
#define MT8173_MUTEX_MOD_DISP_GAMMA		BIT(21)
#define MT8173_MUTEX_MOD_DISP_UFOE		BIT(22)
#define MT8173_MUTEX_MOD_DISP_PWM0		BIT(23)
#define MT8173_MUTEX_MOD_DISP_PWM1		BIT(24)
#define MT8173_MUTEX_MOD_DISP_OD		BIT(25)

#define MT8195_MUTEX_MOD_DISP_OVL0		BIT(0)
#define MT8195_MUTEX_MOD_DISP_WDMA0		BIT(1)
#define MT8195_MUTEX_MOD_DISP_RDMA0		BIT(2)
#define MT8195_MUTEX_MOD_DISP_COLOR0		BIT(3)
#define MT8195_MUTEX_MOD_DISP_CCORR0		BIT(4)
#define MT8195_MUTEX_MOD_DISP_AAL0		BIT(5)
#define MT8195_MUTEX_MOD_DISP_GAMMA0		BIT(6)
#define MT8195_MUTEX_MOD_DISP_DITHER0		BIT(7)
#define MT8195_MUTEX_MOD_DISP_DSI0		BIT(8)
#define MT8195_MUTEX_MOD_DISP_DSC_WRAP0_CORE0	BIT(9)
#define MT8195_MUTEX_MOD_DISP_OVL1		BIT(10)
#define MT8195_MUTEX_MOD_DISP_WDMA1		BIT(11)
#define MT8195_MUTEX_MOD_DISP_RDMA1		BIT(12)
#define MT8195_MUTEX_MOD_DISP_COLOR1		BIT(13)
#define MT8195_MUTEX_MOD_DISP_CCORR1		BIT(14)
#define MT8195_MUTEX_MOD_DISP_AAL1		BIT(15)
#define MT8195_MUTEX_MOD_DISP_GAMMA1		BIT(16)
#define MT8195_MUTEX_MOD_DISP_DITHER1		BIT(17)
#define MT8195_MUTEX_MOD_DISP_DSI1		BIT(18)
#define MT8195_MUTEX_MOD_DISP_DSC_WRAP0_CORE1	BIT(19)
#define MT8195_MUTEX_MOD_DISP_VPP_MERGE		BIT(20)
#define MT8195_MUTEX_MOD_DISP_DP_INTF0		BIT(21)
#define MT8195_MUTEX_MOD_DISP_VPP1_DL_RELAY0	BIT(22)
#define MT8195_MUTEX_MOD_DISP_VPP1_DL_RELAY1	BIT(23)
#define MT8195_MUTEX_MOD_DISP_VDO1_DL_RELAY2	BIT(24)
#define MT8195_MUTEX_MOD_DISP_VDO0_DL_RELAY3	BIT(25)
#define MT8195_MUTEX_MOD_DISP_VDO0_DL_RELAY4	BIT(26)
#define MT8195_MUTEX_MOD_DISP_PWM0		BIT(27)
#define MT8195_MUTEX_MOD_DISP_PWM1		BIT(28)

#define MT8195_MUTEX_MOD_DISP1_MDP_RDMA0 BIT(0)
#define MT8195_MUTEX_MOD_DISP1_MDP_RDMA1 BIT(1)
#define MT8195_MUTEX_MOD_DISP1_MDP_RDMA2 BIT(2)
#define MT8195_MUTEX_MOD_DISP1_MDP_RDMA3 BIT(3)
#define MT8195_MUTEX_MOD_DISP1_MDP_RDMA4 BIT(4)
#define MT8195_MUTEX_MOD_DISP1_MDP_RDMA5 BIT(5)
#define MT8195_MUTEX_MOD_DISP1_MDP_RDMA6 BIT(6)
#define MT8195_MUTEX_MOD_DISP1_MDP_RDMA7 BIT(7)
#define MT8195_MUTEX_MOD_DISP1_VPP_MERGE0 BIT(8)
#define MT8195_MUTEX_MOD_DISP1_VPP_MERGE1 BIT(9)
#define MT8195_MUTEX_MOD_DISP1_VPP_MERGE2 BIT(10)
#define MT8195_MUTEX_MOD_DISP1_VPP_MERGE3 BIT(11)
#define MT8195_MUTEX_MOD_DISP1_VPP_MERGE4 BIT(12)
#define MT8195_MUTEX_MOD_DISP1_VPP2_DL_RELAY BIT(13)
#define MT8195_MUTEX_MOD_DISP1_VPP3_DL_RELAY BIT(14)
#define MT8195_MUTEX_MOD_DISP1_VDO0_DSC_DL_ASYNC BIT(15)
#define MT8195_MUTEX_MOD_DISP1_VDO0_MERGE_DL_ASYNC BIT(16)
#define MT8195_MUTEX_MOD_DISP1_VDO1_OUT_DL_RELAY BIT(17)
#define MT8195_MUTEX_MOD_DISP1_DISP_MIXER BIT(18)
#define MT8195_MUTEX_MOD_DISP1_HDR_VDO_FE0 BIT(19)
#define MT8195_MUTEX_MOD_DISP1_HDR_VDO_FE1 BIT(20)
#define MT8195_MUTEX_MOD_DISP1_HDR_GFX_FE0 BIT(21)
#define MT8195_MUTEX_MOD_DISP1_HDR_GFX_FE1 BIT(22)
#define MT8195_MUTEX_MOD_DISP1_HDR_VDO_BE0 BIT(23)
#define MT8195_MUTEX_MOD_DISP1_HDR_MLOAD BIT(24)
#define MT8195_MUTEX_MOD_DISP1_DPI0 BIT(25)
#define MT8195_MUTEX_MOD_DISP1_DPI1 BIT(26)
#define MT8195_MUTEX_MOD_DISP1_DP_INTF0 BIT(27)

#define MT2712_MUTEX_MOD_DISP_PWM2		BIT(10)
#define MT2712_MUTEX_MOD_DISP_OVL0		BIT(11)
#define MT2712_MUTEX_MOD_DISP_OVL1		BIT(12)
#define MT2712_MUTEX_MOD_DISP_RDMA0		BIT(13)
#define MT2712_MUTEX_MOD_DISP_RDMA1		BIT(14)
#define MT2712_MUTEX_MOD_DISP_RDMA2		BIT(15)
#define MT2712_MUTEX_MOD_DISP_WDMA0		BIT(16)
#define MT2712_MUTEX_MOD_DISP_WDMA1		BIT(17)
#define MT2712_MUTEX_MOD_DISP_COLOR0		BIT(18)
#define MT2712_MUTEX_MOD_DISP_COLOR1		BIT(19)
#define MT2712_MUTEX_MOD_DISP_AAL0		BIT(20)
#define MT2712_MUTEX_MOD_DISP_UFOE		BIT(22)
#define MT2712_MUTEX_MOD_DISP_PWM0		BIT(23)
#define MT2712_MUTEX_MOD_DISP_PWM1		BIT(24)
#define MT2712_MUTEX_MOD_DISP_OD0		BIT(25)
#define MT2712_MUTEX_MOD2_DISP_AAL1		BIT(33)
#define MT2712_MUTEX_MOD2_DISP_OD1		BIT(34)

#define MT2701_MUTEX_MOD_DISP_OVL		BIT(3)
#define MT2701_MUTEX_MOD_DISP_WDMA		BIT(6)
#define MT2701_MUTEX_MOD_DISP_COLOR		BIT(7)
#define MT2701_MUTEX_MOD_DISP_BLS		BIT(9)
#define MT2701_MUTEX_MOD_DISP_RDMA0		BIT(10)
#define MT2701_MUTEX_MOD_DISP_RDMA1		BIT(12)

#define MT2712_MUTEX_SOF_SINGLE_MODE		0
#define MT2712_MUTEX_SOF_DSI0			1
#define MT2712_MUTEX_SOF_DSI1			2
#define MT2712_MUTEX_SOF_DPI0			3
#define MT2712_MUTEX_SOF_DPI1			4
#define MT2712_MUTEX_SOF_DSI2			5
#define MT2712_MUTEX_SOF_DSI3			6
#define MT8167_MUTEX_SOF_DPI0			2
#define MT8167_MUTEX_SOF_DPI1			3

#define MT8183_MUTEX_SOF_DSI0			1
#define MT8183_MUTEX_SOF_DPI0			2

#define MT8183_MUTEX_EOF_DSI0			(MT8183_MUTEX_SOF_DSI0 << 6)
#define MT8183_MUTEX_EOF_DPI0			(MT8183_MUTEX_SOF_DPI0 << 6)

#define MT8195_MUTEX_SOF_DSI0			1
#define MT8195_MUTEX_SOF_DSI1			2
#define MT8195_MUTEX_SOF_DP_INTF0		3
#define MT8195_MUTEX_SOF_DP_INTF1		4
#define MT8195_MUTEX_SOF_DPI0			6
#define MT8195_MUTEX_SOF_DPI1			5
#define MT8195_MUTEX_EOF_DSI0			(MT8195_MUTEX_SOF_DSI0 << 7)
#define MT8195_MUTEX_EOF_DSI1			(MT8195_MUTEX_SOF_DSI1 << 7)
#define MT8195_MUTEX_EOF_DP_INTF0		(MT8195_MUTEX_SOF_DP_INTF0 << 7)
#define MT8195_MUTEX_EOF_DP_INTF1		(MT8195_MUTEX_SOF_DP_INTF1 << 7)
#define MT8195_MUTEX_EOF_DPI0			(MT8195_MUTEX_SOF_DPI0 << 7)
#define MT8195_MUTEX_EOF_DPI1			(MT8195_MUTEX_SOF_DPI1 << 7)

struct mtk_mutex {
	int id;
	bool claimed;
	bool sub_disp_mutex;
};

enum mtk_mutex_sof_id {
	MUTEX_SOF_SINGLE_MODE,
	MUTEX_SOF_DSI0,
	MUTEX_SOF_DSI1,
	MUTEX_SOF_DPI0,
	MUTEX_SOF_DPI1,
	MUTEX_SOF_DSI2,
	MUTEX_SOF_DSI3,
	MUTEX_SOF_DP_INTF0,
	MUTEX_SOF_DP_INTF1,
	DDP_MUTEX_SOF_MAX,
};

struct mtk_mutex_data {
	const unsigned long *mutex_mod;
	const unsigned int *mutex_sof;
	const unsigned int mutex_mod_reg;
	const unsigned int mutex_sof_reg;
	const bool no_clk;
	const bool support_sub_disp_mutex;
	bool one_path_2nd_mutex_en;
	unsigned int one_path_2nd_mutex_id;
	const unsigned long *one_path_2nd_mutex_mod;
};

struct mtk_mutex_ctx {
	struct device			*dev;
	struct clk			*clk;
	void __iomem			*regs;
	struct mtk_mutex		mutex[10];
	struct clk			*clk1;
	void __iomem			*regs1;
	struct mtk_mutex		mutex1[10];
	const struct mtk_mutex_data	*data;
};

static const unsigned long mt2701_mutex_mod[DDP_COMPONENT_ID_MAX] = {
	[DDP_COMPONENT_BLS] = MT2701_MUTEX_MOD_DISP_BLS,
	[DDP_COMPONENT_COLOR0] = MT2701_MUTEX_MOD_DISP_COLOR,
	[DDP_COMPONENT_OVL0] = MT2701_MUTEX_MOD_DISP_OVL,
	[DDP_COMPONENT_RDMA0] = MT2701_MUTEX_MOD_DISP_RDMA0,
	[DDP_COMPONENT_RDMA1] = MT2701_MUTEX_MOD_DISP_RDMA1,
	[DDP_COMPONENT_WDMA0] = MT2701_MUTEX_MOD_DISP_WDMA,
};

static const unsigned long mt2712_mutex_mod[DDP_COMPONENT_ID_MAX] = {
	[DDP_COMPONENT_AAL0] = MT2712_MUTEX_MOD_DISP_AAL0,
	[DDP_COMPONENT_AAL1] = MT2712_MUTEX_MOD2_DISP_AAL1,
	[DDP_COMPONENT_COLOR0] = MT2712_MUTEX_MOD_DISP_COLOR0,
	[DDP_COMPONENT_COLOR1] = MT2712_MUTEX_MOD_DISP_COLOR1,
	[DDP_COMPONENT_OD0] = MT2712_MUTEX_MOD_DISP_OD0,
	[DDP_COMPONENT_OD1] = MT2712_MUTEX_MOD2_DISP_OD1,
	[DDP_COMPONENT_OVL0] = MT2712_MUTEX_MOD_DISP_OVL0,
	[DDP_COMPONENT_OVL1] = MT2712_MUTEX_MOD_DISP_OVL1,
	[DDP_COMPONENT_PWM0] = MT2712_MUTEX_MOD_DISP_PWM0,
	[DDP_COMPONENT_PWM1] = MT2712_MUTEX_MOD_DISP_PWM1,
	[DDP_COMPONENT_PWM2] = MT2712_MUTEX_MOD_DISP_PWM2,
	[DDP_COMPONENT_RDMA0] = MT2712_MUTEX_MOD_DISP_RDMA0,
	[DDP_COMPONENT_RDMA1] = MT2712_MUTEX_MOD_DISP_RDMA1,
	[DDP_COMPONENT_RDMA2] = MT2712_MUTEX_MOD_DISP_RDMA2,
	[DDP_COMPONENT_UFOE] = MT2712_MUTEX_MOD_DISP_UFOE,
	[DDP_COMPONENT_WDMA0] = MT2712_MUTEX_MOD_DISP_WDMA0,
	[DDP_COMPONENT_WDMA1] = MT2712_MUTEX_MOD_DISP_WDMA1,
};

static const unsigned long mt8167_mutex_mod[DDP_COMPONENT_ID_MAX] = {
	[DDP_COMPONENT_AAL0] = MT8167_MUTEX_MOD_DISP_AAL,
	[DDP_COMPONENT_CCORR] = MT8167_MUTEX_MOD_DISP_CCORR,
	[DDP_COMPONENT_COLOR0] = MT8167_MUTEX_MOD_DISP_COLOR,
	[DDP_COMPONENT_DITHER] = MT8167_MUTEX_MOD_DISP_DITHER,
	[DDP_COMPONENT_GAMMA] = MT8167_MUTEX_MOD_DISP_GAMMA,
	[DDP_COMPONENT_OVL0] = MT8167_MUTEX_MOD_DISP_OVL0,
	[DDP_COMPONENT_OVL1] = MT8167_MUTEX_MOD_DISP_OVL1,
	[DDP_COMPONENT_PWM0] = MT8167_MUTEX_MOD_DISP_PWM,
	[DDP_COMPONENT_RDMA0] = MT8167_MUTEX_MOD_DISP_RDMA0,
	[DDP_COMPONENT_RDMA1] = MT8167_MUTEX_MOD_DISP_RDMA1,
	[DDP_COMPONENT_UFOE] = MT8167_MUTEX_MOD_DISP_UFOE,
	[DDP_COMPONENT_WDMA0] = MT8167_MUTEX_MOD_DISP_WDMA0,
};

static const unsigned long mt8173_mutex_mod[DDP_COMPONENT_ID_MAX] = {
	[DDP_COMPONENT_AAL0] = MT8173_MUTEX_MOD_DISP_AAL,
	[DDP_COMPONENT_COLOR0] = MT8173_MUTEX_MOD_DISP_COLOR0,
	[DDP_COMPONENT_COLOR1] = MT8173_MUTEX_MOD_DISP_COLOR1,
	[DDP_COMPONENT_GAMMA] = MT8173_MUTEX_MOD_DISP_GAMMA,
	[DDP_COMPONENT_OD0] = MT8173_MUTEX_MOD_DISP_OD,
	[DDP_COMPONENT_OVL0] = MT8173_MUTEX_MOD_DISP_OVL0,
	[DDP_COMPONENT_OVL1] = MT8173_MUTEX_MOD_DISP_OVL1,
	[DDP_COMPONENT_PWM0] = MT8173_MUTEX_MOD_DISP_PWM0,
	[DDP_COMPONENT_PWM1] = MT8173_MUTEX_MOD_DISP_PWM1,
	[DDP_COMPONENT_RDMA0] = MT8173_MUTEX_MOD_DISP_RDMA0,
	[DDP_COMPONENT_RDMA1] = MT8173_MUTEX_MOD_DISP_RDMA1,
	[DDP_COMPONENT_RDMA2] = MT8173_MUTEX_MOD_DISP_RDMA2,
	[DDP_COMPONENT_UFOE] = MT8173_MUTEX_MOD_DISP_UFOE,
	[DDP_COMPONENT_WDMA0] = MT8173_MUTEX_MOD_DISP_WDMA0,
	[DDP_COMPONENT_WDMA1] = MT8173_MUTEX_MOD_DISP_WDMA1,
};

static const unsigned long mt8183_mutex_mod[DDP_COMPONENT_ID_MAX] = {
	[DDP_COMPONENT_AAL0] = MT8183_MUTEX_MOD_DISP_AAL0,
	[DDP_COMPONENT_CCORR] = MT8183_MUTEX_MOD_DISP_CCORR0,
	[DDP_COMPONENT_COLOR0] = MT8183_MUTEX_MOD_DISP_COLOR0,
	[DDP_COMPONENT_DITHER] = MT8183_MUTEX_MOD_DISP_DITHER0,
	[DDP_COMPONENT_GAMMA] = MT8183_MUTEX_MOD_DISP_GAMMA0,
	[DDP_COMPONENT_OVL0] = MT8183_MUTEX_MOD_DISP_OVL0,
	[DDP_COMPONENT_OVL_2L0] = MT8183_MUTEX_MOD_DISP_OVL0_2L,
	[DDP_COMPONENT_OVL_2L1] = MT8183_MUTEX_MOD_DISP_OVL1_2L,
	[DDP_COMPONENT_RDMA0] = MT8183_MUTEX_MOD_DISP_RDMA0,
	[DDP_COMPONENT_RDMA1] = MT8183_MUTEX_MOD_DISP_RDMA1,
	[DDP_COMPONENT_WDMA0] = MT8183_MUTEX_MOD_DISP_WDMA0,
};

static const unsigned long mt8195_mutex_mod[DDP_COMPONENT_ID_MAX] = {
	[DDP_COMPONENT_OVL0] = MT8195_MUTEX_MOD_DISP_OVL0,
	[DDP_COMPONENT_WDMA0] = MT8195_MUTEX_MOD_DISP_WDMA0,
	[DDP_COMPONENT_RDMA0] = MT8195_MUTEX_MOD_DISP_RDMA0,
	[DDP_COMPONENT_COLOR0] = MT8195_MUTEX_MOD_DISP_COLOR0,
	[DDP_COMPONENT_CCORR] = MT8195_MUTEX_MOD_DISP_CCORR0,
	[DDP_COMPONENT_AAL0] = MT8195_MUTEX_MOD_DISP_AAL0,
	[DDP_COMPONENT_GAMMA] = MT8195_MUTEX_MOD_DISP_GAMMA0,
	[DDP_COMPONENT_DITHER] = MT8195_MUTEX_MOD_DISP_DITHER0,
	[DDP_COMPONENT_MERGE0] = MT8195_MUTEX_MOD_DISP_VPP_MERGE,
	[DDP_COMPONENT_DSC0] = MT8195_MUTEX_MOD_DISP_DSC_WRAP0_CORE0,
	[DDP_COMPONENT_DSI0] = MT8195_MUTEX_MOD_DISP_DSI0,
	[DDP_COMPONENT_PWM0] = MT8195_MUTEX_MOD_DISP_PWM0,
	[DDP_COMPONENT_DP_INTF0] = MT8195_MUTEX_MOD_DISP_DP_INTF0,
	[DDP_COMPONENT_PSEUDO_OVL] = 0,
	[DDP_COMPONENT_ETHDR] = MT8195_MUTEX_MOD_DISP1_HDR_VDO_FE0|
							MT8195_MUTEX_MOD_DISP1_HDR_VDO_FE1|
							MT8195_MUTEX_MOD_DISP1_HDR_GFX_FE0|
							MT8195_MUTEX_MOD_DISP1_HDR_GFX_FE1|
							MT8195_MUTEX_MOD_DISP1_HDR_VDO_BE0|
							MT8195_MUTEX_MOD_DISP1_HDR_MLOAD,
	[DDP_COMPONENT_MERGE5] = MT8195_MUTEX_MOD_DISP1_VPP_MERGE4,
	[DDP_COMPONENT_DPI1] = MT8195_MUTEX_MOD_DISP1_DPI1,
	[DDP_COMPONENT_DP_INTF1] = MT8195_MUTEX_MOD_DISP1_DP_INTF0,
};

static const unsigned long mt8195_one_path_2nd_mutex_mod[DDP_COMPONENT_ID_MAX]
	= {
		[DDP_COMPONENT_PSEUDO_OVL] = MT8195_MUTEX_MOD_DISP1_MDP_RDMA0|
				MT8195_MUTEX_MOD_DISP1_MDP_RDMA1|
				MT8195_MUTEX_MOD_DISP1_MDP_RDMA2|
				MT8195_MUTEX_MOD_DISP1_MDP_RDMA3|
				MT8195_MUTEX_MOD_DISP1_MDP_RDMA4|
				MT8195_MUTEX_MOD_DISP1_MDP_RDMA5|
				MT8195_MUTEX_MOD_DISP1_MDP_RDMA6|
				MT8195_MUTEX_MOD_DISP1_MDP_RDMA7|
				MT8195_MUTEX_MOD_DISP1_VPP_MERGE0|
				MT8195_MUTEX_MOD_DISP1_VPP_MERGE1|
				MT8195_MUTEX_MOD_DISP1_VPP_MERGE2|
				MT8195_MUTEX_MOD_DISP1_VPP_MERGE3,

		[DDP_COMPONENT_ETHDR] = MT8195_MUTEX_MOD_DISP1_DISP_MIXER,
};

static const unsigned int mt2712_mutex_sof[DDP_MUTEX_SOF_MAX] = {
	[MUTEX_SOF_SINGLE_MODE] = MUTEX_SOF_SINGLE_MODE,
	[MUTEX_SOF_DSI0] = MUTEX_SOF_DSI0,
	[MUTEX_SOF_DSI1] = MUTEX_SOF_DSI1,
	[MUTEX_SOF_DPI0] = MUTEX_SOF_DPI0,
	[MUTEX_SOF_DPI1] = MUTEX_SOF_DPI1,
	[MUTEX_SOF_DSI2] = MUTEX_SOF_DSI2,
	[MUTEX_SOF_DSI3] = MUTEX_SOF_DSI3,
};

static const unsigned int mt8167_mutex_sof[DDP_MUTEX_SOF_MAX] = {
	[MUTEX_SOF_SINGLE_MODE] = MUTEX_SOF_SINGLE_MODE,
	[MUTEX_SOF_DSI0] = MUTEX_SOF_DSI0,
	[MUTEX_SOF_DPI0] = MT8167_MUTEX_SOF_DPI0,
	[MUTEX_SOF_DPI1] = MT8167_MUTEX_SOF_DPI1,
};

/* Add EOF setting so overlay hardware can receive frame done irq */
static const unsigned int mt8183_mutex_sof[MUTEX_SOF_DSI3 + 1] = {
	[MUTEX_SOF_SINGLE_MODE] = MUTEX_SOF_SINGLE_MODE,
	[MUTEX_SOF_DSI0] = MUTEX_SOF_DSI0 | MT8183_MUTEX_EOF_DSI0,
	[MUTEX_SOF_DPI0] = MT8183_MUTEX_SOF_DPI0 | MT8183_MUTEX_EOF_DPI0,
};

static const unsigned int mt8195_mutex_sof[DDP_MUTEX_SOF_MAX] = {
	[MUTEX_SOF_SINGLE_MODE] = MUTEX_SOF_SINGLE_MODE,
	[MUTEX_SOF_DSI0] = MT8195_MUTEX_SOF_DSI0 | MT8195_MUTEX_EOF_DSI0,
	[MUTEX_SOF_DSI1] = MT8195_MUTEX_SOF_DSI1 | MT8195_MUTEX_EOF_DSI1,
	[MUTEX_SOF_DPI0] = MT8195_MUTEX_SOF_DPI0 | MT8195_MUTEX_EOF_DPI0,
	[MUTEX_SOF_DPI1] = MT8195_MUTEX_SOF_DPI1 | MT8195_MUTEX_EOF_DPI1,
	[MUTEX_SOF_DP_INTF0] =
		MT8195_MUTEX_SOF_DP_INTF0 | MT8195_MUTEX_EOF_DP_INTF0,
	[MUTEX_SOF_DP_INTF1] =
		MT8195_MUTEX_SOF_DP_INTF1 | MT8195_MUTEX_EOF_DP_INTF1,
};

static const struct mtk_mutex_data mt2701_mutex_driver_data = {
	.mutex_mod = mt2701_mutex_mod,
	.mutex_sof = mt2712_mutex_sof,
	.mutex_mod_reg = MT2701_MUTEX0_MOD0,
	.mutex_sof_reg = MT2701_MUTEX0_SOF0,
};

static const struct mtk_mutex_data mt2712_mutex_driver_data = {
	.mutex_mod = mt2712_mutex_mod,
	.mutex_sof = mt2712_mutex_sof,
	.mutex_mod_reg = MT2701_MUTEX0_MOD0,
	.mutex_sof_reg = MT2701_MUTEX0_SOF0,
};

static const struct mtk_mutex_data mt8167_mutex_driver_data = {
	.mutex_mod = mt8167_mutex_mod,
	.mutex_sof = mt8167_mutex_sof,
	.mutex_mod_reg = MT2701_MUTEX0_MOD0,
	.mutex_sof_reg = MT2701_MUTEX0_SOF0,
	.no_clk = true,
};

static const struct mtk_mutex_data mt8173_mutex_driver_data = {
	.mutex_mod = mt8173_mutex_mod,
	.mutex_sof = mt2712_mutex_sof,
	.mutex_mod_reg = MT2701_MUTEX0_MOD0,
	.mutex_sof_reg = MT2701_MUTEX0_SOF0,
};

static const struct mtk_mutex_data mt8183_mutex_driver_data = {
	.mutex_mod = mt8183_mutex_mod,
	.mutex_sof = mt8183_mutex_sof,
	.mutex_mod_reg = MT8183_MUTEX0_MOD0,
	.mutex_sof_reg = MT8183_MUTEX0_SOF0,
	.no_clk = true,
};

static const struct mtk_mutex_data mt8195_mutex_driver_data = {
	.mutex_mod = mt8195_mutex_mod,
	.mutex_sof = mt8195_mutex_sof,
	.mutex_mod_reg = MT8195_DISP_MUTEX0_MOD0,
	.mutex_sof_reg = MT8195_DISP_MUTEX0_SOF,
	.support_sub_disp_mutex = true,
	.one_path_2nd_mutex_en = true,
	.one_path_2nd_mutex_id = 5,
	.one_path_2nd_mutex_mod = mt8195_one_path_2nd_mutex_mod,
};

static struct mtk_mutex_ctx *mtk_mutex_ctx_get(struct mtk_mutex *mutex)
{
	struct mtk_mutex_ctx *mtx;

	if (mutex->sub_disp_mutex)
		mtx = container_of(mutex, struct mtk_mutex_ctx, mutex1[mutex->id]);
	else
		mtx = container_of(mutex, struct mtk_mutex_ctx, mutex[mutex->id]);

	if (mutex->sub_disp_mutex) {
		if (&mtx->mutex1[mutex->id] != mutex)
			WARN_ON(&mtx->mutex1[mutex->id] != mutex);
	} else {
		if (&mtx->mutex[mutex->id] != mutex)
			WARN_ON(&mtx->mutex[mutex->id] != mutex);
	}
	return mtx;
}

static struct clk *mtk_mutex_clk_get(struct mtk_mutex *mutex,
			struct mtk_mutex_ctx *mtx)
{
	struct clk *clk;

	if (mutex->sub_disp_mutex)
		clk = mtx->clk1;
	else
		clk = mtx->clk;

	return clk;
}

static void __iomem *mtk_mutex_reg_get(struct mtk_mutex *mutex,
			struct mtk_mutex_ctx *mtx)
{
	void __iomem *reg;

	if (mutex->sub_disp_mutex) {
		/*Independent subdisp mutex*/
		reg = mtx->regs1;
	} else {
		reg = mtx->regs;
	}

	return reg;
}

struct mtk_mutex *mtk_mutex_get(struct device *dev, bool sub_mutex)
{
	struct mtk_mutex_ctx *mtx = dev_get_drvdata(dev);
	int i;

	if (sub_mutex && mtx->data->support_sub_disp_mutex) {
		for (i = 0; i < 10; i++)
			if (!mtx->mutex1[i].claimed) {
				mtx->mutex1[i].claimed = true;
				return &mtx->mutex1[i];
			}
		return ERR_PTR(-EBUSY);
	}

	for (i = 0; i < 10; i++)
		if (!mtx->mutex[i].claimed) {
			mtx->mutex[i].claimed = true;
			return &mtx->mutex[i];
		}

	return ERR_PTR(-EBUSY);
}
EXPORT_SYMBOL_GPL(mtk_mutex_get);

void mtk_mutex_put(struct mtk_mutex *mutex)
{
	mutex->claimed = false;
}
EXPORT_SYMBOL_GPL(mtk_mutex_put);

int mtk_mutex_prepare(struct mtk_mutex *mutex)
{
	struct mtk_mutex_ctx *mtx = mtk_mutex_ctx_get(mutex);
	return clk_prepare_enable(mtk_mutex_clk_get(mutex, mtx));
}
EXPORT_SYMBOL_GPL(mtk_mutex_prepare);

void mtk_mutex_unprepare(struct mtk_mutex *mutex)
{
	struct mtk_mutex_ctx *mtx = mtk_mutex_ctx_get(mutex);
	clk_disable_unprepare(mtk_mutex_clk_get(mutex, mtx));
}
EXPORT_SYMBOL_GPL(mtk_mutex_unprepare);

void mtk_mutex_add_comp(struct mtk_mutex *mutex,
			enum mtk_ddp_comp_id id)
{
	struct mtk_mutex_ctx *mtx = mtk_mutex_ctx_get(mutex);
	void __iomem *regs = mtk_mutex_reg_get(mutex, mtx);
	unsigned int reg;
	unsigned int sof_id;
	unsigned int offset;

	switch (id) {
	case DDP_COMPONENT_DSI0:
		sof_id = MUTEX_SOF_DSI0;
		break;
	case DDP_COMPONENT_DSI1:
		sof_id = MUTEX_SOF_DSI0;
		break;
	case DDP_COMPONENT_DSI2:
		sof_id = MUTEX_SOF_DSI2;
		break;
	case DDP_COMPONENT_DSI3:
		sof_id = MUTEX_SOF_DSI3;
		break;
	case DDP_COMPONENT_DPI0:
		sof_id = MUTEX_SOF_DPI0;
		break;
	case DDP_COMPONENT_DPI1:
		sof_id = MUTEX_SOF_DPI1;
		break;
	case DDP_COMPONENT_DP_INTF0:
		sof_id = MUTEX_SOF_DP_INTF0;
		break;
	case DDP_COMPONENT_DP_INTF1:
		sof_id = MUTEX_SOF_DP_INTF1;
		break;
	default:
		if (mtx->data->mutex_mod[id] <= BIT(31)) {
			offset = DISP_REG_MUTEX_MOD(mtx->data->mutex_mod_reg,
						    mutex->id);
			reg = readl_relaxed(regs + offset);
			reg |= mtx->data->mutex_mod[id];
			writel_relaxed(reg, regs + offset);
		} else {
			offset = DISP_REG_MUTEX_MOD2(mutex->id);
			reg = readl_relaxed(regs + offset);
			reg |= mtx->data->mutex_mod[id] >> 32;
			writel_relaxed(reg, regs + offset);
		}

		if (mutex->sub_disp_mutex && mtx->data->one_path_2nd_mutex_en) {
			if (mtx->data->mutex_mod[id] <= BIT(31)) {
				offset = DISP_REG_MUTEX_MOD(mtx->data->mutex_mod_reg,
							mtx->data->one_path_2nd_mutex_id);
				reg = readl_relaxed(regs + offset);
				reg |= mtx->data->one_path_2nd_mutex_mod[id];
				writel_relaxed(reg, regs + offset);
			} else {
				offset = DISP_REG_MUTEX_MOD2(mtx->data->one_path_2nd_mutex_id);
				reg = readl_relaxed(regs + offset);
				reg |= mtx->data->one_path_2nd_mutex_mod[id] >> 32;
				writel_relaxed(reg, regs + offset);
			}
		}
		return;
	}

	writel_relaxed(mtx->data->mutex_sof[sof_id],
		       regs +
		       DISP_REG_MUTEX_SOF(mtx->data->mutex_sof_reg, mutex->id));

	if (mutex->sub_disp_mutex && mtx->data->one_path_2nd_mutex_en) {
		writel_relaxed(mtx->data->mutex_sof[sof_id]|BIT(4),
			regs +
			DISP_REG_MUTEX_SOF(mtx->data->mutex_sof_reg,
			mtx->data->one_path_2nd_mutex_id));
	}
}
EXPORT_SYMBOL_GPL(mtk_mutex_add_comp);

void mtk_mutex_remove_comp(struct mtk_mutex *mutex,
			   enum mtk_ddp_comp_id id)
{
	struct mtk_mutex_ctx *mtx = mtk_mutex_ctx_get(mutex);
	void __iomem *mtx_reg = mtk_mutex_reg_get(mutex, mtx);
	unsigned int reg;
	unsigned int offset;

	switch (id) {
	case DDP_COMPONENT_DSI0:
	case DDP_COMPONENT_DSI1:
	case DDP_COMPONENT_DSI2:
	case DDP_COMPONENT_DSI3:
	case DDP_COMPONENT_DPI0:
	case DDP_COMPONENT_DPI1:
	case DDP_COMPONENT_DP_INTF0:
	case DDP_COMPONENT_DP_INTF1:
		writel_relaxed(MUTEX_SOF_SINGLE_MODE,
			       mtx_reg +
			       DISP_REG_MUTEX_SOF(mtx->data->mutex_sof_reg,
						  mutex->id));
		if (mutex->sub_disp_mutex && mtx->data->one_path_2nd_mutex_en) {
			writel_relaxed(MUTEX_SOF_SINGLE_MODE,
					   mtx_reg +
					   DISP_REG_MUTEX_SOF(mtx->data->mutex_sof_reg,
							  mtx->data->one_path_2nd_mutex_id));
		}
		break;
	default:
		if (mtx->data->mutex_mod[id] <= BIT(31)) {
			offset = DISP_REG_MUTEX_MOD(mtx->data->mutex_mod_reg,
						    mutex->id);
			reg = readl_relaxed(mtx_reg + offset);
			reg &= ~mtx->data->mutex_mod[id];
			writel_relaxed(reg, mtx_reg + offset);
		} else {
			offset = DISP_REG_MUTEX_MOD2(mutex->id);
			reg = readl_relaxed(mtx_reg + offset);
			reg &= ~(mtx->data->mutex_mod[id] >> 32);
			writel_relaxed(reg, mtx_reg + offset);
		}

		if (mutex->sub_disp_mutex && mtx->data->one_path_2nd_mutex_en) {
			if (mtx->data->mutex_mod[id] <= BIT(31)) {
				offset = DISP_REG_MUTEX_MOD(mtx->data->mutex_mod_reg,
								mtx->data->one_path_2nd_mutex_id);
				reg = readl_relaxed(mtx_reg + offset);
				reg &= ~mtx->data->one_path_2nd_mutex_mod[id];
				writel_relaxed(reg, mtx_reg + offset);
			} else {
				offset = DISP_REG_MUTEX_MOD2(mtx->data->one_path_2nd_mutex_id);
				reg = readl_relaxed(mtx_reg + offset);
				reg &= ~(mtx->data->one_path_2nd_mutex_mod[id] >> 32);
				writel_relaxed(reg, mtx_reg + offset);
			}
		}
		break;
	}
}
EXPORT_SYMBOL_GPL(mtk_mutex_remove_comp);

void mtk_mutex_enable(struct mtk_mutex *mutex)
{
	struct mtk_mutex_ctx *mtx = mtk_mutex_ctx_get(mutex);
	void __iomem *regs = mtk_mutex_reg_get(mutex, mtx);

	writel(1, regs + DISP_REG_MUTEX_EN(mutex->id));

	if (mutex->sub_disp_mutex && mtx->data->one_path_2nd_mutex_en)
		writel(1, regs + DISP_REG_MUTEX_EN(mtx->data->one_path_2nd_mutex_id));
}
EXPORT_SYMBOL_GPL(mtk_mutex_enable);

void mtk_mutex_disable(struct mtk_mutex *mutex)
{
	struct mtk_mutex_ctx *mtx = mtk_mutex_ctx_get(mutex);
	void __iomem *regs = mtk_mutex_reg_get(mutex, mtx);

	writel(0, regs + DISP_REG_MUTEX_EN(mutex->id));

	if (mutex->sub_disp_mutex && mtx->data->one_path_2nd_mutex_en)
		writel(0, regs + DISP_REG_MUTEX_EN(mtx->data->one_path_2nd_mutex_id));
}
EXPORT_SYMBOL_GPL(mtk_mutex_disable);

void mtk_mutex_acquire(struct mtk_mutex *mutex)
{
	struct mtk_mutex_ctx *mtx = mtk_mutex_ctx_get(mutex);
	void __iomem *regs = mtk_mutex_reg_get(mutex, mtx);
	u32 tmp;

	writel(1, regs + DISP_REG_MUTEX_EN(mutex->id));
	writel(1, regs + DISP_REG_MUTEX(mutex->id));
	if (readl_poll_timeout_atomic(regs + DISP_REG_MUTEX(mutex->id),
				      tmp, tmp & INT_MUTEX, 1, 10000))
		pr_debug("could not acquire mutex %d\n", mutex->id);

	if (mutex->sub_disp_mutex && mtx->data->one_path_2nd_mutex_en) {
		writel(1, regs + DISP_REG_MUTEX_EN(mtx->data->one_path_2nd_mutex_id));
		writel(1, regs + DISP_REG_MUTEX(mtx->data->one_path_2nd_mutex_id));
		if (readl_poll_timeout_atomic(regs +
					DISP_REG_MUTEX(mtx->data->one_path_2nd_mutex_id),
					tmp, tmp & INT_MUTEX, 1, 10000))
			pr_debug("could not acquire second mutex %d\n",
						mtx->data->one_path_2nd_mutex_id);
	}
}
EXPORT_SYMBOL_GPL(mtk_mutex_acquire);

void mtk_mutex_release(struct mtk_mutex *mutex)
{
	struct mtk_mutex_ctx *mtx = mtk_mutex_ctx_get(mutex);
	void __iomem *regs = mtk_mutex_reg_get(mutex, mtx);

	writel(0, regs + DISP_REG_MUTEX(mutex->id));

	if (mutex->sub_disp_mutex && mtx->data->one_path_2nd_mutex_en)
		writel(0, regs + DISP_REG_MUTEX(mtx->data->one_path_2nd_mutex_id));
}
EXPORT_SYMBOL_GPL(mtk_mutex_release);

static int mtk_mutex_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct mtk_mutex_ctx *mtx;
	struct resource *regs;
	int i;

	mtx = devm_kzalloc(dev, sizeof(*mtx), GFP_KERNEL);
	if (!mtx)
		return -ENOMEM;

	for (i = 0; i < 10; i++)
		mtx->mutex[i].id = i;

	mtx->data = of_device_get_match_data(dev);

	if (!mtx->data->no_clk) {
		mtx->clk = devm_clk_get(dev, NULL);
		if (IS_ERR(mtx->clk)) {
			if (PTR_ERR(mtx->clk) != -EPROBE_DEFER)
				dev_err(dev, "Failed to get clock\n");
			return PTR_ERR(mtx->clk);
		}
	}

	regs = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	mtx->regs = devm_ioremap_resource(dev, regs);
	if (IS_ERR(mtx->regs)) {
		dev_err(dev, "Failed to map mutex registers\n");
		return PTR_ERR(mtx->regs);
	}

	if (mtx->data->support_sub_disp_mutex) {
		for (i = 0; i < 10; i++) {
			mtx->mutex1[i].id = i;
			mtx->mutex1[i].sub_disp_mutex = true;
		}

		mtx->clk1 = devm_clk_get(dev, "sub_mutex");
		if (IS_ERR(mtx->clk1)) {
			if (PTR_ERR(mtx->clk1) != -EPROBE_DEFER)
				dev_err(dev, "Failed to get mutex clock 1\n");
			return PTR_ERR(mtx->clk1);
		}

		regs = platform_get_resource(pdev, IORESOURCE_MEM, 1);
		mtx->regs1 = devm_ioremap_resource(dev, regs);
		if (IS_ERR(mtx->regs1)) {
			dev_err(dev, "Failed to map mutex 1 registers\n");
			return PTR_ERR(mtx->regs1);
		}
	}

	platform_set_drvdata(pdev, mtx);

	return 0;
}

static int mtk_mutex_remove(struct platform_device *pdev)
{
	return 0;
}

static const struct of_device_id mutex_driver_dt_match[] = {
	{ .compatible = "mediatek,mt2701-disp-mutex",
	  .data = &mt2701_mutex_driver_data},
	{ .compatible = "mediatek,mt2712-disp-mutex",
	  .data = &mt2712_mutex_driver_data},
	{ .compatible = "mediatek,mt8167-disp-mutex",
	  .data = &mt8167_mutex_driver_data},
	{ .compatible = "mediatek,mt8173-disp-mutex",
	  .data = &mt8173_mutex_driver_data},
	{ .compatible = "mediatek,mt8183-disp-mutex",
	  .data = &mt8183_mutex_driver_data},
	{ .compatible = "mediatek,mt8195-disp-mutex",
	  .data = &mt8195_mutex_driver_data},
	{},
};
MODULE_DEVICE_TABLE(of, mutex_driver_dt_match);

struct platform_driver mtk_mutex_driver = {
	.probe		= mtk_mutex_probe,
	.remove		= mtk_mutex_remove,
	.driver		= {
		.name	= "mediatek-mutex",
		.owner	= THIS_MODULE,
		.of_match_table = mutex_driver_dt_match,
	},
};

builtin_platform_driver(mtk_mutex_driver);
