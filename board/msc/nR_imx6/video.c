/*
 * Copyright (C) 2016 MSC Technologies
 *       <www.msc-technologies.eu>
 *
 * SPDX-License-Identifier:	GPL-2.0
 */
#include <asm/arch/clock.h>
#include <asm/arch/crm_regs.h>
#include <asm/arch/iomux.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/mx6-pins.h>
#include <asm/arch/mxc_hdmi.h>
#include <asm/arch/sys_proto.h>
#include <asm/gpio.h>
#include <asm/imx-common/iomux-v3.h>
#include <asm/imx-common/mxc_i2c.h>
#include <asm/imx-common/fbpanel.h>
#include <asm/imx-common/video.h>
#include <asm/io.h>
#include <linux/sizes.h>
#include <input.h>
#include <i2c.h>
#include "boardinfo.h"
#include "common.h"

#if !defined(CONFIG_SPL_BUILD)
#define PWM1_GPIO		IMX_GPIO_NR(1, 9)
#define LCD_POW_EN_GPIO	IMX_GPIO_NR(2, 31)
#define LCD_BL_EN_GPIO		IMX_GPIO_NR(3, 16)

static iomux_v3_cfg_t const display_common_pads[] = {
	IOMUX_PADS(PAD_GPIO_9__GPIO1_IO09 | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_EIM_EB3__GPIO2_IO31 | MUX_PAD_CTRL(NO_PAD_CTRL)),
	IOMUX_PADS(PAD_EIM_D16__GPIO3_IO16 | MUX_PAD_CTRL(NO_PAD_CTRL)),
};

static void setup_display_common(void)
{
	SETUP_IOMUX_PADS(display_common_pads);

	gpio_direction_input(PWM1_GPIO);
	gpio_direction_input(LCD_POW_EN_GPIO);
	gpio_direction_input(LCD_BL_EN_GPIO);
}

static void enable_display_common(void)
{
	gpio_direction_output(PWM1_GPIO, 1);
	gpio_direction_output(LCD_POW_EN_GPIO, 0);
	gpio_direction_output(LCD_BL_EN_GPIO, 0);
}

static void disable_display_common(void)
{
	gpio_direction_output(PWM1_GPIO, 0);
	gpio_direction_output(LCD_POW_EN_GPIO, 1);
	gpio_direction_output(LCD_BL_EN_GPIO, 1);
}

#if defined(CONFIG_SYS_USE_LCD_VIDEO)
#define PC_RGB MUX_PAD_CTRL(RGB_PAD_CTRL)
#define PC_RGB_W MUX_PAD_CTRL(WEAK_PULLUP)
static iomux_v3_cfg_t const rgb_pads[] = {
	IOMUX_PADS(PAD_DI0_DISP_CLK__IPU1_DI0_DISP_CLK | PC_RGB),
	IOMUX_PADS(PAD_DI0_PIN2__IPU1_DI0_PIN02 | PC_RGB),
	IOMUX_PADS(PAD_DI0_PIN3__IPU1_DI0_PIN03 | PC_RGB),
	IOMUX_PADS(PAD_DI0_PIN15__IPU1_DI0_PIN15 | PC_RGB),
	IOMUX_PADS(PAD_DISP0_DAT0__IPU1_DISP0_DATA00 | PC_RGB),
	IOMUX_PADS(PAD_DISP0_DAT1__IPU1_DISP0_DATA01 | PC_RGB),
	IOMUX_PADS(PAD_DISP0_DAT2__IPU1_DISP0_DATA02 | PC_RGB),
	IOMUX_PADS(PAD_DISP0_DAT3__IPU1_DISP0_DATA03 | PC_RGB),
	IOMUX_PADS(PAD_DISP0_DAT4__IPU1_DISP0_DATA04 | PC_RGB),
	IOMUX_PADS(PAD_DISP0_DAT5__IPU1_DISP0_DATA05 | PC_RGB),
	IOMUX_PADS(PAD_DISP0_DAT6__IPU1_DISP0_DATA06 | PC_RGB),
	IOMUX_PADS(PAD_DISP0_DAT7__IPU1_DISP0_DATA07 | PC_RGB),
	IOMUX_PADS(PAD_DISP0_DAT8__IPU1_DISP0_DATA08 | PC_RGB),
	IOMUX_PADS(PAD_DISP0_DAT9__IPU1_DISP0_DATA09 | PC_RGB),
	IOMUX_PADS(PAD_DISP0_DAT10__IPU1_DISP0_DATA10 | PC_RGB),
	IOMUX_PADS(PAD_DISP0_DAT11__IPU1_DISP0_DATA11 | PC_RGB),
	IOMUX_PADS(PAD_DISP0_DAT12__IPU1_DISP0_DATA12 | PC_RGB),
	IOMUX_PADS(PAD_DISP0_DAT13__IPU1_DISP0_DATA13 | PC_RGB),
	IOMUX_PADS(PAD_DISP0_DAT14__IPU1_DISP0_DATA14 | PC_RGB),
	IOMUX_PADS(PAD_DISP0_DAT15__IPU1_DISP0_DATA15 | PC_RGB),
	IOMUX_PADS(PAD_DISP0_DAT16__IPU1_DISP0_DATA16 | PC_RGB),
	IOMUX_PADS(PAD_DISP0_DAT17__IPU1_DISP0_DATA17 | PC_RGB),
	IOMUX_PADS(PAD_DISP0_DAT18__IPU1_DISP0_DATA18 | PC_RGB),
	IOMUX_PADS(PAD_DISP0_DAT19__IPU1_DISP0_DATA19 | PC_RGB),
	IOMUX_PADS(PAD_DISP0_DAT20__IPU1_DISP0_DATA20 | PC_RGB),
	IOMUX_PADS(PAD_DISP0_DAT21__IPU1_DISP0_DATA21 | PC_RGB),
	IOMUX_PADS(PAD_DISP0_DAT22__IPU1_DISP0_DATA22 | PC_RGB),
	IOMUX_PADS(PAD_DISP0_DAT23__IPU1_DISP0_DATA23 | PC_RGB),
};

static const iomux_v3_cfg_t rgb_gpio_pads[] = {
	IOMUX_PADS(PAD_DI0_DISP_CLK__IPU1_DI0_DISP_CLK | PC_RGB_W),
	IOMUX_PADS(PAD_DI0_PIN2__IPU1_DI0_PIN02 | PC_RGB_W),
	IOMUX_PADS(PAD_DI0_PIN3__IPU1_DI0_PIN03 | PC_RGB_W),
	IOMUX_PADS(PAD_DI0_PIN15__IPU1_DI0_PIN15 | PC_RGB_W),
	IOMUX_PADS(PAD_DISP0_DAT0__IPU1_DISP0_DATA00 | PC_RGB_W),
	IOMUX_PADS(PAD_DISP0_DAT1__IPU1_DISP0_DATA01 | PC_RGB_W),
	IOMUX_PADS(PAD_DISP0_DAT2__IPU1_DISP0_DATA02 | PC_RGB_W),
	IOMUX_PADS(PAD_DISP0_DAT3__IPU1_DISP0_DATA03 | PC_RGB_W),
	IOMUX_PADS(PAD_DISP0_DAT4__IPU1_DISP0_DATA04 | PC_RGB_W),
	IOMUX_PADS(PAD_DISP0_DAT5__IPU1_DISP0_DATA05 | PC_RGB_W),
	IOMUX_PADS(PAD_DISP0_DAT6__IPU1_DISP0_DATA06 | PC_RGB_W),
	IOMUX_PADS(PAD_DISP0_DAT7__IPU1_DISP0_DATA07 | PC_RGB_W),
	IOMUX_PADS(PAD_DISP0_DAT8__IPU1_DISP0_DATA08 | PC_RGB_W),
	IOMUX_PADS(PAD_DISP0_DAT9__IPU1_DISP0_DATA09 | PC_RGB_W),
	IOMUX_PADS(PAD_DISP0_DAT10__IPU1_DISP0_DATA10 | PC_RGB_W),
	IOMUX_PADS(PAD_DISP0_DAT11__IPU1_DISP0_DATA11 | PC_RGB_W),
	IOMUX_PADS(PAD_DISP0_DAT12__IPU1_DISP0_DATA12 | PC_RGB_W),
	IOMUX_PADS(PAD_DISP0_DAT13__IPU1_DISP0_DATA13 | PC_RGB_W),
	IOMUX_PADS(PAD_DISP0_DAT14__IPU1_DISP0_DATA14 | PC_RGB_W),
	IOMUX_PADS(PAD_DISP0_DAT15__IPU1_DISP0_DATA15 | PC_RGB_W),
	IOMUX_PADS(PAD_DISP0_DAT16__IPU1_DISP0_DATA16 | PC_RGB_W),
	IOMUX_PADS(PAD_DISP0_DAT17__IPU1_DISP0_DATA17 | PC_RGB_W),
	IOMUX_PADS(PAD_DISP0_DAT18__IPU1_DISP0_DATA18 | PC_RGB_W),
	IOMUX_PADS(PAD_DISP0_DAT19__IPU1_DISP0_DATA19 | PC_RGB_W),
	IOMUX_PADS(PAD_DISP0_DAT20__IPU1_DISP0_DATA20 | PC_RGB_W),
	IOMUX_PADS(PAD_DISP0_DAT21__IPU1_DISP0_DATA21 | PC_RGB_W),
	IOMUX_PADS(PAD_DISP0_DAT22__IPU1_DISP0_DATA22 | PC_RGB_W),
	IOMUX_PADS(PAD_DISP0_DAT23__IPU1_DISP0_DATA23 | PC_RGB_W),
};

static void board_setup_lcd(void)
{}

void board_enable_lcd(const struct display_info_t *di, int enable)
{
	if (enable) {
		SETUP_IOMUX_PADS(rgb_pads);
		enable_display_common();
	} else {
		SETUP_IOMUX_PADS(rgb_gpio_pads);
		disable_display_common();
	}
}
#endif /* defined(CONFIG_SYS_USE_LCD_VIDEO) */

#if defined(CONFIG_SYS_USE_LVDS_VIDEO)
static iomux_v3_cfg_t const lvds_pads[] = {
	IOMUX_PADS(PAD_DISP0_DAT4__GPIO4_IO25 | MUX_PAD_CTRL(NO_PAD_CTRL)),
};
#define LVDS_BLEN_GPIO	IMX_GPIO_NR(4, 25)

static void board_setup_lvds(void)
{
	SETUP_IOMUX_PADS(lvds_pads);
	gpio_direction_input(LVDS_BLEN_GPIO);
}

void board_enable_lvds(const struct display_info_t *di, int enable)
{
	if (enable) {
		gpio_direction_output(LVDS_BLEN_GPIO, 1);
		enable_display_common();
	} else {
		gpio_direction_output(LVDS_BLEN_GPIO, 0);
		disable_display_common();
	}
}

void board_enable_lvds2(const struct display_info_t *di, int enable)
{
	/* not used */
}
#endif /* defined(CONFIG_SYS_USE_LVDS_VIDEO) */

#if defined(CONFIG_SYS_USE_HDMI_VIDEO)
static void board_setup_hdmi(void)
{}
#endif /* defined(CONFIG_SYS_USE_HDMI_VIDEO) */

struct display_info_t displays[] = {
#if defined(CONFIG_SYS_USE_HDMI_VIDEO)
	IMX_VD50_1024_768M_60(HDMI, 1, I2C2_BUS | (I2C2_MUX_GPIO << 8)),
	IMX_VD50_1920_1080M_60(HDMI, 1, I2C2_BUS | (I2C2_MUX_GPIO << 8)),
#endif /* defined(CONFIG_SYS_USE_HDMI_VIDEO) */

#if defined(CONFIG_SYS_USE_LVDS_VIDEO)
	IMX_NLT8048AC19_14F(LVDS, 0, 0),
	IMX_MITSUBISHI_AA150XT11DE01(LVDS, 0, 0),
	IMX_MITSUBISHI_AA121TH01(LVDS, 0, 0),
	IMX_MI1040GT(LVDS, 0, 0),
#endif /* defined(CONFIG_SYS_USE_LVDS_VIDEO) */

#if defined(CONFIG_SYS_USE_LCD_VIDEO)
	IMX_AM800480STMQW00(LCD, 0, 0),
#endif /* defined(CONFIG_SYS_USE_LCD_VIDEO) */
};

void board_setup_video(void)
{
	setup_display_common();

#if defined(CONFIG_SYS_USE_LVDS_VIDEO)
	if (boardinfo_has_lvds_video()) {
		printf("LVDS, ");
		board_setup_lvds();
	}
#endif /* defined(#if defined(CONFIG_SYS_USE_LVDS_VIDEO)) */

#if defined(CONFIG_SYS_USE_LCD_VIDEO)
	if (boardinfo_has_rgb_video()) {
		printf("LCD, ");
		board_setup_lcd();
	}
#endif /* defined(CONFIG_SYS_USE_LCD_VIDEO) */

#if defined(CONFIG_SYS_USE_HDMI_VIDEO)
	printf("HDMI, ");
	board_setup_hdmi();
#endif /* defined(CONFIG_SYS_USE_HDMI_VIDEO) */

	printf("\n");

	fbp_setup_display(displays, ARRAY_SIZE(displays));
}

int board_cfb_skip(void)
{
	return NULL != getenv("novideo");
}

void video_get_info_str(int line_number, char *info)
{
#define NR_VIDEO_INFO_BUFF_LEN	64
	char buff[NR_VIDEO_INFO_BUFF_LEN];

	switch (line_number) {
	case 1:
		snprintf(buff, NR_VIDEO_INFO_BUFF_LEN, "%s", "");
		break;
	case 2:
		snprintf(buff, NR_VIDEO_INFO_BUFF_LEN,
			 " board name ........ %s-%s-%s",
			 boardinfo_get_name(),
			 boardinfo_get_variant(),
			 boardinfo_get_feature());
		break;
	case 3:
		snprintf(buff, NR_VIDEO_INFO_BUFF_LEN,
			 " serial number ..... %s",
			 boardinfo_get_serial());
		break;
	default:
		buff[0] = '\0';
		break;
	}
	strcpy(info, buff);
}
#endif /* !defined(CONFIG_SPL_BUILD) */
