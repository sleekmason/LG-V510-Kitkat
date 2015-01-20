/* Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "kgsl.h"
#include "kgsl_sharedmem.h"
#include "kgsl_snapshot.h"

#include "adreno.h"
#include "adreno_pm4types.h"
#include "a2xx_reg.h"
#include "a3xx_reg.h"
#include "adreno_cp_parser.h"

/*                                                  */
#define NUM_DWORDS_OF_RINGBUFFER_HISTORY 100

/*                                                      */

#define SNAPSHOT_OBJ_BUFSIZE 64

#define SNAPSHOT_OBJ_TYPE_IB 0

/*                                                                            */
static int snapshot_frozen_objsize;

static struct kgsl_snapshot_obj {
	int type;
	uint32_t gpuaddr;
	phys_addr_t ptbase;
	void *ptr;
	int dwords;
	struct kgsl_mem_entry *entry;
} objbuf[SNAPSHOT_OBJ_BUFSIZE];

/*                                                   */
static int objbufptr;

/*                                        */
static void push_object(struct kgsl_device *device, int type,
	phys_addr_t ptbase,
	uint32_t gpuaddr, int dwords)
{
	int index;
	void *ptr;
	struct kgsl_mem_entry *entry = NULL;

	/*
                                                                        
                                                                         
                                                                      
         
  */

	for (index = 0; index < objbufptr; index++) {
		if (objbuf[index].gpuaddr == gpuaddr &&
			objbuf[index].ptbase == ptbase) {
				objbuf[index].dwords = dwords;
				return;
			}
	}

	if (objbufptr == SNAPSHOT_OBJ_BUFSIZE) {
		KGSL_DRV_ERR(device, "snapshot: too many snapshot objects\n");
		return;
	}

	/*
                                                                       
                                                                   
  */
	ptr = adreno_convertaddr(device, ptbase, gpuaddr, dwords << 2, &entry);

	if (ptr == NULL) {
		KGSL_DRV_ERR(device,
			"snapshot: Can't find GPU address for %x\n", gpuaddr);
		return;
	}

	/*                                       */
	objbuf[objbufptr].type = type;
	objbuf[objbufptr].gpuaddr = gpuaddr;
	objbuf[objbufptr].ptbase = ptbase;
	objbuf[objbufptr].dwords = dwords;
	objbuf[objbufptr].entry = entry;
	objbuf[objbufptr++].ptr = ptr;
}

/*
                                                                       
               
 */

static int find_object(int type, unsigned int gpuaddr, phys_addr_t ptbase)
{
	int index;

	for (index = 0; index < objbufptr; index++) {
		if (objbuf[index].gpuaddr == gpuaddr &&
			objbuf[index].ptbase == ptbase &&
			objbuf[index].type == type)
			return 1;
	}

	return 0;
}

/*
                                                                          
                      
                                    
                                                                 
                                       
  
                                       
 */
static int snapshot_freeze_obj_list(struct kgsl_device *device,
		phys_addr_t ptbase, struct adreno_ib_object_list *ib_obj_list)
{
	int ret = 0;
	struct adreno_ib_object *ib_objs;
	unsigned int ib2base;
	struct adreno_device *adreno_dev = ADRENO_DEVICE(device);
	int i;

	adreno_readreg(adreno_dev, ADRENO_REG_CP_IB2_BASE, &ib2base);

	for (i = 0; i < ib_obj_list->num_objs; i++) {
		int temp_ret;
		int index;
		int freeze = 1;

		ib_objs = &(ib_obj_list->obj_list[i]);
		/*                                                           */
		for (index = 0; index < objbufptr; index++) {
			if ((objbuf[index].gpuaddr <= ib_objs->gpuaddr) &&
				((objbuf[index].gpuaddr +
				(objbuf[index].dwords << 2)) >=
				(ib_objs->gpuaddr + ib_objs->size)) &&
				(objbuf[index].ptbase == ptbase)) {
				freeze = 0;
				break;
			}
		}

		if (freeze) {
			/*                             */
			if (ib2base == ib_objs->gpuaddr) {
				push_object(device, SNAPSHOT_OBJ_TYPE_IB,
				ptbase, ib_objs->gpuaddr, ib_objs->size >> 2);
			} else {
				temp_ret = kgsl_snapshot_get_object(device,
					ptbase, ib_objs->gpuaddr, ib_objs->size,
					ib_objs->snapshot_obj_type);
				if (temp_ret < 0) {
					if (ret >= 0)
						ret = temp_ret;
				} else {
					snapshot_frozen_objsize += temp_ret;
				}
			}
		}
	}
	return ret;
}

/*
                                                                                
                                                                             
                                                                          
               
 */
static inline int parse_ib(struct kgsl_device *device, phys_addr_t ptbase,
		unsigned int gpuaddr, unsigned int dwords)
{
	struct adreno_device *adreno_dev = ADRENO_DEVICE(device);
	unsigned int ib1base;
	int ret = 0;
	struct adreno_ib_object_list *ib_obj_list;

	/*
                                                                
                                                                     
        
  */

	adreno_readreg(adreno_dev, ADRENO_REG_CP_IB1_BASE, &ib1base);

	if (gpuaddr == ib1base) {
		push_object(device, SNAPSHOT_OBJ_TYPE_IB, ptbase,
			gpuaddr, dwords);
		goto done;
	}

	if (kgsl_snapshot_have_object(device, ptbase, gpuaddr, dwords << 2))
		goto done;

	ret = adreno_ib_create_object_list(device, ptbase,
				gpuaddr, dwords, &ib_obj_list);
	if (ret)
		goto done;

	ret = kgsl_snapshot_add_ib_obj_list(device, ptbase, ib_obj_list);

	if (ret)
		adreno_ib_destroy_obj_list(ib_obj_list);
done:
	return ret;
}

/*                                */
static int snapshot_rb(struct kgsl_device *device, void *snapshot,
	int remain, void *priv)
{
	struct kgsl_snapshot_rb *header = snapshot;
	unsigned int *data = snapshot + sizeof(*header);
	struct adreno_device *adreno_dev = ADRENO_DEVICE(device);
	struct adreno_ringbuffer *rb = &adreno_dev->ringbuffer;
	unsigned int rptr, *rbptr, ibbase;
	phys_addr_t ptbase;
	int index, size, i;
	int parse_ibs = 0, ib_parse_start;

	/*                                               */
	ptbase = kgsl_mmu_get_current_ptbase(&device->mmu);

	/*                                          */
	adreno_readreg(adreno_dev, ADRENO_REG_CP_RB_RPTR, &rptr);

	/*                                  */
	adreno_readreg(adreno_dev, ADRENO_REG_CP_IB1_BASE, &ibbase);

	/*
                                                                       
                                                                      
                 
  */

	index = rptr;
	rbptr = rb->buffer_desc.hostptr;

	do {
		index--;

		if (index < 0) {
			index = rb->sizedwords - 3;

			/*                                           */
			if (index < rb->wptr) {
				index = rb->wptr;
				break;
			}
		}

		if (adreno_cmd_is_ib(rbptr[index]) &&
			rbptr[index + 1] == ibbase)
			break;
	} while (index != rb->wptr);

	/*
                                                                     
                                                                     
                                                                     
  */

	while (index != rb->wptr) {
		index--;

		if (index < 0) {
			index = rb->sizedwords - 2;

			/*
                                                         
                                                           
                 
    */

			if (index < rb->wptr) {
				index = rb->wptr;
				break;
			}
		}

		/*                                                            */
		if ((rbptr[index] == cp_nop_packet(1)) &&
			(rbptr[index + 1] == KGSL_CONTEXT_TO_MEM_IDENTIFIER))
			break;
	}

	/*
                                                                      
                                                 
  */

	ib_parse_start = index;

	/*
                                                                        
           
  */

	size = (rb->sizedwords << 2);

	if (remain < size + sizeof(*header)) {
		KGSL_DRV_ERR(device,
			"snapshot: Not enough memory for the rb section");
		return 0;
	}

	/*                                      */
	header->start = rb->wptr;
	header->end = rb->wptr;
	header->wptr = rb->wptr;
	header->rbsize = rb->sizedwords;
	header->count = rb->sizedwords;

	/*
                                                                  
                                     
  */

	index = rb->wptr;
	for (i = 0; i < rb->sizedwords; i++) {
		*data = rbptr[index];

		/*
                                                              
                                          
   */

		if (parse_ibs == 0 && index == ib_parse_start)
			parse_ibs = 1;
		else if (index == rptr || adreno_rb_ctxtswitch(&rbptr[index]))
			parse_ibs = 0;

		if (parse_ibs && adreno_cmd_is_ib(rbptr[index])) {
			unsigned int ibaddr = rbptr[index + 1];
			unsigned int ibsize = rbptr[index + 2];

			/*
                                                       
                                                      
                      
    */

			struct kgsl_memdesc *memdesc =
				adreno_find_ctxtmem(device, ptbase, ibaddr,
					ibsize << 2);

			/*                                              */
			if (NULL == memdesc)
				if (kgsl_gpuaddr_in_memdesc(
						&device->mmu.setstate_memory,
						ibaddr, ibsize << 2))
					memdesc = &device->mmu.setstate_memory;
			/*
                                                    
                                             
                                      
    */

			if (memdesc != NULL)
				push_object(device, SNAPSHOT_OBJ_TYPE_IB,
					ptbase, ibaddr, ibsize);
			else
				parse_ib(device, ptbase, ibaddr, ibsize);
		}

		index = index + 1;

		if (index == rb->sizedwords)
			index = 0;

		data++;
	}

	/*                                */
	return size + sizeof(*header);
}

static int snapshot_capture_mem_list(struct kgsl_device *device, void *snapshot,
			int remain, void *priv)
{
	struct kgsl_snapshot_replay_mem_list *header = snapshot;
	struct kgsl_process_private *private = NULL;
	struct kgsl_process_private *tmp_private;
	phys_addr_t ptbase;
	struct rb_node *node;
	struct kgsl_mem_entry *entry = NULL;
	int num_mem;
	unsigned int *data = snapshot + sizeof(*header);

	ptbase = kgsl_mmu_get_current_ptbase(&device->mmu);
	mutex_lock(&kgsl_driver.process_mutex);
	list_for_each_entry(tmp_private, &kgsl_driver.process_list, list) {
		if (kgsl_mmu_pt_equal(&device->mmu, tmp_private->pagetable,
			ptbase)) {
			private = tmp_private;
			break;
		}
	}
	mutex_unlock(&kgsl_driver.process_mutex);
	if (!private) {
		KGSL_DRV_ERR(device,
		"Failed to get pointer to process private structure\n");
		return 0;
	}
	/*                                                                   */
	spin_lock(&private->mem_lock);
	for (node = rb_first(&private->mem_rb), num_mem = 0; node; ) {
		entry = rb_entry(node, struct kgsl_mem_entry, node);
		node = rb_next(&entry->node);
		num_mem++;
	}

	if (remain < ((num_mem * 3 * sizeof(unsigned int)) +
			sizeof(*header))) {
		KGSL_DRV_ERR(device,
			"snapshot: Not enough memory for the mem list section");
		spin_unlock(&private->mem_lock);
		return 0;
	}
	header->num_entries = num_mem;
	header->ptbase = (__u32)ptbase;
	/*
                                               
                                              
  */
	for (node = rb_first(&private->mem_rb); node; ) {
		entry = rb_entry(node, struct kgsl_mem_entry, node);
		node = rb_next(&entry->node);

		*data++ = entry->memdesc.gpuaddr;
		*data++ = entry->memdesc.size;
		*data++ = (entry->memdesc.priv & KGSL_MEMTYPE_MASK) >>
							KGSL_MEMTYPE_SHIFT;
	}
	spin_unlock(&private->mem_lock);
	return sizeof(*header) + (num_mem * 3 * sizeof(unsigned int));
}

/*                                            */
static int snapshot_ib(struct kgsl_device *device, void *snapshot,
	int remain, void *priv)
{
	struct kgsl_snapshot_ib *header = snapshot;
	struct kgsl_snapshot_obj *obj = priv;
	struct adreno_device *adreno_dev = ADRENO_DEVICE(device);
	unsigned int *src = obj->ptr;
	unsigned int *dst = snapshot + sizeof(*header);
	struct adreno_ib_object_list *ib_obj_list;
	unsigned int ib1base;

	adreno_readreg(adreno_dev, ADRENO_REG_CP_IB1_BASE, &ib1base);

	if (remain < (obj->dwords << 2) + sizeof(*header)) {
		KGSL_DRV_ERR(device,
			"snapshot: Not enough memory for the ib section");
		return 0;
	}

	/*                                                                */
	if (ib1base == obj->gpuaddr) {
		if (!adreno_ib_create_object_list(device, obj->ptbase,
					obj->gpuaddr, obj->dwords,
					&ib_obj_list)) {
			/*                                 */
			snapshot_freeze_obj_list(device, obj->ptbase,
						ib_obj_list);
			adreno_ib_destroy_obj_list(ib_obj_list);
		}
	}

	/*                                      */
	header->gpuaddr = obj->gpuaddr;
	header->ptbase = (__u32)obj->ptbase;
	header->size = obj->dwords;

	/*                              */
	memcpy((void *)dst, (void *)src, obj->dwords << 2);
	/*                              */

	return (obj->dwords << 2) + sizeof(*header);
}

/*                                               */
static void *dump_object(struct kgsl_device *device, int obj, void *snapshot,
	int *remain)
{
	switch (objbuf[obj].type) {
	case SNAPSHOT_OBJ_TYPE_IB:
		snapshot = kgsl_snapshot_add_section(device,
			KGSL_SNAPSHOT_SECTION_IB, snapshot, remain,
			snapshot_ib, &objbuf[obj]);
		if (objbuf[obj].entry) {
			kgsl_memdesc_unmap(&(objbuf[obj].entry->memdesc));
			kgsl_mem_entry_put(objbuf[obj].entry);
		}
		break;
	default:
		KGSL_DRV_ERR(device,
			"snapshot: Invalid snapshot object type: %d\n",
			objbuf[obj].type);
		break;
	}

	return snapshot;
}

/*                                                
                                    
                                                           
                                                                                
                                                                         
                                                                  
                                                                            
                                                                             
 */

void *adreno_snapshot(struct kgsl_device *device, void *snapshot, int *remain,
		int hang)
{
	int i;
	uint32_t ibbase, ibsize;
	struct adreno_device *adreno_dev = ADRENO_DEVICE(device);
	phys_addr_t ptbase;

	/*                           */
	objbufptr = 0;

	snapshot_frozen_objsize = 0;

	/*                                               */
	ptbase = kgsl_mmu_get_current_ptbase(&device->mmu);

	/*                     */
	snapshot = kgsl_snapshot_add_section(device, KGSL_SNAPSHOT_SECTION_RB,
		snapshot, remain, snapshot_rb, NULL);

	/*
                                                                   
                   
  */
	snapshot = kgsl_snapshot_add_section(device,
			KGSL_SNAPSHOT_SECTION_MEMLIST, snapshot, remain,
			snapshot_capture_mem_list, NULL);
	/*
                                                                  
                                                                  
                                                                   
                                
  */

	adreno_readreg(adreno_dev, ADRENO_REG_CP_IB1_BASE, &ibbase);
	adreno_readreg(adreno_dev, ADRENO_REG_CP_IB1_BUFSZ, &ibsize);

	/*
                                                                         
                                                                   
                                                                      
                                                                 
                                         
  */

	if (!find_object(SNAPSHOT_OBJ_TYPE_IB, ibbase, ptbase) && ibsize) {
		push_object(device, SNAPSHOT_OBJ_TYPE_IB, ptbase,
			ibbase, ibsize);
		KGSL_DRV_ERR(device, "CP_IB1_BASE not found in the ringbuffer. "
			"Dumping %x dwords of the buffer.\n", ibsize);
	}

	adreno_readreg(adreno_dev, ADRENO_REG_CP_IB2_BASE, &ibbase);
	adreno_readreg(adreno_dev, ADRENO_REG_CP_IB2_BUFSZ, &ibsize);

	/*
                                                                      
                                                                       
                                                                         
                                                                      
                 
  */

	if (!find_object(SNAPSHOT_OBJ_TYPE_IB, ibbase, ptbase) && ibsize) {
		push_object(device, SNAPSHOT_OBJ_TYPE_IB, ptbase,
			ibbase, ibsize);
	}

	/*
                                                                       
                                                                        
  */
	for (i = 0; i < objbufptr; i++)
		snapshot = dump_object(device, i, snapshot, remain);

	/*                                                                   */
	if (adreno_dev->gpudev->snapshot)
		snapshot = adreno_dev->gpudev->snapshot(adreno_dev, snapshot,
			remain, hang);

	if (snapshot_frozen_objsize)
		KGSL_DRV_ERR(device, "GPU snapshot froze %dKb of GPU buffers\n",
			snapshot_frozen_objsize / 1024);

	/*
                                                                 
                                                               
          
  */
	queue_work(device->work_queue, &device->snapshot_obj_ws);

	return snapshot;
}
