/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
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

#ifndef __ADRENO_IB_PARSER__
#define __ADRENO_IB_PARSER__

/*
                                                                      
                               
                                                 
                           
                                                      
                                                        
 */
struct adreno_ib_object {
	unsigned int gpuaddr;
	unsigned int size;
	int snapshot_obj_type;
	struct kgsl_mem_entry *entry;
};

/*
                                                                    
                                    
                                       
 */
struct adreno_ib_object_list {
	struct adreno_ib_object *obj_list;
	int num_objs;
};

/*
                                                                            
                           
                                                             
                                                            
                                         
  
                                                                       
 */
static inline void adreno_ib_init_ib_obj(unsigned int gpuaddr,
			unsigned int size, int obj_type,
			struct kgsl_mem_entry *entry,
			struct adreno_ib_object *ib_obj)
{
	ib_obj->gpuaddr = gpuaddr;
	ib_obj->size = size;
	ib_obj->snapshot_obj_type = obj_type;
	ib_obj->entry = entry;
}

int adreno_ib_create_object_list(
		struct kgsl_device *device, phys_addr_t ptbase,
		unsigned int gpuaddr, unsigned int dwords,
		struct adreno_ib_object_list **out_ib_obj_list);

void adreno_ib_destroy_obj_list(struct adreno_ib_object_list *ib_obj_list);

#endif
