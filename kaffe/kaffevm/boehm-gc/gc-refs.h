/*
 * gc-refs.h
 * Manage root set for the boehm-weiser garbage collector
 *
 * Copyright (c) 2004
 *      The Kaffe.org's developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __gcrefs_h
#define __gcrefs_h

#include "gtypes.h"
#include "gc.h"

struct _Collector;
bool KaffeGC_addRef(struct _Collector *collector, const void* mem);
bool KaffeGC_rmRef(struct _Collector *collector, const void* mem);
bool KaffeGC_addWeakRef(struct _Collector *collector, void *mem, void **obj);
bool KaffeGC_rmWeakRef(struct _Collector *collector, void *mem, void **obj);
void KaffeGC_clearWeakRef(struct _Collector *collector, void *mem);

#endif /* __gcrefs_h */
