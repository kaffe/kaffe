/*
 * alpha/osf/md.c
 * OSF/1 Alpha specific functions.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include <machine/fpu.h>
#include <sys/sysinfo.h>
#include <machine/hal_sysinfo.h>
#include <sys/proc.h>

#ifndef FPCR_INED
#define FPCR_INED 0
#endif
#ifndef FPCR_UNFD
#define FPCR_UNFD 0
#endif
#ifndef FPCR_OVFD
#define FPCR_OVFD 0
#endif
#ifndef FPCR_DZED
#define FPCR_DZED 0
#endif

void
init_md(void)
{
#if 1 /* This doesn't seem to make any difference, but let's keep it.  */
	/* Set the bits in the hw fpcr for cpu's that implement
	   all the bits.  */
	__asm__ __volatile__(
		"excb\n\t"
		"mt_fpcr %0\n\t"
		"excb"
		: : "f"(FPCR_INED | FPCR_UNFD | FPCR_DYN_NORMAL
			| FPCR_OVFD | FPCR_DZED));
#endif

#if 0 /* This breaks DoublePrint and DoubleComp tests.  */
	/* Set the software emulation bits in the kernel for
	   those that don't.  */
	ieee_set_fp_control(IEEE_TRAP_ENABLE_INV);
#endif
}


/* This bit turns off unaligned access fixups in favour of SIGBUS.  It
   is not called by Kaffe, but it is useful to invoke from within 
   the debugger.  */

void alpha_disable_uac()
{
  int buf[2];

  buf[0] = SSIN_UACPROC;
  buf[1] = UAC_SIGBUS;
  setsysinfo(SSI_NVPAIRS, buf, 1, 0, 0);
}
