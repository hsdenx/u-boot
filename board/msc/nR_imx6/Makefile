#
# Copyright (C) 2015 MSC Technologies
#       <www.msc-technologies.eu>
#
# SPDX-License-Identifier:	GPL-2.0
#
ifdef CONFIG_SPL_BUILD
obj-y := spl.o
obj-y += uboot.o
obj-y += dram.o
else
obj-y := uboot.o
obj-$(CONFIG_SYS_USE_VIDEO) += video.o
endif

obj-y += boardinfo.o
obj-y += eeprom.o
