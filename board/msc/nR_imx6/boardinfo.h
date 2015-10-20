#ifndef __MSC_NR_IMX6_BOARDINFO_H__
#define __MSC_NR_IMX6_BOARDINFO_H__
/*
 * Copyright (C) 2015 MSC Technologies
 *       <www.msc-technologies.eu>
 *
 * SPDX-License-Identifier:	GPL-2.0
 */

#define BI_MAJOR_VERSION    1
#define BI_MINOR_VERSION    1
#define BI_NAME_LEN         31
#define BI_VARIANT_LEN      3
#define BI_FEATURE_LEN      3
#define BI_SERIAL_LEN       11
#define BI_REVISION_LEN     2

/* board info bits */
#define BI_NAME_BIT         BIT(0)
#define BI_VARIANT_BIT      BIT(1)
#define BI_FEATURE_BIT      BIT(2)
#define BI_SERIAL_BIT       BIT(3)
#define BI_REVISION_BIT     BIT(4)

/* variant specifics */
#define VK_CPUTYPE_OFFS     (4)
#define VK_CPUTYPE(V)       (((V)>>VK_CPUTYPE_OFFS) & 0x0F)

/* feature key bits */
#define FK_RAM_1V35_BIT     BIT(4)
#define FK_LVDS_BIT         BIT(5)
#define FK_IIS_MASTER_BIT   BIT(6)

/* feature string handling */
#define FK_GET_DV(F)         (feature & 0x0f)
#define FK_HAS_RAM_1V35      (feature & FK_RAM_1V35_BIT)
#define FK_HAS_LVDS_VIDEO(F) (feature & FK_LVDS_BIT)
#define FK_HAS_RGB_VIDEO(F)  (!FK_HAS_LVDS_VIDEO(F))
#define FK_IS_IIS_MASTER     (feature & FK_IIS_MASTER_BIT)

struct eeprom_content_head {
	uint8_t     magic[4];
	uint8_t     version_maj;
	uint8_t     version_min;
	uint16_t    chksum;
	uint32_t    reserved[2];
};

struct board_info {
	uint32_t	feature_bits;
	char        board_name[BI_NAME_LEN + 1];
	char        variant_key[BI_VARIANT_LEN + 1];
	char        feature_key[BI_FEATURE_LEN + 1];
	char        serial_number[BI_SERIAL_LEN + 1];
	uint32_t    boot_cnt;
	char        mes_rev[2];
	uint16_t    reserved;
};

#define BI_HAS_VK(I) \
	(strlen((I)->variant_key) != 0)
#define BI_HAS_FK(I) \
	(strlen((I)->feature_key) != 0)

int boardinfo_get(struct board_info *info);
void boardinfo_print(const struct board_info *info);

#if !defined(CONFIG_SPL_BUILD)
int boardinfo_init(void);
const char *boardinfo_get_name(void);
const char *boardinfo_get_variant(void);
const char *boardinfo_get_feature(void);
const char *boardinfo_get_serial(void);
const char *boardinfo_get_mes_revision(void);
bool boardinfo_has_rgb_video(void);
bool boardinfo_has_lvds_video(void);
#endif /* !defined(CONFIG_SPL_BUILD) */

#endif /* __MSC_NR_IMX6_BOARDINFO_H__ */
