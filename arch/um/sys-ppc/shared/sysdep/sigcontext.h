/* 
 * Copyright (C) 2000 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#ifndef __SYS_SIGCONTEXT_PPC_H
#define __SYS_SIGCONTEXT_PPC_H

#define DSISR_WRITE 0x02000000

#define SC_FAULT_ADDR(sc) ({ \
		struct sigcontext *_sc = (sc); \
		long retval = -1; \
		switch (_sc->regs->trap) { \
		case 0x300: \
			/*                */ \
			retval = _sc->regs->dar; \
			break; \
		case 0x400: \
			/*                       */ \
			retval = _sc->regs->nip; \
			break; \
		default: \
			panic("SC_FAULT_ADDR: unhandled trap type\n"); \
		} \
		retval; \
	})

#define SC_FAULT_WRITE(sc) ({ \
		struct sigcontext *_sc = (sc); \
		long retval = -1; \
		switch (_sc->regs->trap) { \
		case 0x300: \
			/*                */ \
			retval = !!(_sc->regs->dsisr & DSISR_WRITE); \
			break; \
		case 0x400: \
			/*                                    */ \
			retval = 0; \
			break; \
		default: \
			panic("SC_FAULT_ADDR: unhandled trap type\n"); \
		} \
		retval; \
	})

#define SC_IP(sc) ((sc)->regs->nip)
#define SC_SP(sc) ((sc)->regs->gpr[1])
#define SEGV_IS_FIXABLE(sc) (1)

#endif

