/*
 * powerpc/linux/md.c
 * Linux PowerPC specific functions.
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
#if defined(HAVE_MALLOPT)
	mallopt(M_MMAP_MAX, 0);
#endif
}

/*
 * Dummy volatile function.
 */
void
thread_dummy(char* s, ...)
{
	volatile int n;
	n = 0;
}
