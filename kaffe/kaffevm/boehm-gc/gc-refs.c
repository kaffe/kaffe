/* gc-refs.c
 * Maintain set of references in the garbage collector root set.
 * This is independent of the actual collection mechanism
 *
 * Copyright (c) 1996, 1997, 1998, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *      The Kaffe.org's developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "debug.h"
#include "config-std.h"
#include "config-mem.h"
#include "gtypes.h"
#include "gc.h"
#include "locks.h"
#include "thread.h"
#include "jthread.h"
#include "errors.h"
#include "md.h"
#include "gc-refs.h"
#include "java_lang_Thread.h"
#include "boehm/include/gc.h"

#define	REFOBJHASHSZ	128
typedef struct _refObject {
  const void*		mem;
  unsigned int		ref;
  struct _refObject*	next;
} refObject;

typedef struct _refTable {
  refObject*		hash[REFOBJHASHSZ];
} refTable;

static refTable			refObjects;

/* This is a bit homemade.  We need a 7-bit hash from the address here */
#define	REFOBJHASH(V)	((((uintp)(V) >> 2) ^ ((uintp)(V) >> 9))%REFOBJHASHSZ)

/*
 * Add a persistent reference to an object.
 */
bool
KaffeGC_addRef(Collector *collector, const void* mem)
{
  uint32 idx;
  refObject* obj;

  idx = REFOBJHASH(mem);
  for (obj = refObjects.hash[idx]; obj != 0; obj = obj->next) {
    /* Found it - just increase reference */
    if (obj->mem == mem) {
      obj->ref++;
      return true;
    }
  }

  /* Not found - create a new one */
  obj = (refObject*)GC_malloc_uncollectable(sizeof(refObject));
  if (!obj)
    return false;
	
  obj->mem = mem;
  obj->ref = 1;
  obj->next = refObjects.hash[idx];
  refObjects.hash[idx] = obj;
  return true;
}

/*
 * Remove a persistent reference to an object.  If the count becomes
 * zero then the reference is removed.
 */
bool
KaffeGC_rmRef(Collector *collector, const void* mem)
{
  uint32 idx;
  refObject** objp;
  refObject* obj;

  idx = REFOBJHASH(mem);
  for (objp = &refObjects.hash[idx]; *objp != 0; objp = &obj->next) {
    obj = *objp;
    /* Found it - just decrease reference */
    if (obj->mem == mem) {
      obj->ref--;
      if (obj->ref == 0) {
	*objp = obj->next;
	GC_free(obj);
      }
      return true;
    }
  }

  /* Not found!! */
  return false;
}
