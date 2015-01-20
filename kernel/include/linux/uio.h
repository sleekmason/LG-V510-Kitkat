#ifndef __LINUX_UIO_H
#define __LINUX_UIO_H

#include <linux/compiler.h>
#include <linux/types.h>

/*
 *	Berkeley style UIO structures	-	Alan Cox 1994.
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 */

struct iovec
{
	void __user *iov_base;	/* BSD uses caddr_t (1003.1g requires void *) */
	__kernel_size_t iov_len; /*                          */
};

/*
                                                    
 */
 
#define UIO_FASTIOV	8
#define UIO_MAXIOV	1024

#ifdef __KERNEL__

struct kvec {
	void *iov_base; /*                                                 */
	size_t iov_len;
};

/*
                                             
  
                                                                      
                                                                           
                                         
 */
static inline size_t iov_length(const struct iovec *iov, unsigned long nr_segs)
{
	unsigned long seg;
	size_t ret = 0;

	for (seg = 0; seg < nr_segs; seg++)
		ret += iov[seg].iov_len;
	return ret;
}

unsigned long iov_shorten(struct iovec *iov, unsigned long nr_segs, size_t to);
#endif

#endif
