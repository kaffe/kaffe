/*
 * alpha/linux/md.c
 * Linux Alpha specific functions.
 *
 * Copyright (c) 2001
 *	Edouard G. Parmelan.  All rights reserved.
 *
 * Copyright (c) 1996, 1997, 2001
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "config.h"
#include <malloc.h>
#include <asm/fpu.h>

void
init_md(void)
{
#if 0 /* This breaks DoublePrint and DoubleComp tests.  */
	/* Set the software emulation bits in the kernel for
	   those that don't implement all of the fpcr bits.  */
	ieee_set_fp_control(IEEE_TRAP_ENABLE_INV);
#endif

#if 0 /* This doesn't seem to make any difference, but let's keep it.  */
	/* Set the bits in the hw fpcr for cpu's that do.  */
	__asm__ __volatile__(
		"excb\n\t"
		"mt_fpcr %0\n\t"
		"excb"
		: : "f"(FPCR_INED | FPCR_UNFD | FPCR_DYN_NORMAL
		        | FPCR_OVFD | FPCR_DZED));
#endif

#if defined(HAVE_MALLOPT)
	/* Turn off the use of mmap in malloc so that we have a
	   continuous heap to present to the gc.  */
	mallopt(M_MMAP_MAX, 0);
#endif
}

#if 0
/* This bit turns off unaligned access fixups in favour of SIGBUS.  It
   is not called by Kaffe, but it is useful to invoke from within gdb.
   It talks to a patch that will likely be included in 2.1.22 or .23,
   so if you don't have that you'll need to ifdef this out.  */

#include <asm/sysinfo.h>
#include <asm/unistd.h>

void alpha_disable_uac()
{
  int buf[2];

  buf[0] = SSIN_UACPROC;
  buf[1] = UAC_SIGBUS;
  syscall(__NR_setsysinfo, SSI_NVPAIRS, buf, 1, 0, 0);
}
#endif

#if defined(TRANSLATOR)
#include "alpha/alpha.c"
#endif
