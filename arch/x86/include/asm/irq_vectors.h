#ifndef _ASM_X86_IRQ_VECTORS_H
#define _ASM_X86_IRQ_VECTORS_H

#include <linux/threads.h>
/*
                           
  
                                                                        
                                                                       
                                                                 
                            
  
                                                                      
                                                                    
               
  
                                                                        
                                           
                                                        
                                                                                
                                                                    
  
                                                                      
  
                                                 
 */

#define NMI_VECTOR			0x02
#define MCE_VECTOR			0x12

/*
                                                                   
                                                      
 */
#define FIRST_EXTERNAL_VECTOR		0x20
/*
                                                                   
                                                
 */
#define VECTOR_OFFSET_START		1

/*
                                                                         
                                                                       
                         
 */
#define IRQ_MOVE_CLEANUP_VECTOR		FIRST_EXTERNAL_VECTOR

#define IA32_SYSCALL_VECTOR		0x80
#ifdef CONFIG_X86_32
# define SYSCALL_VECTOR			0x80
#endif

/*
                                                 
                                            
 */
#define IRQ0_VECTOR			((FIRST_EXTERNAL_VECTOR + 16) & ~15)

#define IRQ1_VECTOR			(IRQ0_VECTOR +  1)
#define IRQ2_VECTOR			(IRQ0_VECTOR +  2)
#define IRQ3_VECTOR			(IRQ0_VECTOR +  3)
#define IRQ4_VECTOR			(IRQ0_VECTOR +  4)
#define IRQ5_VECTOR			(IRQ0_VECTOR +  5)
#define IRQ6_VECTOR			(IRQ0_VECTOR +  6)
#define IRQ7_VECTOR			(IRQ0_VECTOR +  7)
#define IRQ8_VECTOR			(IRQ0_VECTOR +  8)
#define IRQ9_VECTOR			(IRQ0_VECTOR +  9)
#define IRQ10_VECTOR			(IRQ0_VECTOR + 10)
#define IRQ11_VECTOR			(IRQ0_VECTOR + 11)
#define IRQ12_VECTOR			(IRQ0_VECTOR + 12)
#define IRQ13_VECTOR			(IRQ0_VECTOR + 13)
#define IRQ14_VECTOR			(IRQ0_VECTOR + 14)
#define IRQ15_VECTOR			(IRQ0_VECTOR + 15)

/*
                                                              
  
                                                             
                                                                     
                                                                    
 */

#define SPURIOUS_APIC_VECTOR		0xff
/*
               
 */
#if ((SPURIOUS_APIC_VECTOR & 0x0F) != 0x0F)
# error SPURIOUS_APIC_VECTOR definition error
#endif

#define ERROR_APIC_VECTOR		0xfe
#define RESCHEDULE_VECTOR		0xfd
#define CALL_FUNCTION_VECTOR		0xfc
#define CALL_FUNCTION_SINGLE_VECTOR	0xfb
#define THERMAL_APIC_VECTOR		0xfa
#define THRESHOLD_APIC_VECTOR		0xf9
#define REBOOT_VECTOR			0xf8

/*
                                                  
 */
#define X86_PLATFORM_IPI_VECTOR		0xf7

/*
                   
 */
#define IRQ_WORK_VECTOR			0xf6

#define UV_BAU_MESSAGE			0xf5

/*                                                       */
#define XEN_HVM_EVTCHN_CALLBACK		0xf3

/*
                                                                
                                                              
                             
 */
#define LOCAL_TIMER_VECTOR		0xef

/*                                                      */
#if NR_CPUS <= 32
# define NUM_INVALIDATE_TLB_VECTORS	(NR_CPUS)
#else
# define NUM_INVALIDATE_TLB_VECTORS	(32)
#endif

#define INVALIDATE_TLB_VECTOR_END	(0xee)
#define INVALIDATE_TLB_VECTOR_START	\
	(INVALIDATE_TLB_VECTOR_END-NUM_INVALIDATE_TLB_VECTORS+1)

#define NR_VECTORS			 256

#define FPU_IRQ				  13

#define	FIRST_VM86_IRQ			   3
#define LAST_VM86_IRQ			  15

#ifndef __ASSEMBLY__
static inline int invalid_vm86_irq(int irq)
{
	return irq < FIRST_VM86_IRQ || irq > LAST_VM86_IRQ;
}
#endif

/*
                                         
  
                                                                  
                                                                      
                                                           
  
                                                                      
                 
 */

#define NR_IRQS_LEGACY			  16

#define IO_APIC_VECTOR_LIMIT		( 32 * MAX_IO_APICS )

#ifdef CONFIG_X86_IO_APIC
# define CPU_VECTOR_LIMIT		(64 * NR_CPUS)
# define NR_IRQS					\
	(CPU_VECTOR_LIMIT > IO_APIC_VECTOR_LIMIT ?	\
		(NR_VECTORS + CPU_VECTOR_LIMIT)  :	\
		(NR_VECTORS + IO_APIC_VECTOR_LIMIT))
#else /*                      */
# define NR_IRQS			NR_IRQS_LEGACY
#endif

#endif /*                        */
