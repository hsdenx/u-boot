/*
 * Copyright (C) 2015 MSC Technologies
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
#include <asm/imx-common/boot_mode.h>
#include <asm/imx-common/video.h>
#include <asm/imx-common/sata.h>
#include <asm/io.h>
#include <linux/sizes.h>
#include <fsl_esdhc.h>
#include <mmc.h>
#include <miiphy.h>
#include <netdev.h>
#include <phy.h>
#include <input.h>
#include <i2c.h>
#include <spi.h>
#include <usb/ehci-ci.h>
#include <spi_flash.h>
#include "init.h"
#include "boardinfo.h"
#include "common.h"
#include "video.h"

DECLARE_GLOBAL_DATA_PTR;

#define SOFT_RESET_GPIO		IMX_GPIO_NR(7, 13)

int dram_init(void)
{
	gd->ram_size = imx_ddr_size();
	return 0;
}

#define UA_PCTRL	MUX_PAD_CTRL(UART_PAD_CTRL)
#define ENET_PCTRL	MUX_PAD_CTRL(ENET_PAD_CTRL)
#define ENET_NOPULL_PC	MUX_PAD_CTRL(ENET_PAD_CTRL_NO_PULL)
#define I2C_PC		MUX_PAD_CTRL(I2C_PAD_CTRL)

static iomux_v3_cfg_t const uart1_pads[] = {
	IOMUX_PADS(PAD_CSI0_DAT10__UART1_TX_DATA | UA_PCTRL),
	IOMUX_PADS(PAD_CSI0_DAT11__UART1_RX_DATA | UA_PCTRL),
	IOMUX_PADS(PAD_EIM_D19__UART1_CTS_B | UA_PCTRL),
	IOMUX_PADS(PAD_EIM_D20__UART1_RTS_B | UA_PCTRL),
};

static iomux_v3_cfg_t const uart2_pads[] = {
	IOMUX_PADS(PAD_EIM_D26__UART2_TX_DATA | UA_PCTRL),
	IOMUX_PADS(PAD_EIM_D27__UART2_RX_DATA | UA_PCTRL),
};

static iomux_v3_cfg_t const uart3_pads[] = {
	IOMUX_PADS(PAD_EIM_D24__UART3_TX_DATA | UA_PCTRL),
	IOMUX_PADS(PAD_EIM_D25__UART3_RX_DATA | UA_PCTRL),
	IOMUX_PADS(PAD_EIM_D31__UART3_RTS_B | UA_PCTRL),
	IOMUX_PADS(PAD_EIM_D23__UART3_CTS_B | UA_PCTRL),
};

static iomux_v3_cfg_t const enet_pads[] = {
	IOMUX_PADS(PAD_ENET_MDIO__ENET_MDIO | ENET_PCTRL),
	IOMUX_PADS(PAD_ENET_MDC__ENET_MDC | ENET_PCTRL),
	IOMUX_PADS(PAD_RGMII_TXC__RGMII_TXC | ENET_PCTRL),
	IOMUX_PADS(PAD_RGMII_TD0__RGMII_TD0 | ENET_PCTRL),
	IOMUX_PADS(PAD_RGMII_TD1__RGMII_TD1 | ENET_PCTRL),
	IOMUX_PADS(PAD_RGMII_TD2__RGMII_TD2 | ENET_PCTRL),
	IOMUX_PADS(PAD_RGMII_TD3__RGMII_TD3 | ENET_PCTRL),
	IOMUX_PADS(PAD_RGMII_TX_CTL__RGMII_TX_CTL | ENET_PCTRL),
	IOMUX_PADS(PAD_ENET_REF_CLK__ENET_TX_CLK | ENET_NOPULL_PC),
	IOMUX_PADS(PAD_RGMII_RXC__RGMII_RXC | ENET_PCTRL),
	IOMUX_PADS(PAD_RGMII_RD0__RGMII_RD0 | ENET_PCTRL),
	IOMUX_PADS(PAD_RGMII_RD1__RGMII_RD1 | ENET_PCTRL),
	IOMUX_PADS(PAD_RGMII_RD2__RGMII_RD2 | ENET_PCTRL),
	IOMUX_PADS(PAD_RGMII_RD3__RGMII_RD3 | ENET_PCTRL),
	IOMUX_PADS(PAD_RGMII_RX_CTL__RGMII_RX_CTL | ENET_PCTRL),
};

static struct i2c_pads_info mx6q_i2c1_pad_info = {
	.scl = {
		.i2c_mode = MX6Q_PAD_CSI0_DAT9__I2C1_SCL | I2C_PC,
		.gpio_mode = MX6Q_PAD_CSI0_DAT9__GPIO5_IO27 | I2C_PC,
		.gp = IMX_GPIO_NR(5, 27)
	},
	.sda = {
		.i2c_mode = MX6Q_PAD_CSI0_DAT8__I2C1_SDA | I2C_PC,
		.gpio_mode = MX6Q_PAD_CSI0_DAT8__GPIO5_IO26 | I2C_PC,
		.gp = IMX_GPIO_NR(5, 26)
	}
};

static struct i2c_pads_info mx6dl_i2c1_pad_info = {
	.scl = {
		.i2c_mode = MX6DL_PAD_CSI0_DAT9__I2C1_SCL | I2C_PC,
		.gpio_mode = MX6DL_PAD_CSI0_DAT9__GPIO5_IO27 | I2C_PC,
		.gp = IMX_GPIO_NR(5, 27)
	},
	.sda = {
		.i2c_mode = MX6DL_PAD_CSI0_DAT8__I2C1_SDA | I2C_PC,
		.gpio_mode = MX6DL_PAD_CSI0_DAT8__GPIO5_IO26 | I2C_PC,
		.gp = IMX_GPIO_NR(5, 26)
	}
};

static void board_setup_i2c1(void)
{
	setup_i2c(I2C1_BUS, CONFIG_SYS_I2C_SPEED, 0x7f,
		  (is_cpu_type(MXC_CPU_MX6Q) || is_cpu_type(MXC_CPU_MX6D)) ?
		  &mx6q_i2c1_pad_info : &mx6dl_i2c1_pad_info);
}

static struct i2c_pads_info mx6q_i2c2_pad_info = {
	.scl = {
		.i2c_mode = MX6Q_PAD_KEY_COL3__I2C2_SCL | I2C_PC,
		.gpio_mode = MX6Q_PAD_KEY_COL3__GPIO4_IO12 | I2C_PC,
		.gp = IMX_GPIO_NR(4, 12)
	},
	.sda = {
		.i2c_mode = MX6Q_PAD_KEY_ROW3__I2C2_SDA | I2C_PC,
		.gpio_mode = MX6Q_PAD_KEY_ROW3__GPIO4_IO13 | I2C_PC,
		.gp = IMX_GPIO_NR(4, 13)
	}
};

static struct i2c_pads_info mx6dl_i2c2_pad_info = {
	.scl = {
		.i2c_mode = MX6DL_PAD_KEY_COL3__I2C2_SCL | I2C_PC,
		.gpio_mode = MX6DL_PAD_KEY_COL3__GPIO4_IO12 | I2C_PC,
		.gp = IMX_GPIO_NR(4, 12)
	},
	.sda = {
		.i2c_mode = MX6DL_PAD_KEY_ROW3__I2C2_SDA | I2C_PC,
		.gpio_mode = MX6DL_PAD_KEY_ROW3__GPIO4_IO13 | I2C_PC,
		.gp = IMX_GPIO_NR(4, 13)
	}
};

static iomux_v3_cfg_t const i2c2_mux_pads[] = {
	IOMUX_PADS(PAD_GPIO_7__GPIO1_IO07 | MUX_PAD_CTRL(NO_PAD_CTRL)),
};

void board_setup_i2c2(void)
{
	SETUP_IOMUX_PADS(i2c2_mux_pads);

	setup_i2c(I2C2_BUS, CONFIG_SYS_I2C_SPEED, 0x7f,
		  (is_cpu_type(MXC_CPU_MX6Q) || is_cpu_type(MXC_CPU_MX6D)) ?
		  &mx6q_i2c2_pad_info : &mx6dl_i2c2_pad_info);
}

static void board_setup_i2c(void)
{
	board_setup_i2c1();
	board_setup_i2c2();

	/* select bus 2 (mxc1) as default */
	i2c_set_bus_num(I2C2_BUS);
}

#if defined(CONFIG_MXC_SPI)
#if defined(USE_SPINOR)
#define SPINOR_CS IMX_GPIO_NR(5, 2)
static iomux_v3_cfg_t const ecspi4_pads[] = {
	IOMUX_PADS(PAD_EIM_D21__ECSPI4_SCLK | MUX_PAD_CTRL(SPI_PAD_CTRL)),
	IOMUX_PADS(PAD_EIM_D22__ECSPI4_MISO | MUX_PAD_CTRL(SPI_PAD_CTRL)),
	IOMUX_PADS(PAD_EIM_D28__ECSPI4_MOSI | MUX_PAD_CTRL(SPI_PAD_CTRL)),
	IOMUX_PADS(PAD_EIM_A25__GPIO5_IO02  | MUX_PAD_CTRL(GPIO_PAD_CTRL)),
	IOMUX_PADS(PAD_EIM_A16__GPIO2_IO22  | MUX_PAD_CTRL(GPIO_PAD_CTRL)),
};

int board_spi_cs_gpio(unsigned bus, unsigned cs)
{
	return (bus == CONFIG_SF_DEFAULT_BUS && cs == CONFIG_SF_DEFAULT_CS) ?
			(SPINOR_CS) : -1;
}

static void setup_spinor(void)
{
	int ret;

	ret = enable_spi_clk(1, CONFIG_SF_DEFAULT_BUS);
	if (ret) {
		printf("Error: couldn't enable SPI clock\n");
		return;
	}

	SETUP_IOMUX_PADS(ecspi4_pads);
	gpio_request(SPINOR_CS , "spinor_cs");
	gpio_direction_output(SPINOR_CS, 1);
}

#endif /* defined(USE_SPINOR) */
#endif /* defined(CONFIG_MXC_SPI) */

static void setup_iomux_uart(void)
{
	SETUP_IOMUX_PADS(uart1_pads);
	SETUP_IOMUX_PADS(uart2_pads);
	SETUP_IOMUX_PADS(uart3_pads);
}

static void setup_iomux_enet(void)
{
	SETUP_IOMUX_PADS(enet_pads);
}

#if defined(CONFIG_FSL_ESDHC)
/*
 * U-boot device node | Physical Port
 * ----------------------------------
 *  mmc0              |  SD1  (on module)
 *  mmc1              |  SD2  (on baseboard)
 *  mmc2              |  eMMC (on module)
 */
#define USDHC1_CD_GPIO		IMX_GPIO_NR(6, 31)
#define USDHC2_CD_GPIO		IMX_GPIO_NR(4, 20)

static iomux_v3_cfg_t const usdhc1_pads[] = {
	IOMUX_PADS(PAD_SD1_CLK__SD1_CLK | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
	IOMUX_PADS(PAD_SD1_CMD__SD1_CMD | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
	IOMUX_PADS(PAD_SD1_DAT0__SD1_DATA0 | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
	IOMUX_PADS(PAD_SD1_DAT1__SD1_DATA1 | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
	IOMUX_PADS(PAD_SD1_DAT2__SD1_DATA2 | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
	IOMUX_PADS(PAD_SD1_DAT3__SD1_DATA3 | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
	IOMUX_PADS(PAD_EIM_BCLK__GPIO6_IO31 | MUX_PAD_CTRL(NO_PAD_CTRL)),
};

static iomux_v3_cfg_t const usdhc2_pads[] = {
	IOMUX_PADS(PAD_SD2_CLK__SD2_CLK | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
	IOMUX_PADS(PAD_SD2_CMD__SD2_CMD | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
	IOMUX_PADS(PAD_SD2_DAT0__SD2_DATA0 | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
	IOMUX_PADS(PAD_SD2_DAT1__SD2_DATA1 | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
	IOMUX_PADS(PAD_SD2_DAT2__SD2_DATA2 | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
	IOMUX_PADS(PAD_SD2_DAT3__SD2_DATA3 | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
	IOMUX_PADS(PAD_DI0_PIN4__GPIO4_IO20 | MUX_PAD_CTRL(NO_PAD_CTRL)),
};

static iomux_v3_cfg_t const usdhc3_pads[] = {
	IOMUX_PADS(PAD_SD3_CLK__SD3_CLK | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
	IOMUX_PADS(PAD_SD3_CMD__SD3_CMD | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
	IOMUX_PADS(PAD_SD3_DAT0__SD3_DATA0 | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
	IOMUX_PADS(PAD_SD3_DAT1__SD3_DATA1 | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
	IOMUX_PADS(PAD_SD3_DAT2__SD3_DATA2 | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
	IOMUX_PADS(PAD_SD3_DAT3__SD3_DATA3 | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
	IOMUX_PADS(PAD_SD3_DAT4__SD3_DATA4 | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
	IOMUX_PADS(PAD_SD3_DAT5__SD3_DATA5 | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
	IOMUX_PADS(PAD_SD3_DAT6__SD3_DATA6 | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
	IOMUX_PADS(PAD_SD3_DAT7__SD3_DATA7 | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
	IOMUX_PADS(PAD_SD3_RST__SD3_RESET | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
};

int board_mmc_getcd(struct mmc *mmc)
{
	struct fsl_esdhc_cfg *cfg = (struct fsl_esdhc_cfg *)mmc->priv;
	int ret = 0;

	switch (cfg->esdhc_base) {
	case USDHC1_BASE_ADDR:
		ret = !gpio_get_value(USDHC1_CD_GPIO);
		break;
	case USDHC2_BASE_ADDR:
		ret = !gpio_get_value(USDHC2_CD_GPIO);
		break;
	case USDHC3_BASE_ADDR:
		ret = 1;	/* eMMC always present */
		break;
	}
	return ret;
}

static struct fsl_esdhc_cfg usdhc_cfg[CONFIG_SYS_FSL_USDHC_NUM] = {
	{USDHC1_BASE_ADDR},
	{USDHC2_BASE_ADDR},
	{USDHC3_BASE_ADDR},
};

static void init_mmc1(void)
{
	SETUP_IOMUX_PADS(usdhc1_pads);
	usdhc_cfg[0].sdhc_clk = mxc_get_clock(MXC_ESDHC_CLK);
	usdhc_cfg[0].max_bus_width = 4;
	gpio_direction_input(USDHC1_CD_GPIO);
}

static void init_mmc2(void)
{
	SETUP_IOMUX_PADS(usdhc2_pads);
	usdhc_cfg[1].sdhc_clk = mxc_get_clock(MXC_ESDHC2_CLK);
	usdhc_cfg[1].max_bus_width = 4;
	gpio_direction_input(USDHC2_CD_GPIO);
}

static void init_emmc(void)
{
	SETUP_IOMUX_PADS(usdhc3_pads);
	usdhc_cfg[2].sdhc_clk = mxc_get_clock(MXC_ESDHC3_CLK);
	usdhc_cfg[2].max_bus_width = 8;
}

int board_mmc_init(bd_t *bis)
{
	int ret;
	u32 index;

	for (index = 0; index < CONFIG_SYS_FSL_USDHC_NUM; ++index) {
		switch (index) {
		case 0:
			init_mmc1();
			break;
		case 1:
			init_mmc2();
			break;
		case 2:
			init_emmc();
			break;
		default:
			printf("Warning: you configured more USDHC controllers"
			       "(%d) then supported by the board (%d)\n",
			       index + 1, CONFIG_SYS_FSL_USDHC_NUM);
			return -EINVAL;
		}

		ret = fsl_esdhc_initialize(bis, &usdhc_cfg[index]);
		if (ret)
			return ret;
	}

	return 0;
}
#endif /* defined(CONFIG_FSL_ESDHC) */

#if defined(CONFIG_PHY_MICREL_KSZ9031)
static void mmd_write_reg(struct phy_device *phydev, int device, int reg,
			  int val)
{
	phy_write(phydev, MDIO_DEVAD_NONE, 0x0d, device);
	phy_write(phydev, MDIO_DEVAD_NONE, 0x0e, reg);
	phy_write(phydev, MDIO_DEVAD_NONE, 0x0d, BIT(14) | device);
	phy_write(phydev, MDIO_DEVAD_NONE, 0x0e, val);
}

#define RGMII_CONTROL_SIGNAL_PAD_SKEW	0x04
#define RGMII_RX_DATA_PAD_SKEW		0x05
#define RGMII_CLOCK_PAD_SKEW		0x08

static int ksz9031rn_phy_fixup(struct phy_device *phydev)
{
	mmd_write_reg(phydev, 2, RGMII_CONTROL_SIGNAL_PAD_SKEW, 0);
	mmd_write_reg(phydev, 2, RGMII_RX_DATA_PAD_SKEW, 0);
	mmd_write_reg(phydev, 2, RGMII_CLOCK_PAD_SKEW, 0x003ff);
	return 0;
}
#endif /* CONFIG_PHY_MICREL_KSZ9031 */

int board_phy_config(struct phy_device *phydev)
{
#if defined(CONFIG_PHY_MICREL_KSZ9031)
	ksz9031rn_phy_fixup(phydev);
#endif /* CONFIG_PHY_MICREL_KSZ9031 */

	if (phydev->drv->config)
		phydev->drv->config(phydev);

	return 0;
}

void toggle_soft_reset(unsigned long usec)
{
	gpio_direction_output(SOFT_RESET_GPIO, 0);
	udelay(usec);
	gpio_set_value(SOFT_RESET_GPIO, 1);
	udelay(usec);
}

int board_eth_init(bd_t *bis)
{
	setup_iomux_enet();
	toggle_soft_reset(100);
	return cpu_eth_init(bis);
}

int board_early_init_f(void)
{
	setup_iomux_uart();

#if defined(USE_SPINOR)
	setup_spinor();
#endif /* USE_SPINOR */

#if defined(CONFIG_IMX6_CAN_CLK_INIT)
	set_can_clk(CONFIG_IMX6_CAN_CLK_DIV);
#endif /* CONFIG_IMX6_CAN_CLK_INIT */

	return 0;
}

/*
 * Do not overwrite the console
 * Use always serial for U-Boot console
 */
int overwrite_console(void)
{
	return 1;
}

#if defined(CONFIG_USB_EHCI_MX6)
#define USB_OTG_PWR             IMX_GPIO_NR(4, 15)
#define USB_HOST1_PWR           IMX_GPIO_NR(1, 0)

static void usb_otg_init(void)
{
	iomux_v3_cfg_t const usbotg_pads[] = {
			IOMUX_PADS(PAD_KEY_ROW4__GPIO4_IO15 |
				   MUX_PAD_CTRL(0x30b1)),
			IOMUX_PADS(PAD_GPIO_1__USB_OTG_ID   |
				   MUX_PAD_CTRL(USB_OTG_PAD_CTRL)),
			IOMUX_PADS(PAD_KEY_COL4__USB_OTG_OC |
				   MUX_PAD_CTRL(0x1b0b0)),
	};

	SETUP_IOMUX_PADS(usbotg_pads);
}

static void usb_host_init(void)
{
	iomux_v3_cfg_t const usbhost_pads[] = {
			IOMUX_PADS(PAD_GPIO_0__GPIO1_IO00 |
				   MUX_PAD_CTRL(0x30b1)),
	};

	SETUP_IOMUX_PADS(usbhost_pads);
}

int board_ehci_hcd_init(int port)
{
	debug("USB(%d): init hcd\n", port);

	switch (port) {
	case 0:
		usb_otg_init();
		break;
	case 1:
		usb_host_init();
		break;
	default:
		printf("MXC USB port %d not yet supported\n", port);
		return 1;
	}
	return 0;
}

static int usb_power[CONFIG_USB_MAX_CONTROLLER_COUNT];

int board_ehci_power(int port, int on)
{
	if (usb_power[port] == on) {
		debug("USB(%d): power already %s.\n", port,
		      on ? "enabled" : "disabled");
		return 0;
	}

	debug("USB(%d): %s power\n", port, on ? "enable" : "disable");

	usb_power[port] = on;

	switch (port) {
	case 0:
		gpio_direction_output(USB_OTG_PWR, on ? 1 : 0);
		break;
	case 1:
		gpio_direction_output(USB_HOST1_PWR, on ? 1 : 0);
		break;
	default:
		printf("MXC USB port %d not yet supported\n", port);
		return 1;
	}
	return 0;
}

static void calibrate_usb_phy(void)
{
	usb_phy_set_tx_ctrl(0, 4, 4, 7);
	usb_phy_set_tx_ctrl(1, 4, 4, 7);
}

int usb_post_init(void)
{
	 calibrate_usb_phy();
	 return 0;
}

#endif /* defined(CONFIG_USB_EHCI_MX6) */


#if defined(CONFIG_SYS_USE_PCI)
#define PCIE_PERST_GPIO	IMX_GPIO_NR(1, 26)

iomux_v3_cfg_t const pci_pads[] = {
	IOMUX_PADS(PAD_ENET_RXD1__GPIO1_IO26 | MUX_PAD_CTRL(NO_PAD_CTRL)),
};

int imx6_pcie_toggle_reset(void)
{
	SETUP_IOMUX_PADS(pci_pads);

	gpio_request(PCIE_PERST_GPIO, "pci_rst#");
	gpio_direction_output(PCIE_PERST_GPIO, 0);
	mdelay(50);
	gpio_direction_output(PCIE_PERST_GPIO, 1);
	return 0;
}
#endif /* defined(CONFIG_SYS_USE_PCI) */

#if defined(CONFIG_CMD_BMODE)
static const struct boot_mode board_boot_modes[] = {
	{"mmc0",	MAKE_CFGVAL(0x40, 0x20, 0x00, 0x12)},
	{"spi",		MAKE_CFGVAL(0x38, 0x20, 0x00, 0x12)},
	{NULL,	 0},
};
#endif /* defined(CONFIG_CMD_BMODE) */


#if defined(USE_SPINOR)
static void init_spi_flash(void)
{
	struct spi_flash *flash;

	flash = spi_flash_probe(
			CONFIG_SF_DEFAULT_BUS,
			CONFIG_SF_DEFAULT_CS,
			CONFIG_SF_DEFAULT_SPEED,
			CONFIG_SF_DEFAULT_MODE);
	if (!flash)
		printf("!spi_flash_probe() failed\n");
}
#endif /* defined(USE_SPINOR) */

static int setup_pmic_voltages(void)
{
	i2c_set_bus_num(I2C1_BUS);
	return 0;
}

static iomux_v3_cfg_t const aux_pads[] = {
	/* latch enable */
	IOMUX_PADS(PAD_EIM_A23__GPIO6_IO06 | MUX_PAD_CTRL(NO_PAD_CTRL)),
};

#define LATCH_EN_GPIO	IMX_GPIO_NR(6, 6)

static void board_aux_init(void)
{
	SETUP_IOMUX_PADS(aux_pads);

	gpio_direction_output(LATCH_EN_GPIO, 1);
}

int board_init(void)
{
	struct iomuxc *const iomuxc_regs = (struct iomuxc *)IOMUXC_BASE_ADDR;

	clrsetbits_le32(&iomuxc_regs->gpr[1],
			IOMUXC_GPR1_OTG_ID_MASK,
			IOMUXC_GPR1_OTG_ID_GPIO1);

	/* address of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM + 0x100;

	board_aux_init();
	board_setup_i2c();

#if !defined(CONFIG_SPL_BUILD)
	if (boardinfo_init() != 0)
		printf("Warning: failed to initialize boardinfo!\n");

#if defined(CONFIG_SYS_USE_VIDEO)
	board_setup_video();
#endif /* defined(CONFIG_SYS_USE_VIDEO) */
#endif /* !defined(CONFIG_SPL_BUILD) */

#if defined(USE_SATA)
	setup_sata();
#endif /* defined(USE_SATA) */

	return 0;
}

int board_late_init(void)
{
	int ret;
#if !defined(CONFIG_SPL_BUILD)
	char *fdtfile;
#endif /* !defined(CONFIG_SPL_BUILD) */

#ifdef CONFIG_CMD_BMODE
	add_board_boot_modes(board_boot_modes);
#endif

#if !defined(CONFIG_SPL_BUILD)
  #if defined(CONFIG_ENV_VARS_UBOOT_RUNTIME_CONFIG)
	/* set env vars based on EEPROM data */
	setenv("board_name", boardinfo_get_name());
	setenv("variant_key", boardinfo_get_variant());
	setenv("feature_key", boardinfo_get_feature());
	setenv("serial_number", boardinfo_get_serial());
	setenv("revision", boardinfo_get_mes_revision());
  #endif /* defined(CONFIG_ENV_VARS_UBOOT_RUNTIME_CONFIG) */

	fdtfile = getenv("fdtfile");
	if (fdtfile == NULL) {
#define ENV_FDTFILE_MAX_SIZE 64
		char buff[ENV_FDTFILE_MAX_SIZE];
		snprintf(buff, ENV_FDTFILE_MAX_SIZE, "%s-%s-%s-headless.dtb",
			 boardinfo_get_name(), boardinfo_get_variant(),
			 boardinfo_get_feature());

		setenv("fdtfile", buff);
	}
#endif /* !defined(CONFIG_SPL_BUILD) */

#if defined(USE_SPINOR)
	init_spi_flash();
#endif /* defined(USE_SPINOR) */

	ret = setup_pmic_voltages();
	if (ret)
		return -1;

	return 0;
}

int checkboard(void)
{
	puts("Board: MSC nanoRISC i.MX6\n");
	return 0;
}
