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
#include "eeprom.h"

static int __eeprom_read(unsigned chip_addr, unsigned addr, unsigned alen,
		uchar *buffer, unsigned bytecnt)
{
	int chunk_size = 8;
	int ret;
	int chunks_read = 0;
	int bytes_remain = bytecnt;

	while (bytes_remain) {
		int bytes_to_read = min(bytes_remain, chunk_size);

		ret = i2c_read(chip_addr,
				addr + (chunks_read*chunk_size), alen,
				buffer + (chunks_read*chunk_size),
				bytes_to_read);
		if (ret)
			return ret;

		chunks_read += 1;
		bytes_remain -= bytes_to_read;
	}

	return 0;
}

static int __eeprom_write(unsigned chip_addr, unsigned addr, unsigned alen,
		uchar *buffer, unsigned bytecnt)
{
	int chunk_size = 8;
	int ret;
	int chunks_saved = 0;
	int bytes_remain = bytecnt;

	while (bytes_remain) {
		int bytes_to_write = min(bytes_remain, chunk_size);

		ret = i2c_write(chip_addr,
				addr + (chunks_saved*chunk_size), alen,
				buffer + (chunks_saved*chunk_size),
				bytes_to_write);
		if (ret)
			return ret;

		chunks_saved += 1;
		bytes_remain -= bytes_to_write;

		mdelay(5);
	}

	return 0;
}
int i2c_eeprom_read(unsigned chip_addr, unsigned addr, unsigned alen,
		 uchar *buffer, unsigned bytecnt)
{
	int retry = 3;
	int n = 0;
	int ret;

	memset(buffer, 0, bytecnt);

	while (n++ < retry) {
		ret = __eeprom_read(chip_addr, addr, alen, buffer, bytecnt);
		if (!ret)
			break;
		if (ret != -ENODEV)
			break;
		mdelay(10);
	}
	return ret;
}

int i2c_eeprom_write(unsigned chip_addr, unsigned addr, unsigned alen,
		uchar *buffer, unsigned bytecnt)
{
	int retry = 3;
	int n = 0;
	int ret;

	while (n++ < retry) {
		ret = __eeprom_write(chip_addr, addr, alen, buffer, bytecnt);
		if (!ret)
			break;
		if (ret != -ENODEV)
			break;
		mdelay(10);
	}
	mdelay(100);
	return ret;
}
