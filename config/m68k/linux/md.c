/*
 * m68k/linux/md.c
 * Linux m68k specific functions.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include <malloc.h>

void
init_md(void)
{
#if defined(TRANSLATOR)
	extern int jit_debug;
	if (getenv("KAFFE_JIT_DEBUG"))
		jit_debug = 1;
#endif
#if defined(M_MMAP_MAX) && defined(HAVE_MALLOPT)
	mallopt(M_MMAP_MAX, 0);
#endif
}
