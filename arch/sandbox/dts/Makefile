#
# SPDX-License-Identifier:	GPL-2.0+
#

dtb-$(CONFIG_SANDBOX) += sandbox.dtb
dtb-$(CONFIG_UT_DM) += test.dtb

targets += $(dtb-y)

DTC_FLAGS += -R 4 -p 0x1000
# Disable noisy checks by default
DTC_FLAGS += -Wno-unit_address_vs_reg

PHONY += dtbs
dtbs: $(addprefix $(obj)/, $(dtb-y))
	@:

clean-files := *.dtb
