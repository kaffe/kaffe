/*
 * ia64/hpux/md.h
 * HP-UX IA64 configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003, 2005
 *      Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */
 
#ifndef __parisc_ia64_md_h
#define __parisc_ia64_md_h
 
#include "ia64/common.h"
#include "ia64/sysdepCallMethod.h"
#include "ia64/threads.h"

#define	LIBRARYPATH	"SHLIB_PATH"

#include <siginfo.h>
#include <ucontext.h>
#define SIGNAL_ARGS(sig, sc) int sig, siginfo_t* sip, ucontext_t* sc
#define SIGNAL_CONTEXT_POINTER(scp) ucontext_t *scp
#define GET_SIGNAL_CONTEXT_POINTER(sc) (sc)
#define SIGNAL_PC(scp) ((scp)->uc_mcontext.gregs[EIP]) 

#endif
