/*
 * config-setjmp.h
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __config_setjmp_h
#define __config_setjmp_h

#include <setjmp.h>

/*
 * By default use setjmp/longjmp.  There is no standard behavior
 * for saving/restoring signal state on a setjmp/longjmp, so these
 * calls might waste (signficant) amounts of time saving and restoring
 * useless signal state.
 *
 * sigsetjmp/siglongjmp have defined behavior in terms of signal state,
 * but seem to have other problems on system 5-based systems.
 *
 * Beware that switching between setjmp and sigsetjmp may impact the
 * SP_OFFSET hack used in unix-jthreads/jthreads.c.
 */

#if 1
#ifndef JTHREAD_SETJMP
#define JTHREAD_SETJMP(buf)		setjmp((buf))
#endif
#ifndef JTHREAD_LONGJMP
#if defined(__FreeBSD__) && defined(__i386__)
/* XXX Check for a corrupted jump buffer before jumping there. This version for
   XXX FreeBSD/i386 checks the stack pointer and PC counter. This is a temp
   XXX hack to try to catch a bug (to reproduce: run GCTest a zillion times). */
#define JTHREAD_LONGJMP(buf, val)	do {				     \
					  assert((buf)->_jb[0]>0x1000	     \
					    && (u_int)(buf)->_jb[2]>0x1000); \
					  longjmp((buf), (val));	     \
					} while (0)
#else
#define JTHREAD_LONGJMP(buf, val)	longjmp((buf), (val))
#endif
#endif /* !defined(JTHREAD_LONGJMP) */

#ifndef JTHREAD_JMPBUF
#define JTHREAD_JMPBUF			jmp_buf
#endif

#else

#ifndef JTHREAD_SETJMP
#define JTHREAD_SETJMP(buf)		sigsetjmp((buf), 0)
#endif

#ifndef JTHREAD_LONGJMP
#define JTHREAD_LONGJMP(buf, val)	siglongjmp((buf), (val))
#endif

#ifndef JTHREAD_JMPBUF
#define JTHREAD_JMPBUF			sigjmp_buf
#endif

#endif

#endif
