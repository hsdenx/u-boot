/*
 * from linux:
 * 7538ec7d1e5: ARM: at91: remove no-MMU at91x40 support
 *
 * arch/arm/mach-at91/include/mach/cpu.h
 *
 * Copyright (C) 2006 SAN People
 * Copyright (C) 2011 Jean-Christophe PLAGNIOL-VILLARD <plagnioj@jcrosoft.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 *
 */

#ifndef __AT91_MACH_CPU_H__
#define __AT91_MACH_CPU_H__

#ifdef CONFIG_AT91RM9200
#define cpu_is_at91rm9200()	(1)
#else
#define cpu_is_at91rm9200()	(0)
#endif

#ifdef CONFIG_AT91SAM9260
#define cpu_is_at91sam9260()	(1)
#else
#define cpu_is_at91sam9260()	(0)
#endif

#ifdef CONFIG_AT91SAM9G20
#define cpu_is_at91sam9g20()	(1)
#else
#define cpu_is_at91sam9g20()	(0)
#endif

#ifdef CONFIG_AT91SAM9XE
#define cpu_is_at91sam9xe()	(1)
#else
#define cpu_is_at91sam9xe()	(0)
#endif

#ifdef CONFIG_AT91SAM9261
#define cpu_is_at91sam9261()	(1)
#else
#define cpu_is_at91sam9261()	(0)
#endif

#ifdef CONFIG_AT91SAM9G10
#define cpu_is_at91sam9g10()	(1)
#else
#define cpu_is_at91sam9g10()	(0)
#endif

#ifdef CONFIG_AT91SAM9263
#define cpu_is_at91sam9263()	(1)
#else
#define cpu_is_at91sam9263()	(0)
#endif

#ifdef CONFIG_AT91SAM9RL
#define cpu_is_at91sam9rl()	(1)
#else
#define cpu_is_at91sam9rl()	(0)
#endif

#ifdef CONFIG_AT91SAM9G45
#define cpu_is_at91sam9g45()	(1)
#else
#define cpu_is_at91sam9g45()	(0)
#endif

#ifdef CONFIG_AT91SAM9G45ES
#define cpu_is_at91sam9g45es()	(1)
#else
#define cpu_is_at91sam9g45es()	(0)
#endif

#ifdef CONFIG_AT91SAM9M10
#define cpu_is_at91sam9m10()	(1)
#else
#define cpu_is_at91sam9m10()	(0)
#endif

#ifdef CONFIG_AT91SAM9G46
#define cpu_is_at91sam9g46()	(1)
#else
#define cpu_is_at91sam9g46()	(0)
#endif

#ifdef CONFIG_AT91SAM9M11
#define cpu_is_at91sam9m11()	(1)
#else
#define cpu_is_at91sam9m11()	(0)
#endif

#ifdef CONFIG_AT91SAM9X5
#define cpu_is_at91sam9x5()	(1)
#else
#define cpu_is_at91sam9x5()	(0)
#endif

#ifdef CONFIG_AT91SAM9G15
#define cpu_is_at91sam9g15()	(1)
#else
#define cpu_is_at91sam9g15()	(0)
#endif

#ifdef CONFIG_AT91SAM9G35
#define cpu_is_at91sam9g35()	(1)
#else
#define cpu_is_at91sam9g35()	(0)
#endif

#ifdef CONFIG_AT91SAM9X35
#define cpu_is_at91sam9x35()	(1)
#else
#define cpu_is_at91sam9x35()	(0)
#endif

#ifdef CONFIG_AT91SAM9G25
#define cpu_is_at91sam9g25()	(1)
#else
#define cpu_is_at91sam9g25()	(0)
#endif

#ifdef CONFIG_AT91SAM9X25
#define cpu_is_at91sam9x25()	(1)
#else
#define cpu_is_at91sam9x25()	(0)
#endif

#ifdef CONFIG_AT91SAM9N12
#define cpu_is_at91sam9n12()	(1)
#else
#define cpu_is_at91sam9n12()	(0)
#endif

#ifdef CONFIG_SAMA5D3
#define cpu_is_sama5d3()	(1)
#else
#define cpu_is_sama5d3()	(0)
#endif

#ifdef CONFIG_SAMA5D4
#define cpu_is_sama5d4()	(1)
#else
#define cpu_is_sama5d4()	(0)
#endif

#endif
