/*
 * i386/win32/jit-md.h
 * Win32 i386 JIT configuration information.
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __i386_win32_jit_md_h
#define __i386_win32_jit_md_h

/**/
/* Include common information. */
/**/
#include "i386/jit.h"

extern void _callException(void*, void*, void*);
#undef CALL_KAFFE_EXCEPTION
#define	CALL_KAFFE_EXCEPTION(A, B, C) \
	_callException((void*)((A)->retbp), (void*)((B).handler), (void*)(C))

#endif
