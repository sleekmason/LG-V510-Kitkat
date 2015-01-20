/*
 * OpenRISC Linux
 *
 * Linux architectural port borrowing liberally from similar works of
 * others.  All original copyrights apply as per the original source
 * declaration.
 *
 * OpenRISC implementation:
 * Copyright (C) 2003 Matjaz Breskvar <phoenix@bsemi.com>
 * Copyright (C) 2010-2011 Jonas Bonn <jonas@southpole.se>
 * et al.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __ASM_OPENRISC_PROCESSOR_H
#define __ASM_OPENRISC_PROCESSOR_H

#include <asm/spr_defs.h>
#include <asm/page.h>
#include <asm/ptrace.h>

#define STACK_TOP       TASK_SIZE
#define STACK_TOP_MAX	STACK_TOP
/*                                     */
#define KERNEL_SR (SPR_SR_DME | SPR_SR_IME | SPR_SR_ICE \
		   | SPR_SR_DCE | SPR_SR_SM)
#define USER_SR   (SPR_SR_DME | SPR_SR_IME | SPR_SR_ICE \
		   | SPR_SR_DCE | SPR_SR_IEE | SPR_SR_TEE)
/*
                                                       
                                           
 */
#define current_text_addr() ({ __label__ _l; _l: &&_l; })

/*
                                                                
                                                         
 */

#define TASK_SIZE       (0x80000000UL)

/*                                                                 
                       
 */
#define TASK_UNMAPPED_BASE      (TASK_SIZE / 8 * 3)

#ifndef __ASSEMBLY__

struct task_struct;

struct thread_struct {
};

/*
                                                                         
                                                                     
                                                                      
                                                                          
                                                                       
                              
 */
#define user_regs(thread_info)  (((struct pt_regs *)((unsigned long)(thread_info) + THREAD_SIZE - STACK_FRAME_OVERHEAD)) - 1)

/*
                                          
 */

#define task_pt_regs(task) user_regs(task_thread_info(task))
#define current_regs() user_regs(current_thread_info())

extern inline void prepare_to_copy(struct task_struct *tsk)
{
}

#define INIT_SP         (sizeof(init_stack) + (unsigned long) &init_stack)

#define INIT_THREAD  { }


#define KSTK_EIP(tsk)   (task_pt_regs(tsk)->pc)
#define KSTK_ESP(tsk)   (task_pt_regs(tsk)->sp)


extern int kernel_thread(int (*fn)(void *), void * arg, unsigned long flags);

void start_thread(struct pt_regs *regs, unsigned long nip, unsigned long sp);
void release_thread(struct task_struct *);
unsigned long get_wchan(struct task_struct *p);

/*
                                            
 */

extern inline void exit_thread(void)
{
	/*                            */
}

/*
                                                                      
 */
extern unsigned long thread_saved_pc(struct task_struct *t);

#define init_stack      (init_thread_union.stack)

#define cpu_relax()     do { } while (0)

#endif /*              */
#endif /*                            */