/*
 * mips/irix5/md.h
 * Irix 5 MIPS configuration information.
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

#ifndef __mips_irix5_md_h
#define __mips_irix5_md_h

#include "mips/common.h"
#include "mips/sysdepCallMethod.h"
#include "mips/threads.h"


#define SIGNAL_ARGS(sig, sc) int sig, int code UNUSED, struct sigcontext *sc
#define SIGNAL_CONTEXT_POINTER(scp) struct sigcontext *scp
#define GET_SIGNAL_CONTEXT_POINTER(sc) (sc)
#define SIGNAL_PC(scp) 
#define STACK_POINTER(scp) ((scp)->sc_badvaddr)

#include "kaffe-unix-stack.h"

#endif
