#
# SPDX-License-Identifier:	GPL-2.0+
#

dtb-$(CONFIG_TARGET_CANYONLANDS) += arches.dtb canyonlands.dtb glacier.dtb
dtb-$(CONFIG_TARGET_XILINX_PPC440_GENERIC) += xilinx-ppc440-generic.dtb
dtb-$(CONFIG_TARGET_XILINX_PPC405_GENERIC) += xilinx-ppc405-generic.dtb

targets += $(dtb-y)

DTC_FLAGS += -R 4 -p 0x1000
# Disable noisy checks by default
DTC_FLAGS += -Wno-unit_address_vs_reg

PHONY += dtbs
dtbs: $(addprefix $(obj)/, $(dtb-y))
	@:

clean-files := *.dtb
