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

/*
 * To select alternate setjmp/longjmp describe bellow, you must add
 * defines in $CPPFLAGS in config.frag or at configure time.  This is
 * required as md.h may be include after config-setjmp.h.  Including
 * md.h in this file will break others dependencies as definition of
 * sysdepCallmethod() macro.
 *
 * - if JTHREAD_JBLEN is non null use kaffe private function.  You
 * must code them in md.c.
 *
 * - if JTHREAD_USE_SIGSETJMP is defined, use sigsetjmp() in place of
 * setjmp().
 *
 */

#if defined(JTHREAD_JBLEN) && (JTHREAD_JBLEN > 0)

typedef void* kaffe_jmp_buf[JTHREAD_JBLEN];
#define JTHREAD_JMPBUF			kaffe_jmp_buf
#define JTHREAD_SETJMP(jb)		kaffe_setjmp(jb)
#define JTHREAD_LONGJMP(jb, val)	kaffe_longjmp(jb, val)

#elif !defined(JTHREAD_USE_SIGSETJMP)
#ifndef JTHREAD_SETJMP
#define JTHREAD_SETJMP(buf)		setjmp((buf))
#endif
#ifndef JTHREAD_LONGJMP
/* Under the OSkit, __FreeBSD__ is defined but we don't use setjmp
 * from the freebsd libc (appearently).  At any rate, the freebsd
 * setjmp.h has been replaced.  */
#if defined(__FreeBSD__) && defined(__i386__) && !defined(_OSKIT_C_SETJMP_H_)
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
