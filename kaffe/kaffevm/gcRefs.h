/*
 * gcRefs.h
 * Manage root set for garbage collector
 *
 * Copyright (c) 1996, 1997
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __gcrefs_h
#define __gcrefs_h

#include "gtypes.h"
#include "gc.h"

struct _Collector;
bool gc_add_ref(const void* mem);
bool gc_rm_ref(const void* mem);
void gc_walk_refs(struct _Collector*);

#endif /* __gcrefs_h */
