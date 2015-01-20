/*
                                           
                                  
  
                                              
  
 */

#include <linux/init.h>
#include <linux/errno.h>
#include <linux/kernel.h>

#include <asm/ptrace.h>
#include <asm/traps.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/gpio-internal.h>
#include <asm/regs267x.h>

/*                   */
const int __initdata h8300_saved_vectors[]={
#if defined(CONFIG_GDB_DEBUG)
	TRACE_VEC,
	TRAP3_VEC,
#endif
	-1
};

/*                  */
const H8300_VECTOR __initdata h8300_trap_table[] = {
	0,0,0,0,0,
	trace_break,  /*       */
	0,0,
	system_call,  /*          */
	0,0,0,0,0,0,0
};

/*                    */
struct irq_pins {
	unsigned char port_no;
	unsigned char bit_no;
} __attribute__((aligned(1),packed));
/*          */
static const struct irq_pins irq_assign_table0[16]={
        {H8300_GPIO_P5,H8300_GPIO_B0},{H8300_GPIO_P5,H8300_GPIO_B1},
	{H8300_GPIO_P5,H8300_GPIO_B2},{H8300_GPIO_P5,H8300_GPIO_B3},
	{H8300_GPIO_P5,H8300_GPIO_B4},{H8300_GPIO_P5,H8300_GPIO_B5},
	{H8300_GPIO_P5,H8300_GPIO_B6},{H8300_GPIO_P5,H8300_GPIO_B7},
	{H8300_GPIO_P6,H8300_GPIO_B0},{H8300_GPIO_P6,H8300_GPIO_B1},
	{H8300_GPIO_P6,H8300_GPIO_B2},{H8300_GPIO_P6,H8300_GPIO_B3},
	{H8300_GPIO_P6,H8300_GPIO_B4},{H8300_GPIO_P6,H8300_GPIO_B5},
	{H8300_GPIO_PF,H8300_GPIO_B1},{H8300_GPIO_PF,H8300_GPIO_B2},
};
/*          */
static const struct irq_pins irq_assign_table1[16]={
	{H8300_GPIO_P8,H8300_GPIO_B0},{H8300_GPIO_P8,H8300_GPIO_B1},
	{H8300_GPIO_P8,H8300_GPIO_B2},{H8300_GPIO_P8,H8300_GPIO_B3},
	{H8300_GPIO_P8,H8300_GPIO_B4},{H8300_GPIO_P8,H8300_GPIO_B5},
	{H8300_GPIO_PH,H8300_GPIO_B2},{H8300_GPIO_PH,H8300_GPIO_B3},
	{H8300_GPIO_P2,H8300_GPIO_B0},{H8300_GPIO_P2,H8300_GPIO_B1},
	{H8300_GPIO_P2,H8300_GPIO_B2},{H8300_GPIO_P2,H8300_GPIO_B3},
	{H8300_GPIO_P2,H8300_GPIO_B4},{H8300_GPIO_P2,H8300_GPIO_B5},
	{H8300_GPIO_P2,H8300_GPIO_B6},{H8300_GPIO_P2,H8300_GPIO_B7},
};

/*                             */
#define IRQ_GPIO_MAP(irqbit,irq,port,bit)			  \
do {								  \
	if (*(volatile unsigned short *)ITSR & irqbit) {	  \
		port = irq_assign_table1[irq - EXT_IRQ0].port_no; \
		bit  = irq_assign_table1[irq - EXT_IRQ0].bit_no;  \
	} else {						  \
		port = irq_assign_table0[irq - EXT_IRQ0].port_no; \
		bit  = irq_assign_table0[irq - EXT_IRQ0].bit_no;  \
	}							  \
} while(0)

int h8300_enable_irq_pin(unsigned int irq)
{
	if (irq >= EXT_IRQ0 && irq <= EXT_IRQ15) {
		unsigned short ptn = 1 << (irq - EXT_IRQ0);
		unsigned int port_no,bit_no;
		IRQ_GPIO_MAP(ptn, irq, port_no, bit_no);
		if (H8300_GPIO_RESERVE(port_no, bit_no) == 0)
			return -EBUSY;                   /*                 */
		H8300_GPIO_DDR(port_no, bit_no, H8300_GPIO_INPUT);
		*(volatile unsigned short *)ISR &= ~ptn; /*           */
	}

	return 0;
}

void h8300_disable_irq_pin(unsigned int irq)
{
	if (irq >= EXT_IRQ0 && irq <= EXT_IRQ15) {
		/*                                     */
		unsigned short ptn = 1 << (irq - EXT_IRQ0);
		unsigned short port_no,bit_no;
		*(volatile unsigned short *)ISR &= ~ptn;
		*(volatile unsigned short *)IER &= ~ptn;
		IRQ_GPIO_MAP(ptn, irq, port_no, bit_no);
		H8300_GPIO_FREE(port_no, bit_no);
	}
}
