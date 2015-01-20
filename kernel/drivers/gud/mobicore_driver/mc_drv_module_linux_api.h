/**
 * Header file of MobiCore Driver Kernel Module.
 *
 * @addtogroup MobiCore_Driver_Kernel_Module
 * @{
 * Wrapper for Linux API
 * @file
 *
 * Some convenient wrappers for memory functions
 *
 * <!-- Copyright Giesecke & Devrient GmbH 2009-2012 -->
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _MC_DRV_MODULE_LINUX_API_H_
#define _MC_DRV_MODULE_LINUX_API_H_

#include <linux/version.h>
#include <linux/miscdevice.h>
#include <linux/interrupt.h>
#include <linux/highmem.h>
#include <linux/kthread.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <asm/sizes.h>
#include <asm/pgtable.h>
#include <linux/semaphore.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>


/*                      */
#if !defined(TRUE)
#define TRUE (1 == 1)
#endif

#if !defined(FALSE)
#define FALSE (1 != 1)
#endif


/*                     */
#if !defined(__init)
#warning "missing definition: __init"
/*                */
#define __init
#endif


#if !defined(__exit)
#warning "missing definition: __exit"
/*                */
#define __exit
#endif


#if !defined(__must_check)
#warning "missing definition: __must_check"
/*                */
#define __must_check
#endif


#if !defined(__user)
#warning "missing definition: __user"
/*                */
#define __user
#endif

#define INVALID_ORDER       ((unsigned int)(-1))

/*                                                                            */
/*                                                                           */
static inline void *get_page_start(
	void *addr
)
{
	return (void *)(((unsigned long)(addr)) & PAGE_MASK);
}

/*                                                                            */
/*                                                                      */
static inline unsigned int get_offset_in_page(
	void *addr
)
{
	return (unsigned int)(((unsigned long)(addr)) & (~PAGE_MASK));
}

/*                                                                            */
/*                                         */
static inline unsigned int get_nr_of_pages_for_buffer(
	void		*addr_start, /*             */
	unsigned int	len
)
{
	/*                                         
                                                  
                                           
                                           
                                           
                                           
                                            */

	return (get_offset_in_page(addr_start) + len + PAGE_SIZE-1) / PAGE_SIZE;
}


/*                                                                            */
/* 
                                                                               
                                                                      
                                                                      
                                                                      
                                 
                                                                            
                                                                            
                                                                            
  
               
                
 */
static inline unsigned int size_to_order(
	unsigned int size
)
{
	unsigned int order = INVALID_ORDER;

	if (size != 0) {
		/*                                                            
                                                         
                   
                                                       
                                                       

                                                    */
		order = __builtin_clz(
				get_nr_of_pages_for_buffer(NULL, size));

		/*                                                            
                           */
		if (unlikely(order > 31))
			return INVALID_ORDER;
		order = 31 - order;

		/*                                                    */
		/*                             */
		if (((1<<order)*PAGE_SIZE) < size)
			order++;
	}
	return order;
}

/*                   */
#if !defined(list_for_each_entry)
/*               */
#error "missing macro: list_for_each_entry()"
/*                */
#define list_for_each_entry(a, b, c)    if (0)
#endif

/*                                                                            */
/*                                            */
static inline unsigned int addr_to_pfn(
	void *addr
)
{
	/*                               */
	return ((unsigned int)(addr)) >> PAGE_SHIFT;
}


/*                                                                            */
/*                                           */
static inline void *pfn_to_addr(
	unsigned int pfn
)
{
	/*                               */
	return (void *)(pfn << PAGE_SHIFT);
}

#endif /*                             */
/*     */
