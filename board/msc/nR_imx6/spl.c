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
#include <asm/gpio.h>
#include <asm/imx-common/iomux-v3.h>
#include <asm/imx-common/video.h>
#include <mmc.h>
#include <fsl_esdhc.h>
#include <asm/arch/crm_regs.h>
#include <asm/io.h>
#include <asm/arch/sys_proto.h>
#include <spl.h>

#include "boardinfo.h"
#include "init.h"

DECLARE_GLOBAL_DATA_PTR;

#include "dram.h"

static void ccgr_init(void)
{
	struct mxc_ccm_reg *ccm = (struct mxc_ccm_reg *)CCM_BASE_ADDR;

	writel(0x00C03F3F, &ccm->CCGR0);
	writel(0x0030FCF3, &ccm->CCGR1);
	writel(0x0FFFCFC0, &ccm->CCGR2);
	writel(0x3FF00000, &ccm->CCGR3);
	writel(0xFFFFF300, &ccm->CCGR4);
	writel(0x0F0000F3, &ccm->CCGR5);
	writel(0x00000FFF, &ccm->CCGR6);
}

static void gpr_init(void)
{
	struct iomuxc *iomux = (struct iomuxc *)IOMUXC_BASE_ADDR;

	/* enable AXI cache for VDOA/VPU/IPU */
	writel(0xF00000CF, &iomux->gpr[4]);
	/* set IPU AXI-id0 Qos=0xf(bypass) AXI-id1 Qos=0x7 */
	writel(0x007F007F, &iomux->gpr[6]);
	writel(0x007F007F, &iomux->gpr[7]);
}

static bool check_cpu(void)
{
	switch (get_cpu_type())	{
	case MXC_CPU_MX6SOLO:
	case MXC_CPU_MX6DL:
	case MXC_CPU_MX6D:
	case MXC_CPU_MX6Q:
			return true;
	}

	printf("Abort, unknown or unsupported CPU type detected.\n");
	return false;
}

void board_init_f(ulong dummy)
{
	int ret = 0;
	struct board_info info;

	ccgr_init();

	/* setup AIPS and disable watchdog */
	arch_cpu_init();

	gpr_init();

	/* iomux */
	board_early_init_f();

	/* setup GP timer */
	timer_init();

	/* UART clocks enabled and gd valid - init serial console */
	preloader_console_init();

	board_setup_i2c2();

	if (!check_cpu())
		hang();

	ret = boardinfo_get(&info);
	if (ret)
		printf("Warning: Boardinfo not found.\n");
	else
		boardinfo_print(&info);

	/* DDR initialization */
	ret = spl_dram_init(&info);
	if (ret)
		hang();

	/* Clear the BSS. */
	memset(__bss_start, 0, __bss_end - __bss_start);

	/* load/boot image from boot device */
	board_init_r(NULL, 0);
}

void spl_board_init(void)
{
	/* determine boot device from SRC_SBMR1 (BOOT_CFG[4:1]) or SRC_GPR9 */
	u32 boot_device = spl_boot_device();

	switch (boot_device) {
	case BOOT_DEVICE_MMC1:
		puts("Booting from MMC1\n");
		break;
	case BOOT_DEVICE_MMC2:
		puts("Booting from MMC2\n");
		break;
	case BOOT_DEVICE_NAND:
		puts("Booting from NAND\n");
		break;
	case BOOT_DEVICE_SPI:
		puts("Booting from SPI\n");
		break;
	case BOOT_DEVICE_SATA:
		puts("Booting from SATA\n");
		break;
	default:
		puts("Unknown boot device\n");
	}
}
