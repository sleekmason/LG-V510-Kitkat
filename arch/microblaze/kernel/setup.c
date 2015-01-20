/*
 * Copyright (C) 2007-2009 Michal Simek <monstr@monstr.eu>
 * Copyright (C) 2007-2009 PetaLogix
 * Copyright (C) 2006 Atmark Techno, Inc.
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License. See the file "COPYING" in the main directory of this archive
 * for more details.
 */

#include <linux/init.h>
#include <linux/string.h>
#include <linux/seq_file.h>
#include <linux/cpu.h>
#include <linux/initrd.h>
#include <linux/console.h>
#include <linux/debugfs.h>

#include <asm/setup.h>
#include <asm/sections.h>
#include <asm/page.h>
#include <linux/io.h>
#include <linux/bug.h>
#include <linux/param.h>
#include <linux/pci.h>
#include <linux/cache.h>
#include <linux/of_platform.h>
#include <linux/dma-mapping.h>
#include <asm/cacheflush.h>
#include <asm/entry.h>
#include <asm/cpuinfo.h>

#include <asm/prom.h>
#include <asm/pgtable.h>

DEFINE_PER_CPU(unsigned int, KSP);	/*                            */
DEFINE_PER_CPU(unsigned int, KM);	/*                  */
DEFINE_PER_CPU(unsigned int, ENTRY_SP);	/*                          */
DEFINE_PER_CPU(unsigned int, R11_SAVE);	/*                         */
DEFINE_PER_CPU(unsigned int, CURRENT_SAVE);	/*                       */

unsigned int boot_cpuid;
char cmd_line[COMMAND_LINE_SIZE];

void __init setup_arch(char **cmdline_p)
{
	*cmdline_p = cmd_line;

	console_verbose();

	unflatten_device_tree();

	setup_cpuinfo();

	microblaze_cache_init();

	setup_memory();

#ifdef CONFIG_EARLY_PRINTK
	/*                                        */
	remap_early_printk();
#endif

	xilinx_pci_init();

#if defined(CONFIG_SELFMOD_INTC) || defined(CONFIG_SELFMOD_TIMER)
	printk(KERN_NOTICE "Self modified code enable\n");
#endif

#ifdef CONFIG_VT
#if defined(CONFIG_XILINX_CONSOLE)
	conswitchp = &xil_con;
#elif defined(CONFIG_DUMMY_CONSOLE)
	conswitchp = &dummy_con;
#endif
#endif
}

#ifdef CONFIG_MTD_UCLINUX
/*                                                                   
                                                                 */
inline unsigned get_romfs_len(unsigned *addr)
{
#ifdef CONFIG_ROMFS_FS
	if (memcmp(&addr[0], "-rom1fs-", 8) == 0) /*       */
		return be32_to_cpu(addr[2]);
#endif

#ifdef CONFIG_CRAMFS
	if (addr[0] == le32_to_cpu(0x28cd3d45)) /*        */
		return le32_to_cpu(addr[1]);
#endif
	return 0;
}
#endif	/*                         */

unsigned long kernel_tlb;

void __init machine_early_init(const char *cmdline, unsigned int ram,
		unsigned int fdt, unsigned int msr, unsigned int tlb0,
		unsigned int tlb1)
{
	unsigned long *src, *dst;
	unsigned int offset = 0;

	/*                                                         
                                                                 
                                                       
  */
#ifdef CONFIG_MTD_UCLINUX
	int romfs_size;
	unsigned int romfs_base;
	char *old_klimit = klimit;

	romfs_base = (ram ? ram : (unsigned int)&__init_end);
	romfs_size = PAGE_ALIGN(get_romfs_len((unsigned *)romfs_base));
	if (!romfs_size) {
		romfs_base = (unsigned int)&__bss_start;
		romfs_size = PAGE_ALIGN(get_romfs_len((unsigned *)romfs_base));
	}

	/*                                          */
	if (romfs_size > 0) {
		memmove(&_ebss, (int *)romfs_base, romfs_size);
		klimit += romfs_size;
	}
#endif

/*                      */
	memset(__bss_start, 0, __bss_stop-__bss_start);
	memset(_ssbss, 0, _esbss-_ssbss);

	/*                                          */
#ifndef CONFIG_CMDLINE_BOOL
	if (cmdline && cmdline[0] != '\0')
		strlcpy(cmd_line, cmdline, COMMAND_LINE_SIZE);
#endif

	lockdep_init();

/*                                                  */
	early_init_devtree((void *)_fdt_start);

#ifdef CONFIG_EARLY_PRINTK
	setup_early_printk(NULL);
#endif

	/*                                    
                                    */
	kernel_tlb = tlb0 + tlb1;
	/*                                                     
                          */

	printk("Ramdisk addr 0x%08x, ", ram);
	if (fdt)
		printk("FDT at 0x%08x\n", fdt);
	else
		printk("Compiled-in FDT at 0x%08x\n",
					(unsigned int)_fdt_start);

#ifdef CONFIG_MTD_UCLINUX
	printk("Found romfs @ 0x%08x (0x%08x)\n",
			romfs_base, romfs_size);
	printk("#### klimit %p ####\n", old_klimit);
	BUG_ON(romfs_size < 0); /*                      */

	printk("Moved 0x%08x bytes from 0x%08x to 0x%08x\n",
			romfs_size, romfs_base, (unsigned)&_ebss);

	printk("New klimit: 0x%08x\n", (unsigned)klimit);
#endif

#if CONFIG_XILINX_MICROBLAZE0_USE_MSR_INSTR
	if (msr)
		printk("!!!Your kernel has setup MSR instruction but "
				"CPU don't have it %x\n", msr);
#else
	if (!msr)
		printk("!!!Your kernel not setup MSR instruction but "
				"CPU have it %x\n", msr);
#endif

	/*                                                             
                                                                   
                                                                       */
#if !CONFIG_MANUAL_RESET_VECTOR
	offset = 0x2;
#endif
	dst = (unsigned long *) (offset * sizeof(u32));
	for (src = __ivt_start + offset; src < __ivt_end; src++, dst++)
		*dst = *src;

	/*                        */
	per_cpu(KM, 0) = 0x1;	/*                         */
	per_cpu(CURRENT_SAVE, 0) = (unsigned long)current;
}

#ifdef CONFIG_DEBUG_FS
struct dentry *of_debugfs_root;

static int microblaze_debugfs_init(void)
{
	of_debugfs_root = debugfs_create_dir("microblaze", NULL);

	return of_debugfs_root == NULL;
}
arch_initcall(microblaze_debugfs_init);

# ifdef CONFIG_MMU
static int __init debugfs_tlb(void)
{
	struct dentry *d;

	if (!of_debugfs_root)
		return -ENODEV;

	d = debugfs_create_u32("tlb_skip", S_IRUGO, of_debugfs_root, &tlb_skip);
	if (!d)
		return -ENOMEM;
}
device_initcall(debugfs_tlb);
# endif
#endif

static int dflt_bus_notify(struct notifier_block *nb,
				unsigned long action, void *data)
{
	struct device *dev = data;

	/*                                          */
	if (action != BUS_NOTIFY_ADD_DEVICE)
		return 0;

	set_dma_ops(dev, &dma_direct_ops);

	return NOTIFY_DONE;
}

static struct notifier_block dflt_plat_bus_notifier = {
	.notifier_call = dflt_bus_notify,
	.priority = INT_MAX,
};

static int __init setup_bus_notifier(void)
{
	bus_register_notifier(&platform_bus_type, &dflt_plat_bus_notifier);

	return 0;
}

arch_initcall(setup_bus_notifier);