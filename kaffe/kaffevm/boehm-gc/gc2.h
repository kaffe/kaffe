/*
 * gc2.h
 * This interfaces the VM to the Hans-J. Boehm Incremental Garbage
 * Collector (version 6.0 alpha 7).
 * Copyright (c) 2004
 *      The Kaffe.org's developers. All rights reserved. 
 *      See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef _KAFFE_BOEHM_GC_H
#define _KAFFE_BOEHM_GC_H

#include "defs.h"
#include "boehm/include/gc.h"
#include "boehm/include/gc_mark.h"

typedef struct _gcMark {
  struct GC_ms_entry *mark_current;
  struct GC_ms_entry *mark_limit;
  GC_PTR original_object;
} gcMark;

typedef struct {
	uint32 magic;
	uint8 memtype;
	size_t memsize;
} MemDescriptor;

#define SIZEOF_DESC (((sizeof(MemDescriptor) + ALIGNMENTOF_VOIDP - 1) / ALIGNMENTOF_VOIDP) * ALIGNMENTOF_VOIDP)

#define SYSTEM_SIZE(s) ((s) + SIZEOF_DESC)
#define USER_SIZE(s) ((s) - SIZEOF_DESC)
#define ALIGN_FORWARD(p) ((void *)((uintp)(p) + SIZEOF_DESC))
#define ALIGN_BACKWARD(p) ((void *)((uintp)(p) - SIZEOF_DESC))

#endif
