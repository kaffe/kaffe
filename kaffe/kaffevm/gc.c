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
#include "config-std.h"
#include "gc.h"

Collector* main_collector;

void*
jmalloc(size_t sz)
{
	return (GC_malloc(main_collector, sz, GC_ALLOC_FIXED));
}

void*
jrealloc(void* mem, size_t sz)
{
	return (GC_realloc(main_collector, mem, sz, GC_ALLOC_FIXED));
}

void
jfree(void* mem)
{
	GC_free(main_collector, mem);
}
