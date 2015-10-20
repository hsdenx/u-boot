#ifndef __MSC_NR_IMX6_EEPROM_H__
#define __MSC_NR_IMX6_EEPROM_H__
/*
 * Copyright (C) 2015 MSC Technologies
 *       <www.msc-technologies.eu>
 *
 * SPDX-License-Identifier:	GPL-2.0
 */

int i2c_eeprom_read(unsigned chip_addr, unsigned addr, unsigned alen,
		uchar *buffer, unsigned bytecnt);
int i2c_eeprom_write(unsigned chip_addr, unsigned addr, unsigned alen,
		uchar *buffer, unsigned bytecnt);

#endif /* __MSC_NR_IMX6_EEPROM_H__ */
