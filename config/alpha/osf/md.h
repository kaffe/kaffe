/*
 * alpha/osf/md.h
 * OSF/1 Alpha configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __alpha_osf_md_h
#define __alpha_osf_md_h

#include "alpha/common.h"
#include "alpha/threads.h"

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

#include "kaffe-unix-stack.h"

#define KAFFEMD_DISABLE_SETSTACKSIZE

#undef SP_OFFSET
#define SP_OFFSET 34

#include <machine/context.h>
#include <machine/signal.h>

#define SIGNAL_ARGS(sig, sc) int sig, int alpha_dummy, struct sigcontext *sc
#define SIGNAL_CONTEXT_POINTER(scp) struct sigcontext * scp
#define GET_SIGNAL_CONTEXT_POINTER(sc) (sc)
#define SIGNAL_PC(scp) ((void *) (scp)->sc_pc)

/* Alpha's require a little initialisation for proper IEEE math.  */
extern void init_md(void);
#define INIT_MD()       init_md()

#endif
