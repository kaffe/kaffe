/*
 * gc2.c
 * This interfaces the VM to the Hans-J. Boehm Incremental Garbage
 * Collector (version 6.0 alpha 7).
 *
 * Copyright (c) 2001
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *      The Kaffe.org's developers. All rights reserved. 
 *      See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "gtypes.h"
#include "kaffe/jni_md.h"
#include "access.h"
#include "classMethod.h"
#include "locks.h"
#include "errors.h"
#include "boehm/include/gc.h"
#include "boehm/include/gc_mark.h"
#include "java_lang_Thread.h"
#include "debug.h"
#include "thread.h"

/*
 * This record describes an allocation type.
 */
typedef struct _gcFuncs {
        walk_func_t             walk;
        final_func_t            final;
        destroy_func_t          destroy;
	const char*		description;
        int                     nr;		/* only used ifdef STATS */
        int                     mem;		/* only used ifdef STATS */
} gcFuncs;

static iStaticLock	gcman_lock;
static iStaticLock	finman_lock;
static iStaticLock	gc_lock;			/* allocator mutex */
static jboolean finalRunning = false;
static int gcRunning = 0;
static int gcDisabled = 0;
static int gc_init = 0;
static Hjava_lang_Thread* finalman;
static Hjava_lang_Thread* garbageman;
static void (*walkRootSet)(Collector*);

#define GCSTACKSIZE             16384
#define FINALIZERSTACKSIZE      THREADSTACKSIZE

typedef struct
{
  struct _Collector collector;
} BoehmGarbageCollector;

static BoehmGarbageCollector boehm_gc;

#define SYSTEM_SIZE(s) ((s) + sizeof(int))
#define USER_SIZE(s) ((s) - sizeof(int))
#define ALIGN_FORWARD(p) ((void *)((uintp)(p) + sizeof(int)))
#define ALIGN_BACK(p) ((void *)((uintp)(p) - sizeof(int)))

static
void*
GC_malloc_atomic_and_clear(size_t sz)
{
  void* mem;
  //  mem = GC_malloc_atomic(sz);
  mem = GC_malloc(sz);
  if (mem != 0) {
    memset(mem, 0, sz);
  }
  return (mem);
}

static
void*
GC_malloc_atomic_uncollectable_and_clear(size_t sz)
{
  void* mem;
  //  mem = GC_malloc_atomic_uncollectable(sz);
  mem = GC_malloc_uncollectable(sz);
  if (mem != 0) {
    memset(mem, 0, sz);
  }
  return (mem);
}

/* =====================================================================
 * GC Type handling
 * ---------------------------------------------------------------------
 */
static gcFuncs gcFunctions[KGC_ALLOC_MAX_INDEX];

/*
 * register an allocation type under a certain index
 * NB: we could instead return a pointer to the record describing the
 * allocation type.  This would give us more flexibility, but it wouldn't
 * allow us to use compile-time constants.
 */
static void
registerTypeByIndex(gc_alloc_type_t idx, walk_func_t walk, final_func_t final,
		    destroy_func_t destroy,
		    const char *description)
{
  /* once only, please */
  assert (gcFunctions[idx].description == 0);
  /* don't exceed bounds */
  assert (idx < sizeof(gcFunctions)/sizeof(gcFunctions[0]));
  
  gcFunctions[idx].walk = walk;
  gcFunctions[idx].final = final;
  gcFunctions[idx].destroy = destroy;
  gcFunctions[idx].description = description;
}

/*
 * Register a fixed allocation type.  The only reason we tell them apart
 * is for statistical purposes.
 */
static void
KaffeGC_RegisterFixedTypeByIndex
         (Collector* gcif UNUSED, 
	  gc_alloc_type_t idx, const char *description)
{
  registerTypeByIndex(idx, 0, KGC_OBJECT_FIXED, 0, description);
}

/*
 * Register a allocation type that is subject to gc.  
 */
static void
KaffeGC_RegisterGcTypeByIndex
        (Collector* gcif UNUSED,
	 gc_alloc_type_t idx, walk_func_t walk, final_func_t final,
	 destroy_func_t destroy,
	 const char *description)
{
  registerTypeByIndex(idx, walk, final, destroy, description);
}

/* =====================================================================
 * Finalizer
 * ---------------------------------------------------------------------
 */

static
void
finalizeObject(void* ob, void* descriptor)
{
  gcFuncs *func = (gcFuncs *)descriptor;

  func->final(&boehm_gc.collector, ALIGN_FORWARD(ob));
}

static void NONRETURNING
finaliserMan(void* arg UNUSED)
{
  int iLockRoot;

  for (;;) {
    lockStaticMutex(&finman_lock);
    
    finalRunning = false;
    while (finalRunning == false) {
      waitStaticCond(&finman_lock, (jlong)0);
    }
    assert(finalRunning == true);

    GC_invoke_finalizers();

    /* Wake up anyone waiting for the finalizer to finish */
    broadcastStaticCond(&finman_lock);
    unlockStaticMutex(&finman_lock);
  }
}

/*
 * Explicity invoke the garbage collector and wait for it to complete.
 */
static
void
KaffeGC_InvokeGC(Collector* gcif UNUSED, int mustgc)
{
  int iLockRoot;

  lockStaticMutex(&gcman_lock);
  if (gcRunning == 0) {
    gcRunning = mustgc ? 2 : 1;
    if (!gcDisabled)
      signalStaticCond(&gcman_lock);
  }
  unlockStaticMutex(&gcman_lock);

  lockStaticMutex(&gcman_lock);
  while (gcRunning != 0) {
    waitStaticCond(&gcman_lock, (jlong)0);
  }
  unlockStaticMutex(&gcman_lock);
}

static
void
KaffeGC_SignalFinalizer(void)
{
  int iLockRoot;

  lockStaticMutex(&finman_lock);
  if (finalRunning == false) {
    finalRunning = true;
    signalStaticCond(&finman_lock);
  }
  unlockStaticMutex(&finman_lock);
}

static
void
KaffeGC_InvokeFinalizer(Collector *gcif)
{
  KaffeGC_InvokeGC(gcif, 1);
  KaffeGC_SignalFinalizer();
}

static void*
KaffeGC_realloc(Collector *gcif, void* mem, int sz, gc_alloc_type_t type)
{
  int iLockRoot;
  void *new_ptr;

  if (mem == NULL)
    return KGC_malloc(gcif, sz, type);
    
  lockStaticMutex(&gc_lock);
  new_ptr = GC_REALLOC ( ALIGN_BACK(mem), (size_t)SYSTEM_SIZE(sz));
  unlockStaticMutex(&gc_lock);

  return (void *)ALIGN_FORWARD(new_ptr);
}

static void
KaffeGC_free(Collector *gcif UNUSED, void* mem)
{
  int iLockRoot;

  if (mem == NULL)
    return;

  lockStaticMutex(&gc_lock);
  GC_FREE(ALIGN_BACK(mem));
  unlockStaticMutex(&gc_lock);
}

static void*
KaffeGC_malloc(Collector *gcif UNUSED, size_t sz, int type)
{
  void* mem;
  int iLockRoot;

  assert(gc_init != 0);
  assert(gcFunctions[type].description != NULL);
  assert(sz != 0);

  lockStaticMutex(&gc_lock);

  // Allocate memory
  if (gcFunctions[type].final == KGC_OBJECT_FIXED)
    mem = GC_malloc_atomic_uncollectable_and_clear(SYSTEM_SIZE(sz));
  else
    mem = GC_malloc_atomic_and_clear(SYSTEM_SIZE(sz));
	  
  // Attach finalizer
  if (mem != 0) {
    if (gcFunctions[type].final != KGC_OBJECT_FIXED && 
	gcFunctions[type].final != KGC_OBJECT_NORMAL) {
      GC_REGISTER_FINALIZER_NO_ORDER(mem, finalizeObject, &gcFunctions[type], 0, 0);
    }
  }

  unlockStaticMutex(&gc_lock);

  *((int *)mem) = type;

  return ALIGN_FORWARD(mem);
}

static uint32
KaffeGC_GetObjectSize(Collector *gcif UNUSED, const void* mem)
{
  if (mem == NULL)
    return 0;

  return USER_SIZE(GC_size(GC_base(mem)));
}

static void*
KaffeGC_GetObjectBase(Collector *gcif UNUSED, const void* mem)
{
  void *position;

  if (mem == NULL || (position = GC_base(mem)) == 0)
    return NULL;

  return ALIGN_FORWARD(position);
}

static int 
KaffeGC_GetObjectIndex(Collector *gcif UNUSED, const void *mem)
{
  int *idx = (int *)GC_base(mem);
  
  if (idx != NULL)
    return *idx;
  else
    return -1;
}

static const char*
KaffeGC_GetObjectDescription(Collector* gcif, const void* mem)
{
  int idx = KaffeGC_GetObjectIndex(gcif, mem);

  if (idx >= 0)
    return gcFunctions[KaffeGC_GetObjectIndex(gcif, mem)].description;
  else
    return NULL;
}

static
void
KaffeGC_Init(Collector *collector)
{
  gc_init = 1;
  GC_finalizer_notifier = KaffeGC_SignalFinalizer;
  GC_java_finalization = 1;
  GC_finalize_on_demand = 1;
  GC_set_max_heap_size(Kaffe_JavaVMArgs.maxHeapSize);
  /*
    GC_init_gcj_malloc(GC_GCJ_RESERVED_MARK_PROC_INDEX, onObjectMarking);
  */
  GC_init();

  if (GC_get_heap_size() < Kaffe_JavaVMArgs.minHeapSize)
    GC_expand_hp( Kaffe_JavaVMArgs.minHeapSize - GC_get_heap_size());
}

/* =====================================================================
 * Garbage Collector thread
 * ---------------------------------------------------------------------
 */

static void NONRETURNING
gcMan(void* arg)
{
  int iLockRoot;

  for (;;) {
    lockStaticMutex(&gcman_lock);
    
    gcRunning = false;
    while (gcRunning == 0) {
      waitStaticCond(&gcman_lock, 0);
    }

    lockStaticMutex(&gc_lock);
    GC_gcollect();
    unlockStaticMutex(&gc_lock);

    /* Wake up anyone waiting for the finalizer to finish */
    broadcastStaticCond(&gcman_lock);
    unlockStaticMutex(&gcman_lock);    
  }
}

static
void
KaffeGC_EnableGC(Collector* gcif UNUSED)
{
  int iLockRoot;

  lockStaticMutex(&gcman_lock);
  gcDisabled -= 1;
  if( gcDisabled == 0 )
    broadcastStaticCond(&gcman_lock);
  unlockStaticMutex(&gcman_lock);
}

static
void
KaffeGC_DisableGC(Collector* gcif UNUSED)
{
  int iLockRoot;

  lockStaticMutex(&gcman_lock);
  gcDisabled += 1;
  unlockStaticMutex(&gcman_lock);
}

/*
 * Start gc threads, which enable collection
 */
static 
void
/* ARGSUSED */
KaffeGC_Enable(Collector* collector)
{
  errorInfo info;

  if (DBGEXPR(NOGC, false, true))
    {
      /* Start the GC daemons we need */
      finalman = createDaemon(&finaliserMan, "finaliser", 
			      collector, THREAD_MAXPRIO,
			      FINALIZERSTACKSIZE, &info);
      garbageman = createDaemon(&gcMan, "gc", 
				collector, THREAD_MAXPRIO,
				GCSTACKSIZE, &info);
      assert(finalman && garbageman);
    }
}

static void
KaffeGC_MarkAddress(Collector* gcif UNUSED, const void* mem)
{
  
}

static void
KaffeGC_MarkObject(Collector* gcif UNUSED, const void* mem)
{
}

static Hjava_lang_Throwable *
KaffeGC_ThrowOOM(Collector* gcif UNUSED)
{
  dprintf(
	  "Throwing OutOfMemoryError is not implemented in Boehm-GC.\n");
  ABORT();
  
  return NULL;
}

static uintp
KaffeGC_HeapLimit(Collector *gcif UNUSED)
{
  return 0;
}

static uintp
KaffeGC_HeapTotal(Collector *gcif UNUSED)
{
  return GC_get_heap_size();
}

/*
 * vtable for object implementing the collector interface.
 */
static struct GarbageCollectorInterface_Ops GC_Ops = {
  0,              /* reserved */
  0,              /* reserved */
  0,              /* reserved */
  KaffeGC_malloc,
  KaffeGC_realloc,
  KaffeGC_free,
  KaffeGC_InvokeGC,
  KaffeGC_InvokeFinalizer,
  KaffeGC_Init,
  KaffeGC_Enable,
  KaffeGC_MarkAddress,
  KaffeGC_MarkObject,
  KaffeGC_GetObjectSize,
  KaffeGC_GetObjectDescription,
  KaffeGC_GetObjectIndex,
  KaffeGC_GetObjectBase,
  0, /* gcWalkMemory, */
  0, /* gcWalkConservative, */
  KaffeGC_RegisterFixedTypeByIndex,
  KaffeGC_RegisterGcTypeByIndex,
  KaffeGC_ThrowOOM,
  KaffeGC_EnableGC,
  KaffeGC_DisableGC,
  KaffeGC_HeapLimit,
  KaffeGC_HeapTotal
};

/*
 * Initialise the Garbage Collection system.
 */
Collector* 
createGC(void (*_walkRootSet)(Collector*))
{
  walkRootSet = _walkRootSet;
  boehm_gc.collector.ops = &GC_Ops;

  return (&boehm_gc.collector);
}

