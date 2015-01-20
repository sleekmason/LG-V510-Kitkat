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

#include "kgsl.h"
#include "kgsl_sharedmem.h"
#include "kgsl_snapshot.h"

#include "adreno.h"
#include "adreno_pm4types.h"
#include "a2xx_reg.h"
#include "a3xx_reg.h"
#include "adreno_cp_parser.h"

#define MAX_IB_OBJS 1000

/*
                                                                            
                                                                         
                                                         
 */

struct ib_vsc_pipe {
	unsigned int base;
	unsigned int size;
};

/*
                                                                     
                                                                              
                                                                          
           
 */

struct ib_vbo {
	unsigned int base;
	unsigned int stride;
};

/*                                           */
struct ib_parser_variables {
	struct ib_vsc_pipe vsc_pipe[8];
	/*
                                                                    
                                                                  
                                
  */
	unsigned int vsc_size_address;
	struct ib_vbo vbo[16];
	/*                                                            */
	unsigned int vfd_index_max;
	/*
                                                                   
                                                                      
  */
	unsigned int vfd_control_0;
	/*                                                       
                                                              
                                    
  */
	unsigned int sp_vs_pvt_mem_addr;
	unsigned int sp_fs_pvt_mem_addr;
	/*                                                              */
	unsigned int sp_vs_obj_start_reg;
	unsigned int sp_fs_obj_start_reg;
};

/*
                                                                             
                                                                                
                                                   
 */
static int load_state_unit_sizes[7][2] = {
	{ 2, 4 },
	{ 0, 1 },
	{ 2, 4 },
	{ 0, 1 },
	{ 8, 2 },
	{ 8, 2 },
	{ 8, 2 },
};

/*
                                                                         
         
                         
                                                    
                                     
 */
static void adreno_ib_merge_range(struct adreno_ib_object *ib_obj,
		unsigned int gpuaddr, unsigned int size)
{
	unsigned int addr_end1 = ib_obj->gpuaddr + ib_obj->size;
	unsigned int addr_end2 = gpuaddr + size;
	if (gpuaddr < ib_obj->gpuaddr)
		ib_obj->gpuaddr = gpuaddr;
	if (addr_end2 > addr_end1)
		ib_obj->size = addr_end2 - ib_obj->gpuaddr;
	else
		ib_obj->size = addr_end1 - ib_obj->gpuaddr;
}

/*
                                                                 
                                                         
                                   
                                                                
  
                                                                    
                                                       
 */
static struct adreno_ib_object *adreno_ib_check_overlap(unsigned int gpuaddr,
		unsigned int size, struct adreno_ib_object_list *ib_obj_list)
{
	struct adreno_ib_object *ib_obj;
	int i;

	for (i = 0; i < ib_obj_list->num_objs; i++) {
		ib_obj = &(ib_obj_list->obj_list[i]);
		if (kgsl_addr_range_overlap(ib_obj->gpuaddr, ib_obj->size,
			gpuaddr, size))
			/*                 */
			return ib_obj;
	}
	return NULL;
}

/*
                                                         
                                                         
                                                            
                                              
                                   
                                                                             
        
  
                                                                              
                                                                           
                                           
  
                                       
 */
static int adreno_ib_add_range(struct kgsl_device *device,
				phys_addr_t ptbase,
				unsigned int gpuaddr,
				unsigned int size, int type,
				struct adreno_ib_object_list *ib_obj_list)
{
	struct adreno_ib_object *ib_obj;
	struct kgsl_mem_entry *entry;

	entry = kgsl_get_mem_entry(device, ptbase, gpuaddr, size);
	if (!entry)
		/*
                                                      
                                                        
              
   */
		return 0;

	if (!size) {
		size = entry->memdesc.size;
		gpuaddr = entry->memdesc.gpuaddr;
	}

	ib_obj = adreno_ib_check_overlap(gpuaddr, size, ib_obj_list);
	if (ib_obj) {
		adreno_ib_merge_range(ib_obj, gpuaddr, size);
	} else {
		if (MAX_IB_OBJS == ib_obj_list->num_objs) {
			KGSL_DRV_ERR(device,
			"Max objects reached %d\n", ib_obj_list->num_objs);
			return -ENOMEM;
		}
		adreno_ib_init_ib_obj(gpuaddr, size, type, entry,
			&(ib_obj_list->obj_list[ib_obj_list->num_objs]));
		ib_obj_list->num_objs++;
	}
	return 0;
}

/*
                                               
                                             
                                    
                                               
                                                          
  
                                       
 */
static int ib_save_mip_addresses(struct kgsl_device *device, unsigned int *pkt,
		phys_addr_t ptbase, struct adreno_ib_object_list *ib_obj_list)
{
	int ret = 0;
	int num_levels = (pkt[1] >> 22) & 0x03FF;
	int i;
	unsigned int *hostptr;
	struct kgsl_mem_entry *ent;
	unsigned int block, type;
	int unitsize = 0;

	block = (pkt[1] >> 19) & 0x07;
	type = pkt[2] & 0x03;

	if (type == 0)
		unitsize = load_state_unit_sizes[block][0];
	else
		unitsize = load_state_unit_sizes[block][1];

	if (3 == block && 1 == type) {
		ent = kgsl_get_mem_entry(device, ptbase, pkt[2] & 0xFFFFFFFC,
					(num_levels * unitsize) << 2);
		if (!ent)
			return -EINVAL;

		hostptr = (unsigned int *)kgsl_gpuaddr_to_vaddr(&ent->memdesc,
				pkt[2] & 0xFFFFFFFC);
		if (!hostptr) {
			kgsl_mem_entry_put(ent);
			return -EINVAL;
		}
		for (i = 0; i < num_levels; i++) {
			ret = adreno_ib_add_range(device, ptbase, hostptr[i],
				0, SNAPSHOT_GPU_OBJECT_GENERIC, ib_obj_list);
			if (ret < 0)
				break;
		}
		kgsl_memdesc_unmap(&ent->memdesc);
		kgsl_mem_entry_put(ent);
	}
	return ret;
}

/*
                                                  
                                             
                                    
                                               
                                                          
                                                                
  
                                                                            
         
                                       
 */
static int ib_parse_load_state(struct kgsl_device *device, unsigned int *pkt,
	phys_addr_t ptbase, struct adreno_ib_object_list *ib_obj_list,
	struct ib_parser_variables *ib_parse_vars)
{
	unsigned int block, source, type;
	int ret = 0;
	int unitsize = 0;

	/*
                                                                         
                                                                         
                                                                    
                                                                    
                                                                         
                                                   
  */

	if (type3_pkt_size(pkt[0]) < 2)
		return 0;

	/*
                         
                              
                                
                     
                                    
  */

	block = (pkt[1] >> 19) & 0x07;
	source = (pkt[1] >> 16) & 0x07;
	type = pkt[2] & 0x03;

	if (source == 4) {
		if (type == 0)
			unitsize = load_state_unit_sizes[block][0];
		else
			unitsize = load_state_unit_sizes[block][1];

		/*                                             */

		ret = adreno_ib_add_range(device, ptbase, pkt[2] & 0xFFFFFFFC,
				(((pkt[1] >> 22) & 0x03FF) * unitsize) << 2,
				SNAPSHOT_GPU_OBJECT_SHADER,
				ib_obj_list);
		if (ret < 0)
			return ret;
	}
	/*                       */
	ret = ib_save_mip_addresses(device, pkt, ptbase, ib_obj_list);
	return ret;
}

/*
                                                                              
                                
 */

static int ib_parse_set_bin_data(struct kgsl_device *device, unsigned int *pkt,
	phys_addr_t ptbase, struct adreno_ib_object_list *ib_obj_list,
	struct ib_parser_variables *ib_parse_vars)
{
	int ret = 0;

	if (type3_pkt_size(pkt[0]) < 2)
		return 0;

	/*                         */
	ret = adreno_ib_add_range(device, ptbase, pkt[1], 0,
		SNAPSHOT_GPU_OBJECT_GENERIC, ib_obj_list);
	if (ret < 0)
		return ret;

	/*                                                    */
	ret = adreno_ib_add_range(device, ptbase, pkt[2], 32,
		SNAPSHOT_GPU_OBJECT_GENERIC, ib_obj_list);

	return ret;
}

/*
                                                                             
                                                                                
                                        
 */

static int ib_parse_mem_write(struct kgsl_device *device, unsigned int *pkt,
	phys_addr_t ptbase, struct adreno_ib_object_list *ib_obj_list,
	struct ib_parser_variables *ib_parse_vars)
{
	int ret = 0;

	if (type3_pkt_size(pkt[0]) < 1)
		return 0;

	/*
                                                                       
                                                                        
                                                                         
  */

	ret = adreno_ib_add_range(device, ptbase, pkt[1] & 0xFFFFFFFC, 0,
		SNAPSHOT_GPU_OBJECT_GENERIC, ib_obj_list);
	if (ret < 0)
		return ret;

	return ret;
}

/*
                                                      
                                                   
                                            
                                        
                                                          
  
                                                                   
                              
 */
static int ib_add_type0_entries(struct kgsl_device *device,
	phys_addr_t ptbase, struct adreno_ib_object_list *ib_obj_list,
	struct ib_parser_variables *ib_parse_vars)
{
	int ret = 0;
	int i;
	/*                                      */

	for (i = 0; i < ARRAY_SIZE(ib_parse_vars->vsc_pipe); i++) {
		if (ib_parse_vars->vsc_pipe[i].base != 0 &&
			ib_parse_vars->vsc_pipe[i].size != 0) {
			ret = adreno_ib_add_range(device, ptbase,
				ib_parse_vars->vsc_pipe[i].base,
				ib_parse_vars->vsc_pipe[i].size,
				SNAPSHOT_GPU_OBJECT_GENERIC,
				ib_obj_list);
			if (ret < 0)
				return ret;
			ib_parse_vars->vsc_pipe[i].size = 0;
			ib_parse_vars->vsc_pipe[i].base = 0;
		}
	}

	/*                                        */

	if (ib_parse_vars->vsc_size_address) {
		ret = adreno_ib_add_range(device, ptbase,
			ib_parse_vars->vsc_size_address, 32,
			SNAPSHOT_GPU_OBJECT_GENERIC, ib_obj_list);
		if (ret < 0)
			return ret;
		ib_parse_vars->vsc_size_address = 0;
	}

	/*                                   */
	if (ib_parse_vars->sp_vs_pvt_mem_addr) {
		ret = adreno_ib_add_range(device, ptbase,
			ib_parse_vars->sp_vs_pvt_mem_addr, 8192,
			SNAPSHOT_GPU_OBJECT_GENERIC, ib_obj_list);
		if (ret < 0)
			return ret;

		ib_parse_vars->sp_vs_pvt_mem_addr = 0;
	}

	if (ib_parse_vars->sp_fs_pvt_mem_addr) {
		ret = adreno_ib_add_range(device, ptbase,
				ib_parse_vars->sp_fs_pvt_mem_addr, 8192,
				SNAPSHOT_GPU_OBJECT_GENERIC,
				ib_obj_list);
		if (ret < 0)
			return ret;

		ib_parse_vars->sp_fs_pvt_mem_addr = 0;
	}

	if (ib_parse_vars->sp_vs_obj_start_reg) {
		ret = adreno_ib_add_range(device, ptbase,
			ib_parse_vars->sp_vs_obj_start_reg & 0xFFFFFFE0,
			0, SNAPSHOT_GPU_OBJECT_GENERIC, ib_obj_list);
		if (ret < 0)
			return -ret;
		ib_parse_vars->sp_vs_obj_start_reg = 0;
	}

	if (ib_parse_vars->sp_fs_obj_start_reg) {
		ret = adreno_ib_add_range(device, ptbase,
			ib_parse_vars->sp_fs_obj_start_reg & 0xFFFFFFE0,
			0, SNAPSHOT_GPU_OBJECT_GENERIC, ib_obj_list);
		if (ret < 0)
			return ret;
		ib_parse_vars->sp_fs_obj_start_reg = 0;
	}

	/*               */

	/*                                                             */
	for (i = 0; i < (ib_parse_vars->vfd_control_0) >> 27; i++) {
		int size;

		/*
                                                
                                                            
                                     
   */

		if (ib_parse_vars->vbo[i].base != 0) {
			size = ib_parse_vars->vbo[i].stride *
					ib_parse_vars->vfd_index_max;

			ret = adreno_ib_add_range(device, ptbase,
				ib_parse_vars->vbo[i].base,
				0, SNAPSHOT_GPU_OBJECT_GENERIC, ib_obj_list);
			if (ret < 0)
				return ret;
		}

		ib_parse_vars->vbo[i].base = 0;
		ib_parse_vars->vbo[i].stride = 0;
	}

	ib_parse_vars->vfd_control_0 = 0;
	ib_parse_vars->vfd_index_max = 0;

	return ret;
}

/*
                                                                              
                                                                           
                                                                               
                         
 */

static int ib_parse_draw_indx(struct kgsl_device *device, unsigned int *pkt,
	phys_addr_t ptbase, struct adreno_ib_object_list *ib_obj_list,
	struct ib_parser_variables *ib_parse_vars)
{
	int ret = 0;

	if (type3_pkt_size(pkt[0]) < 3)
		return 0;

	/*                                           */

	if (type3_pkt_size(pkt[0]) > 3) {
		ret = adreno_ib_add_range(device, ptbase, pkt[4], pkt[5],
			SNAPSHOT_GPU_OBJECT_GENERIC, ib_obj_list);
		if (ret < 0)
			return ret;
	}

	/*
                                                                    
                                            
  */
	ret = ib_add_type0_entries(device, ptbase, ib_obj_list,
				ib_parse_vars);
	return ret;
}

/*
                                                                             
                                                            
 */

static int ib_parse_type3(struct kgsl_device *device, unsigned int *ptr,
	phys_addr_t ptbase, struct adreno_ib_object_list *ib_obj_list,
	struct ib_parser_variables *ib_parse_vars)
{
	int opcode = cp_type3_opcode(*ptr);

	if (opcode == CP_LOAD_STATE)
		return ib_parse_load_state(device, ptr, ptbase, ib_obj_list,
					ib_parse_vars);
	else if (opcode == CP_SET_BIN_DATA)
		return ib_parse_set_bin_data(device, ptr, ptbase, ib_obj_list,
					ib_parse_vars);
	else if (opcode == CP_MEM_WRITE)
		return ib_parse_mem_write(device, ptr, ptbase, ib_obj_list,
					ib_parse_vars);
	else if (opcode == CP_DRAW_INDX)
		return ib_parse_draw_indx(device, ptr, ptbase, ib_obj_list,
					ib_parse_vars);

	return 0;
}

/*
                                                                           
                                                                             
                                                                               
                                                                           
                                                                   
 */

static void ib_parse_type0(struct kgsl_device *device, unsigned int *ptr,
	phys_addr_t ptbase, struct adreno_ib_object_list *ib_obj_list,
	struct ib_parser_variables *ib_parse_vars)
{
	struct adreno_device *adreno_dev = ADRENO_DEVICE(device);
	int size = type0_pkt_size(*ptr);
	int offset = type0_pkt_offset(*ptr);
	int i;

	for (i = 0; i < size; i++, offset++) {

		/*                         */

		if (offset >= adreno_getreg(adreno_dev,
				ADRENO_REG_VSC_PIPE_DATA_ADDRESS_0) &&
			offset <= adreno_getreg(adreno_dev,
					ADRENO_REG_VSC_PIPE_DATA_LENGTH_7)) {
			int index = offset - adreno_getreg(adreno_dev,
					ADRENO_REG_VSC_PIPE_DATA_ADDRESS_0);

			/*                                              
                                         
     
               
              
           
               
              
           
         
    */

			if ((index % 3) == 0)
				ib_parse_vars->vsc_pipe[index / 3].base =
								ptr[i + 1];
			else if ((index % 3) == 1)
				ib_parse_vars->vsc_pipe[index / 3].size =
								ptr[i + 1];
		} else if ((offset >= adreno_getreg(adreno_dev,
					ADRENO_REG_VFD_FETCH_INSTR_0_0)) &&
			(offset <= adreno_getreg(adreno_dev,
					ADRENO_REG_VFD_FETCH_INSTR_1_F))) {
			int index = offset -
				adreno_getreg(adreno_dev,
					ADRENO_REG_VFD_FETCH_INSTR_0_0);

			/*
                                                   
                                                         
                
    */

			if ((index % 2) == 0)
				ib_parse_vars->vbo[index >> 1].stride =
					(ptr[i + 1] >> 7) & 0x1FF;
			else
				ib_parse_vars->vbo[index >> 1].base =
					ptr[i + 1];
		} else {
			/*
                                                     
                  
    */

			if (offset ==
				adreno_getreg(adreno_dev,
						ADRENO_REG_VFD_CONTROL_0))
				ib_parse_vars->vfd_control_0 = ptr[i + 1];
			else if (offset ==
				adreno_getreg(adreno_dev,
						ADRENO_REG_VFD_INDEX_MAX))
				ib_parse_vars->vfd_index_max = ptr[i + 1];
			else if (offset ==
				adreno_getreg(adreno_dev,
						ADRENO_REG_VSC_SIZE_ADDRESS))
				ib_parse_vars->vsc_size_address = ptr[i + 1];
			else if (offset == adreno_getreg(adreno_dev,
					ADRENO_REG_SP_VS_PVT_MEM_ADDR_REG))
				ib_parse_vars->sp_vs_pvt_mem_addr = ptr[i + 1];
			else if (offset == adreno_getreg(adreno_dev,
					ADRENO_REG_SP_FS_PVT_MEM_ADDR_REG))
				ib_parse_vars->sp_fs_pvt_mem_addr = ptr[i + 1];
			else if (offset == adreno_getreg(adreno_dev,
					ADRENO_REG_SP_VS_OBJ_START_REG))
				ib_parse_vars->sp_vs_obj_start_reg = ptr[i + 1];
			else if (offset == adreno_getreg(adreno_dev,
					ADRENO_REG_SP_FS_OBJ_START_REG))
				ib_parse_vars->sp_fs_obj_start_reg = ptr[i + 1];
		}
	}
	ib_add_type0_entries(device, ptbase, ib_obj_list,
				ib_parse_vars);
}

/*
                                                            
                                                       
                                                                         
                
                                      
                                
                                                                          
  
                                                                            
                                         
                                       
 */
static int adreno_ib_find_objs(struct kgsl_device *device,
				phys_addr_t ptbase,
				unsigned int gpuaddr, unsigned int dwords,
				struct adreno_ib_object_list *ib_obj_list)
{
	int ret = 0;
	int rem = dwords;
	int i;
	struct ib_parser_variables ib_parse_vars;
	unsigned int *src;
	struct adreno_ib_object *ib_obj;
	struct kgsl_mem_entry *entry;

	/*                                           */
	for (i = 0; i < ib_obj_list->num_objs; i++) {
		ib_obj = &(ib_obj_list->obj_list[i]);
		if ((ib_obj->gpuaddr <= gpuaddr) &&
			((ib_obj->gpuaddr + ib_obj->size) >=
			(gpuaddr + (dwords << 2))))
			return 0;
	}

	entry = kgsl_get_mem_entry(device, ptbase, gpuaddr, (dwords << 2));
	if (!entry)
		return -EINVAL;

	src = (unsigned int *)kgsl_gpuaddr_to_vaddr(&entry->memdesc, gpuaddr);
	if (!src) {
		kgsl_mem_entry_put(entry);
		return -EINVAL;
	}

	memset(&ib_parse_vars, 0, sizeof(struct ib_parser_variables));

	ret = adreno_ib_add_range(device, ptbase, gpuaddr, dwords << 2,
				SNAPSHOT_GPU_OBJECT_IB, ib_obj_list);
	if (ret)
		goto done;

	for (i = 0; rem > 0; rem--, i++) {
		int pktsize;

		/*
                                                                
                                        
   */
		if (!pkt_is_type0(src[i]) && !pkt_is_type3(src[i]))
			break;

		pktsize = type3_pkt_size(src[i]);

		if (!pktsize || (pktsize + 1) > rem)
			break;

		if (pkt_is_type3(src[i])) {
			if (adreno_cmd_is_ib(src[i])) {
				unsigned int gpuaddrib2 = src[i + 1];
				unsigned int size = src[i + 2];

				ret = adreno_ib_find_objs(
						device, ptbase,
						gpuaddrib2, size,
						ib_obj_list);
				if (ret < 0)
					goto done;
			} else {
				ret = ib_parse_type3(device, &src[i], ptbase,
						ib_obj_list,
						&ib_parse_vars);
				/*
                                             
                                                 
                                      
     */

				if (ret < 0)
					goto done;
			}
		} else if (pkt_is_type0(src[i])) {
			ib_parse_type0(device, &src[i], ptbase, ib_obj_list,
					&ib_parse_vars);
		}

		i += pktsize;
		rem -= pktsize;
	}
	/*
                                                                      
                                                                       
                                                                       
                                                                    
          
  */
	ret = ib_add_type0_entries(device, ptbase, ib_obj_list,
				&ib_parse_vars);
done:
	kgsl_memdesc_unmap(&entry->memdesc);
	kgsl_mem_entry_put(entry);
	return ret;
}


/*
                                                                     
                                                       
                                                                         
                
                                      
                                
                                                                          
  
                                                                       
                                   
                                                         
 */
int adreno_ib_create_object_list(struct kgsl_device *device,
		phys_addr_t ptbase,
		unsigned int gpuaddr, unsigned int dwords,
		struct adreno_ib_object_list **out_ib_obj_list)
{
	int ret = 0;
	struct adreno_ib_object_list *ib_obj_list;

	if (!out_ib_obj_list)
		return -EINVAL;

	ib_obj_list = kzalloc(sizeof(*ib_obj_list), GFP_KERNEL);
	if (!ib_obj_list)
		return -ENOMEM;

	ib_obj_list->obj_list = vmalloc(MAX_IB_OBJS *
					sizeof(struct adreno_ib_object));

	if (!ib_obj_list->obj_list) {
		kfree(ib_obj_list);
		return -ENOMEM;
	}

	ret = adreno_ib_find_objs(device, ptbase, gpuaddr, dwords,
		ib_obj_list);

	if (ret)
		adreno_ib_destroy_obj_list(ib_obj_list);
	else
		*out_ib_obj_list = ib_obj_list;

	return ret;
}

/*
                                                           
                                
  
                                               
 */
void adreno_ib_destroy_obj_list(struct adreno_ib_object_list *ib_obj_list)
{
	int i;

	if (!ib_obj_list)
		return;

	for (i = 0; i < ib_obj_list->num_objs; i++) {
		if (ib_obj_list->obj_list[i].entry)
			kgsl_mem_entry_put(ib_obj_list->obj_list[i].entry);
	}
	vfree(ib_obj_list->obj_list);
	kfree(ib_obj_list);
}
