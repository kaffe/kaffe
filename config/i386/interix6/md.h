/*
 * i386/interix6/md.h
 * Windows Vista Ultimate Interix i386 configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003, 2007
 *      Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __i386_interix6_md_h
#define __i386_interix6_md_h

/**/
/* Thread handling */
/**/
#include "i386/common.h"
#include "i386/threads.h"

/* It looks like that cygwin/gcc is aligning uint64 on 8 bytes.
 * Putting this here fixes kaffe behaviour.
 */
#undef ALIGNMENT_OF_SIZE
#define ALIGNMENT_OF_SIZE(S)    ((S) < 8 ? (S) : 8)

/* Redefine the stack pointer offset */
#undef SP_OFFSET
#define	SP_OFFSET	4

#define SIGNAL_ARGS(sig, sc) int sig
#define SIGNAL_CONTEXT_POINTER(scp) int scp
#define GET_SIGNAL_CONTEXT_POINTER(sc) (NULL)
#define SIGNAL_PC(scp) (0)

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

#endif
