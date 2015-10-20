/*
 * Copyright (C) 2015 MSC Technologies
 *       <www.msc-technologies.eu>
 *
 * SPDX-License-Identifier:	GPL-2.0
 */
#include <common.h>
#include <errno.h>
#include <i2c.h>
#include <command.h>
#include <asm/arch/sys_proto.h>

#include "boardinfo.h"
#include "eeprom.h"
#include "init.h"

#define BI_STR "Boardinfo"
#define BI_OFFSET    (sizeof(struct eeprom_content_head))

#define BI_HAS_FEATURE(I, F) \
	((I)->feature_bits & BI_##F##_BIT)

#define BI_ENABLE_FEATURE(I, F) \
		(I)->feature_bits |= BI_##F##_BIT

#define BI_PRINT(format, ...) \
	do { \
		printf("%s: ", BI_STR); \
		printf(format, ## __VA_ARGS__); \
	} \
	while (0)

#if defined(DEBUG)
  #define BI_DEBUG(format, ...) \
	do { \
		printf("%s: ", BI_STR); \
		printf(format, ## __VA_ARGS__); \
	} \
	while (0)
#else /* defined(DEBUG) */
  #define BI_DEBUG(format, ...)
#endif /* defined(DEBUG) */

struct eeprom_info {
	const char *variant;
	unsigned   bus;
	unsigned   chip_addr;
	unsigned   alen;
};

struct eeprom_info eeprom_variants[] = {
	/* old nomenclature */
	{ "003", CONFIG_SYS_I2C_EEPROM_BUS, CONFIG_SYS_I2C_EEPROM_ADDR, 1 },
	{ "004", CONFIG_SYS_I2C_EEPROM_BUS, CONFIG_SYS_I2C_EEPROM_ADDR, 1 },
	{ "005", CONFIG_SYS_I2C_EEPROM_BUS, CONFIG_SYS_I2C_EEPROM_ADDR, 1 },
	{ "006", CONFIG_SYS_I2C_EEPROM_BUS, CONFIG_SYS_I2C_EEPROM_ADDR, 1 },
	{ "008", CONFIG_SYS_I2C_EEPROM_BUS, CONFIG_SYS_I2C_EEPROM_ADDR, 1 },
	{ "009", CONFIG_SYS_I2C_EEPROM_BUS, CONFIG_SYS_I2C_EEPROM_ADDR, 2 },
	{ "010", CONFIG_SYS_I2C_EEPROM_BUS, CONFIG_SYS_I2C_EEPROM_ADDR, 2 },
	{ "011", CONFIG_SYS_I2C_EEPROM_BUS, CONFIG_SYS_I2C_EEPROM_ADDR, 2 },
	{ "012", CONFIG_SYS_I2C_EEPROM_BUS, CONFIG_SYS_I2C_EEPROM_ADDR, 1 },
	{ "032", CONFIG_SYS_I2C_EEPROM_BUS, CONFIG_SYS_I2C_EEPROM_ADDR, 1 },

	/* new nomenclature */
	{ "112", CONFIG_SYS_I2C_EEPROM_BUS, CONFIG_SYS_I2C_EEPROM_ADDR, 1 },
	{ "123", CONFIG_SYS_I2C_EEPROM_BUS, CONFIG_SYS_I2C_EEPROM_ADDR, 1 },
	{ "133", CONFIG_SYS_I2C_EEPROM_BUS, CONFIG_SYS_I2C_EEPROM_ADDR, 1 },
	{ "143", CONFIG_SYS_I2C_EEPROM_BUS, CONFIG_SYS_I2C_EEPROM_ADDR, 1 },

	/* sentinel */
	{  NULL, 0,                          0,                         0 },
};

static const struct eeprom_info *find_eeprom_variant(const char *variant)
{
	struct eeprom_info *ptr;
	for (ptr = eeprom_variants; ptr->variant; ptr++) {
		if (strcmp(ptr->variant, variant) == 0)
			return ptr;
	}
	return NULL;
}

#if !defined(CONFIG_SPL_BUILD)
static bool str_to_uint32(const char *src, uint32_t *dst)
{
	char *p = NULL;

	*dst = simple_strtoul(src, &p, 16);
	if (p == NULL)
		return false;

	return true;
}
#endif /* !defined(CONFIG_SPL_BUILD) */

static int boardinfo_read_head(const struct eeprom_info *eeprom,
		const struct eeprom_content_head *head)
{
	if ((eeprom == NULL) || (head == NULL))
		return 1;

	return i2c_eeprom_read(eeprom->chip_addr, 0, eeprom->alen,
			       (uint8_t *)head, sizeof(*head));
}

static int boardinfo_write_head(const struct eeprom_info *eeprom,
		const struct eeprom_content_head *head)
{
	if ((eeprom == NULL) || (head == NULL))
		return 1;

	return i2c_eeprom_write(eeprom->chip_addr, 0, eeprom->alen,
				(uint8_t *)head, sizeof(*head));
}

static int boardinfo_read_content(const struct eeprom_info *eeprom,
		const struct board_info *info)
{
	if ((eeprom == NULL) || (info == NULL))
		return 1;

	return i2c_eeprom_read(eeprom->chip_addr, BI_OFFSET, eeprom->alen,
			       (uint8_t *)info, sizeof(*info));
}

static int boardinfo_write_content(const struct eeprom_info *eeprom,
		const struct board_info *info)
{
	if ((eeprom == NULL) || (info == NULL))
		return 1;

	return i2c_eeprom_write(eeprom->chip_addr, BI_OFFSET, eeprom->alen,
				(uint8_t *)info, sizeof(*info));
}

static int boardinfo_check_magic(struct eeprom_content_head *head)
{
	if (head == NULL)
		return 1;

	if (head->magic[0] != 'm' || head->magic[1] != 's' ||
	    head->magic[2] != 'c')
		return 1;

	return 0;
}

static int boardinfo_check_version(struct eeprom_content_head *head)
{
	if (head == NULL)
		return 1;

	if (head->version_maj != BI_MAJOR_VERSION ||
	    head->version_min > BI_MINOR_VERSION)
		return 1;

	return 0;
}

int boardinfo_calc_checksum(const struct board_info *info, uint16_t *chksum)
{
	int i;
	unsigned char *ptr;

	if (info == NULL)
		return 1;

	ptr = (unsigned char *)info;
	*chksum = 0;
	for (i = 0; i < sizeof(*info); i++)
		*chksum += ptr[i];
	return 0;
}

static int boardinfo_check_checksum(const struct board_info *info,
				    uint16_t chksum)
{
	uint16_t chksum_temp;
	int ret;

	ret = boardinfo_calc_checksum(info, &chksum_temp);
	if (ret)
		return ret;

	if (chksum_temp != chksum)
		return 1;

	return 0;
}

static void boardinfo_clean(struct board_info *info)
{
	memset(info, 0, sizeof(*info));
}

int __boardinfo_get(const struct eeprom_info *eeprom, struct board_info *info)
{
	struct eeprom_content_head head;
	struct board_info info_tmp;

	if (i2c_set_bus_num(eeprom->bus) || i2c_probe(eeprom->chip_addr))
		return -ENODEV;

	if (boardinfo_read_head(eeprom, &head))	{
		BI_DEBUG("EEPROM read head failed.\n");
		return -EIO;
	}

	if (boardinfo_check_magic(&head)) {
		BI_DEBUG("Magic check failed.\n");
		return -EIO;
	}

	if (boardinfo_check_version(&head)) {
		BI_DEBUG("Version check failed.\n");
		return -EIO;
	}

	if (boardinfo_read_content(eeprom, &info_tmp)) {
		BI_DEBUG("EEPROM read content failed.\n");
		return -EIO;
	}

	if (boardinfo_check_checksum(&info_tmp, head.chksum)) {
		BI_DEBUG("Checksum check failed.\n");
		return -EIO;
	}

	memcpy(info, &info_tmp, sizeof(info_tmp));
	return 0;
}

int boardinfo_get(struct board_info *info)
{
	struct eeprom_info *eeprom;
	int ret;

	if (info == NULL)
		return -ENODATA;

	boardinfo_clean(info);

	for (eeprom = eeprom_variants; eeprom->variant; eeprom++) {
		BI_DEBUG("Trying variant '%s'.\n", eeprom->variant);
		ret = __boardinfo_get(eeprom, info);
		if (ret == 0)
			return 0;
	}
	return -ENXIO;
}

int boardinfo_save(struct board_info *info)
{
	static const struct eeprom_info *eeprom;
	struct eeprom_content_head head;
	uint16_t chksum;
	int ret;

	if (info == NULL)
		return -ENODATA;

	if (!BI_HAS_VK(info)) {
		BI_PRINT("missing variant information!\n");
		return -EINVAL;
	}

	eeprom = find_eeprom_variant(info->variant_key);
	if (eeprom == NULL) {
		BI_PRINT("unknown or not supported module variant '%s'\n",
			 info->variant_key);
		return -EINVAL;
	}

	if (i2c_set_bus_num(eeprom->bus) || i2c_probe(eeprom->chip_addr)) {
		BI_PRINT("I2C device (%02x:%02x) not found!\n",
			 eeprom->bus, eeprom->chip_addr);
		return -ENODEV;
	}

	ret = boardinfo_write_content(eeprom, info);
	if (ret)
		return -EIO;

	boardinfo_calc_checksum(info, &chksum);

	head.magic[0] = 'm';
	head.magic[1] = 's';
	head.magic[2] = 'c';
	head.magic[3] = 0;
	head.version_maj  = BI_MAJOR_VERSION;
	head.version_min  = BI_MINOR_VERSION;
	head.chksum = chksum;

	ret = boardinfo_write_head(eeprom, &head);
	if (ret)
		return -EIO;

	return 0;
}

void boardinfo_print(const struct board_info *info)
{
	if (info == NULL)
		return;

	BI_PRINT("\n");
	printf("  name ......... %s\n",
	       BI_HAS_FEATURE(info, NAME) ? info->board_name : "N/A");
	printf("  variant ...... %s\n",
	       BI_HAS_FEATURE(info, VARIANT) ? info->variant_key : "N/A");
	printf("  feature ...... %s\n",
	       BI_HAS_FEATURE(info, FEATURE) ? info->feature_key : "N/A");
	printf("  serial ....... %s\n",
	       BI_HAS_FEATURE(info, SERIAL) ? info->serial_number : "N/A");
	if (BI_HAS_FEATURE(info, REVISION))
		printf("  revision ..... %c%c\n", info->mes_rev[0],
		       info->mes_rev[1]);
	else
		printf("  revision ..... N/A\n");
}

#if !defined(CONFIG_SPL_BUILD)

static struct board_info info;
static uint32_t dirty;
static char revision[3];

int boardinfo_set_name(const char *name)
{
#if defined(CONFIG_BOARDINFO_OVERWRITE_PROTECTION)
	if (BI_HAS_FEATURE(&info, BOARD_NAME)) {
		BOARDINFO_PRINT("not allowed, boardname already set.\n");
		return 0;
	}
#endif /* defined(CONFIG_BOARDINFO_OVERWRITE_PROTECTION) */

	if (strlen(name) > BI_NAME_LEN) {
		BI_PRINT("name too long for boardname (max %d characters).\n",
			 BI_NAME_LEN);
		return 0;
	}

	memset(info.board_name, 0, sizeof(info.board_name));
	strncpy(info.board_name, name, BI_NAME_LEN);

	BI_ENABLE_FEATURE(&info, NAME);
	dirty |= BI_NAME_BIT;

	BI_PRINT("OK\n");
	return 0;
}

const char *boardinfo_get_name(void)
{
	if (BI_HAS_FEATURE(&info, NAME))
		return info.board_name;
	return "N/A";
}

int boardinfo_set_variant(const char *string)
{
#if defined(CONFIG_BOARDINFO_OVERWRITE_PROTECTION)
	if (BI_HAS_FEATURE(&info, VARIANT)) {
		BI_PRINT("not allowed, variant key already set.\n");
		return 0;
	}
#endif /* defined(CONFIG_BOARDINFO_OVERWRITE_PROTECTION) */

	if (strlen(string) > BI_VARIANT_LEN) {
		BI_PRINT("string too long for variant key (max %d characters).\n",
			 BI_VARIANT_LEN);
		return 0;
	}

	memset(info.variant_key, 0, sizeof(info.variant_key));
	strncpy(info.variant_key, string, BI_VARIANT_LEN);

	BI_ENABLE_FEATURE(&info, VARIANT);
	dirty |= BI_VARIANT_BIT;

	BI_PRINT("OK\n");
	return 0;
}

const char *boardinfo_get_variant(void)
{
	if (BI_HAS_FEATURE(&info, VARIANT))
		return info.variant_key;
	return "N/A";
}

int boardinfo_set_feature(const char *string)
{
#if defined(CONFIG_BOARDINFO_OVERWRITE_PROTECTION)
	if (BI_HAS_FEATURE(&info, FEATURE)) {
		BI_PRINT("not allowed, feature key already set.\n");
		return 0;
	}
#endif /* defined(CONFIG_BOARDINFO_OVERWRITE_PROTECTION) */

	if (strlen(string) > BI_FEATURE_LEN) {
		BI_PRINT("string too long for feature key (max %d characters).\n",
			 BI_FEATURE_LEN);
		return 0;
	}

	memset(info.feature_key, 0, sizeof(info.feature_key));
	strncpy(info.feature_key, string, BI_FEATURE_LEN);

	BI_ENABLE_FEATURE(&info, FEATURE);
	dirty |= BI_FEATURE_BIT;

	BI_PRINT("OK\n");
	return 0;
}

const char *boardinfo_get_feature(void)
{
	if (BI_HAS_FEATURE(&info, FEATURE))
		return info.feature_key;
	return "N/A";
}

const char *boardinfo_get_mes_revision(void)
{
	if (BI_HAS_FEATURE(&info, REVISION)) {
		sprintf(revision, "%c%c", info.mes_rev[0], info.mes_rev[1]);
		return revision;
	}
	return "N/A";
}

int boardinfo_init(void)
{
	return boardinfo_get(&info);
}

int boardinfo_set_serial(const char *string)
{
#if defined(CONFIG_BOARDINFO_OVERWRITE_PROTECTION)
	if (BI_HAS_FEATURE(&info, SERIAL)) {
		BOARDINFO_PRINT("not allowed, serial number already set.\n");
		return 0;
	}
#endif /* defined(CONFIG_BOARDINFO_OVERWRITE_PROTECTION) */

	if (strlen(string) > BI_SERIAL_LEN) {
		BI_PRINT("string too long for serial number (max %d characters).\n",
			 BI_SERIAL_LEN);
		return 0;
	}

	memset(info.serial_number, 0, sizeof(info.serial_number));
	strncpy(info.serial_number, string, BI_SERIAL_LEN);

	BI_ENABLE_FEATURE(&info, SERIAL);
	dirty |= BI_SERIAL_BIT;

	BI_PRINT("OK\n");
	return 0;
}

int boardinfo_set_revision(const char *string)
{
#if defined(CONFIG_BOARDINFO_OVERWRITE_PROTECTION)
	if (BI_HAS_FEATURE(&info, REVISION)) {
		BOARDINFO_PRINT("not allowed, serial number already set.\n");
		return 0;
	}
#endif /* defined(CONFIG_BOARDINFO_OVERWRITE_PROTECTION) */

	if (strlen(string) > BI_REVISION_LEN) {
		BI_PRINT("string too long for serial number (max %d characters).\n",
			 BI_REVISION_LEN);
		return 0;
	}

	info.mes_rev[0] = string[0];
	info.mes_rev[1] = string[1];

	BI_ENABLE_FEATURE(&info, REVISION);
	dirty |= BI_REVISION_BIT;

	BI_PRINT("OK\n");
	return 0;
}


const char *boardinfo_get_serial(void)
{
	if (BI_HAS_FEATURE(&info, SERIAL))
		return info.serial_number;
	return "N/A";
}

static bool boardinfo_is_complete(void)
{
	if (BI_HAS_FEATURE(&info, NAME) &&
	    BI_HAS_FEATURE(&info, VARIANT) &&
	    BI_HAS_FEATURE(&info, FEATURE) &&
	    BI_HAS_FEATURE(&info, SERIAL))
		return true;

	return false;
}

bool boardinfo_has_rgb_video(void)
{
	uint32_t feature = 0;

	if (!BI_HAS_FEATURE(&info, FEATURE))
		return false;

	if (!str_to_uint32(info.feature_key, &feature))
		return false;

	return FK_HAS_RGB_VIDEO(feature);
}

bool boardinfo_has_lvds_video(void)
{
	uint32_t feature;

	if (!BI_HAS_FEATURE(&info, FEATURE))
		return false;

	if (!str_to_uint32(info.feature_key, &feature))
		return false;

	return FK_HAS_LVDS_VIDEO(feature);
}

#if defined(CONFIG_CMD_BOARDINFO)

static int do_boardinfo_show(cmd_tbl_t *cmdtp, int flag, int argc,
		char *const argv[])
{
	boardinfo_print(&info);
	return CMD_RET_SUCCESS;
}

static int do_boardinfo_name(cmd_tbl_t *cmdtp, int flag, int argc,
		char *const argv[])
{
	boardinfo_set_name(argv[1]);
	return CMD_RET_SUCCESS;
}

static int do_boardinfo_variant(cmd_tbl_t *cmdtp, int flag, int argc,
		char *const argv[])
{
	boardinfo_set_variant(argv[1]);
	return CMD_RET_SUCCESS;
}

static int do_boardinfo_feature(cmd_tbl_t *cmdtp, int flag, int argc,
		char *const argv[])
{
	boardinfo_set_feature(argv[1]);
	return CMD_RET_SUCCESS;
}

static int do_boardinfo_serial(cmd_tbl_t *cmdtp, int flag, int argc,
			       char *const argv[])
{
	boardinfo_set_serial(argv[1]);
	return CMD_RET_SUCCESS;
}

static int do_boardinfo_revision(cmd_tbl_t *cmdtp, int flag, int argc,
				 char *const argv[])
{
	boardinfo_set_revision(argv[1]);
	return CMD_RET_SUCCESS;
}



static int do_boardinfo_save(cmd_tbl_t *cmdtp, int flag, int argc,
			     char *const argv[])
{
	if (dirty) {
		int ret = boardinfo_save(&info);
		if (ret == 0)
			dirty = 0;

		BI_PRINT("save %s.\n", (ret == 0) ? "done" : "failed");
	} else {
		BI_PRINT("nothing to save, ignore.\n");
	}

	return CMD_RET_SUCCESS;
}

static int do_boardinfo_complete(cmd_tbl_t *cmdtp, int flag, int argc,
				 char *const argv[])
{
	if (boardinfo_is_complete() == false) {
		BI_PRINT("NOK, not complete.\n");
		return CMD_RET_FAILURE;
	}

	if (dirty) {
		BI_PRINT("NOK, dirty (not saved yet).\n");
		return CMD_RET_FAILURE;
	}

	BI_PRINT("OK, complete.\n");
	return CMD_RET_SUCCESS;
}

static cmd_tbl_t cmd_boardinfo_sub[] = {
		U_BOOT_CMD_MKENT(show, 2, 0, do_boardinfo_show, "", ""),
		U_BOOT_CMD_MKENT(name, 3, 0, do_boardinfo_name, "", ""),
		U_BOOT_CMD_MKENT(variant, 3, 0, do_boardinfo_variant, "", ""),
		U_BOOT_CMD_MKENT(feature, 3, 0, do_boardinfo_feature, "", ""),
		U_BOOT_CMD_MKENT(serial, 3, 0, do_boardinfo_serial, "", ""),
		U_BOOT_CMD_MKENT(revision, 3, 0, do_boardinfo_revision, "", ""),
		U_BOOT_CMD_MKENT(save, 2, 0, do_boardinfo_save, "", ""),
		U_BOOT_CMD_MKENT(complete, 2, 0, do_boardinfo_complete, "", ""),
};

static int do_boardinfo(cmd_tbl_t *cmdtp, int flag, int argc,
		char *const argv[])
{
	cmd_tbl_t *c;

	argc--; argv++;

	c = find_cmd_tbl(argv[0], cmd_boardinfo_sub,
			 ARRAY_SIZE(cmd_boardinfo_sub));

	if (c)
		return c->cmd(cmdtp, flag, argc, argv);
	else
		return CMD_RET_USAGE;
}

U_BOOT_CMD(
	boardinfo, 3, 1, do_boardinfo,
	"Miscellaneous boardinfo commands",
	"show              - read and dump boardinfo\n"
	"boardinfo name <string>     - set boardname.\n"
	"boardinfo variant <string>  - set variant key.\n"
	"boardinfo feature <string>  - set feature key.\n"
	"boardinfo serial <string>   - set serial number.\n"
	"boardinfo revision <string> - set revision.\n"
	"boardinfo save              - save boardinfo to EEPROM\n"
	"boardinfo complete          - check boardinfo completeness and integrity\n"
);

#endif /* CONFIG_CMD_BOARDINFO */
#endif /* !defined(CONFIG_SPL_BUILD) */
