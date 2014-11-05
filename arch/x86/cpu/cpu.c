/*
 * (C) Copyright 2008-2011
 * Graeme Russ, <graeme.russ@gmail.com>
 *
 * (C) Copyright 2002
 * Daniel Engström, Omicron Ceti AB, <daniel@omicron.se>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <command.h>
#include <errno.h>
#include <malloc.h>
#include <asm/control_regs.h>
#include <asm/cpu.h>
#include <asm/processor.h>
#include <asm/processor-flags.h>
#include <asm/interrupt.h>
#include <linux/compiler.h>

/*
 * Constructor for a conventional segment GDT (or LDT) entry
 * This is a macro so it can be used in initialisers
 */
#define GDT_ENTRY(flags, base, limit)			\
	((((base)  & 0xff000000ULL) << (56-24)) |	\
	 (((flags) & 0x0000f0ffULL) << 40) |		\
	 (((limit) & 0x000f0000ULL) << (48-16)) |	\
	 (((base)  & 0x00ffffffULL) << 16) |		\
	 (((limit) & 0x0000ffffULL)))

struct gdt_ptr {
	u16 len;
	u32 ptr;
} __packed;

static void load_ds(u32 segment)
{
	asm volatile("movl %0, %%ds" : : "r" (segment * X86_GDT_ENTRY_SIZE));
}

static void load_es(u32 segment)
{
	asm volatile("movl %0, %%es" : : "r" (segment * X86_GDT_ENTRY_SIZE));
}

static void load_fs(u32 segment)
{
	asm volatile("movl %0, %%fs" : : "r" (segment * X86_GDT_ENTRY_SIZE));
}

static void load_gs(u32 segment)
{
	asm volatile("movl %0, %%gs" : : "r" (segment * X86_GDT_ENTRY_SIZE));
}

static void load_ss(u32 segment)
{
	asm volatile("movl %0, %%ss" : : "r" (segment * X86_GDT_ENTRY_SIZE));
}

static void load_gdt(const u64 *boot_gdt, u16 num_entries)
{
	struct gdt_ptr gdt;

	gdt.len = (num_entries * 8) - 1;
	gdt.ptr = (u32)boot_gdt;

	asm volatile("lgdtl %0\n" : : "m" (gdt));
}

void setup_gdt(gd_t *id, u64 *gdt_addr)
{
	/* CS: code, read/execute, 4 GB, base 0 */
	gdt_addr[X86_GDT_ENTRY_32BIT_CS] = GDT_ENTRY(0xc09b, 0, 0xfffff);

	/* DS: data, read/write, 4 GB, base 0 */
	gdt_addr[X86_GDT_ENTRY_32BIT_DS] = GDT_ENTRY(0xc093, 0, 0xfffff);

	/* FS: data, read/write, 4 GB, base (Global Data Pointer) */
	id->arch.gd_addr = id;
	gdt_addr[X86_GDT_ENTRY_32BIT_FS] = GDT_ENTRY(0xc093,
		     (ulong)&id->arch.gd_addr, 0xfffff);

	/* 16-bit CS: code, read/execute, 64 kB, base 0 */
	gdt_addr[X86_GDT_ENTRY_16BIT_CS] = GDT_ENTRY(0x109b, 0, 0x0ffff);

	/* 16-bit DS: data, read/write, 64 kB, base 0 */
	gdt_addr[X86_GDT_ENTRY_16BIT_DS] = GDT_ENTRY(0x1093, 0, 0x0ffff);

	load_gdt(gdt_addr, X86_GDT_NUM_ENTRIES);
	load_ds(X86_GDT_ENTRY_32BIT_DS);
	load_es(X86_GDT_ENTRY_32BIT_DS);
	load_gs(X86_GDT_ENTRY_32BIT_DS);
	load_ss(X86_GDT_ENTRY_32BIT_DS);
	load_fs(X86_GDT_ENTRY_32BIT_FS);
}

int __weak x86_cleanup_before_linux(void)
{
#ifdef CONFIG_BOOTSTAGE_STASH
	bootstage_stash((void *)CONFIG_BOOTSTAGE_STASH,
			CONFIG_BOOTSTAGE_STASH_SIZE);
#endif

	return 0;
}

int x86_cpu_init_f(void)
{
	const u32 em_rst = ~X86_CR0_EM;
	const u32 mp_ne_set = X86_CR0_MP | X86_CR0_NE;

	/* initialize FPU, reset EM, set MP and NE */
	asm ("fninit\n" \
	     "movl %%cr0, %%eax\n" \
	     "andl %0, %%eax\n" \
	     "orl  %1, %%eax\n" \
	     "movl %%eax, %%cr0\n" \
	     : : "i" (em_rst), "i" (mp_ne_set) : "eax");

	return 0;
}
int cpu_init_f(void) __attribute__((weak, alias("x86_cpu_init_f")));

int x86_cpu_init_r(void)
{
	/* Initialize core interrupt and exception functionality of CPU */
	cpu_init_interrupts();
	return 0;
}
int cpu_init_r(void) __attribute__((weak, alias("x86_cpu_init_r")));

void x86_enable_caches(void)
{
	unsigned long cr0;

	cr0 = read_cr0();
	cr0 &= ~(X86_CR0_NW | X86_CR0_CD);
	write_cr0(cr0);
	wbinvd();
}
void enable_caches(void) __attribute__((weak, alias("x86_enable_caches")));

void x86_disable_caches(void)
{
	unsigned long cr0;

	cr0 = read_cr0();
	cr0 |= X86_CR0_NW | X86_CR0_CD;
	wbinvd();
	write_cr0(cr0);
	wbinvd();
}
void disable_caches(void) __attribute__((weak, alias("x86_disable_caches")));

int x86_init_cache(void)
{
	enable_caches();

	return 0;
}
int init_cache(void) __attribute__((weak, alias("x86_init_cache")));

int do_reset(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	printf("resetting ...\n");

	/* wait 50 ms */
	udelay(50000);
	disable_interrupts();
	reset_cpu(0);

	/*NOTREACHED*/
	return 0;
}

void  flush_cache(unsigned long dummy1, unsigned long dummy2)
{
	asm("wbinvd\n");
}

void __attribute__ ((regparm(0))) generate_gpf(void);

/* segment 0x70 is an arbitrary segment which does not exist */
asm(".globl generate_gpf\n"
	".hidden generate_gpf\n"
	".type generate_gpf, @function\n"
	"generate_gpf:\n"
	"ljmp   $0x70, $0x47114711\n");

void __reset_cpu(ulong addr)
{
	printf("Resetting using x86 Triple Fault\n");
	set_vector(13, generate_gpf);	/* general protection fault handler */
	set_vector(8, generate_gpf);	/* double fault handler */
	generate_gpf();			/* start the show */
}
void reset_cpu(ulong addr) __attribute__((weak, alias("__reset_cpu")));

int dcache_status(void)
{
	return !(read_cr0() & 0x40000000);
}

/* Define these functions to allow ehch-hcd to function */
void flush_dcache_range(unsigned long start, unsigned long stop)
{
}

void invalidate_dcache_range(unsigned long start, unsigned long stop)
{
}

void dcache_enable(void)
{
	enable_caches();
}

void dcache_disable(void)
{
	disable_caches();
}

void icache_enable(void)
{
}

void icache_disable(void)
{
}

int icache_status(void)
{
	return 1;
}

void cpu_enable_paging_pae(ulong cr3)
{
	__asm__ __volatile__(
		/* Load the page table address */
		"movl	%0, %%cr3\n"
		/* Enable pae */
		"movl	%%cr4, %%eax\n"
		"orl	$0x00000020, %%eax\n"
		"movl	%%eax, %%cr4\n"
		/* Enable paging */
		"movl	%%cr0, %%eax\n"
		"orl	$0x80000000, %%eax\n"
		"movl	%%eax, %%cr0\n"
		:
		: "r" (cr3)
		: "eax");
}

void cpu_disable_paging_pae(void)
{
	/* Turn off paging */
	__asm__ __volatile__ (
		/* Disable paging */
		"movl	%%cr0, %%eax\n"
		"andl	$0x7fffffff, %%eax\n"
		"movl	%%eax, %%cr0\n"
		/* Disable pae */
		"movl	%%cr4, %%eax\n"
		"andl	$0xffffffdf, %%eax\n"
		"movl	%%eax, %%cr4\n"
		:
		:
		: "eax");
}

static bool has_cpuid(void)
{
	unsigned long flag;

	asm volatile("pushf\n" \
		"pop %%eax\n"
		"mov %%eax, %%ecx\n"	/* ecx = flags */
		"xor %1, %%eax\n"
		"push %%eax\n"
		"popf\n"		/* flags ^= $2 */
		"pushf\n"
		"pop %%eax\n"		/* eax = flags */
		"push %%ecx\n"
		"popf\n"		/* flags = ecx */
		"xor %%ecx, %%eax\n"
		"mov %%eax, %0"
		: "=r" (flag)
		: "i" (1 << 21)
		: "eax", "ecx", "memory");

	return flag != 0;
}

static bool can_detect_long_mode(void)
{
	unsigned long flag;

	asm volatile("mov $0x80000000, %%eax\n"
		"cpuid\n"
		"mov %%eax, %0"
		: "=r" (flag)
		:
		: "eax", "ebx", "ecx", "edx", "memory");

	return flag > 0x80000000UL;
}

static bool has_long_mode(void)
{
	unsigned long flag;

	asm volatile("mov $0x80000001, %%eax\n"
		"cpuid\n"
		"mov %%edx, %0"
		: "=r" (flag)
		:
		: "eax", "ebx", "ecx", "edx", "memory");

	return flag & (1 << 29) ? true : false;
}

int cpu_has_64bit(void)
{
	return has_cpuid() && can_detect_long_mode() &&
		has_long_mode();
}

int print_cpuinfo(void)
{
	printf("CPU:   %s\n", cpu_has_64bit() ? "x86_64" : "x86");

	return 0;
}

#define PAGETABLE_SIZE		(6 * 4096)

/**
 * build_pagetable() - build a flat 4GiB page table structure for 64-bti mode
 *
 * @pgtable: Pointer to a 24iKB block of memory
 */
static void build_pagetable(uint32_t *pgtable)
{
	uint i;

	memset(pgtable, '\0', PAGETABLE_SIZE);

	/* Level 4 needs a single entry */
	pgtable[0] = (uint32_t)&pgtable[1024] + 7;

	/* Level 3 has one 64-bit entry for each GiB of memory */
	for (i = 0; i < 4; i++) {
		pgtable[1024 + i * 2] = (uint32_t)&pgtable[2048] +
							0x1000 * i + 7;
	}

	/* Level 2 has 2048 64-bit entries, each repesenting 2MiB */
	for (i = 0; i < 2048; i++)
		pgtable[2048 + i * 2] = 0x183 + (i << 21UL);
}

int cpu_jump_to_64bit(ulong setup_base, ulong target)
{
	uint32_t *pgtable;

	pgtable = memalign(4096, PAGETABLE_SIZE);
	if (!pgtable)
		return -ENOMEM;

	build_pagetable(pgtable);
	cpu_call64((ulong)pgtable, setup_base, target);
	free(pgtable);

	return -EFAULT;
}
