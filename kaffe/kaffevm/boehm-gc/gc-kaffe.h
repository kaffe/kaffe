/*
 * gc_kaffe.h -- Interface to Kaffe's specific Boehm-GC malloc.
 * 
 * Copyright(c) 2004
 *    The Kaffe.org's developers. See ChangeLog for details.
 */
#ifndef _GC_Kaffe_H
#define _GC_Kaffe_H

#include "boehm/include/gc_mark.h"

void * GC_kaffe_malloc(size_t lb);
void GC_kaffe_init(GC_mark_proc proc);

#endif
