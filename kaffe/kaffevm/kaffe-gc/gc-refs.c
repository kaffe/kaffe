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
#include "gc-incremental.h"
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
      refs = KGC_malloc(collector, size * sizeof(void **), KGC_ALLOC_REF);
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

bool
KaffeGC_addWeakRef(Collector *collector, void* mem, void** refobj)
{
  int idx;
  weakRefObject* obj;

  idx = REFOBJHASH(mem);

  lockStaticMutex(&weakRefLock);
  for (obj = weakRefObjects.hash[idx]; obj != 0; obj = obj->next) {
    /* Found it - just register a new weak reference */
    if (obj->mem == mem) {
      obj->ref++;

      if (obj->ref >= obj->allRefSize)
	if (!resizeWeakReferenceObject(collector, obj, obj->ref * 2 + 1))
	  {
	    unlockStaticMutex(&weakRefLock);
	    return false;
	  }

      obj->allRefs[obj->ref-1] = refobj;

      unlockStaticMutex(&weakRefLock);
      return true;
    }
  }

  /* Not found - create a new one */
  obj = (weakRefObject*)KGC_malloc(collector, sizeof(weakRefObject), KGC_ALLOC_REF);
  if (obj == NULL)
    {
      unlockStaticMutex(&weakRefLock);
      return false;
    }

  obj->mem = mem;
  obj->ref = 1;
  unlockStaticMutex(&weakRefLock);
  obj->allRefs = (void ***)KGC_malloc(collector, sizeof(void ***), KGC_ALLOC_REF);
  lockStaticMutex(&weakRefLock);
  obj->allRefs[0] = refobj;
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

/*
 * Walk the thread's internal context.
 * This is invoked by the garbage collector thread, which is not
 * stopped.
 *      
 * We will iterate through all threads, including the garbage collector
 * and those threads that haven't been started yet.
 */     
static void    
TwalkThread(Collector* collector, jthread_t jtid)
{       
  void *from;
  unsigned len;  
        
  /* Don't walk the gc thread's stack.  It was not stopped and
   * we hence don't have valid sp information.  In addition, there's
   * absolutely no reason why we should walk it at all.
   */
  if (jtid == KTHREAD(current)()) {
    DBG(JTHREAD,
	dprintf("%p NOT walking jtid %p\n", KTHREAD(current)(), jtid);
	); 
      return;
  }
        
  /* Ask threading system what the interesting stack range is;
   * If the thread is too young, the threading system will return
   * 0 from extract_stack.  In that case, we don't have walk anything.
   */
  if (KTHREAD(extract_stack)(jtid, &from, &len)) {
    DBG(JTHREAD|DBG_GCWALK,
	dprintf("walking stack of `%s' thread\n", nameThread(KTHREAD(get_data)(jtid)->jlThread));
	);
    /* and walk it if needed */
    KaffeGC_WalkConservative(collector, from, len);
  }
}

static Collector *running_collector;	

/* XXX will be fixed on thread interface gets fixed.
 * The best way to fix this is not to use walkThreads at all.
 *
 * That very much depends on what best means.  If we don't explicitly
 * walk threads while pushing roots, we are stuck calling
 * soft_instanceof in walkObject, which is a big source of overhead.
 */
static void
liveThreadWalker(jthread_t tid, void *private)
{
  Collector *c = (Collector *)private;
  threadData *thread_data = KTHREAD(get_data)(tid);

  /* if the gc is invoked while a new thread is being
   * initialized, we should not make any assumptions
   * about what is stored in its thread_data 
   */
  if (THREAD_DATA_INITIALIZED(thread_data))
    {
      Hjava_lang_VMThread *thread = (Hjava_lang_VMThread *)thread_data->jlThread;
      jnirefs *table;

      KGC_markObject(c, NULL, unhand(thread)->thread);
      KGC_markObject(c, NULL, thread);

      for(table = thread_data->jnireferences; table != NULL; table = table->prev)
	{
	  int i;

	  for (i = 0; i < table->frameSize; i++)
	    if (table->objects[i] != NULL)
	      KGC_markObject(c, NULL, table->objects[i]);
	}
  
      if (thread_data->exceptObj != NULL)
        {
          KGC_markObject(c, NULL, thread_data->exceptObj);
        }
    }

  TwalkThread(c, tid);
}

/*
 * Walk the set of registered root references.  
 * This is invoked at the beginning of each GC cycle. 
 */
void
KaffeGC_walkRefs(Collector* collector)
{
  int i;
  strongRefObject* robj;
  
DBG(GCWALK,
    dprintf("Walking gc roots...\n");
    );

  /* Walk the referenced objects */
  for (i = 0; i < REFOBJHASHSZ; i++) {
    for (robj = strongRefObjects.hash[i]; robj != 0; robj = robj->next) {
      KGC_markObject(collector, NULL, robj->mem);
    }
  }
 
DBG(GCWALK,
    dprintf("Walking live threads...\n");
    );

 running_collector = collector;
 /* Walk the thread objects as the threading system has them
  * registered.  Terminating a thread will remove it from the
  * threading system, and then we won't walk it here anymore
  */
 KTHREAD(walkLiveThreads_r)(liveThreadWalker, collector);
 DBG(GCWALK,
     dprintf("Following references now...\n");
     );
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
