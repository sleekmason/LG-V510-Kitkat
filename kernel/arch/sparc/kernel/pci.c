/* pci.c: UltraSparc PCI controller support.
 *
 * Copyright (C) 1997, 1998, 1999 David S. Miller (davem@redhat.com)
 * Copyright (C) 1998, 1999 Eddie C. Dost   (ecd@skynet.be)
 * Copyright (C) 1999 Jakub Jelinek   (jj@ultra.linux.cz)
 *
 * OF tree based PCI bus probing taken from the PowerPC port
 * with minor modifications, see there for credits.
 */

#include <linux/export.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/capability.h>
#include <linux/errno.h>
#include <linux/pci.h>
#include <linux/msi.h>
#include <linux/irq.h>
#include <linux/init.h>
#include <linux/of.h>
#include <linux/of_device.h>

#include <asm/uaccess.h>
#include <asm/pgtable.h>
#include <asm/irq.h>
#include <asm/prom.h>
#include <asm/apb.h>

#include "pci_impl.h"

/*                                                  */
struct pci_pbm_info *pci_pbm_root = NULL;

/*                                     */
int pci_num_pbms = 0;

volatile int pci_poke_in_progress;
volatile int pci_poke_cpu = -1;
volatile int pci_poke_faulted;

static DEFINE_SPINLOCK(pci_poke_lock);

void pci_config_read8(u8 *addr, u8 *ret)
{
	unsigned long flags;
	u8 byte;

	spin_lock_irqsave(&pci_poke_lock, flags);
	pci_poke_cpu = smp_processor_id();
	pci_poke_in_progress = 1;
	pci_poke_faulted = 0;
	__asm__ __volatile__("membar #Sync\n\t"
			     "lduba [%1] %2, %0\n\t"
			     "membar #Sync"
			     : "=r" (byte)
			     : "r" (addr), "i" (ASI_PHYS_BYPASS_EC_E_L)
			     : "memory");
	pci_poke_in_progress = 0;
	pci_poke_cpu = -1;
	if (!pci_poke_faulted)
		*ret = byte;
	spin_unlock_irqrestore(&pci_poke_lock, flags);
}

void pci_config_read16(u16 *addr, u16 *ret)
{
	unsigned long flags;
	u16 word;

	spin_lock_irqsave(&pci_poke_lock, flags);
	pci_poke_cpu = smp_processor_id();
	pci_poke_in_progress = 1;
	pci_poke_faulted = 0;
	__asm__ __volatile__("membar #Sync\n\t"
			     "lduha [%1] %2, %0\n\t"
			     "membar #Sync"
			     : "=r" (word)
			     : "r" (addr), "i" (ASI_PHYS_BYPASS_EC_E_L)
			     : "memory");
	pci_poke_in_progress = 0;
	pci_poke_cpu = -1;
	if (!pci_poke_faulted)
		*ret = word;
	spin_unlock_irqrestore(&pci_poke_lock, flags);
}

void pci_config_read32(u32 *addr, u32 *ret)
{
	unsigned long flags;
	u32 dword;

	spin_lock_irqsave(&pci_poke_lock, flags);
	pci_poke_cpu = smp_processor_id();
	pci_poke_in_progress = 1;
	pci_poke_faulted = 0;
	__asm__ __volatile__("membar #Sync\n\t"
			     "lduwa [%1] %2, %0\n\t"
			     "membar #Sync"
			     : "=r" (dword)
			     : "r" (addr), "i" (ASI_PHYS_BYPASS_EC_E_L)
			     : "memory");
	pci_poke_in_progress = 0;
	pci_poke_cpu = -1;
	if (!pci_poke_faulted)
		*ret = dword;
	spin_unlock_irqrestore(&pci_poke_lock, flags);
}

void pci_config_write8(u8 *addr, u8 val)
{
	unsigned long flags;

	spin_lock_irqsave(&pci_poke_lock, flags);
	pci_poke_cpu = smp_processor_id();
	pci_poke_in_progress = 1;
	pci_poke_faulted = 0;
	__asm__ __volatile__("membar #Sync\n\t"
			     "stba %0, [%1] %2\n\t"
			     "membar #Sync"
			     : /*            */
			     : "r" (val), "r" (addr), "i" (ASI_PHYS_BYPASS_EC_E_L)
			     : "memory");
	pci_poke_in_progress = 0;
	pci_poke_cpu = -1;
	spin_unlock_irqrestore(&pci_poke_lock, flags);
}

void pci_config_write16(u16 *addr, u16 val)
{
	unsigned long flags;

	spin_lock_irqsave(&pci_poke_lock, flags);
	pci_poke_cpu = smp_processor_id();
	pci_poke_in_progress = 1;
	pci_poke_faulted = 0;
	__asm__ __volatile__("membar #Sync\n\t"
			     "stha %0, [%1] %2\n\t"
			     "membar #Sync"
			     : /*            */
			     : "r" (val), "r" (addr), "i" (ASI_PHYS_BYPASS_EC_E_L)
			     : "memory");
	pci_poke_in_progress = 0;
	pci_poke_cpu = -1;
	spin_unlock_irqrestore(&pci_poke_lock, flags);
}

void pci_config_write32(u32 *addr, u32 val)
{
	unsigned long flags;

	spin_lock_irqsave(&pci_poke_lock, flags);
	pci_poke_cpu = smp_processor_id();
	pci_poke_in_progress = 1;
	pci_poke_faulted = 0;
	__asm__ __volatile__("membar #Sync\n\t"
			     "stwa %0, [%1] %2\n\t"
			     "membar #Sync"
			     : /*            */
			     : "r" (val), "r" (addr), "i" (ASI_PHYS_BYPASS_EC_E_L)
			     : "memory");
	pci_poke_in_progress = 0;
	pci_poke_cpu = -1;
	spin_unlock_irqrestore(&pci_poke_lock, flags);
}

static int ofpci_verbose;

static int __init ofpci_debug(char *str)
{
	int val = 0;

	get_option(&str, &val);
	if (val)
		ofpci_verbose = 1;
	return 1;
}

__setup("ofpci_debug=", ofpci_debug);

static unsigned long pci_parse_of_flags(u32 addr0)
{
	unsigned long flags = 0;

	if (addr0 & 0x02000000) {
		flags = IORESOURCE_MEM | PCI_BASE_ADDRESS_SPACE_MEMORY;
		flags |= (addr0 >> 22) & PCI_BASE_ADDRESS_MEM_TYPE_64;
		flags |= (addr0 >> 28) & PCI_BASE_ADDRESS_MEM_TYPE_1M;
		if (addr0 & 0x40000000)
			flags |= IORESOURCE_PREFETCH
				 | PCI_BASE_ADDRESS_MEM_PREFETCH;
	} else if (addr0 & 0x01000000)
		flags = IORESOURCE_IO | PCI_BASE_ADDRESS_SPACE_IO;
	return flags;
}

/*                                                                          
                                                                           
           
 */
static void pci_parse_of_addrs(struct platform_device *op,
			       struct device_node *node,
			       struct pci_dev *dev)
{
	struct resource *op_res;
	const u32 *addrs;
	int proplen;

	addrs = of_get_property(node, "assigned-addresses", &proplen);
	if (!addrs)
		return;
	if (ofpci_verbose)
		printk("    parse addresses (%d bytes) @ %p\n",
		       proplen, addrs);
	op_res = &op->resource[0];
	for (; proplen >= 20; proplen -= 20, addrs += 5, op_res++) {
		struct resource *res;
		unsigned long flags;
		int i;

		flags = pci_parse_of_flags(addrs[0]);
		if (!flags)
			continue;
		i = addrs[0] & 0xff;
		if (ofpci_verbose)
			printk("  start: %llx, end: %llx, i: %x\n",
			       op_res->start, op_res->end, i);

		if (PCI_BASE_ADDRESS_0 <= i && i <= PCI_BASE_ADDRESS_5) {
			res = &dev->resource[(i - PCI_BASE_ADDRESS_0) >> 2];
		} else if (i == dev->rom_base_reg) {
			res = &dev->resource[PCI_ROM_RESOURCE];
			flags |= IORESOURCE_READONLY | IORESOURCE_CACHEABLE
			      | IORESOURCE_SIZEALIGN;
		} else {
			printk(KERN_ERR "PCI: bad cfg reg num 0x%x\n", i);
			continue;
		}
		res->start = op_res->start;
		res->end = op_res->end;
		res->flags = flags;
		res->name = pci_name(dev);
	}
}

static struct pci_dev *of_create_pci_dev(struct pci_pbm_info *pbm,
					 struct device_node *node,
					 struct pci_bus *bus, int devfn)
{
	struct dev_archdata *sd;
	struct pci_slot *slot;
	struct platform_device *op;
	struct pci_dev *dev;
	const char *type;
	u32 class;

	dev = alloc_pci_dev();
	if (!dev)
		return NULL;

	sd = &dev->dev.archdata;
	sd->iommu = pbm->iommu;
	sd->stc = &pbm->stc;
	sd->host_controller = pbm;
	sd->op = op = of_find_device_by_node(node);
	sd->numa_node = pbm->numa_node;

	sd = &op->dev.archdata;
	sd->iommu = pbm->iommu;
	sd->stc = &pbm->stc;
	sd->numa_node = pbm->numa_node;

	if (!strcmp(node->name, "ebus"))
		of_propagate_archdata(op);

	type = of_get_property(node, "device_type", NULL);
	if (type == NULL)
		type = "";

	if (ofpci_verbose)
		printk("    create device, devfn: %x, type: %s\n",
		       devfn, type);

	dev->bus = bus;
	dev->sysdata = node;
	dev->dev.parent = bus->bridge;
	dev->dev.bus = &pci_bus_type;
	dev->dev.of_node = of_node_get(node);
	dev->devfn = devfn;
	dev->multifunction = 0;		/*              */
	set_pcie_port_type(dev);

	list_for_each_entry(slot, &dev->bus->slots, list)
		if (PCI_SLOT(dev->devfn) == slot->number)
			dev->slot = slot;

	dev->vendor = of_getintprop_default(node, "vendor-id", 0xffff);
	dev->device = of_getintprop_default(node, "device-id", 0xffff);
	dev->subsystem_vendor =
		of_getintprop_default(node, "subsystem-vendor-id", 0);
	dev->subsystem_device =
		of_getintprop_default(node, "subsystem-id", 0);

	dev->cfg_size = pci_cfg_space_size(dev);

	/*                                                  
                                                   
                                                    
                                                    
                                             
  */
	pci_read_config_dword(dev, PCI_CLASS_REVISION, &class);
	dev->class = class >> 8;
	dev->revision = class & 0xff;

	dev_set_name(&dev->dev, "%04x:%02x:%02x.%d", pci_domain_nr(bus),
		dev->bus->number, PCI_SLOT(devfn), PCI_FUNC(devfn));

	if (ofpci_verbose)
		printk("    class: 0x%x device name: %s\n",
		       dev->class, pci_name(dev));

	/*                                                  
                                                     
             
  */
	if ((dev->class >> 8) == PCI_CLASS_STORAGE_IDE)
		pci_set_master(dev);

	dev->current_state = 4;		/*                     */
	dev->error_state = pci_channel_io_normal;
	dev->dma_mask = 0xffffffff;

	if (!strcmp(node->name, "pci")) {
		/*                  */
		dev->hdr_type = PCI_HEADER_TYPE_BRIDGE;
		dev->rom_base_reg = PCI_ROM_ADDRESS1;
	} else if (!strcmp(type, "cardbus")) {
		dev->hdr_type = PCI_HEADER_TYPE_CARDBUS;
	} else {
		dev->hdr_type = PCI_HEADER_TYPE_NORMAL;
		dev->rom_base_reg = PCI_ROM_ADDRESS;

		dev->irq = sd->op->archdata.irqs[0];
		if (dev->irq == 0xffffffff)
			dev->irq = PCI_IRQ_NONE;
	}

	pci_parse_of_addrs(sd->op, node, dev);

	if (ofpci_verbose)
		printk("    adding to system ...\n");

	pci_device_add(dev, bus);

	return dev;
}

static void __devinit apb_calc_first_last(u8 map, u32 *first_p, u32 *last_p)
{
	u32 idx, first, last;

	first = 8;
	last = 0;
	for (idx = 0; idx < 8; idx++) {
		if ((map & (1 << idx)) != 0) {
			if (first > idx)
				first = idx;
			if (last < idx)
				last = idx;
		}
	}

	*first_p = first;
	*last_p = last;
}

/*                                                                  
                                                                      
                               
 */
static void __devinit pci_cfg_fake_ranges(struct pci_dev *dev,
					  struct pci_bus *bus,
					  struct pci_pbm_info *pbm)
{
	struct pci_bus_region region;
	struct resource *res, res2;
	u8 io_base_lo, io_limit_lo;
	u16 mem_base_lo, mem_limit_lo;
	unsigned long base, limit;

	pci_read_config_byte(dev, PCI_IO_BASE, &io_base_lo);
	pci_read_config_byte(dev, PCI_IO_LIMIT, &io_limit_lo);
	base = (io_base_lo & PCI_IO_RANGE_MASK) << 8;
	limit = (io_limit_lo & PCI_IO_RANGE_MASK) << 8;

	if ((io_base_lo & PCI_IO_RANGE_TYPE_MASK) == PCI_IO_RANGE_TYPE_32) {
		u16 io_base_hi, io_limit_hi;

		pci_read_config_word(dev, PCI_IO_BASE_UPPER16, &io_base_hi);
		pci_read_config_word(dev, PCI_IO_LIMIT_UPPER16, &io_limit_hi);
		base |= (io_base_hi << 16);
		limit |= (io_limit_hi << 16);
	}

	res = bus->resource[0];
	if (base <= limit) {
		res->flags = (io_base_lo & PCI_IO_RANGE_TYPE_MASK) | IORESOURCE_IO;
		res2.flags = res->flags;
		region.start = base;
		region.end = limit + 0xfff;
		pcibios_bus_to_resource(dev, &res2, &region);
		if (!res->start)
			res->start = res2.start;
		if (!res->end)
			res->end = res2.end;
	}

	pci_read_config_word(dev, PCI_MEMORY_BASE, &mem_base_lo);
	pci_read_config_word(dev, PCI_MEMORY_LIMIT, &mem_limit_lo);
	base = (mem_base_lo & PCI_MEMORY_RANGE_MASK) << 16;
	limit = (mem_limit_lo & PCI_MEMORY_RANGE_MASK) << 16;

	res = bus->resource[1];
	if (base <= limit) {
		res->flags = ((mem_base_lo & PCI_MEMORY_RANGE_TYPE_MASK) |
			      IORESOURCE_MEM);
		region.start = base;
		region.end = limit + 0xfffff;
		pcibios_bus_to_resource(dev, res, &region);
	}

	pci_read_config_word(dev, PCI_PREF_MEMORY_BASE, &mem_base_lo);
	pci_read_config_word(dev, PCI_PREF_MEMORY_LIMIT, &mem_limit_lo);
	base = (mem_base_lo & PCI_PREF_RANGE_MASK) << 16;
	limit = (mem_limit_lo & PCI_PREF_RANGE_MASK) << 16;

	if ((mem_base_lo & PCI_PREF_RANGE_TYPE_MASK) == PCI_PREF_RANGE_TYPE_64) {
		u32 mem_base_hi, mem_limit_hi;

		pci_read_config_dword(dev, PCI_PREF_BASE_UPPER32, &mem_base_hi);
		pci_read_config_dword(dev, PCI_PREF_LIMIT_UPPER32, &mem_limit_hi);

		/*
                                                           
                                                        
                                               
   */
		if (mem_base_hi <= mem_limit_hi) {
			base |= ((long) mem_base_hi) << 32;
			limit |= ((long) mem_limit_hi) << 32;
		}
	}

	res = bus->resource[2];
	if (base <= limit) {
		res->flags = ((mem_base_lo & PCI_MEMORY_RANGE_TYPE_MASK) |
			      IORESOURCE_MEM | IORESOURCE_PREFETCH);
		region.start = base;
		region.end = limit + 0xfffff;
		pcibios_bus_to_resource(dev, res, &region);
	}
}

/*                                                                 
                              
 */
static void __devinit apb_fake_ranges(struct pci_dev *dev,
				      struct pci_bus *bus,
				      struct pci_pbm_info *pbm)
{
	struct pci_bus_region region;
	struct resource *res;
	u32 first, last;
	u8 map;

	pci_read_config_byte(dev, APB_IO_ADDRESS_MAP, &map);
	apb_calc_first_last(map, &first, &last);
	res = bus->resource[0];
	res->flags = IORESOURCE_IO;
	region.start = (first << 21);
	region.end = (last << 21) + ((1 << 21) - 1);
	pcibios_bus_to_resource(dev, res, &region);

	pci_read_config_byte(dev, APB_MEM_ADDRESS_MAP, &map);
	apb_calc_first_last(map, &first, &last);
	res = bus->resource[1];
	res->flags = IORESOURCE_MEM;
	region.start = (first << 21);
	region.end = (last << 21) + ((1 << 21) - 1);
	pcibios_bus_to_resource(dev, res, &region);
}

static void __devinit pci_of_scan_bus(struct pci_pbm_info *pbm,
				      struct device_node *node,
				      struct pci_bus *bus);

#define GET_64BIT(prop, i)	((((u64) (prop)[(i)]) << 32) | (prop)[(i)+1])

static void __devinit of_scan_pci_bridge(struct pci_pbm_info *pbm,
					 struct device_node *node,
					 struct pci_dev *dev)
{
	struct pci_bus *bus;
	const u32 *busrange, *ranges;
	int len, i, simba;
	struct pci_bus_region region;
	struct resource *res;
	unsigned int flags;
	u64 size;

	if (ofpci_verbose)
		printk("of_scan_pci_bridge(%s)\n", node->full_name);

	/*                          */
	busrange = of_get_property(node, "bus-range", &len);
	if (busrange == NULL || len != 8) {
		printk(KERN_DEBUG "Can't get bus-range for PCI-PCI bridge %s\n",
		       node->full_name);
		return;
	}
	ranges = of_get_property(node, "ranges", &len);
	simba = 0;
	if (ranges == NULL) {
		const char *model = of_get_property(node, "model", NULL);
		if (model && !strcmp(model, "SUNW,simba"))
			simba = 1;
	}

	bus = pci_add_new_bus(dev->bus, dev, busrange[0]);
	if (!bus) {
		printk(KERN_ERR "Failed to create pci bus for %s\n",
		       node->full_name);
		return;
	}

	bus->primary = dev->bus->number;
	bus->subordinate = busrange[1];
	bus->bridge_ctl = 0;

	/*                                                         */
	/*                                                     */
	res = &dev->resource[PCI_BRIDGE_RESOURCES];
	for (i = 0; i < PCI_NUM_RESOURCES - PCI_BRIDGE_RESOURCES; ++i) {
		res->flags = 0;
		bus->resource[i] = res;
		++res;
	}
	if (simba) {
		apb_fake_ranges(dev, bus, pbm);
		goto after_ranges;
	} else if (ranges == NULL) {
		pci_cfg_fake_ranges(dev, bus, pbm);
		goto after_ranges;
	}
	i = 1;
	for (; len >= 32; len -= 32, ranges += 8) {
		flags = pci_parse_of_flags(ranges[0]);
		size = GET_64BIT(ranges, 6);
		if (flags == 0 || size == 0)
			continue;
		if (flags & IORESOURCE_IO) {
			res = bus->resource[0];
			if (res->flags) {
				printk(KERN_ERR "PCI: ignoring extra I/O range"
				       " for bridge %s\n", node->full_name);
				continue;
			}
		} else {
			if (i >= PCI_NUM_RESOURCES - PCI_BRIDGE_RESOURCES) {
				printk(KERN_ERR "PCI: too many memory ranges"
				       " for bridge %s\n", node->full_name);
				continue;
			}
			res = bus->resource[i];
			++i;
		}

		res->flags = flags;
		region.start = GET_64BIT(ranges, 1);
		region.end = region.start + size - 1;
		pcibios_bus_to_resource(dev, res, &region);
	}
after_ranges:
	sprintf(bus->name, "PCI Bus %04x:%02x", pci_domain_nr(bus),
		bus->number);
	if (ofpci_verbose)
		printk("    bus name: %s\n", bus->name);

	pci_of_scan_bus(pbm, node, bus);
}

static void __devinit pci_of_scan_bus(struct pci_pbm_info *pbm,
				      struct device_node *node,
				      struct pci_bus *bus)
{
	struct device_node *child;
	const u32 *reg;
	int reglen, devfn, prev_devfn;
	struct pci_dev *dev;

	if (ofpci_verbose)
		printk("PCI: scan_bus[%s] bus no %d\n",
		       node->full_name, bus->number);

	child = NULL;
	prev_devfn = -1;
	while ((child = of_get_next_child(node, child)) != NULL) {
		if (ofpci_verbose)
			printk("  * %s\n", child->full_name);
		reg = of_get_property(child, "reg", &reglen);
		if (reg == NULL || reglen < 20)
			continue;

		devfn = (reg[0] >> 8) & 0xff;

		/*                                           
                                               
                                                  
                                           
   */
		if (devfn == prev_devfn)
			continue;
		prev_devfn = devfn;

		/*                                      */
		dev = of_create_pci_dev(pbm, child, bus, devfn);
		if (!dev)
			continue;
		if (ofpci_verbose)
			printk("PCI: dev header type: %x\n",
			       dev->hdr_type);

		if (dev->hdr_type == PCI_HEADER_TYPE_BRIDGE ||
		    dev->hdr_type == PCI_HEADER_TYPE_CARDBUS)
			of_scan_pci_bridge(pbm, child, dev);
	}
}

static ssize_t
show_pciobppath_attr(struct device * dev, struct device_attribute * attr, char * buf)
{
	struct pci_dev *pdev;
	struct device_node *dp;

	pdev = to_pci_dev(dev);
	dp = pdev->dev.of_node;

	return snprintf (buf, PAGE_SIZE, "%s\n", dp->full_name);
}

static DEVICE_ATTR(obppath, S_IRUSR | S_IRGRP | S_IROTH, show_pciobppath_attr, NULL);

static void __devinit pci_bus_register_of_sysfs(struct pci_bus *bus)
{
	struct pci_dev *dev;
	struct pci_bus *child_bus;
	int err;

	list_for_each_entry(dev, &bus->devices, bus_list) {
		/*                                                   
                                                      
                                                    
                                                        
                                                    
              
   */
		err = sysfs_create_file(&dev->dev.kobj, &dev_attr_obppath.attr);
		(void) err;
	}
	list_for_each_entry(child_bus, &bus->children, node)
		pci_bus_register_of_sysfs(child_bus);
}

struct pci_bus * __devinit pci_scan_one_pbm(struct pci_pbm_info *pbm,
					    struct device *parent)
{
	LIST_HEAD(resources);
	struct device_node *node = pbm->op->dev.of_node;
	struct pci_bus *bus;

	printk("PCI: Scanning PBM %s\n", node->full_name);

	pci_add_resource_offset(&resources, &pbm->io_space,
				pbm->io_space.start);
	pci_add_resource_offset(&resources, &pbm->mem_space,
				pbm->mem_space.start);
	bus = pci_create_root_bus(parent, pbm->pci_first_busno, pbm->pci_ops,
				  pbm, &resources);
	if (!bus) {
		printk(KERN_ERR "Failed to create bus for %s\n",
		       node->full_name);
		pci_free_resource_list(&resources);
		return NULL;
	}
	bus->secondary = pbm->pci_first_busno;
	bus->subordinate = pbm->pci_last_busno;

	pci_of_scan_bus(pbm, node, bus);
	pci_bus_add_devices(bus);
	pci_bus_register_of_sysfs(bus);

	return bus;
}

void __devinit pcibios_fixup_bus(struct pci_bus *pbus)
{
}

void pcibios_update_irq(struct pci_dev *pdev, int irq)
{
}

resource_size_t pcibios_align_resource(void *data, const struct resource *res,
				resource_size_t size, resource_size_t align)
{
	return res->start;
}

int pcibios_enable_device(struct pci_dev *dev, int mask)
{
	u16 cmd, oldcmd;
	int i;

	pci_read_config_word(dev, PCI_COMMAND, &cmd);
	oldcmd = cmd;

	for (i = 0; i < PCI_NUM_RESOURCES; i++) {
		struct resource *res = &dev->resource[i];

		/*                                 */
		if (!(mask & (1<<i)))
			continue;

		if (res->flags & IORESOURCE_IO)
			cmd |= PCI_COMMAND_IO;
		if (res->flags & IORESOURCE_MEM)
			cmd |= PCI_COMMAND_MEMORY;
	}

	if (cmd != oldcmd) {
		printk(KERN_DEBUG "PCI: Enabling device: (%s), cmd %x\n",
		       pci_name(dev), cmd);
                /*                                                           */
		pci_write_config_word(dev, PCI_COMMAND, cmd);
	}
	return 0;
}

char * __devinit pcibios_setup(char *str)
{
	return str;
}

/*                                                 */

/*                                                             
                                                              
                   
  
                                                                       
                                                    
 */
static int __pci_mmap_make_offset_bus(struct pci_dev *pdev, struct vm_area_struct *vma,
				      enum pci_mmap_state mmap_state)
{
	struct pci_pbm_info *pbm = pdev->dev.archdata.host_controller;
	unsigned long space_size, user_offset, user_size;

	if (mmap_state == pci_mmap_io) {
		space_size = resource_size(&pbm->io_space);
	} else {
		space_size = resource_size(&pbm->mem_space);
	}

	/*                                    */
	user_offset = vma->vm_pgoff << PAGE_SHIFT;
	user_size = vma->vm_end - vma->vm_start;

	if (user_offset >= space_size ||
	    (user_offset + user_size) > space_size)
		return -EINVAL;

	if (mmap_state == pci_mmap_io) {
		vma->vm_pgoff = (pbm->io_space.start +
				 user_offset) >> PAGE_SHIFT;
	} else {
		vma->vm_pgoff = (pbm->mem_space.start +
				 user_offset) >> PAGE_SHIFT;
	}

	return 0;
}

/*                                                                
                                                                            
  
                                                                            
                                                                            
                                                                           
                                                                 
  
                                                           
 */
static int __pci_mmap_make_offset(struct pci_dev *pdev,
				  struct vm_area_struct *vma,
				  enum pci_mmap_state mmap_state)
{
	unsigned long user_paddr, user_size;
	int i, err;

	/*                                                     
                                                      
                          
  */
	err = __pci_mmap_make_offset_bus(pdev, vma, mmap_state);
	if (err)
		return err;

	/*                                                   
          
  */
	if ((pdev->class >> 8) == PCI_CLASS_BRIDGE_HOST)
		return err;

	/*                                                     
                       
  */
	user_paddr = vma->vm_pgoff << PAGE_SHIFT;
	user_size = vma->vm_end - vma->vm_start;

	for (i = 0; i <= PCI_ROM_RESOURCE; i++) {
		struct resource *rp = &pdev->resource[i];
		resource_size_t aligned_end;

		/*         */
		if (!rp->flags)
			continue;

		/*            */
		if (i == PCI_ROM_RESOURCE) {
			if (mmap_state != pci_mmap_mem)
				continue;
		} else {
			if ((mmap_state == pci_mmap_io &&
			     (rp->flags & IORESOURCE_IO) == 0) ||
			    (mmap_state == pci_mmap_mem &&
			     (rp->flags & IORESOURCE_MEM) == 0))
				continue;
		}

		/*                                                 
                                                              
                                                          
                   
   */
		aligned_end = (rp->end + PAGE_SIZE) & PAGE_MASK;

		if ((rp->start <= user_paddr) &&
		    (user_paddr + user_size) <= aligned_end)
			break;
	}

	if (i > PCI_ROM_RESOURCE)
		return -EINVAL;

	return 0;
}

/*                                                                            
           
 */
static void __pci_mmap_set_flags(struct pci_dev *dev, struct vm_area_struct *vma,
					    enum pci_mmap_state mmap_state)
{
	vma->vm_flags |= (VM_IO | VM_RESERVED);
}

/*                                                                         
                  
 */
static void __pci_mmap_set_pgprot(struct pci_dev *dev, struct vm_area_struct *vma,
					     enum pci_mmap_state mmap_state)
{
	/*                                                         */
}

/*                                                                               
                                                                                    
                                                                             
                                                                              
                                              
  
                                                             
 */
int pci_mmap_page_range(struct pci_dev *dev, struct vm_area_struct *vma,
			enum pci_mmap_state mmap_state,
			int write_combine)
{
	int ret;

	ret = __pci_mmap_make_offset(dev, vma, mmap_state);
	if (ret < 0)
		return ret;

	__pci_mmap_set_flags(dev, vma, mmap_state);
	__pci_mmap_set_pgprot(dev, vma, mmap_state);

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	ret = io_remap_pfn_range(vma, vma->vm_start,
				 vma->vm_pgoff,
				 vma->vm_end - vma->vm_start,
				 vma->vm_page_prot);
	if (ret)
		return ret;

	return 0;
}

#ifdef CONFIG_NUMA
int pcibus_to_node(struct pci_bus *pbus)
{
	struct pci_pbm_info *pbm = pbus->sysdata;

	return pbm->numa_node;
}
EXPORT_SYMBOL(pcibus_to_node);
#endif

/*                                           */

int pci_domain_nr(struct pci_bus *pbus)
{
	struct pci_pbm_info *pbm = pbus->sysdata;
	int ret;

	if (!pbm) {
		ret = -ENXIO;
	} else {
		ret = pbm->index;
	}

	return ret;
}
EXPORT_SYMBOL(pci_domain_nr);

#ifdef CONFIG_PCI_MSI
int arch_setup_msi_irq(struct pci_dev *pdev, struct msi_desc *desc)
{
	struct pci_pbm_info *pbm = pdev->dev.archdata.host_controller;
	unsigned int irq;

	if (!pbm->setup_msi_irq)
		return -EINVAL;

	return pbm->setup_msi_irq(&irq, pdev, desc);
}

void arch_teardown_msi_irq(unsigned int irq)
{
	struct msi_desc *entry = irq_get_msi_desc(irq);
	struct pci_dev *pdev = entry->dev;
	struct pci_pbm_info *pbm = pdev->dev.archdata.host_controller;

	if (pbm->teardown_msi_irq)
		pbm->teardown_msi_irq(irq, pdev);
}
#endif /*                   */

static void ali_sound_dma_hack(struct pci_dev *pdev, int set_bit)
{
	struct pci_dev *ali_isa_bridge;
	u8 val;

	/*                                                            
                                         
  */
	ali_isa_bridge = pci_get_device(PCI_VENDOR_ID_AL,
					 PCI_DEVICE_ID_AL_M1533,
					 NULL);

	pci_read_config_byte(ali_isa_bridge, 0x7e, &val);
	if (set_bit)
		val |= 0x01;
	else
		val &= ~0x01;
	pci_write_config_byte(ali_isa_bridge, 0x7e, val);
	pci_dev_put(ali_isa_bridge);
}

int pci64_dma_supported(struct pci_dev *pdev, u64 device_mask)
{
	u64 dma_addr_mask;

	if (pdev == NULL) {
		dma_addr_mask = 0xffffffff;
	} else {
		struct iommu *iommu = pdev->dev.archdata.iommu;

		dma_addr_mask = iommu->dma_addr_mask;

		if (pdev->vendor == PCI_VENDOR_ID_AL &&
		    pdev->device == PCI_DEVICE_ID_AL_M5451 &&
		    device_mask == 0x7fffffff) {
			ali_sound_dma_hack(pdev,
					   (dma_addr_mask & 0x80000000) != 0);
			return 1;
		}
	}

	if (device_mask >= (1UL << 32UL))
		return 0;

	return (device_mask & dma_addr_mask) == dma_addr_mask;
}

void pci_resource_to_user(const struct pci_dev *pdev, int bar,
			  const struct resource *rp, resource_size_t *start,
			  resource_size_t *end)
{
	struct pci_pbm_info *pbm = pdev->dev.archdata.host_controller;
	unsigned long offset;

	if (rp->flags & IORESOURCE_IO)
		offset = pbm->io_space.start;
	else
		offset = pbm->mem_space.start;

	*start = rp->start - offset;
	*end = rp->end - offset;
}

void pcibios_set_master(struct pci_dev *dev)
{
	/*                                         */
}

static int __init pcibios_init(void)
{
	pci_dfl_cache_line_size = 64 >> 2;
	return 0;
}
subsys_initcall(pcibios_init);

#ifdef CONFIG_SYSFS
static void __devinit pci_bus_slot_names(struct device_node *node,
					 struct pci_bus *bus)
{
	const struct pci_slot_names {
		u32	slot_mask;
		char	names[0];
	} *prop;
	const char *sp;
	int len, i;
	u32 mask;

	prop = of_get_property(node, "slot-names", &len);
	if (!prop)
		return;

	mask = prop->slot_mask;
	sp = prop->names;

	if (ofpci_verbose)
		printk("PCI: Making slots for [%s] mask[0x%02x]\n",
		       node->full_name, mask);

	i = 0;
	while (mask) {
		struct pci_slot *pci_slot;
		u32 this_bit = 1 << i;

		if (!(mask & this_bit)) {
			i++;
			continue;
		}

		if (ofpci_verbose)
			printk("PCI: Making slot [%s]\n", sp);

		pci_slot = pci_create_slot(bus, i, sp, NULL);
		if (IS_ERR(pci_slot))
			printk(KERN_ERR "PCI: pci_create_slot returned %ld\n",
			       PTR_ERR(pci_slot));

		sp += strlen(sp) + 1;
		mask &= ~this_bit;
		i++;
	}
}

static int __init of_pci_slot_init(void)
{
	struct pci_bus *pbus = NULL;

	while ((pbus = pci_find_next_bus(pbus)) != NULL) {
		struct device_node *node;

		if (pbus->self) {
			/*                 */
			node = pbus->self->dev.of_node;
		} else {
			struct pci_pbm_info *pbm = pbus->sysdata;

			/*                     */
			node = pbm->op->dev.of_node;
		}

		pci_bus_slot_names(node, pbus);
	}

	return 0;
}

module_init(of_pci_slot_init);
#endif
