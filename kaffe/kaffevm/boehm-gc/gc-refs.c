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
#include "gc2.h"

#define	REFOBJHASHSZ	128
typedef struct _strongRefObject {
  const void*		mem;
  unsigned int		ref;
  struct _strongRefObject* next;
} strongRefObject;

typedef struct _strongRefTable {
  strongRefObject*		hash[REFOBJHASHSZ];
} strongRefTable;

typedef struct _weakRefObject {
  const void *          mem;
  unsigned int          ref;
  void ***              allRefs;
  struct _weakRefObject *next;
} weakRefObject;

typedef struct _weakRefTable {
  weakRefObject*               hash[REFOBJHASHSZ];
} weakRefTable;

static strongRefTable			strongRefObjects;
static weakRefTable                     weakRefObjects;

/* This is a bit homemade.  We need a 7-bit hash from the address here */
#define	REFOBJHASH(V)	((((uintp)(V) >> 2) ^ ((uintp)(V) >> 9))%REFOBJHASHSZ)

/*
 * Add a persistent reference to an object.
 */
bool
KaffeGC_addRef(Collector *collector, const void* mem)
{
  uint32 idx;
  strongRefObject* obj;

  idx = REFOBJHASH(mem);
  for (obj = strongRefObjects.hash[idx]; obj != 0; obj = obj->next) {
    /* Found it - just increase reference */
    if (obj->mem == mem) {
      obj->ref++;
      return true;
    }
  }

  /* Not found - create a new one */
  obj = (strongRefObject*)GC_malloc_uncollectable(sizeof(strongRefObject));
  if (!obj)
    return false;
	
  obj->mem = ALIGN_BACKWARD(mem);
  obj->ref = 1;
  obj->next = strongRefObjects.hash[idx];
  strongRefObjects.hash[idx] = obj;
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
  strongRefObject** objp;
  strongRefObject* obj;

  idx = REFOBJHASH(mem);
  mem = ALIGN_BACKWARD(mem);

  for (objp = &strongRefObjects.hash[idx]; *objp != 0; objp = &obj->next) {
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

bool
KaffeGC_addWeakRef(Collector *collector, void* mem, void** refobj)
{
  int idx;
  weakRefObject* obj;

  idx = REFOBJHASH(mem);
  for (obj = weakRefObjects.hash[idx]; obj != 0; obj = obj->next) {
    /* Found it - just register a new weak reference */
    if (obj->mem == mem) {
      void ***newRefs;
      obj->ref++;

      newRefs = (void ***)GC_malloc_uncollectable(sizeof(void ***)*obj->ref);
      memcpy(newRefs, obj->allRefs, sizeof(void ***)*(obj->ref-1));
      GC_free(obj->allRefs);

      obj->allRefs = newRefs;
      obj->allRefs[obj->ref-1] = refobj;
      return true;
    }
  }

  /* Not found - create a new one */
  obj = (weakRefObject*)GC_malloc_uncollectable(sizeof(weakRefObject));
  if (!obj)
    return false;

  obj->mem = mem;
  obj->ref = 1;
  obj->allRefs = (void ***)GC_malloc_uncollectable(sizeof(void ***));
  obj->allRefs[0] = refobj;
  obj->next = weakRefObjects.hash[idx];
  weakRefObjects.hash[idx] = obj;
  return true;
}

bool
KaffeGC_rmWeakRef(Collector *collector, void* mem, void** refobj)
{
  uint32 idx;
  weakRefObject** objp;
  weakRefObject* obj;
  unsigned int i;

  idx = REFOBJHASH(mem);
  for (objp = &weakRefObjects.hash[idx]; *objp != 0; objp = &obj->next) {
    obj = *objp;
    /* Found it - just decrease reference */
    if (obj->mem == mem)
      {
	for (i = 0; i < obj->ref; i++)
	  {
	    if (obj->allRefs[i] == refobj)
	      {
		void ***newRefs;
		
		obj->ref--;
		newRefs = (void ***)GC_malloc_uncollectable(sizeof(void ***)*obj->ref);
		memcpy(newRefs, obj->allRefs, i*sizeof(void ***));
		memcpy(&newRefs[i], &obj->allRefs[i+1], obj->ref*sizeof(void ***));
		GC_free(obj->allRefs);
		obj->allRefs = newRefs;
		break;
	      }
	  }
	if (i == obj->ref)
	  return false;
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

/**
 * This function clear all weak references to the specified object. 
 * The references are then removed from the database.
 *
 * @param collector a garbage collector instance.
 * @param mem a valid memory object.
 */
void
KaffeGC_clearWeakRef(Collector *collector, void* mem)
{ 
  uint32 idx;
  weakRefObject** objp;
  weakRefObject* obj;
  unsigned int i;

  idx = REFOBJHASH(mem);
  for (objp = &weakRefObjects.hash[idx]; *objp != 0; objp = &obj->next)
    {
      obj = *objp;
      /* Found it - clear all references attached to it. */
      if (obj->mem == mem)
	{
	  for (i = 0; i < obj->ref; i++)
	    *(obj->allRefs[i]) = NULL;
	  GC_free(obj->allRefs);

	  *objp = obj->next;
	  GC_free(obj);
	  return;
	}
    }
}
