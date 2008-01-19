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
#include "locks.h"

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
  unsigned int          allRefSize;
  unsigned short        keep_object;
  bool                  destroyed;
  void ***              allRefs;
  struct _weakRefObject *next;
} weakRefObject;

typedef struct _weakRefTable {
  weakRefObject*               hash[REFOBJHASHSZ];
} weakRefTable;

static strongRefTable			strongRefObjects;
static weakRefTable                     weakRefObjects;
static iStaticLock                      strongRefLock;
static iStaticLock                      weakRefLock;

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

  DBG(REFERENCE, dprintf("Adding persistent reference for object %p\n",
                 mem); );

  idx = REFOBJHASH(mem);
  for (obj = strongRefObjects.hash[idx]; obj != 0; obj = obj->next) {
    /* Found it - just increase reference */
    if (obj->mem == mem) {
      obj->ref++;
      return true;
    }
  }

  /* Not found - create a new one */
  obj = (strongRefObject*)KGC_malloc(collector, sizeof(strongRefObject), KGC_ALLOC_REF);
  if (!obj)
    return false;
	
  obj->mem = mem;
  obj->ref = 1;

  lockStaticMutex(&strongRefLock);
  obj->next = strongRefObjects.hash[idx];
  strongRefObjects.hash[idx] = obj;
  unlockStaticMutex(&strongRefLock);
  return true;
}

/*
 * Remove a persistent reference to an object.  If the count becomes
 * zero then the reference is removed.
 */
bool
KaffeGC_rmRef(Collector *collector, void* mem)
{
  uint32 idx;
  strongRefObject** objp;
  strongRefObject* obj;

  DBG(REFERENCE, dprintf("Removing persistent reference for object %p\n",
                 mem); );

  idx = REFOBJHASH(mem);
  lockStaticMutex(&strongRefLock);
  for (objp = &strongRefObjects.hash[idx]; *objp != 0; objp = &obj->next) {
    obj = *objp;
    /* Found it - just decrease reference */
    if (obj->mem == mem) {
      obj->ref--;
      if (obj->ref == 0) {
	*objp = obj->next;
	KGC_free(collector, obj);
      }
      unlockStaticMutex(&strongRefLock);
      return true;
    }
  }

  unlockStaticMutex(&strongRefLock);
  /* Not found!! */
  return false;
}

/**
 * Grow the weak reference list for a weakly referenced object.
 * Assert: weakRefLock is held by the calling thread.
 */
static bool
resizeWeakReferenceObject(Collector *collector, weakRefObject *obj, unsigned int size)
{
  unsigned int previousSize;
  void ***refs, ***oldRefs;

  if (size == 0)
    {
      obj->allRefSize = 0;
      oldRefs = obj->allRefs;
      obj->allRefs = NULL;

      unlockStaticMutex(&weakRefLock);
      KGC_free(collector, oldRefs);
      lockStaticMutex(&weakRefLock);
      return true;
    }

  obj->keep_object++;
  do
    {
      previousSize = obj->allRefSize;
      unlockStaticMutex(&weakRefLock);
      refs = KGC_malloc(collector, size * sizeof(void **), KGC_ALLOC_VMWEAKREF);
      lockStaticMutex(&weakRefLock);
      if (refs == NULL)
	{
	  obj->keep_object--;
	  return false;
	}

      /* Check that nothing has changed. */
      if (previousSize != obj->allRefSize)
	{
	  unlockStaticMutex(&weakRefLock);
	  KGC_free(collector, refs);
	  lockStaticMutex(&weakRefLock);
	  continue;
	}

      obj->allRefSize = size;

      oldRefs = obj->allRefs;
      obj->allRefs = refs;
      if (oldRefs != NULL)
	{
	  memcpy(refs, oldRefs, sizeof(void **) * obj->ref);
      
	  unlockStaticMutex(&weakRefLock);
	  KGC_free(collector, oldRefs);
	  lockStaticMutex(&weakRefLock);
	}

      obj->keep_object--;
      return true;
    }
  while (1);
}

static weakRefObject *
findWeakRefObject(void *mem)
{
  int idx;
  weakRefObject* obj;

  idx = REFOBJHASH(mem);

  for (obj = weakRefObjects.hash[idx]; obj != 0; obj = obj->next) {
    /* Found it! */
    if (obj->mem == mem)
      return obj;
  }

  return NULL;
}     

static bool
insertInWeakRef(Collector *collector, weakRefObject *obj, void **refobj)
{
  obj->ref++;
  
  if (obj->ref >= obj->allRefSize)
    if (!resizeWeakReferenceObject(collector, obj, obj->ref * 2 + 1))
      return false;
  
  obj->allRefs[obj->ref-1] = refobj;

  return true;
}

bool
KaffeGC_addWeakRef(Collector *collector, void* mem, void** refobj)
{
  weakRefObject* obj, *obj2;
  int idx;

  DBG(REFERENCE, dprintf("Adding weak reference for object %p\n",
                 mem); );

  lockStaticMutex(&weakRefLock);
  obj = findWeakRefObject(mem);
  if (obj != NULL)
    {
      bool ret = insertInWeakRef(collector, obj, refobj);
      
      unlockStaticMutex(&weakRefLock);
      return ret;
    }

  /* Not found - create a new one */
  unlockStaticMutex(&weakRefLock);
  obj = (weakRefObject*)KGC_malloc(collector, sizeof(weakRefObject), KGC_ALLOC_VMWEAKREF);
  if (obj == NULL)
    return false;

  obj->mem = mem;
  obj->ref = 1;
  obj->allRefs = (void ***)KGC_malloc(collector, sizeof(void ***), KGC_ALLOC_VMWEAKREF);
  lockStaticMutex(&weakRefLock);
  obj->allRefs[0] = refobj;

  /* Now we check whether has inserted the reference
   * in the meantime.
   */
  obj2 = findWeakRefObject(mem);
  if (obj2 != NULL)
    {
      bool ret;
      
      /* Calling free is safe as the GC thread is not
       * called at that time.
       */
      KGC_free(collector, obj->allRefs);
      KGC_free(collector, obj);

      ret = insertInWeakRef(collector, obj2, refobj);
      unlockStaticMutex(&weakRefLock);

      return ret;
    }

  idx = REFOBJHASH(mem);
  obj->next = weakRefObjects.hash[idx];
  weakRefObjects.hash[idx] = obj;
  unlockStaticMutex(&weakRefLock);

  return true;
}

bool
KaffeGC_rmWeakRef(Collector *collector, void* mem, void** refobj)
{
  uint32 idx;
  weakRefObject** objp;
  weakRefObject* obj;
  unsigned int i;


  DBG(REFERENCE, dprintf("Removing weak reference for object %p \n",
                 mem); );

  idx = REFOBJHASH(mem);

  lockStaticMutex(&weakRefLock);

  for (objp = &weakRefObjects.hash[idx]; *objp != 0; objp = &obj->next) {
    obj = *objp;
    /* Found it - just decrease reference */
    if (obj->mem == mem)
      {
	bool found = false;

	for (i = 0; i < obj->ref; i++)
	  {
	    if (obj->allRefs[i] == refobj)
	      {
		memcpy(&obj->allRefs[i], &obj->allRefs[i+1], sizeof(obj->allRefs[0]) * (obj->ref - i));
		obj->ref--;
		found = true;
		break;
	      }
	  }

	if (obj->ref == 0) {
	  if (!obj->destroyed)
	    *objp = obj->next;

	  obj->next = NULL;
	  obj->destroyed = true;
	  
	  unlockStaticMutex(&weakRefLock);
	  if (obj->allRefs != NULL)
	    KGC_free(collector, obj->allRefs);
	  KGC_free(collector, obj);
	  lockStaticMutex(&weakRefLock);
	}
	unlockStaticMutex(&weakRefLock);
	return found;
      }
  }

  unlockStaticMutex(&weakRefLock);

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

  DBG(REFERENCE, dprintf("Clearing all weak references for object %p\n",
                 mem); );

  idx = REFOBJHASH(mem);

  lockStaticMutex(&weakRefLock);
  for (objp = &weakRefObjects.hash[idx]; *objp != 0; objp = &obj->next)
    {
      obj = *objp;
      /* Found it - clear all references attached to it. */
      if (obj->mem == mem)
	{
	  for (i = 0; i < obj->ref; i++)
	    *(obj->allRefs[i]) = NULL;
	  obj->ref = 0;

	  if (obj->allRefs != NULL)
	    {
	      KGC_free(collector, obj->allRefs);
	      obj->allRefs = NULL;
	    }

	  obj->allRefSize = 0;

	  if (!obj->destroyed)
	    *objp = obj->next;
	  obj->next = NULL;
	  obj->destroyed = true;
	  if (obj->keep_object == 0)
	    KGC_free(collector, obj);
	  
	  unlockStaticMutex(&weakRefLock);
	  return;
	}
    }
  unlockStaticMutex(&weakRefLock);
}

void KaffeGC_initRefs()
{
  initStaticLock(&strongRefLock);
  initStaticLock(&weakRefLock);
}

void
KaffeGC_markAllRefs(Collector* collector)
{
  int i;
  strongRefObject* robj;
  
  /* Walk the referenced objects */
  for (i = 0; i < REFOBJHASHSZ; i++) {
    for (robj = strongRefObjects.hash[i]; robj != 0; robj = robj->next) {
      KGC_markObject(collector, NULL, robj->mem);
    }
  }
}
