/*
 * i386/win32/md.h
 * WIN32 i386 configuration information.
 *
 * Copyright (c) 1996, 1997, 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003
 *      Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __i386_win32_md_h
#define __i386_win32_md_h

#undef	NEED_sysdepCallMethod

#include "i386/common.h"
#include "i386/threads.h"

#include "support.h"

#if !defined(EXTERN_C)
#define EXTERN_C extern
#endif

EXTERN_C void* _currentFrame(void);
#undef	STACK_CURRENT_FRAME
#define	STACK_CURRENT_FRAME()	_currentFrame() /* Provided by WIN32-MD.ASM */

#if defined(TRANSLATOR)
#include "jit-md.h"
#endif

#define	TRAMPOLINE_FUNCTION()	/* Provided by WIN32-MD.ASM */

extern void _sysdepCallMethod(void*);

static inline void sysdepCallMethod(callMethodInfo *call) {
	/* Provided by WIN32-MD.ASM */
	return _sysdepCallMethod((void*)(call));
}

extern int _compareAndExchange(void*, void*, void*);

#undef COMPARE_AND_EXCHANGE
#define	COMPARE_AND_EXCHANGE(A,O,N) _compareAndExchange((void*)(A),(void*)(O),(void*)(N))

#define	PATHSEP		";"
#define	DIRSEP		"\\"

/*
 * For MS C++ the alignment is 8 bytes rather than the Unix 4
 */
#undef ALIGNMENT_OF_SIZE
#define	ALIGNMENT_OF_SIZE(S)	((S) < 8 ? (S) : 8)

/*
 * We don't support UNIX style exceptions.
 */
#define	NO_UNIX_EXCEPTIONS	1

/* since we have no SEGV or DIV handler in SMX, check manually */
#define CREATE_NULLPOINTER_CHECKS 1
#define CREATE_DIVZERO_CHECKS 1

#endif
