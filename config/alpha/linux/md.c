/*
 * alpha/linux/md.c
 * Linux Alpha specific functions.
 *
 * Copyright (c) 2001
 *	Edouard G. Parmelan.  All rights reserved.
 *
 * Copyright (c) 1996, 1997, 2001
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "config.h"
#include <malloc.h>
#include <asm/fpu.h>

void
init_md(void)
{
#if defined(HAVE_MALLOPT)
	/* Turn off the use of mmap in malloc so that we have a
	   continuous heap to present to the gc.  */
	mallopt(M_MMAP_MAX, 0);
#endif
}

#if defined(TRANSLATOR)
#include "alpha/alpha.c"
#endif
