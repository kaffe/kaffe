/*
 * gc.c
 * The garbage collector.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

/* Use the incremental garbage collector */
#include "mem/gc-incremental.c"

void*
jmalloc(size_t sz)
{
	return ((*Kaffe_GarbageCollectorInterface._malloc)(sz, GC_ALLOC_FIXED));
}

void
jfree(void* mem)
{
	(*Kaffe_GarbageCollectorInterface._free)(mem);
}
