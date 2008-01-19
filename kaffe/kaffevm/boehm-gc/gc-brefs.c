/* gc-brefs.c
 * Boehm gc wrapper functions for KaffeGC reference implementation.
 *
 * Copyright (c) 2008
 *      Dalibor Topic <robilad@kaffe.org>
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. (GPLv2 or any later version).
 */

#include "gc.h"
#include "gc-refs.h"
#include "gc2.h"
#include "gtypes.h"

/**
 * Call KaffeGC_addRef with corrected object address.
 * 
 * @param collector the garbage collector
 * @param mem the object
 *
 * @return TRUE if a reference could be added, otherwise FALSE.
 */
bool
BoehmGC_addRef(Collector *collector, const void* mem)
{
  return KaffeGC_addRef(collector, ALIGN_BACKWARD(mem));
}

/**
 * Call KaffeGC_rmRef with corrected object address. 
 *
 * @param collector the garbage collector
 * @param mem the object
 *
 * @return TRUE if a reference could be removed, otherwise FALSE.
 */
bool
BoehmGC_rmRef(Collector *collector, void* mem)
{
  return KaffeGC_rmRef(collector, ALIGN_BACKWARD(mem));
}
