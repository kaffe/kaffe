/*
 * s390/linux/md.h
 * Linux s390 configuration information (clone from i386/linux/md.h by ROSSP).
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003
 *      Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __ks390_linux_md_h
#define __ks390_linux_md_h

#include "s390/common.h"
#include "s390/sysdepCallMethod.h"
#include "s390/threads.h"

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

/* Linux requires a little initialisation */
extern void init_md(void);
#define	INIT_MD()	init_md()

#include "sigcontextinfo.h"

#define SIGNAL_ARGS(sig, sc) int sig, struct sigcontext sc
#define SIGNAL_CONTEXT_POINTER(scp) struct sigcontext * scp
#define GET_SIGNAL_CONTEXT_POINTER(sc) (&sc)
#define SIGNAL_PC(scp) (GET_PC(scp))
#define STACK_POINTER(scp) (GET_STACK(scp))

#endif
