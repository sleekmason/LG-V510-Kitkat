/**
 * Header file of MobiCore Driver Kernel Module.
 *
 * @addtogroup MCD_MCDIMPL_KMOD_IMPL
 * @{
 * Internal structures of the McDrvModule
 * @file
 *
 * Header file the MobiCore Driver Kernel Module,
 * its internal structures and defines.
 *
 * <!-- Copyright Giesecke & Devrient GmbH 2009-2012 -->
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _MC_DRV_KMOD_H_
#define _MC_DRV_KMOD_H_

#include "mc_drv_module_linux_api.h"
#include "public/mc_drv_module_api.h"
/*                             */
#include "platform.h"

/*                               */
#define ARM_CPSR_MASK 0x1F
#define ARM_MONITOR_MODE 0b10110
#define ARM_SECURITY_EXTENSION_MASK 0x30

/* 
                                                                         
                                                                          
 */
#define MC_ARM_L2_TABLE_ENTRIES		256

/*                                                                           */
#define MC_DRV_KMOD_CONTG_BUFFER_MAX	16

/*                                                                 */
#define MC_DRV_KMOD_L2_TABLE_PER_PAGES	4

/*                                                    */
struct l2table {
	pte_t	table_entries[MC_ARM_L2_TABLE_ENTRIES];
};

#define INVALID_ADDRESS     ((void *)(-1))

/*                  */
#define L2_FLAG_SMALL_XN    (1U <<  0)
#define L2_FLAG_SMALL       (1U <<  1)
#define L2_FLAG_B           (1U <<  2)
#define L2_FLAG_C           (1U <<  3)
#define L2_FLAG_AP0         (1U <<  4)
#define L2_FLAG_AP1         (1U <<  5)
#define L2_FLAG_SMALL_TEX0  (1U <<  6)
#define L2_FLAG_SMALL_TEX1  (1U <<  7)
#define L2_FLAG_SMALL_TEX2  (1U <<  8)
#define L2_FLAG_APX         (1U <<  9)
#define L2_FLAG_S           (1U << 10)
#define L2_FLAG_NG          (1U << 11)

/* 
                                       
                                                                      
                
                                                                      
 */
struct mc_contg_buffer {
	unsigned int	handle; /*               */
	void		*virt_user_addr; /*                              */
	void		*virt_kernel_addr; /*                                */
	void		*phys_addr; /*                          */
	unsigned int	num_pages; /*                   */
};

/*                                              */
struct mc_instance {
	/*                */
	unsigned int	handle;
	/*                                    */
	pid_t		pid_vnr;
	/*                                                  
                             */
	struct mc_contg_buffer	contg_buffers[MC_DRV_KMOD_CONTG_BUFFER_MAX];
};

/*                                          */
struct mc_l2_table_store {
	struct l2table table[MC_DRV_KMOD_L2_TABLE_PER_PAGES];
};

/*                                                            */
struct mc_l2_tables_set {
	struct list_head		list;
	unsigned int			usage_bitmap;	/*                */
	struct mc_l2_table_store	*kernel_virt;	/*                          */
	struct mc_l2_table_store	*phys;		/*                    */
	struct page			*page;		/*                          */
};

/* 
                                                                              
                                                                      
                                                                            
                                                                    
 */
struct mc_used_l2_table {
	struct list_head	list;

	/*                                      */
	unsigned int		handle;
	unsigned int		flags;

	/*                         */
	struct mc_instance	*owner;

	/*                                              */
	struct mc_l2_tables_set	*set;

	/*                          */
	unsigned int		idx;

	/*                 */
	unsigned int		nr_of_pages;
};

#define MC_WSM_L2_CONTAINER_WSM_LOCKED_BY_APP   (1U << 0)
#define MC_WSM_L2_CONTAINER_WSM_LOCKED_BY_MC    (1U << 1)


/*                                         */
struct mc_ssiq_ctx {
	/*                          */
	atomic_t	counter;
};

/*                                */
struct mc_daemon_ctx {
	/*                  */
	struct semaphore	sem;
	struct fasync_struct	*async_queue;
	/*                */
	unsigned int		ssiq_counter;
};

/*                                              */
struct mc_drv_kmod_ctx {

	/*                            */
	atomic_t		unique_counter;

	/*                          */
	struct mc_ssiq_ctx	ssiq_ctx;

	/*                          */
	struct mc_daemon_ctx	daemon_ctx;

	/*                                */
	struct mc_instance	*daemon_inst;

	/*                              */
	struct list_head	mc_l2_tables_sets;

	/*                                 */
	struct list_head	mc_used_l2_tables;

	/*                                                 */
	struct semaphore	wsm_l2_sem;
};

/*                                            */
struct mc_trace_buf {
	uint32_t version; /*                           */
	uint32_t length; /*                                               */
	uint32_t write_pos; /*                       */
	char  buff[1]; /*                           */
};

/*                                      */
void mobicore_log_read(void);
long mobicore_log_setup(void *);
void mobicore_log_free(void);

#define MCDRV_DBG_ERROR(txt, ...) \
	printk(KERN_ERR "mcDrvKMod [%d] %s() ### ERROR: " txt, \
		task_pid_vnr(current), \
		__func__, \
		##__VA_ARGS__)

/*                              */
#define DUMMY_FUNCTION()    do {} while (0)

#if defined(DEBUG)

/*                       */
#if defined(DEBUG_VERBOSE)
#define MCDRV_DBG_VERBOSE          MCDRV_DBG
#else
#define MCDRV_DBG_VERBOSE(...)     DUMMY_FUNCTION()
#endif

#define MCDRV_DBG(txt, ...) \
	printk(KERN_INFO "mcDrvKMod [%d on CPU%d] %s(): " txt, \
		task_pid_vnr(current), \
		raw_smp_processor_id(), \
		__func__, \
		##__VA_ARGS__)

#define MCDRV_DBG_WARN(txt, ...) \
	printk(KERN_WARNING "mcDrvKMod [%d] %s() WARNING: " txt, \
		task_pid_vnr(current), \
		__func__, \
		##__VA_ARGS__)

#define MCDRV_ASSERT(cond) \
	do { \
		if (unlikely(!(cond))) { \
			panic("mcDrvKMod Assertion failed: %s:%d\n", \
				__FILE__, __LINE__); \
		} \
	} while (0)

#else

#define MCDRV_DBG_VERBOSE(...)	DUMMY_FUNCTION()
#define MCDRV_DBG(...)		DUMMY_FUNCTION()
#define MCDRV_DBG_WARN(...)	DUMMY_FUNCTION()

#define MCDRV_ASSERT(...)	DUMMY_FUNCTION()

#endif /*                      */


#endif /*                 */
/*     */
