/*
 * alpha/netbsd1/md.c
 * NetBSD alpha specific functions.
 *
 * Copyright (c) 2001
 *	Edouard G. Parmelan.  All rights reserved.
 *
 * Copyright (c) 1996, 1997, 2001
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Written by Tim Wilkinson <tim@tjwassoc.co.uk>
 * Modified by Paul Reilly <pmr@pajato.com>
 */

#include <machine/fpu.h>

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

#ifdef __NetBSD__
/* Define these beasts to get past compile errors.  The definitions
are pure guesses. */

#define FPCR_DYN_NORMAL 0

#endif

void
init_md(void)
{
        /* Set the bits in the hw fpcr for cpu's that implement
           all the bits.  */
        __asm__ __volatile__(
                "excb\n\t"
                "mt_fpcr %0\n\t"
                "excb"
                : : "f"(FPCR_INED | FPCR_UNFD | FPCR_DYN_NORMAL
                        | FPCR_OVFD | FPCR_DZED));

#if 0
/* This code does not work on NetBSD and I do not have a clue why.
   Disable it until forced to fix it, or someone else fixes it. */

        /* Set the software emulation bits in the kernel for
           those that don't.  */
        ieee_set_fp_control(IEEE_TRAP_ENABLE_INV);
#endif
}

#if defined(TRANSLATOR)
#include "alpha/alpha.c"
#endif
