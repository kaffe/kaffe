/*
 * powerpc/aix/md.h
 * AIX powerpc configuration information.
 *
 * Copyright (c) 1996 Sandpiper software consulting, LLC
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Written by Rick Greer <rick@sandpiper.com>, 1996;
 * Used by Brad Harvey <b_harvey@hitachi.com>, 1997 for AIX port.
 * 
 */

#ifndef __powerpc_aix_md_h
#define __powerpc_aix_md_h

#include "powerpc/common.h"
#include "powerpc/threads.h"

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

#ifdef SP_OFFSET
#undef SP_OFFSET
#endif
#define SP_OFFSET	3

/* Version without GAS */
extern int __aix_cmpxchg(void **A, void *O, void *N);

#undef COMPARE_AND_EXCHANGE
#define COMPARE_AND_EXCHANGE(A,O,N)		\
	__aix_cmpxchg((void**)(A), (void*)(O), (void*)(N))

#define SIGNAL_ARGS(sig, sc) int sig
#undef HAVE_SIGALTSTACK

/* align data types to their size */
/* might be wrong, some 8byte stuff is 4byte aligned */
#define   ALIGNMENT_OF_SIZE(S)    (S)

#endif
