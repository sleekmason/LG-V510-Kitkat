/* fhc.h: FHC and Clock board register definitions.
 *
 * Copyright (C) 1997, 1999 David S. Miller (davem@redhat.com)
 */

#ifndef _SPARC64_FHC_H
#define _SPARC64_FHC_H

/*                               */
#define CLOCK_CTRL	0x00UL	/*              */
#define CLOCK_STAT1	0x10UL	/*            */
#define CLOCK_STAT2	0x20UL	/*            */
#define CLOCK_PWRSTAT	0x30UL	/*              */
#define CLOCK_PWRPRES	0x40UL	/*                */
#define CLOCK_TEMP	0x50UL	/*             */
#define CLOCK_IRQDIAG	0x60UL	/*                 */
#define CLOCK_PWRSTAT2	0x70UL	/*                  */

#define CLOCK_CTRL_LLED		0x04	/*                   */
#define CLOCK_CTRL_MLED		0x02	/*                  */
#define CLOCK_CTRL_RLED		0x01	/*                    */

/*                                      */
#define FHC_PREGS_ID	0x00UL	/*        */
#define  FHC_ID_VERS		0xf0000000 /*                      */
#define  FHC_ID_PARTID		0x0ffff000 /*                              */
#define  FHC_ID_MANUF		0x0000007e /*                                   */
#define  FHC_ID_RESV		0x00000001 /*               */
#define FHC_PREGS_RCS	0x10UL	/*                                   */
#define  FHC_RCS_POR		0x80000000 /*                              */
#define  FHC_RCS_SPOR		0x40000000 /*                                  */
#define  FHC_RCS_SXIR		0x20000000 /*                             */
#define  FHC_RCS_BPOR		0x10000000 /*                                  */
#define  FHC_RCS_BXIR		0x08000000 /*                                  */
#define  FHC_RCS_WEVENT		0x04000000 /*                                   */
#define  FHC_RCS_CFATAL		0x02000000 /*                                   */
#define  FHC_RCS_FENAB		0x01000000 /*                                  */
#define FHC_PREGS_CTRL	0x20UL	/*                      */
#define  FHC_CONTROL_ICS	0x00100000 /*                            */
#define  FHC_CONTROL_FRST	0x00080000 /*                           */
#define  FHC_CONTROL_LFAT	0x00040000 /*                               */
#define  FHC_CONTROL_SLINE	0x00010000 /*                               */
#define  FHC_CONTROL_DCD	0x00008000 /*                            */
#define  FHC_CONTROL_POFF	0x00004000 /*                              */
#define  FHC_CONTROL_FOFF	0x00002000 /*                            */
#define  FHC_CONTROL_AOFF	0x00001000 /*                               */
#define  FHC_CONTROL_BOFF	0x00000800 /*                               */
#define  FHC_CONTROL_PSOFF	0x00000400 /*                                   */
#define  FHC_CONTROL_IXIST	0x00000200 /*                                   */
#define  FHC_CONTROL_XMSTR	0x00000100 /*                                   */
#define  FHC_CONTROL_LLED	0x00000040 /*                 */
#define  FHC_CONTROL_MLED	0x00000020 /*                   */
#define  FHC_CONTROL_RLED	0x00000010 /*               */
#define  FHC_CONTROL_BPINS	0x00000003 /*                           */
#define FHC_PREGS_BSR	0x30UL	/*                           */
#define  FHC_BSR_DA64		0x00040000 /*                                    */
#define  FHC_BSR_DB64		0x00020000 /*                                    */
#define  FHC_BSR_BID		0x0001e000 /*                                    */
#define  FHC_BSR_SA		0x00001c00 /*                                    */
#define  FHC_BSR_SB		0x00000380 /*                                    */
#define  FHC_BSR_NDIAG		0x00000040 /*                                    */
#define  FHC_BSR_NTBED		0x00000020 /*                                    */
#define  FHC_BSR_NIA		0x0000001c /*                                    */
#define  FHC_BSR_SI		0x00000001 /*                                    */
#define FHC_PREGS_ECC	0x40UL	/*                                    */
#define FHC_PREGS_JCTRL	0xf0UL	/*                           */
#define  FHC_JTAG_CTRL_MENAB	0x80000000 /*                                */
#define  FHC_JTAG_CTRL_MNONE	0x40000000 /*                                   */
#define FHC_PREGS_JCMD	0x100UL	/*                           */
#define FHC_IREG_IGN	0x00UL	/*                */
#define FHC_FFREGS_IMAP	0x00UL	/*                  */
#define FHC_FFREGS_ICLR	0x10UL	/*                  */
#define FHC_SREGS_IMAP	0x00UL	/*                 */
#define FHC_SREGS_ICLR	0x10UL	/*                 */
#define FHC_UREGS_IMAP	0x00UL	/*               */
#define FHC_UREGS_ICLR	0x10UL	/*               */
#define FHC_TREGS_IMAP	0x00UL	/*              */
#define FHC_TREGS_ICLR	0x10UL	/*              */

#endif /*                   */
