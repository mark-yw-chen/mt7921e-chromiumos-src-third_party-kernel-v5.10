// SPDX-License-Identifier: GPL-2.0-only
//
// Copyright (c) 2020 MediaTek Inc.
// Author: Weiyi Lu <weiyi.lu@mediatek.com>

#include <linux/clk-provider.h>
#include <linux/platform_device.h>

#include "clk-mtk.h"
#include "clk-gate.h"

#include <dt-bindings/clock/mt8195-clk.h>

static const struct mtk_gate_regs wpe_cg_regs = {
	.set_ofs = 0x0,
	.clr_ofs = 0x0,
	.sta_ofs = 0x0,
};

static const struct mtk_gate_regs wpe_vpp0_cg_regs = {
	.set_ofs = 0x58,
	.clr_ofs = 0x58,
	.sta_ofs = 0x58,
};

static const struct mtk_gate_regs wpe_vpp1_cg_regs = {
	.set_ofs = 0x5c,
	.clr_ofs = 0x5c,
	.sta_ofs = 0x5c,
};

#define GATE_WPE(_id, _name, _parent, _shift)			\
	GATE_MTK(_id, _name, _parent, &wpe_cg_regs, _shift, &mtk_clk_gate_ops_no_setclr_inv)

#define GATE_WPE_VPP0(_id, _name, _parent, _shift)			\
	GATE_MTK(_id, _name, _parent, &wpe_vpp0_cg_regs, _shift, &mtk_clk_gate_ops_no_setclr_inv)

#define GATE_WPE_VPP1(_id, _name, _parent, _shift)			\
	GATE_MTK(_id, _name, _parent, &wpe_vpp1_cg_regs, _shift, &mtk_clk_gate_ops_no_setclr_inv)

static const struct mtk_gate wpe_clks[] = {
	GATE_WPE(CLK_WPE_VPP0, "wpe_vpp0", "wpe_vpp_sel", 16),
	GATE_WPE(CLK_WPE_VPP1, "wpe_vpp1", "wpe_vpp_sel", 17),
	GATE_WPE(CLK_WPE_SMI_LARB7, "wpe_smi_larb7", "wpe_vpp_sel", 18),
	GATE_WPE(CLK_WPE_SMI_LARB8, "wpe_smi_larb8", "wpe_vpp_sel", 19),
	GATE_WPE(CLK_WPE_EVENT_TX, "wpe_event_tx", "wpe_vpp_sel", 20),
	GATE_WPE(CLK_WPE_SMI_LARB7_P, "wpe_smi_larb7_p", "wpe_vpp_sel", 24),
	GATE_WPE(CLK_WPE_SMI_LARB8_P, "wpe_smi_larb8_p", "wpe_vpp_sel", 25),
};

static const struct mtk_gate wpe_vpp0_clks[] = {
	/* WPE_VPP0 */
	GATE_WPE_VPP0(CLK_WPE_VPP0_VGEN, "wpe_vpp0_vgen", "img_sel", 0),
	GATE_WPE_VPP0(CLK_WPE_VPP0_EXT, "wpe_vpp0_ext", "img_sel", 1),
	GATE_WPE_VPP0(CLK_WPE_VPP0_VFC, "wpe_vpp0_vfc", "img_sel", 2),
	GATE_WPE_VPP0(CLK_WPE_VPP0_CACH0_TOP, "wpe_vpp0_cach0_top", "img_sel", 3),
	GATE_WPE_VPP0(CLK_WPE_VPP0_CACH0_DMA, "wpe_vpp0_cach0_dma", "img_sel", 4),
	GATE_WPE_VPP0(CLK_WPE_VPP0_CACH1_TOP, "wpe_vpp0_cach1_top", "img_sel", 5),
	GATE_WPE_VPP0(CLK_WPE_VPP0_CACH1_DMA, "wpe_vpp0_cach1_dma", "img_sel", 6),
	GATE_WPE_VPP0(CLK_WPE_VPP0_CACH2_TOP, "wpe_vpp0_cach2_top", "img_sel", 7),
	GATE_WPE_VPP0(CLK_WPE_VPP0_CACH2_DMA, "wpe_vpp0_cach2_dma", "img_sel", 8),
	GATE_WPE_VPP0(CLK_WPE_VPP0_CACH3_TOP, "wpe_vpp0_cach3_top", "img_sel", 9),
	GATE_WPE_VPP0(CLK_WPE_VPP0_CACH3_DMA, "wpe_vpp0_cach3_dma", "img_sel", 10),
	GATE_WPE_VPP0(CLK_WPE_VPP0_PSP, "wpe_vpp0_psp", "img_sel", 11),
	GATE_WPE_VPP0(CLK_WPE_VPP0_PSP2, "wpe_vpp0_psp2", "img_sel", 12),
	GATE_WPE_VPP0(CLK_WPE_VPP0_SYNC, "wpe_vpp0_sync", "img_sel", 13),
	GATE_WPE_VPP0(CLK_WPE_VPP0_C24, "wpe_vpp0_c24", "img_sel", 14),
	GATE_WPE_VPP0(CLK_WPE_VPP0_MDP_CROP, "wpe_vpp0_mdp_crop", "img_sel", 15),
	GATE_WPE_VPP0(CLK_WPE_VPP0_ISP_CROP, "wpe_vpp0_isp_crop", "img_sel", 16),
	GATE_WPE_VPP0(CLK_WPE_VPP0_TOP, "wpe_vpp0_top", "img_sel", 17),
	/* WPE_VPP1 */
	GATE_WPE_VPP1(CLK_WPE_VPP0_VECI, "wpe_vpp0_veci", "img_sel", 0),
	GATE_WPE_VPP1(CLK_WPE_VPP0_VEC2I, "wpe_vpp0_vec2i", "img_sel", 1),
	GATE_WPE_VPP1(CLK_WPE_VPP0_VEC3I, "wpe_vpp0_vec3i", "img_sel", 2),
	GATE_WPE_VPP1(CLK_WPE_VPP0_WPEO, "wpe_vpp0_wpeo", "img_sel", 3),
	GATE_WPE_VPP1(CLK_WPE_VPP0_MSKO, "wpe_vpp0_msko", "img_sel", 4),
};

static const struct mtk_gate wpe_vpp1_clks[] = {
	/* WPE_VPP0 */
	GATE_WPE_VPP0(CLK_WPE_VPP1_VGEN, "wpe_vpp1_vgen", "img_sel", 0),
	GATE_WPE_VPP0(CLK_WPE_VPP1_EXT, "wpe_vpp1_ext", "img_sel", 1),
	GATE_WPE_VPP0(CLK_WPE_VPP1_VFC, "wpe_vpp1_vfc", "img_sel", 2),
	GATE_WPE_VPP0(CLK_WPE_VPP1_CACH0_TOP, "wpe_vpp1_cach0_top", "img_sel", 3),
	GATE_WPE_VPP0(CLK_WPE_VPP1_CACH0_DMA, "wpe_vpp1_cach0_dma", "img_sel", 4),
	GATE_WPE_VPP0(CLK_WPE_VPP1_CACH1_TOP, "wpe_vpp1_cach1_top", "img_sel", 5),
	GATE_WPE_VPP0(CLK_WPE_VPP1_CACH1_DMA, "wpe_vpp1_cach1_dma", "img_sel", 6),
	GATE_WPE_VPP0(CLK_WPE_VPP1_CACH2_TOP, "wpe_vpp1_cach2_top", "img_sel", 7),
	GATE_WPE_VPP0(CLK_WPE_VPP1_CACH2_DMA, "wpe_vpp1_cach2_dma", "img_sel", 8),
	GATE_WPE_VPP0(CLK_WPE_VPP1_CACH3_TOP, "wpe_vpp1_cach3_top", "img_sel", 9),
	GATE_WPE_VPP0(CLK_WPE_VPP1_CACH3_DMA, "wpe_vpp1_cach3_dma", "img_sel", 10),
	GATE_WPE_VPP0(CLK_WPE_VPP1_PSP, "wpe_vpp1_psp", "img_sel", 11),
	GATE_WPE_VPP0(CLK_WPE_VPP1_PSP2, "wpe_vpp1_psp2", "img_sel", 12),
	GATE_WPE_VPP0(CLK_WPE_VPP1_SYNC, "wpe_vpp1_sync", "img_sel", 13),
	GATE_WPE_VPP0(CLK_WPE_VPP1_C24, "wpe_vpp1_c24", "img_sel", 14),
	GATE_WPE_VPP0(CLK_WPE_VPP1_MDP_CROP, "wpe_vpp1_mdp_crop", "img_sel", 15),
	GATE_WPE_VPP0(CLK_WPE_VPP1_ISP_CROP, "wpe_vpp1_isp_crop", "img_sel", 16),
	GATE_WPE_VPP0(CLK_WPE_VPP1_TOP, "wpe_vpp1_top", "img_sel", 17),
	/* WPE_VPP1 */
	GATE_WPE_VPP1(CLK_WPE_VPP1_VECI, "wpe_vpp1_veci", "img_sel", 0),
	GATE_WPE_VPP1(CLK_WPE_VPP1_VEC2I, "wpe_vpp1_vec2i", "img_sel", 1),
	GATE_WPE_VPP1(CLK_WPE_VPP1_VEC3I, "wpe_vpp1_vec3i", "img_sel", 2),
	GATE_WPE_VPP1(CLK_WPE_VPP1_WPEO, "wpe_vpp1_wpeo", "img_sel", 3),
	GATE_WPE_VPP1(CLK_WPE_VPP1_MSKO, "wpe_vpp1_msko", "img_sel", 4),
};

static const struct mtk_clk_desc wpe_desc = {
	.clks = wpe_clks,
	.num_clks = ARRAY_SIZE(wpe_clks),
};

static const struct mtk_clk_desc wpe_vpp0_desc = {
	.clks = wpe_vpp0_clks,
	.num_clks = ARRAY_SIZE(wpe_vpp0_clks),
};

static const struct mtk_clk_desc wpe_vpp1_desc = {
	.clks = wpe_vpp1_clks,
	.num_clks = ARRAY_SIZE(wpe_vpp1_clks),
};

static const struct of_device_id of_match_clk_mt8195_wpe[] = {
	{
		.compatible = "mediatek,mt8195-wpesys",
		.data = &wpe_desc,
	}, {
		.compatible = "mediatek,mt8195-wpesys_vpp0",
		.data = &wpe_vpp0_desc,
	}, {
		.compatible = "mediatek,mt8195-wpesys_vpp1",
		.data = &wpe_vpp1_desc,
	}, {
		/* sentinel */
	}
};

static struct platform_driver clk_mt8195_wpe_drv = {
	.probe = mtk_clk_simple_probe,
	.driver = {
		.name = "clk-mt8195-wpe",
		.of_match_table = of_match_clk_mt8195_wpe,
	},
};

builtin_platform_driver(clk_mt8195_wpe_drv);
