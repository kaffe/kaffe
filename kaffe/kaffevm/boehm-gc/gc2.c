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
#include "boehm/include/gc_gcj.h"
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

#define SYSTEM_SIZE(s) ((s) + sizeof(void *))
#define USER_SIZE(s) ((s) - sizeof(void *))

static
void*
GC_malloc_atomic_and_clear(size_t sz, void *ptr_vtable)
{
  void* mem;
  mem = GC_malloc_atomic(sz);
  if (mem != 0) {
    uint8 *idx = (uint8 *)mem + GC_size(mem) - sizeof(void *);
    memset(mem, 0, sz);

    *((void **)mem) = ptr_vtable;
  }
  return (mem);
}

static
void*
GC_malloc_atomic_uncollectable_and_clear(size_t sz, void *ptr_vtable)
{
  void* mem;
  mem = GC_malloc_atomic_uncollectable(sz);
  if (mem != 0) {
    uint8 *idx = (uint8 *)mem + GC_size(mem) - sizeof(void *);

    memset(mem, 0, sz);
    *((void **)idx) = ptr_vtable;
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

  func->final(&boehm_gc.collector, ob);
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

  char *s;

  s = "signaling the gc man\n";
  write(fileno(stderr), s, strlen(s));

  lockStaticMutex(&gcman_lock);
  if (gcRunning == 0) {
    gcRunning = mustgc ? 2 : 1;
    if (!gcDisabled)
      signalStaticCond(&gcman_lock);
  }
  unlockStaticMutex(&gcman_lock);


  s = "waiting for the end of the gc\n";
  write(fileno(stderr), s, strlen(s));
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
  new_ptr = GC_realloc ( mem, (size_t)SYSTEM_SIZE(sz));
  if (new_ptr) {
    uint8 *idx = (uint8 *)new_ptr + GC_size(new_ptr) - sizeof(void *);
    memset(new_ptr, 0, sz);
    *((void **)idx) = &gcFunctions[type];
  }
  unlockStaticMutex(&gc_lock);

  return new_ptr;
}

static void
KaffeGC_free(Collector *gcif UNUSED, void* mem)
{
  int iLockRoot;

  if (mem == NULL)
    return;

  lockStaticMutex(&gc_lock);
  GC_FREE(mem);
  unlockStaticMutex(&gc_lock);
}

static void*
KaffeGC_malloc(Collector *gcif UNUSED, size_t sz, int type)
{
  void* mem;
  int iLockRoot;

  assert(gcFunctions[type].description != NULL);
  assert(sz != 0);

  lockStaticMutex(&gc_lock);

  // Allocate memory
  if (gcFunctions[type].final == KGC_OBJECT_FIXED)
    mem = GC_malloc_atomic_uncollectable_and_clear(SYSTEM_SIZE(sz), &gcFunctions[type]);
  else
    mem = GC_malloc_atomic_and_clear(SYSTEM_SIZE(sz), &gcFunctions[type]);
	  
  // Attach finalizer
  if (mem != 0) {
    if (gcFunctions[type].final != KGC_OBJECT_FIXED && 
	gcFunctions[type].final != KGC_OBJECT_NORMAL) {
      GC_REGISTER_FINALIZER_NO_ORDER(mem, finalizeObject, &gcFunctions[type], 0, 0);
    }
  }

  unlockStaticMutex(&gc_lock);

  return mem;
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

  if (mem == NULL)
    return NULL;

  return GC_base(mem);
}

static int 
KaffeGC_GetObjectIndex(Collector *gcif UNUSED, const void *mem)
{
  uint8 *idx_ptr = (uint8 *)GC_base(mem);
  void **idx;

  if (idx_ptr != NULL)
    {
      idx = (void **)&idx_ptr[GC_size(idx_ptr) - sizeof(void *)];
      //assert(*idx > gcFunctions && *idx < &gcFunctions[KGC_ALLOC_MAX_INDEX]);
      return (gcFuncs *)(*idx) - gcFunctions;
    }
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

static int KGC_init = 0;

void GC_stop_world()
{
  if (!KGC_init)
    jthread_suspendall();
}

void GC_start_world()
{
  if (!KGC_init)
    jthread_unsuspendall();
}

void GC_stop_init()
{
}

static
void
KaffeGC_Init(Collector *collector)
{
  KGC_init = 1;
  GC_all_interior_pointers = 0;
  GC_finalizer_notifier = KaffeGC_SignalFinalizer;
  GC_java_finalization = 1;
  GC_finalize_on_demand = 1;
  GC_set_max_heap_size(Kaffe_JavaVMArgs.maxHeapSize);
  //  GC_init_gcj_malloc(GC_GCJ_RESERVED_MARK_PROC_INDEX, onObjectMarking);
  GC_init();

  if (GC_get_heap_size() < Kaffe_JavaVMArgs.minHeapSize)
    GC_expand_hp( Kaffe_JavaVMArgs.minHeapSize - GC_get_heap_size());
  KGC_init = 0;
}

/* =====================================================================
 * Garbage Collector thread
 * ---------------------------------------------------------------------
 */

static void NONRETURNING
gcMan(void* arg)
{
  char *s;
  int iLockRoot;

  for (;;) {
    lockStaticMutex(&gcman_lock);
    
    gcRunning = false;
    s = "waiting for a condition\n";
    write(fileno(stderr), s, strlen(s));
    while (gcRunning == 0) {
      waitStaticCond(&gcman_lock, 0);
    }

    s = "entering gcollect\n";
    write(fileno(stderr), s, strlen(s));
    GC_gcollect();

    s = "exiting gcollect\n";
    write(fileno(stderr), s, strlen(s));

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

