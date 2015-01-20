/*
 * pseries Memory Hotplug infrastructure.
 *
 * Copyright (C) 2008 Badari Pulavarty, IBM Corporation
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version
 *      2 of the License, or (at your option) any later version.
 */

#include <linux/of.h>
#include <linux/memblock.h>
#include <linux/vmalloc.h>
#include <linux/memory.h>

#include <asm/firmware.h>
#include <asm/machdep.h>
#include <asm/pSeries_reconfig.h>
#include <asm/sparsemem.h>

static unsigned long get_memblock_size(void)
{
	struct device_node *np;
	unsigned int memblock_size = MIN_MEMORY_BLOCK_SIZE;
	struct resource r;

	np = of_find_node_by_path("/ibm,dynamic-reconfiguration-memory");
	if (np) {
		const __be64 *size;

		size = of_get_property(np, "ibm,lmb-size", NULL);
		if (size)
			memblock_size = be64_to_cpup(size);
		of_node_put(np);
	} else  if (machine_is(pseries)) {
		/*                                              */
		unsigned int memzero_size = 0;

		np = of_find_node_by_path("/memory@0");
		if (np) {
			if (!of_address_to_resource(np, 0, &r))
				memzero_size = resource_size(&r);
			of_node_put(np);
		}

		if (memzero_size) {
			/*                                                   
                                             
    */
			char buf[64];

			sprintf(buf, "/memory@%x", memzero_size);
			np = of_find_node_by_path(buf);
			if (np) {
				if (!of_address_to_resource(np, 0, &r))
					memblock_size = resource_size(&r);
				of_node_put(np);
			}
		}
	}
	return memblock_size;
}

/*                                                                   
                                                                    
                                                                 
                                                                 
                   
 */
unsigned long memory_block_size_bytes(void)
{
	return get_memblock_size();
}

static int pseries_remove_memblock(unsigned long base, unsigned int memblock_size)
{
	unsigned long start, start_pfn;
	struct zone *zone;
	int ret;

	start_pfn = base >> PAGE_SHIFT;

	if (!pfn_valid(start_pfn)) {
		memblock_remove(base, memblock_size);
		return 0;
	}

	zone = page_zone(pfn_to_page(start_pfn));

	/*
                                                     
                                          
   
                                                           
                                                               
                                                           
                                                        
  */
	ret = __remove_pages(zone, start_pfn, memblock_size >> PAGE_SHIFT);
	if (ret)
		return ret;

	/*
                                           
  */
	memblock_remove(base, memblock_size);

	/*
                                                          
  */
	start = (unsigned long)__va(base);
	ret = remove_section_mapping(start, start + memblock_size);

	/*                                                          
                              
  */
	vm_unmap_aliases();

	return ret;
}

static int pseries_remove_memory(struct device_node *np)
{
	const char *type;
	const unsigned int *regs;
	unsigned long base;
	unsigned int lmb_size;
	int ret = -EINVAL;

	/*
                                                   
  */
	type = of_get_property(np, "device_type", NULL);
	if (type == NULL || strcmp(type, "memory") != 0)
		return 0;

	/*
                                                 
  */
	regs = of_get_property(np, "reg", NULL);
	if (!regs)
		return ret;

	base = *(unsigned long *)regs;
	lmb_size = regs[3];

	ret = pseries_remove_memblock(base, lmb_size);
	return ret;
}

static int pseries_add_memory(struct device_node *np)
{
	const char *type;
	const unsigned int *regs;
	unsigned long base;
	unsigned int lmb_size;
	int ret = -EINVAL;

	/*
                                                 
  */
	type = of_get_property(np, "device_type", NULL);
	if (type == NULL || strcmp(type, "memory") != 0)
		return 0;

	/*
                                          
  */
	regs = of_get_property(np, "reg", NULL);
	if (!regs)
		return ret;

	base = *(unsigned long *)regs;
	lmb_size = regs[3];

	/*
                                                    
  */
	ret = memblock_add(base, lmb_size);
	return (ret < 0) ? -EINVAL : 0;
}

static int pseries_drconf_memory(unsigned long *base, unsigned int action)
{
	unsigned long memblock_size;
	int rc;

	memblock_size = get_memblock_size();
	if (!memblock_size)
		return -EINVAL;

	if (action == PSERIES_DRCONF_MEM_ADD) {
		rc = memblock_add(*base, memblock_size);
		rc = (rc < 0) ? -EINVAL : 0;
	} else if (action == PSERIES_DRCONF_MEM_REMOVE) {
		rc = pseries_remove_memblock(*base, memblock_size);
	} else {
		rc = -EINVAL;
	}

	return rc;
}

static int pseries_memory_notifier(struct notifier_block *nb,
				unsigned long action, void *node)
{
	int err = 0;

	switch (action) {
	case PSERIES_RECONFIG_ADD:
		err = pseries_add_memory(node);
		break;
	case PSERIES_RECONFIG_REMOVE:
		err = pseries_remove_memory(node);
		break;
	case PSERIES_DRCONF_MEM_ADD:
	case PSERIES_DRCONF_MEM_REMOVE:
		err = pseries_drconf_memory(node, action);
		break;
	}
	return notifier_from_errno(err);
}

static struct notifier_block pseries_mem_nb = {
	.notifier_call = pseries_memory_notifier,
};

static int __init pseries_memory_hotplug_init(void)
{
	if (firmware_has_feature(FW_FEATURE_LPAR))
		pSeries_reconfig_notifier_register(&pseries_mem_nb);

	return 0;
}
machine_device_initcall(pseries, pseries_memory_hotplug_init);
