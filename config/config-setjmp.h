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
#define JTHREAD_SETJMP(buf)       setjmp((buf))
#define JTHREAD_LONGJMP(buf, val) longjmp((buf), (val))
#define JTHREAD_JMPBUF            jmp_buf
#else
#define JTHREAD_SETJMP(buf)       sigsetjmp((buf), 0)
#define JTHREAD_LONGJMP(buf, val) siglongjmp((buf), (val))
#define JTHREAD_JMPBUF            sigjmp_buf
#endif

#endif
