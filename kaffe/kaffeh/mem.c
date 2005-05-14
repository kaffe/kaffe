/*
 * mem.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "jni_md.h"
#include "kaffe/jmalloc.h"
#include "gtypes.h"
#include "gc.h"
#include "debug.h"
#include "errors.h"

struct _Collector;
struct _errorInfo;

/*
 * Kaffeh version of various memory-related operations.
 */

static void* gcMalloc(struct _Collector*, size_t, gc_alloc_type_t);
static void* gcRealloc(struct _Collector*, void*, size_t, gc_alloc_type_t);
static void  gcFree(struct _Collector*, void*);
static bool gcAddRef(struct _Collector*, const void*);
static bool gcRmRef(struct _Collector*, void*);

/*
 * We use a very simple 'fake' garbage collector interface
 */

struct GarbageCollectorInterface_Ops GC_Ops = {
	NULL,
	NULL,
	NULL,
	gcMalloc,
	gcRealloc,
	gcFree,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	gcAddRef,
	gcRmRef,
	NULL,
	NULL,
};

struct _Collector c = { & GC_Ops }, *main_collector = &c;


/*
 * The real GC malloc zeroes memory, so our malloc does also.
 */
void*
jmalloc(size_t sz)
{
  	void	*p;

	if (!sz) {
		++sz; /* never malloc(0), it may return NULL */
	}

	if ((p = malloc(sz)) == NULL) {
		dprintf("Out of memory.\n");
		exit(1);
	}
	memset(p, 0, sz);
	return(p);
}

void*
jrealloc(void* mem, size_t sz)
{
	return(realloc(mem, sz));
}

void
jfree(void* mem)
{
	free(mem);
}

static void *
gcMalloc(struct _Collector *collector UNUSED, size_t sz, 
	 gc_alloc_type_t type UNUSED)
{
	return(jmalloc(sz));
}

static void *
gcRealloc(struct _Collector *collector UNUSED, void *mem,
	  size_t sz, gc_alloc_type_t type UNUSED)
{
	return(jrealloc(mem, sz));
}

static bool
gcAddRef(struct _Collector* collector UNUSED, const void* mem UNUSED)
{
	return true;
}

static bool
gcRmRef(struct _Collector* collector UNUSED, void* mem UNUSED)
{
	return true;
}

static void  gcFree(struct _Collector *collector UNUSED, void *mem)
{
	jfree(mem);
}

void
postOutOfMemory(struct _errorInfo* einfo UNUSED)
{
	dprintf("Error: kaffeh ran out of memory.\n");
}

void 
postExceptionMessage(struct _errorInfo *e UNUSED, 
	const char *name, const char *msgfmt, ...)
{
	va_list ap;

	dprintf("kaffeh: %s: ", name);

	va_start(ap, msgfmt);
	vfprintf(stderr, msgfmt, ap);
	va_end(ap);

	dprintf("\n");
}
