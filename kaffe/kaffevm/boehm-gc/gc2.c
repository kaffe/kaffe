/*
 * gc2.c
 * This interfaces the VM to the Hans-J. Boehm Incremental Garbage
 * Collector (version 6.3)
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
#include "jni_md.h"
#include "access.h"
#include "classMethod.h"
#include "locks.h"
#include "errors.h"
#include "boehm/include/gc.h"
#include "boehm/include/gc_mark.h"
#include "gtypes.h"
#include "java_lang_Thread.h"
#include "debug.h"
#include "thread.h"
#include "gc-refs.h"
#include "gc-kaffe.h"
#include "gc2.h"

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
static iStaticLock	gcmanend_lock;
static iStaticLock	finman_lock;
static volatile int finalRunning = -1; 
static volatile int gcRunning = -1;
static volatile int gcDisabled = 0;
static Hjava_lang_Thread* finalman;
static Hjava_lang_Thread* garbageman;

#define GCSTACKSIZE             (1024*1024)
#define FINALIZERSTACKSIZE      (128*1024)

typedef struct
{
  struct _Collector collector;
} BoehmGarbageCollector;

static BoehmGarbageCollector boehm_gc;

#define MAGIC_GC 0xcaffe130

static inline void
clearAndAddDescriptor(void *mem, MemDescriptor *desc)
{
    MemDescriptor *idx = (MemDescriptor *)mem;
    *idx = *desc;
    memset(ALIGN_FORWARD(idx), 0, desc->memsize);
}

static void KaffeGC_InvokeGC(Collector* gcif, int mustgc);
     
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
finalizeObject(void* ob, UNUSED void* descriptor)
{
  MemDescriptor *desc = (MemDescriptor *)ob;
  gcFuncs *f = &gcFunctions[desc->memtype];

  assert(desc->magic == MAGIC_GC);
  
  if (f->final != KGC_OBJECT_NORMAL && f->final != NULL)
    f->final(&boehm_gc.collector, ALIGN_FORWARD(ob));

  KaffeGC_clearWeakRef(&boehm_gc.collector, ALIGN_FORWARD(ob));

  if (f->destroy != NULL)
    f->destroy(&boehm_gc.collector, ALIGN_FORWARD(ob));
}

static void NONRETURNING
finaliserMan(void* arg UNUSED)
{

  lockStaticMutex(&finman_lock);
  for (;;) {
    
    finalRunning = 0;
    while (finalRunning == 0) {
      waitStaticCond(&finman_lock, (jlong)0);
    }
    assert(finalRunning == 1);

    GC_invoke_finalizers();

    /* Wake up anyone waiting for the finalizer to finish */
    broadcastStaticCond(&finman_lock);
  }
  unlockStaticMutex(&finman_lock);
}


static
void
KaffeGC_SignalFinalizer(void)
{

  lockStaticMutex(&finman_lock);
  if (finalRunning == 0) {
    finalRunning = 1;
    signalStaticCond(&finman_lock);
  }
  unlockStaticMutex(&finman_lock);
}

static void
KaffeGC_InvokeFinalizer(Collector *gcif)
{
  KaffeGC_InvokeGC(gcif, 1);
  KaffeGC_SignalFinalizer();
}

/* =====================================================================
 * Garbage Collector
 * ---------------------------------------------------------------------
 */

/*
 * Explicity invoke the garbage collector and wait for it to complete.
 */
static
void
KaffeGC_InvokeGC(Collector* gcif UNUSED, int mustgc)
{

  while (gcRunning < 0)
    KTHREAD(yield)();
  
  lockStaticMutex(&gcman_lock);

  if (gcRunning == 0) {
    gcRunning = mustgc ? 2 : 1;
    if (!gcDisabled)
      signalStaticCond(&gcman_lock);
  }

  lockStaticMutex(&gcmanend_lock);
  unlockStaticMutex(&gcman_lock);
  while (gcRunning != 0) {
    waitStaticCond(&gcmanend_lock, (jlong)0);
  }
  unlockStaticMutex(&gcmanend_lock);
}

static void NONRETURNING
gcMan(UNUSED void* arg)
{

  lockStaticMutex(&gcman_lock);
  gcRunning = 0;
  for (;;) {
    while (gcRunning == 0) {
      waitStaticCond(&gcman_lock, 0);
    }

    GC_gcollect();

    /* Wake up anyone waiting for the finalizer to finish */
    lockStaticMutex(&gcmanend_lock);
    gcRunning = 0;
    broadcastStaticCond(&gcmanend_lock);
    unlockStaticMutex(&gcmanend_lock);
  }
}

static void
KaffeGC_EnableGC(Collector* gcif UNUSED)
{

  lockStaticMutex(&gcman_lock);
  gcDisabled -= 1;
  if( gcDisabled == 0 )
    broadcastStaticCond(&gcman_lock);
  unlockStaticMutex(&gcman_lock);
}

static void
KaffeGC_DisableGC(Collector* gcif UNUSED)
{

  lockStaticMutex(&gcman_lock);
  gcDisabled += 1;
  unlockStaticMutex(&gcman_lock);
}

/* =====================================================================
 * Memory allocation
 * ---------------------------------------------------------------------
 */

static void*
KaffeGC_realloc(Collector *gcif, void* mem, size_t sz, gc_alloc_type_t type)
{
  void *new_ptr;

  if (mem == NULL)
    return KGC_malloc(gcif, sz, type);
    
  new_ptr = GC_realloc ( ALIGN_BACKWARD(mem), (size_t)SYSTEM_SIZE(sz));
  if (new_ptr) {
    MemDescriptor *desc = (MemDescriptor *)new_ptr;
    
    assert(desc->magic == MAGIC_GC);
    if (sz > desc->memsize) {
      memset((void *)((uintp)new_ptr + SYSTEM_SIZE(desc->memsize)), 0, sz-desc->memsize);
    }
    desc->memtype = type;
    desc->memsize = sz;

    return ALIGN_FORWARD(new_ptr);
  }

  return NULL;
}

static void
KaffeGC_free(Collector *gcif UNUSED, void* mem)
{
  MemDescriptor *desc = (MemDescriptor *)ALIGN_BACKWARD(mem);

  if (mem == NULL)
    return;

  assert(desc->magic == MAGIC_GC);
  GC_free(desc);
}

static void*
KaffeGC_malloc(Collector *gcif UNUSED, size_t sz, gc_alloc_type_t type)
{
  void* mem;
  MemDescriptor desc;

  assert(gcFunctions[type].description != NULL);
  assert(sz != 0);

  desc.memtype = type;
  desc.memsize = sz;
  desc.magic = MAGIC_GC;
  // Allocate memory
  if (gcFunctions[type].final == KGC_OBJECT_FIXED)
    {
      if (type == KGC_ALLOC_THREADCTX)
	mem = GC_malloc_uncollectable(SYSTEM_SIZE(sz));
      else
	mem = GC_malloc_atomic_uncollectable(SYSTEM_SIZE(sz));
    }
  else
    mem = GC_kaffe_malloc(SYSTEM_SIZE(sz));

  // Attach finalizer
  if (mem != 0) {
    clearAndAddDescriptor(mem, &desc);
	  
    if ( gcFunctions[type].final != KGC_OBJECT_FIXED
	 && (gcFunctions[type].final != KGC_OBJECT_NORMAL
	     || gcFunctions[type].destroy != NULL)) {
      GC_REGISTER_FINALIZER_NO_ORDER(mem, finalizeObject, 0, 0, 0);
    }
    return ALIGN_FORWARD(mem);
  }

  return NULL;
}


/* =====================================================================
 * Utilities
 * ---------------------------------------------------------------------
 */

static uint32
KaffeGC_GetObjectSize(Collector *gcif UNUSED, const void* mem)
{
  MemDescriptor *desc;

  if (mem == NULL)
    return 0;

  desc = (MemDescriptor *)GC_base((void *)(uintp)mem);
  if (desc == NULL)
    return 0;

  assert(desc->magic == MAGIC_GC);
  return desc->memsize;
}

static void*
KaffeGC_GetObjectBase(Collector *gcif UNUSED, void* mem)
{
  void *ptr;

  if (mem == NULL)
    return NULL;

  ptr = GC_base(mem);
  if (ptr == NULL)
    return NULL;

  return ALIGN_FORWARD(ptr);
}

static int 
KaffeGC_GetObjectIndex(Collector *gcif UNUSED, const void *mem)
{
  MemDescriptor *desc = (MemDescriptor *)GC_base((void *)(uintp)mem);

  if (desc != NULL)
  {
    assert(desc->magic == MAGIC_GC);
    return desc->memtype;
  }
  else
    return -1;
}

static const char*
KaffeGC_GetObjectDescription(Collector* gcif, const void* mem)
{
  int idx = KaffeGC_GetObjectIndex(gcif, mem);

  if (idx >= 0)
    return gcFunctions[idx].description;
  else
    return NULL;
}

static Hjava_lang_Throwable *
KaffeGC_ThrowOOM(Collector* gcif UNUSED)
{
  dprintf(
	  "Throwing OutOfMemoryError is not implemented in Boehm-GC.\n");
  KAFFEVM_ABORT();
  
  return NULL;
}

static uintp
KaffeGC_HeapFree(Collector *gcif UNUSED)
{
  return GC_get_free_bytes();
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

static const char *
KaffeGC_getCollectorName(Collector *gcif UNUSED)
{
  return "boehm-gc";
}

/* =====================================================================
 * Address marking
 * ---------------------------------------------------------------------
 */

static void
KaffeGC_MarkAddress(UNUSED Collector* gcif, void *gc_info, const void* mem)
{
  gcMark *info_mark = (gcMark *)gc_info;

  info_mark->mark_current =
    GC_mark_and_push(ALIGN_BACKWARD(mem), info_mark->mark_current, info_mark->mark_limit,
		     (GC_PTR *) info_mark->original_object);
}

static struct GC_ms_entry *
onObjectMarking(GC_word *addr, struct GC_ms_entry * mark_stack_ptr,
                struct GC_ms_entry * mark_stack_limit, UNUSED GC_word env)
{
  MemDescriptor *desc = (MemDescriptor *)addr;
  int type = desc->memtype;
  gcMark info_mark;
  walk_func_t walkf;
  
  /* Temporary hack. If the magic bytes are not there it means the descriptor has not
   * been initialized. The Boehm-Weiser GC should not call the function but it actually happens
   * we just silently ignore the problem. :(
   */
  if (desc->magic != MAGIC_GC)
    return mark_stack_ptr;
  
  info_mark.mark_current = mark_stack_ptr;
  info_mark.mark_limit = mark_stack_limit;
  info_mark.original_object = addr;

  walkf = gcFunctions[type].walk;
  if (walkf != NULL)
    {
      walkf(&boehm_gc.collector, &info_mark, ALIGN_FORWARD(addr), desc->memsize); 
    }

  return info_mark.mark_current;
}

/* =====================================================================
 * Initialization
 * ---------------------------------------------------------------------
 */

static void
KaffeGC_warnproc(char *msg, GC_word arg)
{
DBG(GCDIAG, 
     dprintf(msg, arg);
    );
}

static void
KaffeGC_Init(Collector *collector UNUSED)
{
  GC_all_interior_pointers = 0;
  GC_finalizer_notifier = KaffeGC_SignalFinalizer;
  GC_java_finalization = 1;
  GC_finalize_on_demand = 1;
  GC_set_warn_proc(KaffeGC_warnproc);
  GC_init();
  GC_set_max_heap_size((size_t)Kaffe_JavaVMArgs.maxHeapSize);

  if (GC_get_heap_size() < (size_t)Kaffe_JavaVMArgs.minHeapSize)
    GC_expand_hp( Kaffe_JavaVMArgs.minHeapSize - GC_get_heap_size());

  GC_kaffe_init(onObjectMarking);
}


/* =====================================================================
 * GC starter
 * ---------------------------------------------------------------------
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

/*
 * vtable for object implementing the collector interface.
 */
static struct GarbageCollectorInterface_Ops GC_Ops = {
  KaffeGC_getCollectorName,              /* reserved */
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
  KaffeGC_MarkAddress,
  KaffeGC_GetObjectSize,
  KaffeGC_GetObjectDescription,
  KaffeGC_GetObjectIndex,
  KaffeGC_GetObjectBase,
  KaffeGC_RegisterFixedTypeByIndex,
  KaffeGC_RegisterGcTypeByIndex,
  KaffeGC_ThrowOOM,
  KaffeGC_EnableGC,
  KaffeGC_DisableGC,
  KaffeGC_HeapFree,
  KaffeGC_HeapLimit,
  KaffeGC_HeapTotal,
  KaffeGC_addRef,
  KaffeGC_rmRef,
  KaffeGC_addWeakRef,
  KaffeGC_rmWeakRef
};

/*
 * Initialise the Garbage Collection system.
 */
Collector* createGC(void)
{
  boehm_gc.collector.ops = &GC_Ops;
  initStaticLock(&gcman_lock);
  initStaticLock(&gcmanend_lock);
  initStaticLock(&finman_lock);

  return (&boehm_gc.collector);
}

