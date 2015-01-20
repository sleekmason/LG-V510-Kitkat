/** @addtogroup MCD_MCDIMPL_KMOD_KAPI Mobicore Driver Module API inside Kernel.
 * @ingroup  MCD_MCDIMPL_KMOD
 * @{
 * Interface to Mobicore Driver Kernel Module inside Kernel.
 * @file
 *
 * Interface to be used by module MobiCoreKernelAPI.
 *
 * <!-- Copyright Giesecke & Devrient GmbH 2010-2012 -->
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _MOBICORE_KERNELMODULE_API_H_
#define _MOBICORE_KERNELMODULE_API_H_

struct mc_instance;

/* 
                                                
  
                                                          
 */
struct mc_instance *mobicore_open(
	void
);

/* 
                                                                   
                           
                              
 */
int mobicore_release(
	struct mc_instance	*instance
);

/* 
                                                         
                  
                                      
  
                        
  
 */
int mobicore_allocate_wsm(
	struct mc_instance	*instance,
	unsigned long		requested_size,
	uint32_t		*handle,
	void			**kernel_virt_addr,
	void			**phys_addr
);

/* 
                                                         
                  
                                      
  
                        
  
 */
int mobicore_free(
	struct mc_instance	*instance,
	uint32_t		handle
);

/* 
                                                    
                  
                                                                    
                            
                                  
                                                           
  
                        
  
 */
int mobicore_map_vmem(
	struct mc_instance	*instance,
	void			*addr,
	uint32_t		len,
	uint32_t		*handle,
	void			**phys_wsm_l2_table
);

/* 
                                              
                  
                
  
                        
  
 */
int mobicore_unmap_vmem(
	struct mc_instance	*instance,
	uint32_t		handle
);
#endif /*                               */
/*     */
