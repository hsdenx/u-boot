/*
 * Copyright (C) 2015 MSC Technologies
 *       <www.msc-technologies.eu>
 *
 * SPDX-License-Identifier:	GPL-2.0
 */
#include <asm/arch/clock.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/iomux.h>
#include <asm/arch/mx6-pins.h>
#include <asm/imx-common/iomux-v3.h>
#include <fsl_esdhc.h>
#include <asm/io.h>
#include <asm/arch/sys_proto.h>
#include <spl.h>
#include "boardinfo.h"

#define DRAM_STR "DRAM"
#define DRAM_PRINT(format, ...) \
	do { \
		printf("%s: ", DRAM_STR); \
		printf(format, ## __VA_ARGS__); \
	} \
	while (0)

#if defined(CONFIG_SPL_BUILD)
#include <asm/arch/mx6-ddr.h>

static struct mx6dq_iomux_ddr_regs mx6dq_ddr_ioregs = {
	.dram_cas     = 0x30,
	.dram_dqm0    = 0x28,
	.dram_dqm1    = 0x28,
	.dram_dqm2    = 0x28,
	.dram_dqm3    = 0x28,
	.dram_dqm4    = 0x28,
	.dram_dqm5    = 0x28,
	.dram_dqm6    = 0x28,
	.dram_dqm7    = 0x28,
	.dram_ras     = 0x30,
	.dram_reset   = 0x30,
	.dram_sdba2   = 0x00,
	.dram_sdcke0  = 0x30,
	.dram_sdcke1  = 0x30,
	.dram_sdclk_0 = 0x30,
	.dram_sdclk_1 = 0x30,
	.dram_sdodt0  = 0x30,
	.dram_sdodt1  = 0x30,
	.dram_sdqs0   = 0x28,
	.dram_sdqs1   = 0x28,
	.dram_sdqs2   = 0x28,
	.dram_sdqs3   = 0x28,
	.dram_sdqs4   = 0x28,
	.dram_sdqs5   = 0x28,
	.dram_sdqs6   = 0x28,
	.dram_sdqs7   = 0x28,
};

static struct mx6dq_iomux_grp_regs mx6dq_grp_ioregs = {
	.grp_b7ds        = 0x28,
	.grp_addds       = 0x30,
	.grp_ddrmode_ctl = 0x20000,
	.grp_ddrpke      = 0x00,
	.grp_ddrmode     = 0x20000,
	.grp_b0ds        = 0x28,
	.grp_ctlds       = 0x30,
	.grp_b1ds        = 0x28,
	.grp_ddr_type    = 0xc0000,
	.grp_b2ds        = 0x28,
	.grp_b3ds        = 0x28,
	.grp_b4ds        = 0x28,
	.grp_b5ds        = 0x28,
	.grp_b6ds        = 0x28,
};

static struct mx6sdl_iomux_ddr_regs mx6sdl_ddr_ioregs = {
	.dram_cas     = 0x30,
	.dram_dqm0    = 0x28,
	.dram_dqm1    = 0x28,
	.dram_dqm2    = 0x28,
	.dram_dqm3    = 0x28,
	.dram_dqm4    = 0x28,
	.dram_dqm5    = 0x28,
	.dram_dqm6    = 0x28,
	.dram_dqm7    = 0x28,
	.dram_ras     = 0x30,
	.dram_reset   = 0x30,
	.dram_sdba2   = 0x00,
	.dram_sdcke0  = 0x30,
	.dram_sdcke1  = 0x30,
	.dram_sdclk_0 = 0x30,
	.dram_sdclk_1 = 0x30,
	.dram_sdodt0  = 0x30,
	.dram_sdodt1  = 0x30,
	.dram_sdqs0   = 0x28,
	.dram_sdqs1   = 0x28,
	.dram_sdqs2   = 0x28,
	.dram_sdqs3   = 0x28,
	.dram_sdqs4   = 0x28,
	.dram_sdqs5   = 0x28,
	.dram_sdqs6   = 0x28,
	.dram_sdqs7   = 0x28,
};

static struct mx6sdl_iomux_grp_regs mx6sdl_grp_ioregs = {
	.grp_b7ds        = 0x28,
	.grp_addds       = 0x30,
	.grp_ddrmode_ctl = 0x20000,
	.grp_ddrpke      = 0x00,
	.grp_ddrmode     = 0x20000,
	.grp_b0ds        = 0x28,
	.grp_ctlds       = 0x30,
	.grp_b1ds        = 0x28,
	.grp_ddr_type    = 0xc0000,
	.grp_b2ds        = 0x28,
	.grp_b3ds        = 0x28,
	.grp_b4ds        = 0x28,
	.grp_b5ds        = 0x28,
	.grp_b6ds        = 0x28,
};

static struct mx6_mmdc_calibration mx6dq_calib = {
	.p0_mpwldectrl0 = 0x001f001f,
	.p0_mpwldectrl1 = 0x001f001f,
	.p0_mpdgctrl0   = 0x43260335,
	.p0_mpdgctrl1   = 0x031a030b,
	.p0_mprddlctl   = 0x37373737,
	.p0_mpwrdlctl   = 0x41444840,

	.p1_mpwldectrl0 = 0x001f001f,
	.p1_mpwldectrl1 = 0x001f001f,
	.p1_mpdgctrl0   = 0x4323033b,
	.p1_mpdgctrl1   = 0x0323026f,
	.p1_mprddlctl   = 0x37373737,
	.p1_mpwrdlctl   = 0x4835483e,
};

static struct mx6_mmdc_calibration mx6sdl_calib = {
	.p0_mpwldectrl0 = 0x001f001f,
	.p0_mpwldectrl1 = 0x001f001f,
	.p0_mpdgctrl0   = 0x42190217,
	.p0_mpdgctrl1   = 0x017b017b,
	.p0_mprddlctl   = 0x38383838,
	.p0_mpwrdlctl   = 0x3f3f3f40,

	.p1_mpwldectrl0 = 0x001f001f,
	.p1_mpwldectrl1 = 0x001f001f,
	.p1_mpdgctrl0   = 0x4176017b,
	.p1_mpdgctrl1   = 0x015f016c,
	.p1_mprddlctl   = 0x38383838,
	.p1_mpwrdlctl   = 0x3538382e,
};

static struct mx6_ddr3_cfg NT5CC128M16FP_DII = {
	/* DDR3/L 1600 11-11-11 */
	/* ie 1600 for DDR3-1600 (800,1066,1333,1600) */
	.mem_speed = 1600,
	/* chip density (Gb) (1,2,4,8) */
	.density   = 2,
	/* bus width (bits) (4,8,16) */
	.width     = 16,
	/* number of banks */
	.banks     = 8,
	/* row address bits (11-16)*/
	.rowaddr   = 14,
	/* col address bits (9-12) */
	.coladdr   = 10,
	/* page size (K) (1-2) */
	.pagesz    = 2,
	/* tRCD=tRP=CL (ns*100) */
	.trcd      = 1325,
	/* tRAS min (ns*100) */
	.trcmin    = 4875,
	/* tRAS min (ns*100) */
	.trasmin   = 3500,
	/* self-refresh temperature: 0=normal, 1=extended */
	.SRT       = 1,
};

static struct mx6_ddr3_cfg NT5CC128M16IP_DI = {
	/* DDR3/L 1600 11-11-11 */
	/* ie 1600 for DDR3-1600 (800,1066,1333,1600) */
	.mem_speed = 1600,
	/* chip density (Gb) (1,2,4,8) */
	.density   = 2,
	/* bus width (bits) (4,8,16) */
	.width     = 16,
	/* number of banks */
	.banks     = 8,
	/* row address bits (11-16)*/
	.rowaddr   = 14,
	/* col address bits (9-12) */
	.coladdr   = 10,
	/* page size (K) (1-2) */
	.pagesz    = 2,
	/* tRCD=tRP=CL (ns*100) */
	.trcd      = 1325,
	/* tRAS min (ns*100) */
	.trcmin    = 4875,
	/* tRAS min (ns*100) */
	.trasmin   = 3500,
	/* self-refresh temperature: 0=normal, 1=extended */
	.SRT       = 1,
};

static struct mx6_ddr3_cfg NT5CC256M16CP_DII = {
	/* DDR3/L 1600 11-11-11 */
	/* ie 1600 for DDR3-1600 (800,1066,1333,1600) */
	.mem_speed = 1600,
	/* chip density (Gb) (1,2,4,8) */
	.density   = 4,
	/* bus width (bits) (4,8,16) */
	.width     = 16,
	/* number of banks */
	.banks     = 8,
	/* row address bits (11-16)*/
	.rowaddr   = 15,
	/* col address bits (9-12) */
	.coladdr   = 10,
	/* page size (K) (1-2) */
	.pagesz    = 2,
	/* tRCD=tRP=CL (ns*100) */
	.trcd      = 1375,
	/* tRAS min (ns*100) */
	.trcmin    = 4875,
	/* tRAS min (ns*100) */
	.trasmin   = 3500,
	/* self-refresh temperature: 0=normal, 1=extended */
	.SRT       = 1,
};

static struct mx6_ddr3_cfg NT5CC256M16DP_DI = {
	/* DDR3/L 1600 11-11-11 */
	/* ie 1600 for DDR3-1600 (800,1066,1333,1600) */
	.mem_speed = 1600,
	/* chip density (Gb) (1,2,4,8) */
	.density   = 4,
	/* bus width (bits) (4,8,16) */
	.width     = 16,
	/* number of banks */
	.banks     = 8,
	/* row address bits (11-16)*/
	.rowaddr   = 15,
	/* col address bits (9-12) */
	.coladdr   = 10,
	/* page size (K) (1-2) */
	.pagesz    = 2,
	/* tRCD=tRP=CL (ns*100) */
	.trcd      = 1375,
	/* tRAS min (ns*100) */
	.trcmin    = 4875,
	/* tRAS min (ns*100) */
	.trasmin   = 3500,
	/* self-refresh temperature: 0=normal, 1=extended */
	.SRT       = 1,
};

/* System Information: Varies per board design, layout, and term choices */
static struct mx6_ddr_sysinfo mem_64bit = {
	/* size of bus (in dwords: 0=16bit,1=32bit,2=64bit) */
	.dsize      = 2,
	.cs1_mirror = 0,            /* enable address mirror (0|1) */
	/* config for full 4GB range so that get_mem_size() works */
	.cs_density = 32,           /* density per chip select (Gb) */
	.ncs        = 1,            /* number chip selects used (1|2) */
	.bi_on      = 1,            /* Bank interleaving enable */
	.rtt_nom    = 2,            /* Rtt_Nom (DDR3_RTT_*) */
	.rtt_wr     = 2,            /* Rtt_Wr (DDR3_RTT_*) */
	.ralat      = 5,            /* Read Additional Latency (0-7) */
	.walat      = 0,            /* Write Additional Latency (0-3) */
	.mif3_mode  = 3,            /* Command prediction working mode */
	.rst_to_cke = 0x23,         /* Time from SDE enable to CKE rise */
	/* Time from SDE enable until DDR reset# is high */
	.sde_to_rst = 0x10,
};

static struct mx6_ddr_sysinfo mem_32bit = {
	/* size of bus (in dwords: 0=16bit,1=32bit,2=64bit) */
	.dsize      = 1,
	.cs1_mirror = 0,            /* enable address mirror (0|1) */
	/* config for full 4GB range so that get_mem_size() works */
	.cs_density = 32,           /* density per chip select (Gb) */
	.ncs        = 1,            /* number chip selects used (1|2) */
	.bi_on      = 1,            /* Bank interleaving enable */
	.rtt_nom    = 2,            /* Rtt_Nom (DDR3_RTT_*) */
	.rtt_wr     = 2,            /* Rtt_Wr (DDR3_RTT_*) */
	.ralat      = 5,            /* Read Additional Latency (0-7) */
	.walat      = 0,            /* Write Additional Latency (0-3) */
	.mif3_mode  = 3,            /* Command prediction working mode */
	.rst_to_cke = 0x23,         /* Time from SDE enable to CKE rise */
	/* Time from SDE enable until DDR reset# is high */
	.sde_to_rst = 0x10,
};

struct dram_settings {
	char *variant;
	unsigned width;
	struct mx6_ddr_sysinfo *mem;
	struct mx6_mmdc_calibration *calib;
	struct mx6_ddr3_cfg *cfg;
};

static struct dram_settings dram_variants[] = {
	/* old nomenclature */
	{ "003", 32, &mem_32bit, &mx6sdl_calib, &NT5CC256M16CP_DII },
	{ "004", 64, &mem_64bit, &mx6sdl_calib, &NT5CC256M16CP_DII },
	{ "005", 64, &mem_64bit, &mx6dq_calib,  &NT5CC256M16CP_DII },
	{ "006", 32, &mem_32bit, &mx6dq_calib,  &NT5CC128M16FP_DII },
	{ "008", 64, &mem_64bit, &mx6sdl_calib, &NT5CC128M16IP_DI  },
	{ "009", 32, &mem_32bit, &mx6sdl_calib, &NT5CC128M16FP_DII },
	{ "010", 64, &mem_64bit, &mx6sdl_calib, &NT5CC128M16FP_DII },
	{ "011", 64, &mem_64bit, &mx6dq_calib,  &NT5CC256M16CP_DII },
	{ "012", 64, &mem_64bit, &mx6sdl_calib, &NT5CC256M16CP_DII },
	{ "032", 32, &mem_32bit, &mx6sdl_calib, &NT5CC256M16DP_DI  },

	/* new nomenclature */
	{ "112", 32, &mem_32bit, &mx6sdl_calib, &NT5CC256M16CP_DII },
	{ "123", 64, &mem_64bit, &mx6dq_calib,  &NT5CC256M16CP_DII },
	{ "133", 64, &mem_64bit, &mx6sdl_calib, &NT5CC256M16CP_DII },
	{ "143", 64, &mem_64bit, &mx6dq_calib,  &NT5CC256M16CP_DII },

	/* sentinel */
	{  NULL, 0,  NULL ,      NULL,          NULL },
};

static const struct dram_settings *find_dram_settings(const char *variant)
{
	struct dram_settings *ptr;

	for (ptr = dram_variants; ptr->variant; ptr++) {
		if (strcmp(ptr->variant, variant) == 0)
			return ptr;
	}
	return NULL;
}

int spl_dram_init(const struct board_info *info)
{
	const struct dram_settings *settings;

	if (!info || !BI_HAS_VK(info))
		goto defaults;

	settings = find_dram_settings(info->variant_key);
	if (settings) {
		switch (get_cpu_type())	{
		case MXC_CPU_MX6SOLO:
		case MXC_CPU_MX6DL:
			mx6sdl_dram_iocfg(settings->width,
					  &mx6sdl_ddr_ioregs,
					  &mx6sdl_grp_ioregs);
			break;
		case MXC_CPU_MX6D:
		case MXC_CPU_MX6Q:
			mx6dq_dram_iocfg(settings->width,
					 &mx6dq_ddr_ioregs,
					 &mx6dq_grp_ioregs);
			break;
		}
		mx6_dram_cfg(settings->mem, settings->calib, settings->cfg);
		goto finish;
	}

	DRAM_PRINT("Warning: unknown or not supported module variant '%s'. "
		   "Trying conservative defaults!\n",
		   info->variant_key);

defaults:
	switch (get_cpu_type())	{
	case MXC_CPU_MX6SOLO:
		mx6sdl_dram_iocfg(32, &mx6sdl_ddr_ioregs,
				  &mx6sdl_grp_ioregs);
		mx6_dram_cfg(&mem_32bit, &mx6sdl_calib,
			     &NT5CC256M16CP_DII);
		break;
	case MXC_CPU_MX6DL:
		mx6sdl_dram_iocfg(64, &mx6sdl_ddr_ioregs,
				  &mx6sdl_grp_ioregs);
		mx6_dram_cfg(&mem_32bit, &mx6sdl_calib,
			     &NT5CC256M16CP_DII);
		break;
	case MXC_CPU_MX6D:
	case MXC_CPU_MX6Q:
		mx6dq_dram_iocfg(64, &mx6dq_ddr_ioregs,
				 &mx6dq_grp_ioregs);
		mx6_dram_cfg(&mem_32bit, &mx6dq_calib,
			     &NT5CC256M16CP_DII);
		break;
	}

finish:
	udelay(100);
	return 0;
}
#endif /* defined(CONFIG_SPL_BUILD) */
