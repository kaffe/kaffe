/*
 * i386/cygwin32/md.h
 * Windows'95 (Cygnus GNU C) i386 configuration information.
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

#ifndef __i386_cygwin32_md_h
#define __i386_cygwin32_md_h

/**/
/* Thread handling */
/**/
#include "i386/common.h"
#include "i386/threads.h"

/* Redefine the stack pointer offset */
#undef SP_OFFSET
#define	SP_OFFSET	7

#define SIGNAL_ARGS(sig, sc) int sig
#define SIGNAL_CONTEXT_POINTER(scp) int scp
#define GET_SIGNAL_CONTEXT_POINTER(sc) (NULL)
#define SIGNAL_PC(scp) (0)

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

#endif
