/*
 * x86_64/linux/md.c
 * Linux X86-64 specific functions.
 *
 * Copyright (c) 2002
 *	MandrakeSoft.  All rights reserved.
 *
 * Copyright (C) 2000
 *  Silicon Graphics, Inc.  All Rights Reserved.
 *  IA64_context_{save,restore} functions from State Threads Library
 *
 * Copyright (c) 2001
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "config.h"
#include <stdlib.h>

void
init_md(void)
{
#if defined(M_MMAP_MAX) && defined(HAVE_MALLOPT)
	mallopt(M_MMAP_MAX, 0);
#endif
}
