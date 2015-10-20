#ifndef __MSC_NR_IMX6_H__
#define __MSC_NR_IMX6_H__
/*
 * Copyright (C) 2015 MSC Technologies
 *       <www.msc-technologies.eu>
 *
 * SPDX-License-Identifier:	GPL-2.0
 */
#include "mx6_common.h"

#define USE_SATA

#define CONFIG_SPL_BOARD_INIT
#define CONFIG_SPL_SPI_LOAD

#define CONFIG_CMD_FUSE
#define CONFIG_IMX_THERMAL

#if defined(CONFIG_SPI_FLASH)
/*
  -----------------------------------------------------------
  |   partition name |       size | start-addr |   end-addr |
  -----------------------------------------------------------
  |           unused | 0x     400 | 0x       0 | 0x     3ff |
  |              SPL | 0x   1fc00 | 0x     400 | 0x   1ffff |
  |              env | 0x   20000 | 0x   20000 | 0x   3ffff |
  |            uboot | 0x   80000 | 0x   40000 | 0x   bffff |
  |             user | 0x  140000 | 0x   c0000 | 0x  1fffff |
  -----------------------------------------------------------
*/
#define SPI_ENV_PART_OFFS      0x20000
#define SPI_UBOOT_PART_OFFS    0x40000
#define CONFIG_SYS_SPI_U_BOOT_OFFS        SPI_UBOOT_PART_OFFS
#define MTDIDS_DEFAULT                    "nor0=nor"
#define MTDPARTS_DEFAULT  \
	"mtdparts=nor:128k(SPL),128k(env),512k(uboot),-(user)"
#endif

#if defined(CONFIG_SYS_USE_NANDFLASH)
	/* TBD */
#endif

#include "imx6_spl.h"

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN               (10 * SZ_1M)

#define CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_BOARD_LATE_INIT

/* Command definition */

#define CONFIG_SYS_MEMTEST_START	0x10000000
#define CONFIG_SYS_MEMTEST_END	(CONFIG_SYS_MEMTEST_START + 10 * SZ_1M)

/* I2C Configs */
#define CONFIG_SYS_I2C
#define CONFIG_SYS_I2C_MXC
#define CONFIG_SYS_I2C_SPEED		100000
#define CONFIG_SYS_I2C_MXC_I2C1		/* enable I2C bus 1 */
#define CONFIG_SYS_I2C_MXC_I2C2		/* enable I2C bus 2 */
#define I2C1_BUS	0
#define I2C2_BUS	1
#define CONFIG_SYS_I2C_EEPROM_BUS	I2C2_BUS
#define CONFIG_SYS_I2C_EEPROM_ADDR	0x54

#if defined(CONFIG_SYS_USE_HDMI_VIDEO)
  #define CONFIG_I2C_EDID
#endif /* defined(CONFIG_SYS_USE_HDMI_VIDEO) */

/* MMC Configuration */
#define CONFIG_SYS_FSL_USDHC_NUM	3
#define CONFIG_SYS_FSL_ESDHC_ADDR	0

/* USB Configs */
#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_MX6
#define CONFIG_USB_STORAGE
#define CONFIG_USB_MAX_CONTROLLER_COUNT		2
#define CONFIG_EHCI_HCD_INIT_AFTER_RESET
#define CONFIG_MXC_USB_PORTSC	(PORT_PTS_UTMI | PORT_PTS_PTW)
#define CONFIG_MXC_USB_FLAGS	0

/* Ethernet Configuration */
#define CONFIG_FEC_MXC
#define CONFIG_MII
#define IMX_FEC_BASE		ENET_BASE_ADDR
#define CONFIG_FEC_XCV_TYPE	RGMII
#define CONFIG_ETHPRIME		"FEC"
#define CONFIG_FEC_MXC_PHYADDR	0
#define CONFIG_PHYLIB
#define CONFIG_PHY_MICREL_KSZ9031

/* Framebuffer */
#if defined(CONFIG_SYS_USE_VIDEO)
  #define CONFIG_VIDEO
  #define CONFIG_VIDEO_IPUV3
  #define CONFIG_CFB_CONSOLE
  #define CONFIG_VGA_AS_SINGLE_DEVICE
  #define CONFIG_SYS_CONSOLE_IS_IN_ENV
  #define CONFIG_SYS_CONSOLE_OVERWRITE_ROUTINE
  #define CONFIG_CONSOLE_EXTRA_INFO
  #define CONFIG_VIDEO_BMP_RLE8
  #define CONFIG_SPLASH_SCREEN
  #define CONFIG_SPLASH_SCREEN_ALIGN
  #define CONFIG_BMP_16BPP
  #define CONFIG_VIDEO_LOGO
  #define CONFIG_VIDEO_BMP_LOGO
  #define CONFIG_IPUV3_CLK		264000000
  #define CONFIG_CMD_HDMIDETECT
  #define CONFIG_IMX_HDMI
  #define CONFIG_CMD_FBPANEL
#endif /* defined(CONFIG_VIDEO) */

#if defined(CONFIG_CMD_FUSE) || defined(CONFIG_IMX_THERMAL)
  #define CONFIG_MXC_OCOTP
#endif

#define CONFIG_MXC_UART
#if defined(CONFIG_USE_UART1)
  #define CONFIG_MXC_UART_BASE	UART1_BASE
  #define CONSOLE_DEV		"ttymxc0"
#elif defined(CONFIG_USE_UART2)
  #define CONFIG_MXC_UART_BASE	UART2_BASE
  #define CONSOLE_DEV		"ttymxc1"
#elif defined(CONFIG_USE_UART3)
  #define CONFIG_MXC_UART_BASE	UART3_BASE
  #define CONSOLE_DEV		"ttymxc2"
#else
  #define CONFIG_MXC_UART_BASE	UART2_BASE
  #define CONSOLE_DEV		"ttymxc1"
#endif

#define CONFIG_ENV_VARS_UBOOT_RUNTIME_CONFIG
#define OPT_ARGS                     "consoleblank=0"

#if defined(CONFIG_SPI_FLASH)
#define SPINOR_ENV_SETTINGS \
	"mtdparts=" MTDPARTS_DEFAULT "\0" \
	"mtdids=" MTDIDS_DEFAULT "\0" \
	"spi_spl_image=msc_nR_imx6_spi-SPL.spi\0" \
	"spi_update_spl=" \
		"mmc dev ${mmcdev}; " \
		"if mmc rescan; then " \
			"echo Loading spl image (${spi_spl_image}) from" \
				" mmc ...; " \
			"if fatload mmc ${mmcdev}:${mmcfatpart} $loadaddr" \
				" ${spi_spl_image}; then " \
				"sf probe; " \
				"echo Updating spl partition ...; " \
				"sf update $loadaddr SPL $filesize; " \
				"echo succeeded; " \
			"else " \
				"echo failed; " \
			"fi; " \
		"fi; " \
		"\0" \
	"spi_uboot_image=msc_nR_imx6_spi-u-boot.img\0" \
	"spi_update_uboot=" \
		"mmc dev ${mmcdev}; " \
		"if mmc rescan; then " \
			"echo Loading uboot image (${spi_uboot_image}) from" \
			" mmc ...; " \
			"if fatload mmc ${mmcdev}:${mmcfatpart} $loadaddr " \
				"${spi_uboot_image}; then " \
				"sf probe; " \
				"echo Updating uboot partition ...; " \
				"sf update $loadaddr uboot $filesize; " \
				"echo succeeded; " \
			"else " \
				"echo failed; " \
			"fi; " \
		"fi; " \
		"\0" \
	"spi_update_all=" \
		"run spi_update_spl; " \
		"run spi_update_uboot; " \
		"\0" \
	"\0"
#else
#define SPINOR_ENV_SETTINGS "\0"
#endif

#if defined(USE_SATA)
  #define SATA_ENV_SETTINGS "\0"
#else
  #define SATA_ENV_SETTINGS "\0"
#endif

#define CONFIG_EXTRA_ENV_SETTINGS \
	"image=zImage\0" \
	"envfile=uEnv.txt\0" \
	"console=" CONSOLE_DEV "\0" \
	"imageroot=boot\0" \
	"fdtroot=boot\0" \
	"fdt_high=0xffffffff\0" \
	"initrd_high=0xffffffff\0" \
	"fdt_addr=0x18000000\0" \
	"boot_fdt=try\0" \
	"mmcdev=0\0" \
	"mmcfatpart=1\0" \
	"mmcextpart=2\0" \
	"mmcroot=/dev/mmcblk0p2 rootwait rw\0" \
	"emmcdev=2\0" \
	"emmcextpart=1\0" \
	"emmcroot=/dev/mmcblk0p1 rootwait rw\0" \
	"usbdev=0\0" \
	"usbfatpart=1\0" \
	"opt_args=" OPT_ARGS "\0" \
	"bootdevs=usb mmc1 mmc0 emmc\0" \
	"mmcargs=" \
		"setenv bootargs console=${console},${baudrate} " \
		"root=${mmcroot} " \
		"${opt_args} " \
		"\0" \
	"emmcargs=" \
		"setenv bootargs console=${console},${baudrate} " \
		"root=${emmcroot} " \
		"${opt_args} " \
		"\0" \
	"usbargs=" \
		"setenv bootargs console=${console},${baudrate} " \
		"root=${usbroot} " \
		"${opt_args} " \
		"\0" \
	"mmcloadenv=" \
		"echo Loading environment (${envfile}) from MMC${uenvdev} ...; " \
		"fatload mmc ${uenvdev}:${uenvpart} ${loadaddr} ${envfile}; " \
		"\0" \
	"emmcloadenv=" \
		"echo Loading environment (${envfile}) from eMMC ...; " \
		"ext2load mmc ${emmcdev}:${emmcextpart} ${loadaddr} ${imageroot}/${envfile}; " \
		"\0" \
	"usbloadenv=" \
		"echo Loading environment (${envfile}) from USB ...; " \
		"fatload usb ${usbdev}:${uenvpart} ${loadaddr} ${imageroot}/${envfile}; " \
		"\0" \
	"importenv=" \
		"echo Importing environment (${envfile}) ...; " \
		"env import -t ${loadaddr} ${filesize}; " \
		"\0" \
	"mmcloadimage=" \
		"echo Loading linux image (${image}) from MMC${mmcdev} ...; " \
		"fatload mmc ${mmcdev}:${mmcfatpart} ${loadaddr} ${image}; " \
		"\0" \
	"emmcloadimage=" \
		"echo Loading linux image (${imageroot}/${image}) from eMMC ...; " \
		"ext2load mmc ${emmcdev}:${emmcextpart} ${loadaddr} ${imageroot}/${image}; " \
		"\0" \
	"mmcloadfdt=" \
		"fatsize mmc ${mmcdev}:${mmcfatpart} ${fdtfile}; " \
		"if test $? -ne 0; then " \
			"setenv fdtfile ${fallbackfdtfile}; " \
		"fi; " \
		"echo Loading FDT image (${fdtfile}) from MMC${mmcdev} ...; " \
		"fatload mmc ${mmcdev}:${mmcfatpart} ${fdt_addr} ${fdtfile}; " \
		"\0" \
	"emmcloadfdt=" \
		"ext4size mmc ${emmcdev}:${emmcextpart} ${fdtroot}/${fdtfile}; " \
		"if test $? -ne 0; then " \
			"setenv fdtfile ${fallbackfdtfile}; " \
		"fi; " \
		"echo Loading FDT image (${fdtroot}/${fdtfile}) from eMMC ...; " \
		"ext2load mmc ${emmcdev}:${emmcextpart} ${fdt_addr} ${fdtroot}/${fdtfile}; " \
		"\0" \
	"buildfallbackfdtname="\
		"if test ${variant_key} = 009 && test ${feature_key} = 014; then " \
			"fallbackfdtfile=bosch-mpc1360d.dtb; " \
		"elif test ${variant_key} = 010 && test ${feature_key} = 014; then " \
			"fallbackfdtfile=bosch-mpc2460d.dtb; " \
		"elif test ${variant_key} = 011 && test ${feature_key} = 014; then " \
			"fallbackfdtfile=bosch-mpc4560d.dtb; " \
		"else " \
			"fallbackfdtfile=default.dtb; " \
		"fi; " \
		"\0" \
	"mmcboot=echo Booting from MMC${uenvdev} ...; " \
		"run mmcargs; " \
		"if test ${boot_fdt} = yes || test ${boot_fdt} = try; then " \
			"if run mmcloadfdt; then " \
				"bootz ${loadaddr} - ${fdt_addr}; " \
			"else " \
				"if test ${boot_fdt} = try; then " \
					"bootz; " \
				"else " \
					"echo WARN: Cannot load the DT; " \
				"fi; " \
			"fi; " \
		"else " \
			"bootz; " \
		"fi; " \
		"\0" \
	"emmcboot=echo Booting from eMMC ...; " \
		"run emmcargs; " \
		"if test ${boot_fdt} = yes || test ${boot_fdt} = try; then " \
			"if run emmcloadfdt; then " \
				"bootz ${loadaddr} - ${fdt_addr}; " \
			"else " \
				"if test ${boot_fdt} = try; then " \
					"bootz; " \
				"else " \
					"echo WARN: Cannot load the DT; " \
				"fi; " \
			"fi; " \
		"else " \
			"bootz; " \
		"fi; " \
		"\0" \
	"mmc_boot=" \
		"if mmc dev ${mmcdev}; then " \
			"if mmc rescan; then " \
				"setenv uenvdev ${mmcdev}; " \
				"setenv uenvpart ${mmcfatpart}; " \
				"if run mmcloadenv; then " \
					"run importenv; " \
				"fi; " \
				"if test -n \"$uenvcmd\"; then " \
					"echo Running uenvcmd ...;" \
					"run uenvcmd; " \
				"fi;" \
				"if run mmcloadimage; then " \
					"run mmcboot; " \
				"else " \
					"echo ERR: Load image(s) from MMC${uenvdev} failed; false; " \
				"fi; " \
			"else " \
				"echo ERR: MMC${mmcdev} scan failed; false; " \
			"fi;" \
		"else " \
			"echo ERR: MMC${mmcdev} start failed; false; " \
		"fi;" \
		"\0" \
	"emmc_boot=" \
		"mmc dev ${emmcdev}; " \
		"if mmc rescan; then " \
			"if run emmcloadenv; then " \
				"run importenv; " \
			"fi; " \
			"if test -n \"$uenvcmd\"; then " \
				"echo Running uenvcmd ...;" \
				"run uenvcmd; " \
			"fi;" \
			"if run emmcloadimage; then " \
				"run emmcboot; " \
			"else " \
				"echo ERR: Load image(s) from eMMC failed; false; " \
			"fi; " \
		"else " \
			"echo ERR: eMMC scan failed; false; " \
		"fi;" \
		"\0" \
	"usb_boot=" \
		"if usb start && usb dev 0; then " \
			"setenv uenvpart ${usbfatpart}; " \
			"if run usbloadenv; then " \
				"run importenv; " \
			"fi; " \
			"if test -n \"$uenvcmd\"; then " \
				"echo Running uenvcmd ...;" \
				"run uenvcmd; " \
			"fi;" \
		"else " \
			"echo ERR: USB start failed; false; " \
		"fi; "\
		"\0" \
	"mmc0_boot=" \
		"setenv mmcdev 0; " \
		"run mmc_boot; " \
		"\0" \
	"mmc1_boot=" \
		"setenv mmcdev 1; " \
		"run mmc_boot; " \
		"\0" \
	SPINOR_ENV_SETTINGS \
	SATA_ENV_SETTINGS \
	""

#define CONFIG_BOOTCOMMAND \
		"if boardinfo complete; then " \
			"run buildfallbackfdtname; " \
			"for btype in ${bootdevs}; do " \
				"echo Attempting ${btype} boot...; " \
				"if run ${btype}_boot; then; " \
					"exit; " \
				"fi; " \
			"done; " \
		"else " \
			"echo ERR: Aborting boot OS, boardinfo is not complete!; false; " \
		"fi; "

/* Physical Memory Map */
#define CONFIG_NR_DRAM_BANKS                1
#define PHYS_SDRAM                          MMDC0_ARB_BASE_ADDR

#define CONFIG_SYS_SDRAM_BASE               PHYS_SDRAM
#define CONFIG_SYS_INIT_RAM_ADDR            IRAM_BASE_ADDR
#define CONFIG_SYS_INIT_RAM_SIZE            IRAM_SIZE

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

#if defined(CONFIG_SPI_FLASH)
  #ifndef CONFIG_SPL_BUILD
    #define CONFIG_SPI_FLASH_MTD
    #define CONFIG_SPI_FLASH_BAR
  #endif
  #define CONFIG_MXC_SPI
  #define CONFIG_SF_DEFAULT_BUS             3
  #define CONFIG_SF_DEFAULT_CS              1
  #define CONFIG_SF_DEFAULT_SPEED           20000000
  #define CONFIG_SF_DEFAULT_MODE            (SPI_MODE_0)
#endif

#define CONFIG_ENV_SIZE                     (SZ_128K)

#if defined(CONFIG_ENV_IS_IN_MMC)
  #define CONFIG_ENV_OFFSET                 (SZ_512K)
  #define CONFIG_SYS_MMC_ENV_DEV            0
#elif defined(CONFIG_ENV_IS_IN_SPI_FLASH)
  #define CONFIG_ENV_OFFSET                 SPI_ENV_PART_OFFS
  #define CONFIG_ENV_SECT_SIZE              (SZ_1K)
  #define CONFIG_ENV_SPI_BUS                CONFIG_SF_DEFAULT_BUS
  #define CONFIG_ENV_SPI_CS                 CONFIG_SF_DEFAULT_CS
  #define CONFIG_ENV_SPI_MAX_HZ             CONFIG_SF_DEFAULT_SPEED
  #define CONFIG_ENV_SPI_MODE               CONFIG_SF_DEFAULT_MODE
#endif

#if defined(CONFIG_SYS_USE_PCI)
  #define CONFIG_CMD_PCI
  #define CONFIG_PCI
  #define CONFIG_PCI_PNP
  #define CONFIG_PCI_SCAN_SHOW
  #define CONFIG_PCIE_IMX
#endif

#if defined(USE_SATA)
  #define CONFIG_CMD_SATA
  #define CONFIG_SYS_SATA_MAX_DEVICE        1
  #define CONFIG_DWC_AHSATA
  #define CONFIG_DWC_AHSATA_PORT_ID         0
  #define CONFIG_DWC_AHSATA_BASE_ADDR       SATA_ARB_BASE_ADDR
  #define CONFIG_LBA48
  #define CONFIG_LIBATA
#endif

/* other */
#define CONFIG_CMD_BMODE
#define CONFIG_CMD_STRINGS
#define CONFIG_CMD_MTDPARTS
#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS
#define CONFIG_CMD_MEMTEST

#if defined(CONFIG_SPL_BUILD)
  #undef CONFIG_OF_LIBFDT
#endif /* defined(CONFIG_SPL_BUILD) */

#endif /* __MSC_NR_IMX6_H__ */
