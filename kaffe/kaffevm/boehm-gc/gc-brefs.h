/* gc-brefs.h
 * Boehm gc wrapper functions for KaffeGC reference implementation.
 *
 * Copyright (c) 2008 
 *     Dalibor Topic  <robilad@kaffe.org>
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. (GPLv2 or any later version).
 */

#ifndef KAFFE_GC_BREFS_H
#define KAFFE_GC_BREFS_H

bool BoehmGC_addRef(Collector *collector, const void* mem);
bool BoehmGC_rmRef(Collector *collector, void* mem);

#endif
