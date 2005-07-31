/* gc-incremental.c
 * The garbage collector.
 * The name is misleading.  GC is non-incremental at this point.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003, 2004
 *	Kaffe.org contributors. See ChangeLog for details. All rights reserved.
 *   
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

/* XXX this should be controllable, somehow. */
#define	SUPPORT_VERBOSEMEM

#include "config.h"
#include "debug.h"
#include "config-std.h"
#include "config-mem.h"
#include "defs.h"
#include "gtypes.h"
#include "gc.h"
#include "gc-mem.h"
#include "locks.h"
#include "thread.h"
#include "jthread.h"
#include "errors.h"
#include "md.h"
#include "stats.h"
#include "classMethod.h"
#include "gc-incremental.h"
#include "gc-refs.h"
#include "jvmpi_kaffe.h"

static struct _gcStats {
        uint32  totalmem;
        uint32  totalobj;
        uint32  freedmem;
        uint32  freedobj;
        uint32  markedobj;
        uint32  markedmem;
        uint32  allocobj;
        uint32  allocmem;
        uint32  finalobj;
        uint32  finalmem;
} gcStats;

/* Avoid recursively allocating OutOfMemoryError */
#define OOM_ALLOCATING		((void *) -1)

#define STACK_SWEEP_MARGIN      1024
#define GCSTACKSIZE		16384
#define FINALIZERSTACKSIZE	THREADSTACKSIZE

/*
 * Object implementing the collector interface.
 */
static struct CollectorImpl {
	Collector 	collector;
	/* XXX include static below here for encapsulation */
} gc_obj;

/* XXX don't use these types ! */
static Hjava_lang_Thread* garbageman;
static Hjava_lang_Thread* finalman;

static gcList gclists[6];
static const int nofin_white = 5;
static const int fin_white = 4;
static const int grey = 3;
static const int nofin_black = 2;
static const int fin_black = 1;
static const int finalise = 0;

static int gc_init = 0;
static volatile int gcDisabled = 0;
static volatile int gcRunning = -1;
static volatile bool finalRunning = false;
#if defined(KAFFE_STATS)
static timespent gc_time;
static timespent sweep_time;
static counter gcgcablemem;
static counter gcfixedmem;
#endif /* KAFFE_STATS */

/* Is this pointer within our managed heap? */
#define IS_A_HEAP_POINTER(from) \
        ((uintp) (from) >= gc_get_heap_base() && \
	 (uintp) (from) < gc_get_heap_base() + gc_get_heap_range())

static void *outOfMem;
static void *outOfMem_allocator;

#if defined(SUPPORT_VERBOSEMEM)

static struct {
  ssize_t size;
  int count;
  uint64 total;
} objectSizes[] = {

#define OBJECTSIZE(SZ)  { SZ, 0, 0 },

  /* The smaller sizes should match what's in the
     freelist[] array defined in gc-mem.c */

  OBJECTSIZE(16)
  OBJECTSIZE(24)
  OBJECTSIZE(32)
  OBJECTSIZE(40)
  OBJECTSIZE(48)
  OBJECTSIZE(56)
  OBJECTSIZE(64)
  OBJECTSIZE(80)
  OBJECTSIZE(96)
  OBJECTSIZE(112)
  OBJECTSIZE(128)
  OBJECTSIZE(160)
  OBJECTSIZE(192)
  OBJECTSIZE(224)
  OBJECTSIZE(240)
  OBJECTSIZE(496)
  OBJECTSIZE(1000)
  OBJECTSIZE(2016)
  OBJECTSIZE(4040)
  OBJECTSIZE(8192)
  OBJECTSIZE(12288)
  OBJECTSIZE(16483)
  OBJECTSIZE(32768)
  OBJECTSIZE(65536)
  OBJECTSIZE(131072)
  OBJECTSIZE(262144)
  OBJECTSIZE(524288)
  OBJECTSIZE(1048576)
  OBJECTSIZE(0x7FFFFFFF)

#undef  OBJECTSIZE

  { -1, -1, 0 }

};

static void objectStatsChange(gc_unit*, int);
static void objectStatsPrint(void);
static void objectSizesAdd(size_t);
static void objectSizesPrint(void);

#define	OBJECTSTATSADD(M)	objectStatsChange(M, 1)
#define	OBJECTSTATSREMOVE(M)	objectStatsChange(M, -1)
#define	OBJECTSTATSPRINT()	objectStatsPrint()
#define	OBJECTSIZESADD(S)	objectSizesAdd(S)

#else

#define	OBJECTSTATSADD(M)
#define	OBJECTSTATSREMOVE(M)
#define	OBJECTSTATSPRINT()
#define	OBJECTSIZESADD(S)

#endif

/* For statistics gathering, record how many objects and how 
 * much memory was marked.
 */
static inline
void record_marked(int nr_of_objects, uint32 size)
{       
        gcStats.markedobj += nr_of_objects;
        gcStats.markedmem += size;
} 

static iStaticLock	gcman; 
static iStaticLock	finman;
static iStaticLock	gcmanend;
static iStaticLock	finmanend;
static iStaticLock	gc_lock;	/* allocator mutex */

static void gcFree(Collector* gcif, void* mem);

/* Standard GC function sets.  We call them "allocation types" now */
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
gcRegisterFixedTypeByIndex(Collector* gcif UNUSED, 
	gc_alloc_type_t idx, const char *description)
{
	registerTypeByIndex(idx, NULL, KGC_OBJECT_FIXED, NULL, description);
}

/*
 * Register a allocation type that is subject to gc.  
 */
static void
gcRegisterGcTypeByIndex(Collector* gcif UNUSED,
	gc_alloc_type_t idx, walk_func_t walk, final_func_t final,
	destroy_func_t destroy,
	const char *description)
{
	registerTypeByIndex(idx, walk, final, destroy, description);
}

static void startGC(Collector *gcif);
static void finishGC(Collector *gcif);
static void startFinalizer(void);
static void markObjectDontCheck(gc_unit *unit, gc_block *info, uintp idx);

/* Return true if gc_unit is pointer to an allocated object */
static inline int
gc_heap_isobject(gc_block *info, gc_unit *unit)
{
	uintp p = (uintp) UTOMEM(unit) - gc_get_heap_base();

	if (!(p & (MEMALIGN - 1)) && p < gc_get_heap_range() && GCBLOCKINUSE(info)) {
		/* Make sure 'unit' refers to the beginning of an
		 * object.  We do this by making sure it is correctly
		 * aligned within the block.
		 */
		uint16 idx = GCMEM2IDX(info, unit);
		if (idx < info->nr &&
		    GCBLOCK2MEM(info, idx) == unit &&
		    (KGC_GET_COLOUR(info, idx) & KGC_COLOUR_INUSE) == KGC_COLOUR_INUSE) {
			return 1;
		}
	}
	return 0;
}

static void
markObjectDontCheck(gc_unit *unit, gc_block *info, uintp idx)
{
	/* If the object has been traced before, don't do it again. */
	if (KGC_GET_COLOUR(info, idx) != KGC_COLOUR_WHITE) {
		return;
	}
DBG(GCWALK,	
	dprintf("  marking @%p: %s\n", UTOMEM(unit),
			describeObject(UTOMEM(unit)));
    );

	DBG(GCSTAT,
	    switch (KGC_GET_FUNCS(info, idx)) {
	    case KGC_ALLOC_NORMALOBJECT:
	    case KGC_ALLOC_FINALIZEOBJECT:
	    case KGC_ALLOC_PRIMARRAY:
	    case KGC_ALLOC_REFARRAY: {
		    Hjava_lang_Object *obj;
		    obj = (Hjava_lang_Object *)(unit+1);
		    if (obj->vtable != NULL) {
			    Hjava_lang_Class *c;
			    c = OBJECT_CLASS(obj);
			    if (c)
				    c->live_count++;
		    }
	    default: {
	    }
	    }});
	    

	/* If we found a new white object, mark it as grey and
	 * move it into the grey list.
	 */
	KGC_SET_COLOUR(info, idx, KGC_COLOUR_GREY);
	UREMOVELIST(unit);
	UAPPENDLIST(gclists[grey], unit);
}

/*
 * Mark the memory given by an address if it really is an object.
 */
static void
gcMarkAddress(Collector* gcif UNUSED, void *gc_info UNUSED, const void* mem)
{
	gc_block* info;
	gc_unit* unit;

	/*
	 * First we check to see if the memory 'mem' is in fact the
	 * beginning of an object.  If not we just return.
	 */

	/* Get block info for this memory - if it exists */
	info = gc_mem2block(mem);
	unit = UTOUNIT(mem);
	if (gc_heap_isobject(info, unit)) {
		markObjectDontCheck(unit, info, GCMEM2IDX(info, unit));
	}
}

/*
 * Mark an object.  Argument is assumed to point to a valid object,
 * and never, ever, be null.
 */
static void
gcMarkObject(Collector* gcif UNUSED, void *gc_info UNUSED, const void* objp)
{
  gc_unit *unit = UTOUNIT(objp);
  gc_block *info = gc_mem2block(unit);
  DBG(GCDIAG, assert(gc_heap_isobject(info, unit)));
  markObjectDontCheck(unit, info, GCMEM2IDX(info, unit));
}

void
KaffeGC_WalkConservative(Collector* gcif, const void* base, uint32 size)
{
	const int8* mem;

DBG(GCWALK,	
	dprintf("scanning %d bytes conservatively from %p-%p\n", 
		size, base, ((const char *)base) + size);
    );

	record_marked(1, size);

	if (size > 0) {
		for (mem = ((const int8*)base) + (size & (uintp)-ALIGNMENTOF_VOIDP) - sizeof(void*);
		     (const void*)mem >= base;
		     mem -= ALIGNMENTOF_VOIDP) {
			const void *p = *(void * const *)mem;
			if (p) {
				gcMarkAddress(gcif, NULL, p);
			}
		}
	}
}

/*
 * Like walkConservative, except that length is computed from the block size
 * of the object.  Must be called with pointer to object allocated by gc.
 */
static
uint32
gcGetObjectSize(Collector* gcif UNUSED, const void* mem)
{
	return (GCBLOCKSIZE(gc_mem2block(UTOUNIT(mem))));
}

static
int
gcGetObjectIndex(Collector* gcif UNUSED, const void* mem)
{
	gc_unit* unit = UTOUNIT(mem);
	gc_block* info = gc_mem2block(unit);
	if (!gc_heap_isobject(info, unit)) {
		return (-1);
	} else {
		return (KGC_GET_FUNCS(info, GCMEM2IDX(info, unit)));
	}
}

/*
 * Given a pointer within an object, find the base of the object.
 * This works for both gcable and fixed object types.
 *
 * This method uses many details of the allocator implementation.
 * Specifically, it relies on the contiguous layout of block infos
 * and the way gc_mem2block and GCMEM2IDX are implemented.
 */
static
void*
gcGetObjectBase(Collector *gcif UNUSED, void* mem)
{
	int idx;
	gc_block* info;

	/* quickly reject pointers that are not part of this heap */
	if (!IS_A_HEAP_POINTER(mem)) {
		return (NULL);
	}

	lockStaticMutex(&gc_lock);
	/* the allocator initializes all block infos of a large
	   object using the address of the first page allocated
	   for the large object. Hence, simply using GCMEM2* works
	   even for large blocks
	  */

	info = gc_mem2block(mem);
	idx = GCMEM2IDX(info, mem);

	/* report fixed objects as well */
	if (idx < info->nr && 
	    ((KGC_GET_COLOUR(info, idx) & KGC_COLOUR_INUSE) || 
	     (KGC_GET_COLOUR(info, idx) & KGC_COLOUR_FIXED))) 
	{
	    	mem = UTOMEM(GCBLOCK2MEM(info, idx));
		unlockStaticMutex(&gc_lock);
		return mem;
	}
	unlockStaticMutex(&gc_lock);
	return (NULL);
}

static
const char*
gcGetObjectDescription(Collector* gcif, const void* mem)
{
	return (gcFunctions[gcGetObjectIndex(gcif, mem)].description);
}

/*
 * Walk a bit of memory.
 */
void
KaffeGC_WalkMemory(Collector* gcif, void* mem)
{
	gc_block* info;
	int idx;
	gc_unit* unit;
	uint32 size;
	walk_func_t walkf;

	unit = UTOUNIT(mem);
	info = gc_mem2block(unit);
	idx = GCMEM2IDX(info, unit);

	if (KGC_GET_COLOUR(info, idx) == KGC_COLOUR_BLACK) {
		return;
	}

	UREMOVELIST(unit);

	/* if the object is about to be finalized, put it directly
	 * into the finalise list, otherwise put it into the black
	 * list.
	 */
	if (KGC_GET_STATE(info, idx) == KGC_STATE_INFINALIZE) {
		gcStats.finalobj += 1;
		gcStats.finalmem += GCBLOCKSIZE(info);
		UAPPENDLIST(gclists[finalise], unit);
	} else if (KGC_GET_STATE(info, idx) == KGC_STATE_NEEDFINALIZE) {
		UAPPENDLIST(gclists[fin_black], unit);
	} else {
		UAPPENDLIST(gclists[nofin_black], unit);
	}
	
	KGC_SET_COLOUR(info, idx, KGC_COLOUR_BLACK);

	assert(KGC_GET_FUNCS(info, idx) < 
		sizeof(gcFunctions)/sizeof(gcFunctions[0]));
	size = GCBLOCKSIZE(info);
	record_marked(1, size);
	walkf = gcFunctions[KGC_GET_FUNCS(info, idx)].walk;
	if (walkf != 0) {
DBG(GCWALK,	
		dprintf("walking %d bytes @%p: %s\n", size, mem, 
			describeObject(mem));
    );
		walkf(gcif, NULL, mem, size);
	}
}

#if !(defined(NDEBUG) || !defined(KAFFE_VMDEBUG))
static int
gcClearCounts(Hjava_lang_Class *c, void *_ UNUSED)
{
	c->live_count = 0;
	return 0;
}

static int
gcDumpCounts(Hjava_lang_Class *c, void *_ UNUSED)
{
	if (c->live_count)
		dprintf("%7d %s\n", c->live_count,	c->name->data);
	return 0;
}
#endif /* !(defined(NDEBUG) || !defined(KAFFE_VMDEBUG)) */

/*
 * The Garbage Collector sits in a loop starting a collection, waiting
 * until it's finished incrementally, then tidying up before starting
 * another one.
 */
static void NONRETURNING
gcMan(void* arg)
{
	gc_unit* unit;
	gc_block* info;
	uintp idx;
	Collector *gcif = (Collector*)arg;

	lockStaticMutex(&gcman);
	gcRunning = 0;
	/* Wake up anyone waiting for the GC to finish every time we're done */
	for (;;) {

		while (gcRunning == 0) {
			waitStaticCond(&gcman, (jlong)0);
		}
		/* We have observed that gcRunning went from 0 to 1 or 2 
		 * One thread requested a gc.  We will decide whether to gc
		 * or not, and then we will set gcRunning back to 0 and
		 * inform the calling thread of the change
		 */
		assert(gcRunning > 0);

		/* 
		 * gcRunning will either be 1 or 2.  If it's 1, we can apply
		 * some heuristics for when we skip a collection.
		 * If it's 2, we must collect.  See gcInvokeGC.
		 */

		/* First, since multiple thread can wake us up without 
		 * coordinating with each other, we must make sure that we
		 * don't collect multiple times in a row.
		 */
                if (gcRunning == 1 && gcStats.allocmem == 0) {
			/* XXX: If an application runs out of memory, it may be 
			 * possible that an outofmemory error was raised and the
			 * application in turn dropped some references.  Then
			 * allocmem will be 0, yet a gc would be in order.
			 * Once we implement OOM Errors properly, we will fix 
			 * this; for now, this guards against wakeups by 
			 * multiple threads.
			 */
DBG(GCSTAT,
			dprintf("skipping collection cause allocmem==0...\n");
    );
			goto gcend;
                }

		/*
		 * Now try to decide whether we should postpone the gc and get
		 * some memory from the system instead.
		 *
		 * If we already use the maximum amount of memory, we must gc.
		 *
		 * Otherwise, wait until the newly allocated memory is at 
		 * least 1/4 of the total memory in use.  Assuming that the
		 * gc will collect all newly allocated memory, this would 
		 * asymptotically converge to a memory usage of approximately
		 * 4/3 the amount of long-lived and fixed data combined.
		 *
		 * Feel free to tweak this parameter.
		 * NB: Boehm calls this the liveness factor, we stole the
		 * default 1/4 setting from there.
		 *
		 * XXX: make this a run-time configurable parameter.
		 */
		if (gcRunning == 1 
		    && gc_get_heap_total() < gc_get_heap_limit() 
		    && gcStats.allocmem * 4 < gcStats.totalmem * 1) {
DBG(GCSTAT,
			dprintf("skipping collection since alloc/total "
				"%dK/%dK = %.2f < 1/3\n",
				gcStats.allocmem/1024, 
				gcStats.totalmem/1024,
				gcStats.allocmem/(double)gcStats.totalmem);
    );
			goto gcend;
		}

		DBG(GCSTAT, walkClassPool(gcClearCounts, NULL));
		    
		startGC(gcif);

		/* process any objects found by walking the root references */
		while (gclists[grey].cnext != &gclists[grey]) {
			unit = gclists[grey].cnext;
			KaffeGC_WalkMemory(gcif, UTOMEM(unit));
		}

		/* Now walk any white objects which will be finalized.  They
		 * may get reattached, so anything they reference must also
		 * be live just in case.
		 */
		while (gclists[fin_white].cnext != &gclists[fin_white]) {
			unit = gclists[fin_white].cnext;
			info = gc_mem2block(unit);
			idx = GCMEM2IDX(info, unit);
		
			assert (KGC_GET_STATE(info, idx) == KGC_STATE_NEEDFINALIZE);

			/* this assert is somewhat expensive */
			DBG(GCDIAG,
			    assert(gc_heap_isobject(info, unit)));
			KGC_SET_STATE(info, idx, KGC_STATE_INFINALIZE);
			markObjectDontCheck(unit, info, idx);
		}

		/* now process the objects that are referenced by objects to be finalized */
		while (gclists[grey].cnext != &gclists[grey]) {
			unit = gclists[grey].cnext;
			KaffeGC_WalkMemory(gcif, UTOMEM(unit));
		}

		finishGC(gcif);

		DBG(GCSTAT,
		    dprintf("REACHABLE OBJECT HISTOGRAM\n");
		    dprintf("%-7s %s\n", "COUNT", "CLASS");
		    dprintf("%-7s %s\n", "-------",
			    "-----------------------------------"
			    "-----------------------------------");
		    walkClassPool(gcDumpCounts, NULL));
		    
		startFinalizer();

		if (Kaffe_JavaVMArgs.enableVerboseGC > 0) {
			/* print out all the info you ever wanted to know */
			dprintf(
			    "<GC: heap %dK, total before %dK,"
			    " after %dK (%d/%d objs)\n %2.1f%% free,"
			    " allocated %dK (#%d), marked %dK, "
			    "swept %dK (#%d)\n"
			    " %d objs (%dK) awaiting finalization>\n",
			(int)(gc_get_heap_total()/1024), 
			gcStats.totalmem/1024, 
			(gcStats.totalmem-gcStats.freedmem)/1024, 
			gcStats.totalobj,
			gcStats.totalobj-gcStats.freedobj,
			(1.0 - (((float)gcStats.totalmem-gcStats.freedmem)/
				gc_get_heap_total())) * 100.0,
			gcStats.allocmem/1024,
			gcStats.allocobj,
			gcStats.markedmem/1024, 
			gcStats.freedmem/1024,
			gcStats.freedobj,
			gcStats.finalobj,
			gcStats.finalmem/1024);
		}
		if (Kaffe_JavaVMArgs.enableVerboseGC > 1) {
			OBJECTSTATSPRINT();
		}
		gcStats.totalmem -= gcStats.freedmem;
		gcStats.totalobj -= gcStats.freedobj;
		gcStats.allocobj = 0;
		gcStats.allocmem = 0;

gcend:;
		/* now signal any waiters */
		lockStaticMutex(&gcmanend);
		gcRunning = 0;
		broadcastStaticCond(&gcmanend);
		unlockStaticMutex(&gcmanend);
	}
	unlockStaticMutex(&gcman);
}

/*
 * Start the GC process by scanning the root and thread stack objects.
 */
static
void
startGC(Collector *gcif)
{
	gc_unit* unit;
	gc_block* info;
	uintp idx;

	gcStats.freedmem = 0;
	gcStats.freedobj = 0;
	gcStats.markedobj = 0;
	gcStats.markedmem = 0;

#if defined(ENABLE_JVMPI)
	if( JVMPI_EVENT_ISENABLED(JVMPI_EVENT_GC_START) )
	{
		JVMPI_Event ev;

		ev.event_type = JVMPI_EVENT_GC_START;
		jvmpiPostEvent(&ev);
	}
#endif

	KTHREAD(lockGC)();
	lockStaticMutex(&gc_lock);
	
	/* disable the mutator to protect colour lists */
	STOPWORLD();

	/* measure time */
	startTiming(&gc_time, "gctime-scan");

	/*
	 * Since objects whose finaliser has to be run need to
	 * be kept alive, we have to mark them here. They will
	 * be put back into the finalise list later on during
	 * the gc pass.
	 *
	 * Since these objects are treated like garbage, we have
	 * to set their colour to white before marking them.
	 */
	while (gclists[finalise].cnext != &gclists[finalise]) {
		unit = gclists[finalise].cnext;
		info = gc_mem2block(unit);
		idx = GCMEM2IDX(info, unit);

		KGC_SET_COLOUR (info, idx, KGC_COLOUR_WHITE);
		gcStats.finalobj -= 1;
		gcStats.finalmem -= GCBLOCKSIZE(info);

		markObjectDontCheck(unit, info, idx); 
	}

	/*
	 * Now we may walk static strong references.
	 */
	KaffeGC_walkRefs(gcif);
}

/*
 * Finish off the GC process.  Any unreached (white) objects are moved
 * for finalising and the finaliser woken.
 * The reached (black) objects are moved onto the now empty white list
 * and recoloured white.
 */
static
void
finishGC(Collector *gcif)
{
	gc_unit* unit;
	gc_block* info;
	int idx;
	gcList   toRemove;
	int i;

	/* There shouldn't be any grey objects at this point */
	assert(gclists[grey].cnext == &gclists[grey]);

	if (gclists[nofin_white].cnext != &gclists[nofin_white]) {
		toRemove.cnext = gclists[nofin_white].cnext;
		toRemove.cprev = gclists[nofin_white].cprev;

		toRemove.cnext->cprev = &toRemove;
		toRemove.cprev->cnext = &toRemove;

		URESETLIST(gclists[nofin_white]);
	} else {
		URESETLIST(toRemove);
	}	

	stopTiming(&gc_time);
	
	RESUMEWORLD();
	
	/* 
	 * Now move the black objects back to the white queue for next time.
	 */
	for (i=1; i<3; i++) {
		while (gclists[i].cnext != &gclists[i]) {
			unit = gclists[i].cnext;
			UREMOVELIST(unit);

			info = gc_mem2block(unit);
			idx = GCMEM2IDX(info, unit);

			assert(KGC_GET_COLOUR(info, idx) == KGC_COLOUR_BLACK);

			UAPPENDLIST(gclists[i+3], unit);

			KGC_SET_COLOUR(info, idx, KGC_COLOUR_WHITE);
		}
	}
	
	KTHREAD(unlockGC)();
	unlockStaticMutex(&gc_lock);

	startTiming(&sweep_time, "gctime-sweep");

	while (toRemove.cnext != &toRemove) {
		destroy_func_t destroy;

		unit = toRemove.cnext; 
		info = gc_mem2block(unit);
		idx = GCMEM2IDX(info, unit);

		gcStats.freedmem += GCBLOCKSIZE(info);
		gcStats.freedobj += 1;
		OBJECTSTATSREMOVE(unit);

#if defined(ENABLE_JVMPI)
		if( JVMPI_EVENT_ISENABLED(JVMPI_EVENT_OBJECT_FREE) )
		{
			JVMPI_Event ev;

			ev.event_type = JVMPI_EVENT_OBJECT_FREE;
			ev.u.obj_free.obj_id = UTOMEM(unit);
			jvmpiPostEvent(&ev);
		}
#endif

		/* clear all weak references to the object if it has not already been
		 * during the finalisation mark phase.
		 */
		KaffeGC_clearWeakRef(gcif, UTOMEM(unit));

		/* invoke destroy function before freeing the object */
		info = gc_mem2block(unit);
		idx = GCMEM2IDX(info, unit);
		destroy = gcFunctions[KGC_GET_FUNCS(info,idx)].destroy;
		if (destroy != 0) {
			destroy(gcif, UTOMEM(unit));
		}

		UREMOVELIST(unit);
		addToCounter(&gcgcablemem, "gcmem-gcable objects", 1, 
			-((jlong)GCBLOCKSIZE(info)));
		gc_heap_free(unit);
	}

#if defined(ENABLE_JVMPI)
	if( JVMPI_EVENT_ISENABLED(JVMPI_EVENT_GC_FINISH) )
	{ 
		JVMPI_Event ev;

		ev.event_type = JVMPI_EVENT_GC_FINISH;
		ev.u.gc_info.used_objects = (jlong)gcStats.markedobj;
		ev.u.gc_info.used_object_space = (jlong)gcStats.markedmem;
		ev.u.gc_info.total_object_space = (jlong)gcStats.totalmem;
		jvmpiPostEvent(&ev);
	}
#endif

	stopTiming(&sweep_time);
}

static
void
startFinalizer(void)
{
	int start;

        start = 0;

	lockStaticMutex(&gc_lock);
	/* If there's stuff to be finalised then we'd better do it */
	if (gclists[finalise].cnext != &gclists[finalise]) {
		start = 1;
	}
	unlockStaticMutex(&gc_lock);

	lockStaticMutex(&finman);
	if (start != 0 && finalRunning == false) {
		finalRunning = true;
		signalStaticCond(&finman);
	}
	unlockStaticMutex(&finman);

}

/*
 * The finaliser sits in a loop waiting to finalise objects.  When a
 * new finalised list is available, it is woken by the GC and finalises
 * the objects in turn.  An object is only finalised once after which
 * it is deleted.
 */
static void finaliserJob(Collector *gcif)
{
  gc_block* info = NULL;
  gc_unit* unit = NULL;
  int idx = 0;
  int func = 0;

  /*
   * Loop until the list of objects whose finaliser needs to be run is empty
   * [ checking the condition without holding a lock is ok, since we're the only
   * thread removing elements from the list (the list can never shrink during
   * a gc pass) ].
   *
   * According to the spec, the finalisers have to be run without any user
   * visible locks held. Therefore, we must temporarily release the finman
   * lock and may not hold the gc_lock while running the finalisers as they
   * are exposed to the user by java.lang.Runtime.
   * 
   * In addition, we must prevent an object and everything it references from
   * being collected while the finaliser is run (since we can't hold the gc_lock,
   * there may be several gc passes in the meantime). To do so, we keep the
   * object in the finalise list and only remove it from there when its
   * finaliser is done (simply adding the object to the grey list while its
   * finaliser is run only works as long as there's at most one gc pass).
   *
   * In order to determine the finaliser of an object, we have to access the
   * gc_block that contains it and its index. Doing this without holding a
   * lock only works as long as both, the gc_blocks and the indices of the
   * objects in a gc_block, are constant.
   */
  while (gclists[finalise].cnext != &gclists[finalise]) {
    unit = gclists[finalise].cnext;
    lockStaticMutex(&gc_lock);
    info = gc_mem2block(unit);
    idx = GCMEM2IDX(info, unit);
    func = KGC_GET_FUNCS(info, idx); 
    unlockStaticMutex(&gc_lock);

    /* Clear weak references to this object. Because according to the Java API spec.
     * "Suppose that the garbage collector determines at a certain point in time 
     * that an object is weakly reachable. At that time it will atomically clear
     * all weak references to that object and all weak references to any other
     * weakly-reachable objects from which that object is reachable through a chain
     * of strong and soft references."
     */
    KaffeGC_clearWeakRef(gcif, UTOMEM(unit));

    /* Call finaliser */
    unlockStaticMutex(&finman);
    (*gcFunctions[func].final)(gcif, UTOMEM(unit));
    lockStaticMutex(&finman);
    
    /* and remove unit from the finaliser list */
    lockStaticMutex(&gc_lock);
    info = gc_mem2block(unit);
    UREMOVELIST(unit);
    UAPPENDLIST(gclists[nofin_white], unit);
    
    gcStats.finalmem -= GCBLOCKSIZE(info);
    gcStats.finalobj -= 1;
    
    assert(KGC_GET_STATE(info,idx) == KGC_STATE_INFINALIZE);
    /* Objects are only finalised once */
    KGC_SET_STATE(info, idx, KGC_STATE_FINALIZED);
    KGC_SET_COLOUR(info, idx, KGC_COLOUR_WHITE);
    unlockStaticMutex(&gc_lock);
  }
  info = NULL;
  unit = NULL;
  idx = 0;
}

static void NONRETURNING
finaliserMan(void* arg)
{
  Collector *gcif = (Collector*)arg;

  lockStaticMutex(&finman);
  for (;;) {
    finalRunning = false;
    while (finalRunning == false) {
      waitStaticCond(&finman, (jlong)0);
    }
    assert(finalRunning == true);
    
    finaliserJob(gcif);
    
    /* Wake up anyone waiting for the finalizer to finish */
    lockStaticMutex(&finmanend);
    broadcastStaticCond(&finmanend);
    unlockStaticMutex(&finmanend);
  }
  unlockStaticMutex(&finman);
}

static
void
gcEnableGC(Collector* gcif UNUSED)
{

	lockStaticMutex(&gcman);
	gcDisabled -= 1;
	if( gcDisabled == 0 )
		broadcastStaticCond(&gcman);
	unlockStaticMutex(&gcman);
}

static
void
gcDisableGC(Collector* gcif UNUSED)
{

	lockStaticMutex(&gcman);
	gcDisabled += 1;
	unlockStaticMutex(&gcman);
}

/*
 * Explicity invoke the garbage collector and wait for it to complete.
 */
static
void
gcInvokeGC(Collector* gcif UNUSED, int mustgc)
{

	while (gcRunning < 0)
		KTHREAD(yield)();

	lockStaticMutex(&gcman);
	if (gcRunning == 0) {
		gcRunning = mustgc ? 2 : 1;
		if (!gcDisabled)
			signalStaticCond(&gcman);
	}

	lockStaticMutex(&gcmanend);
	unlockStaticMutex(&gcman);

	while (gcRunning != 0) {
		waitStaticCond(&gcmanend, (jlong)0);
	}
	unlockStaticMutex(&gcmanend);
}

/*
 * GC and invoke the finalizer.  Used to run finalizers on exit.
 */
static
void
gcInvokeFinalizer(Collector* gcif)
{

	/* First invoke the GC */
	KGC_invoke(gcif, 1);

	/* Run the finalizer (if might already be running as a result of
	 * the GC)
	 */
	lockStaticMutex(&finman);
	if (finalRunning == false) {
		finalRunning = true;
		signalStaticCond(&finman); 
	}
	lockStaticMutex(&finmanend);
	unlockStaticMutex(&finman);
	waitStaticCond(&finmanend, (jlong)0);
	unlockStaticMutex(&finmanend);
}

/*
 * Allocate a new object.  The object is attached to the white queue.
 * After allocation, if incremental collection is active we peform
 * a little garbage collection.  If we finish it, we wakeup the garbage
 * collector.
 */

void throwOutOfMemory(void) NONRETURNING;

static
void*
gcMalloc(Collector* gcif, size_t size, gc_alloc_type_t fidx)
{
	gc_block* info;
	gc_unit* unit;
	void * volatile mem;	/* needed on SGI, see comment below */
	int i;
	size_t bsz;
	int times = 0;

	assert(gc_init != 0);
	assert(gcFunctions[fidx].description != NULL);
	assert(size != 0);

	size += sizeof(gc_unit);

	lockStaticMutex(&gc_lock);

	for (unit=NULL; unit==NULL;) {
		times++;
		unit = gc_heap_malloc(size);
	
		/* keep pointer to object */
		mem = UTOMEM(unit);
		if (unit == 0) {
			switch (times) {
			case 1:
				/* Try invoking GC if it is available */
				if (garbageman != 0) {
					unlockStaticMutex(&gc_lock);
					KGC_invoke(gcif, 0);
					lockStaticMutex(&gc_lock);
				}
				break;

			case 2:
				/* Grow the heap */
				DBG (GCSYSALLOC, dprintf ("growing heap by %u bytes of type %s (%2.1f%% free)\n", 
							  (unsigned int)size, gcFunctions[fidx].description,
							  (1.0 - (gcStats.totalmem / gc_get_heap_total())) * 100.0); );
				
				gc_heap_grow(size);
				break;

			default:
				if (DBGEXPR(CATCHOUTOFMEM, true, false)) {
					/*
					 * If we ran out of memory, a OutOfMemoryException is
					 * thrown.  If we fail to allocate memory for it, all
					 * is lost.
					 */

					assert (!!!"Ran out of memory!");
				}
				/* Guess we've really run out */
				unlockStaticMutex(&gc_lock);
				return (NULL);
			}
		}
	}

	info = gc_mem2block(mem);
	i = GCMEM2IDX(info, unit);

	bsz = GCBLOCKSIZE(info);
	gcStats.totalmem += bsz;
	gcStats.totalobj += 1;
	gcStats.allocmem += bsz;
	gcStats.allocobj += 1;

	KGC_SET_FUNCS(info, i, fidx);

	OBJECTSTATSADD(unit);
	OBJECTSIZESADD(size);

	/* Determine whether we need to finalise or not */
	if (gcFunctions[fidx].final == KGC_OBJECT_NORMAL ||
	    gcFunctions[fidx].final == KGC_OBJECT_FIXED) {
		KGC_SET_STATE(info, i, KGC_STATE_NORMAL);
	}
	else {
		KGC_SET_STATE(info, i, KGC_STATE_NEEDFINALIZE);
	}

	/* If object is fixed, we give it the fixed colour and do not
	 * attach it to any lists.  This object is not part of the GC
	 * regime and must be freed explicitly.
	 */
	if (gcFunctions[fidx].final == KGC_OBJECT_FIXED) {
		addToCounter(&gcfixedmem, "gcmem-fixed objects", 1, bsz);
		KGC_SET_COLOUR(info, i, KGC_COLOUR_FIXED);
	}
	else {
		addToCounter(&gcgcablemem, "gcmem-gcable objects", 1, bsz);
		/*
		 * Note that as soon as we put the object on the white list,
		 * the gc might come along and free the object if it can't
		 * find any references to it.  This is why we need to keep
		 * a reference in `mem'.  Note that keeping a reference in
		 * `unit' will not do because markObject performs a UTOUNIT()!
		 * In addition, on some architectures (SGI), we must tell the
		 * compiler to not delay computing mem by defining it volatile.
		 */
		KGC_SET_COLOUR(info, i, KGC_COLOUR_WHITE);

		if (KGC_GET_STATE(info, i) == KGC_STATE_NEEDFINALIZE) {
			UAPPENDLIST(gclists[fin_white], unit);
		} else {
			UAPPENDLIST(gclists[nofin_white], unit);
		}
	}

	/* It is not safe to allocate java objects the first time
	 * gcMalloc is called, but it should be safe after gcEnable
	 * has been called.
	 */
	if (garbageman && !outOfMem && !outOfMem_allocator) {
		outOfMem_allocator = KTHREAD(current)();
	}

	unlockStaticMutex(&gc_lock);

	/* KTHREAD(current)() will be null in some window before we
	 * should try allocating java objects
	 */
	if (!outOfMem && outOfMem_allocator
	    && outOfMem_allocator == KTHREAD(current)()) { 
		outOfMem = OOM_ALLOCATING;
		outOfMem = OutOfMemoryError; /* implicit allocation */
		outOfMem_allocator = NULL;
		gc_add_ref(outOfMem);
	}
	return (mem);
}

static
struct Hjava_lang_Throwable *
gcThrowOOM(Collector *gcif UNUSED)
{
	Hjava_lang_Throwable *ret = NULL;
	int reffed;

	/*
	 * Make sure we are the only thread to use this exception
	 * object.
	 */
	lockStaticMutex(&gc_lock);
	ret = outOfMem;
	reffed = outOfMem != 0;
	outOfMem = NULL;
	/* We try allocating reserved pages before we allocate the
	 * outOfMemory error.  We can use some or all of the reserved
	 * pages to actually grab an error.
	 */
	if (gc_primitive_use_reserve()) {
		if (!ret || ret == OOM_ALLOCATING) {
			unlockStaticMutex(&gc_lock);
			ret = OutOfMemoryError; /* implicit allocation */
			lockStaticMutex(&gc_lock);
		}
	}
	if (ret == OOM_ALLOCATING || ret == NULL) {
		/* die now */
		unlockStaticMutex(&gc_lock);
		dprintf(
			"Not enough memory to throw OutOfMemoryError!\n");
		KAFFEVM_ABORT();
	}
	unlockStaticMutex(&gc_lock);
	if (reffed) gc_rm_ref(ret);
	return ret;
}

/*
 * Reallocate an object.
 */
static
void*
gcRealloc(Collector* gcif, void* mem, size_t size, gc_alloc_type_t fidx)
{
	gc_block* info;
	int idx;
	void* newmem;
	gc_unit* unit;
	size_t osize;

	assert(gcFunctions[fidx].final == KGC_OBJECT_FIXED);

	/* If nothing to realloc from, just allocate */
	if (mem == NULL) {
		return (gcMalloc(gcif, size, fidx));
	}

	lockStaticMutex(&gc_lock);
	unit = UTOUNIT(mem);
	info = gc_mem2block(unit);
	idx = GCMEM2IDX(info, unit);
	osize = GCBLOCKSIZE(info) - sizeof(gc_unit);

	assert(KGC_GET_FUNCS(info, idx) == fidx);

	/* Can only handled fixed objects at the moment */
	assert(KGC_GET_COLOUR(info, idx) == KGC_COLOUR_FIXED);
	info = NULL;
	unlockStaticMutex(&gc_lock);

	/* If we'll fit into the current space, just send it back */
	if (osize >= size) {
		return (mem);
	}

	/* Allocate new memory, copy data, and free the old */
	newmem = gcMalloc(gcif, size, fidx);
	memcpy(newmem, mem, (size_t)osize);
	gcFree(gcif, mem);

	return (newmem);
}

/*
 * Explicitly free an object.
 */
static
void
gcFree(Collector* gcif UNUSED, void* mem)
{
	gc_block* info;
	int idx;
	gc_unit* unit;

	if (mem != NULL) {
		lockStaticMutex(&gc_lock);
		unit = UTOUNIT(mem);
		info = gc_mem2block(unit);
		idx = GCMEM2IDX(info, unit);

		if (KGC_GET_COLOUR(info, idx) == KGC_COLOUR_FIXED) {
			size_t sz = GCBLOCKSIZE(info);
			
			OBJECTSTATSREMOVE(unit);

			/* Keep the stats correct */
			gcStats.totalmem -= sz;
			gcStats.totalobj -= 1;
			addToCounter(&gcfixedmem, "gcmem-fixed objects", 1, -(jlong)sz);

			gc_heap_free(unit);
		}
		else {
			assert(!!!"Attempt to explicitly free nonfixed object");
		}
		unlockStaticMutex(&gc_lock);
	}
}

static
void
gcInit(Collector *collector UNUSED)
{
	gc_init = 1;
}

/*
 * Start gc threads, which enable collection
 */
static 
void
/* ARGSUSED */
gcEnable(Collector* collector)
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

#if defined(SUPPORT_VERBOSEMEM)

/* --------------------------------------------------------------------- */
/* The following functions are strictly for statistics gathering	 */

static
void
objectStatsChange(gc_unit* unit, int diff)
{
	gc_block* info;
	int idx;

	info = gc_mem2block(unit);
	idx = KGC_GET_FUNCS(info, GCMEM2IDX(info, unit));

	assert(idx >= 0 && gcFunctions[idx].description!=NULL);
	gcFunctions[idx].nr += diff * 1;
	gcFunctions[idx].mem += diff * GCBLOCKSIZE(info);
}

static
void
objectStatsPrint(void)
{
	int cnt = 0;

	dprintf("Memory statistics:\n");
	dprintf("------------------\n");

	while (cnt < KGC_ALLOC_MAX_INDEX) {
		dprintf("%14.14s: Nr %6d  Mem %6dK",
			gcFunctions[cnt].description, 
			gcFunctions[cnt].nr, 
			gcFunctions[cnt].mem/1024);
		if (++cnt % 2 != 0) {
			dprintf("   ");
		} else {
			dprintf("\n");
		}
	}

	if (cnt % 2 != 0) {
		dprintf("\n");
	}

        objectSizesPrint();
}

static
void
objectSizesAdd(size_t sz)
{
        int i;
        for (i = 0; objectSizes[i].size > 0 &&  sz > (size_t)objectSizes[i].size; i++)
                ;
        objectSizes[i].count++;

	/* This might be slightly misleading as it
	 * doesn't take into account the block overhead
	 * for small allocations */

	if (objectSizes[i].size > MAX_SMALL_OBJECT_SIZE) {
		objectSizes[i].total += sz;
	}
	else {
		objectSizes[i].total += objectSizes[i].size;
	}
}

static
void
objectSizesPrint(void)
{
        int i;
        uint64 total;
        int count;
        int nr;
        int j;

        total = 0;
        count = 0;
        for (i = 0; objectSizes[i].size != -1; i++) {
                count += objectSizes[i].count;
                total += objectSizes[i].total;
        }
        nr = i;

        if (total == 0) {
                return;
        }

        dprintf("Percentage size allocations: %% of allocation counts / %% of memory\n");
        dprintf("-----------------------------------------------------------------\n");


        for (i = 0; i < nr; ) {
                for (j = 0; j < 3 && i < nr; j++, i++) {
                        dprintf("%10zd :%5.1f /%5.1f  ", objectSizes[i].size, 
				(float)(objectSizes[i].count * 100 / (float)count), 
				(float)(objectSizes[i].total * 100 / (float)total));
                }
                dprintf("\n");
        }
}

#endif

static uintp
gcGetHeapLimit(Collector *gcif UNUSED)
{
  return gc_get_heap_limit();
}

static uintp
gcGetHeapTotal(Collector *gcif UNUSED)
{
  return gc_get_heap_total();
}

static uintp
gcGetHeapFree(Collector *gcif)
{
  return gcGetHeapTotal(gcif) - gcStats.totalmem;
}


static const char *
gcGetName(UNUSED Collector *gcif)
{
	return "kaffe-gc";
}

/*
 * vtable for object implementing the collector interface.
 */
static struct GarbageCollectorInterface_Ops KGC_Ops = {
	gcGetName,		/* reserved */
	NULL,		/* reserved */
	NULL,		/* reserved */
	gcMalloc,
	gcRealloc,
	gcFree,
	gcInvokeGC,
	gcInvokeFinalizer,
	gcInit,
	gcEnable,
	gcMarkAddress,
	gcMarkObject,
	gcGetObjectSize,
	gcGetObjectDescription,
	gcGetObjectIndex,
	gcGetObjectBase,
	gcRegisterFixedTypeByIndex,
	gcRegisterGcTypeByIndex,
	gcThrowOOM,
	gcEnableGC,
	gcDisableGC,
	gcGetHeapFree,
	gcGetHeapLimit,
	gcGetHeapTotal,
	KaffeGC_addRef,
	KaffeGC_rmRef,
	KaffeGC_addWeakRef,
	KaffeGC_rmWeakRef
};

/*
 * Initialise the Garbage Collection system.
 */
Collector* 
createGC(void)
{
  initStaticLock(&gcman);
  initStaticLock(&gcmanend);
  initStaticLock(&finman);
  initStaticLock(&finmanend);
  initStaticLock(&gc_lock);

  KaffeGC_initRefs();

  URESETLIST(gclists[nofin_white]);
  URESETLIST(gclists[fin_white]);
  URESETLIST(gclists[grey]);
  URESETLIST(gclists[nofin_black]);
  URESETLIST(gclists[fin_black]);
  URESETLIST(gclists[finalise]);
  gc_obj.collector.ops = &KGC_Ops;
  
  gc_heap_initialise ();
  gc_primitive_reserve(KGC_NUMBER_PAGE_IN_RESERVE);
  
  return (&gc_obj.collector);
}

