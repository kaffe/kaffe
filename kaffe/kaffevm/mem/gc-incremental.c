/* gc-incremental.c
 * The incremental garbage collector.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#define	DBG(s)
#define	FDBG(s)
#define	FTDBG(s)
#define	ADBG(s)
#define	STATS

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "gtypes.h"
#include "gc.h"
#include "classMethod.h"
#include "locks.h"
#include "thread.h"
#include "errors.h"
#include "exception.h"
#include "external.h"
#include "lookup.h"
#include "soft.h"
#include "md.h"
#include "jni.h"

static gcList gclists[5];
static int mustfree = 4;		/* temporary list */
static int white = 3;
static int grey = 2;
static int black = 1;
static int finalise = 0;

static bool gcRunning = false;
static bool finalRunning = false;

#if defined(STATS)

/* Class and object space statistics */
static struct {
	int	classNr;
	int	objectNr;
	int	arrayNr;
	int	methodNr;
	int	fieldNr;
	int	staticNr;
	int	dispatchNr;
	int	bytecodeNr;
	int	exceptionNr;
	int	constantNr;
	int	utf8constNr;
	int	interfaceNr;
	int	jitNr;
	int	lockNr;
	int	refNr;
	int	threadCtxNr;

	int	fixedNr;
	int	otherNr;

	int	classMem;
	int	objectMem;
	int	arrayMem;
	int	methodMem;
	int	fieldMem;
	int	staticMem;
	int	dispatchMem;
	int	bytecodeMem;
	int	exceptionMem;
	int	constantMem;
	int	utf8constMem;
	int	interfaceMem;
	int	jitMem;
	int	lockMem;
	int	refMem;
	int	threadCtxMem;

	int	fixedMem;
	int	otherMem;
} objectStats;

static void objectStatsChange(gc_unit*, int);

#define	OBJECTSTATSADD(M)	objectStatsChange(M, 1)
#define	OBJECTSTATSREMOVE(M)	objectStatsChange(M, -1)
#define	OBJECTSTATSPRINT()	objectStatsPrint()

#else

#define	OBJECTSTATSADD(M)
#define	OBJECTSTATSREMOVE(M)
#define	OBJECTSTATSPRINT()

#endif

/* We run the GC after allocating 1Mbyte of memory.  If we are
 * doing this incrementally, then we will have GC our entire heap
 * by the time we've allocated this much new space.
 */
#define	ALLOCCOUNTGC	(1024*1024)

static iLock gcman;
static iLock finman;

int gc_mode = GC_DISABLED;	/* GC will be enabled after the first
				 * thread is setup.
				 */
static void gcFree(void*);

static void finalizeObject(void*);

void walkMemory(void*);

static void walkNull(void*, uint32);
static void walkClass(void*, uint32);
static void walkObject(void*, uint32);
static void walkRefArray(void*, uint32);

/* Standard GC function sets */
static gcFuncs gcFunctions[] = {
	{ walkConservative, GC_OBJECT_NORMAL },	/* GC_ALLOC_NORMAL */
	{ walkNull,	    GC_OBJECT_NORMAL },	/* GC_ALLOC_NOWALK */
	{ walkNull,	    GC_OBJECT_FIXED  },	/* GC_ALLOC_FIXED */
	{ walkObject,	    finalizeObject   },	/* GC_ALLOC_NORMALOBJECT */
	{ walkNull,	    GC_OBJECT_NORMAL },	/* GC_ALLOC_PRIMARRAY */
	{ walkRefArray,     GC_OBJECT_NORMAL },	/* GC_ALLOC_REFARRAY */
	{ walkClass,        GC_OBJECT_NORMAL },	/* GC_ALLOC_CLASSOBJECT */
	{ 0,       	    0		     },	/* GC_ALLOC_FINALIZEOBJECT */
	{ walkConservative, GC_OBJECT_NORMAL },	/* GC_ALLOC_METHOD */
	{ walkConservative, GC_OBJECT_NORMAL },	/* GC_ALLOC_FIELD */
	{ walkNull,	    GC_OBJECT_NORMAL },	/* GC_ALLOC_STATICDATA */
	{ walkConservative, GC_OBJECT_NORMAL },	/* GC_ALLOC_DISPATCHTABLE */
	{ walkNull,	    GC_OBJECT_NORMAL },	/* GC_ALLOC_BYTECODE */
	{ walkConservative, GC_OBJECT_NORMAL },	/* GC_ALLOC_EXCEPTIONTABLE */
	{ walkConservative, GC_OBJECT_NORMAL },	/* GC_ALLOC_CONSTANT */
	{ walkNull,	    GC_OBJECT_NORMAL },	/* GC_ALLOC_UTF8CONST */
	{ walkConservative, GC_OBJECT_NORMAL },	/* GC_ALLOC_INTERFACE */
	{ walkConservative, GC_OBJECT_NORMAL },	/* GC_ALLOC_JITCODE */
	{ walkNull,	    GC_OBJECT_FIXED  },	/* GC_ALLOC_LOCK */
	{ walkNull,	    GC_OBJECT_FIXED  },	/* GC_ALLOC_THREADCTX */
	{ walkNull,	    GC_OBJECT_FIXED  },	/* GC_ALLOC_REF */
};

#define	REFOBJALLOCSZ	128
#define	REFOBJHASHSZ	128
typedef struct _refObject {
	void*			mem;
	unsigned int		ref;
	struct _refObject*	next;
} refObject;
typedef struct _refTable {
	refObject*		hash[REFOBJHASHSZ];
} refTable;
static refTable			refObjects;
#define	REFOBJHASH(V)		((((uintp)(V))/(2*sizeof(uintp)))%REFOBJHASHSZ)

struct _gcStats gcStats;
extern size_t gc_heap_total;
extern gc_block* gc_objecthash[];
extern Hjava_lang_Class* ThreadClass;

static void startGC(void);
static void finishGC(void);

/*
 * Initalise the Garbage Collection system.
 */
static
void
initGc(void)
{
	URESETLIST(gclists[white]);
	URESETLIST(gclists[grey]);
	URESETLIST(gclists[black]);
	URESETLIST(gclists[finalise]);
	URESETLIST(gclists[mustfree]);
}

/*
 * Mark the memory as reached if it really is an object.
 */
void
markObject(void* mem)
{
	gc_block* info;
	gc_block* hptr;
	uintp hidx;
	int idx;
	gc_unit* unit;

	/*
	 * First we check to see if the memory 'mem' is in fact the
	 * beginning of an object.  If not we just return.
	 */

	/* Get block info for this memory - if it exists */
	unit = UTOUNIT(mem);
	info = GCMEM2BLOCK(unit);

	/* Get hash index for this block */
	hidx = GC_OBJECT_HASHIDX(info);
	for (hptr = gc_objecthash[hidx]; hptr != 0; hptr = hptr->next) {
		if (hptr == info) {
			/* Make sure 'unit' refers to the beginning of an
			 * object.  We do this by making sure it is correctly
			 * aligned within the block.
			 */
			idx = GCMEM2IDX(info, unit);
			if (idx < info->nr && GCBLOCK2MEM(info, idx) == unit && (GC_GET_COLOUR(info, idx) & GC_COLOUR_INUSE) == GC_COLOUR_INUSE) {
				goto found;
			}
			break;
		}
	}

	/* Not found */
	return;

	/* It is a real object ... */

	found:;
	/* If object's been traced before, don't do it again */
	if (GC_GET_COLOUR(info, idx) != GC_COLOUR_WHITE) {
		return;
	}

	/* If we found a new white object, mark it as grey and
	 * move it into the grey list.
	 */
	LOCK();
	GC_SET_COLOUR(info, idx, GC_COLOUR_GREY);
	UREMOVELIST(unit);
	UAPPENDLIST(gclists[grey], unit);
	UNLOCK();
}

void
walkConservative(void* base, uint32 size)
{
	int8* mem;

DBG(	printf("walkConservative: %x-%x\n", base, base+size);
	fflush(stdout);							)

	gcStats.markedobj += 1;

	if (size > 0) {
		gcStats.markedmem += size;

		for (mem = ((int8*)base) + (size & -ALIGNMENTOF_VOIDP) - sizeof(void*); (void*)mem >= base; mem -= ALIGNMENTOF_VOIDP) {
			void *p = *(void **)mem;
			if (p)
				markObject(p);
		}
	}
}

/*
 * Walk an object.
 */
static
void
walkObject(void* base, uint32 size)
{
	Hjava_lang_Object *obj = (Hjava_lang_Object*)base;
	walkConservative(base, size);

	/* Special magic to handle thread objects */
	/* 
	 * Note that there is a window after the object is allocated but
	 * before dtable is set.
	 */
	if ((obj->dtable != 0) && soft_instanceof(ThreadClass, obj)) {
		(*Kaffe_ThreadInterface.GcWalkThread)((Hjava_lang_Thread*)base);
	}
}

/*
 * Walk an object which cannot reference anything.
 */
static
void
walkNull(void* base, uint32 size)
{
	gcStats.markedobj += 1;
        gcStats.markedmem += size;
}

/*
 * Walk a bit of memory.
 */
void
walkMemory(void* mem)
{
	gc_block* info;
	int idx;
	gc_unit* unit;

	unit = UTOUNIT(mem);
	info = GCMEM2BLOCK(unit);
	idx = GCMEM2IDX(info, unit);

	UREMOVELIST(unit);
	UAPPENDLIST(gclists[black], unit);
	GC_SET_COLOUR(info, idx, GC_COLOUR_BLACK);

	(*gcFunctions[GC_GET_FUNCS(info, idx)].walk)(mem, GCBLOCKSIZE(info));
}

/*
 * Walk a class object.
 */
static
void
walkClass(void* base, uint32 size)
{
	Hjava_lang_Class* class;
	Field* fld;
	int n;

	gcStats.markedobj += 1;
	gcStats.markedmem += size;

	class = (Hjava_lang_Class*)base;

	markObject(class->name);
	if (class->state >= CSTATE_PREPARED) {
		markObject(class->superclass);
	}
	markObject(class->constants.data);
	markObject(class->methods);
	markObject(class->fields);
	if (!CLASS_IS_PRIMITIVE(class)) {
		markObject(class->dtable);
	}
	markObject(class->interfaces);
	markObject(class->loader);

	/* Walk the static data elements */
	if (class->state >= CSTATE_DOING_PREPARE) {
        	fld = CLASS_SFIELDS(class);
        	n = CLASS_NSFIELDS(class);
        	for (; --n >= 0; fld++) {
			if (FIELD_ISREF(fld)) {
				markObject(*(void**)FIELD_ADDRESS(fld));
			}
        	}
	}
}

/*
 * Walk an array object objects.
 */
static
void
walkRefArray(void* base, uint32 size)
{
	Hjava_lang_Object* arr;
	int i;
	Hjava_lang_Object** ptr;
#ifdef notyet
	markObject (.. class ..);
#endif
	gcStats.markedobj += 1;
	gcStats.markedmem += size;

	arr = (Hjava_lang_Object*)base;
	ptr = OBJARRAY_DATA(arr);
	for (i = ARRAY_SIZE(arr); --i>= 0; ) {
		Hjava_lang_Object* el = *ptr++;
		if (el != NULL) {
			markObject(el);
		}
	}
}

/*
 * The Garbage Collector sits in a loop starting a collection, waiting
 * until it's finished incrementally, then tidying up before starting
 * another one.
 */
void
gcMan(void* arg)
{
	gc_unit* unit;
	gc_unit* nunit;
	gc_block* info;
	int idx;

	initStaticLock(&gcman);
	lockStaticMutex(&gcman);

	for(;;) {
		gcRunning = false;
		waitStaticCond(&gcman, 0);
		assert(gcRunning == true);

		startGC();

		for (unit = gclists[grey].cnext; unit != &gclists[grey]; unit = gclists[grey].cnext) {
			walkMemory(UTOMEM(unit));
		}
		/* Now walk any white objects which will be finalied.  They
		 * may get reattached, so anything they reference must also
		 * be live just in case.
		 */
		for (unit = gclists[white].cnext; unit != &gclists[white]; unit = nunit) {
			nunit = unit->cnext;
			info = GCMEM2BLOCK(unit);
			idx = GCMEM2IDX(info, unit);
			if (GC_GET_STATE(info, idx) == GC_STATE_NEEDFINALIZE) {
				GC_SET_STATE(info, idx, GC_STATE_INFINALIZE);
				markObject(UTOMEM(unit));
			}
		}
		/* We may now have more grey objects, so walk them */
		for (unit = gclists[grey].cnext; unit != &gclists[grey]; unit = gclists[grey].cnext) {
			walkMemory(UTOMEM(unit));
		}

		finishGC();

		if (Kaffe_JavaVMArgs[0].enableVerboseGC > 0) {
			fprintf(stderr, "<GC: heap %dK, total %dK, alloc %dK, marked %dK, freeing %dK>\n", gc_heap_total/1024, gcStats.totalmem/1024, gcStats.allocmem/1024, gcStats.markedmem/1024, (gcStats.totalmem > gcStats.markedmem ? (gcStats.totalmem - gcStats.markedmem)/1024 : 0));
		}
		if (Kaffe_JavaVMArgs[0].enableVerboseGC > 1) {
			OBJECTSTATSPRINT();
		}

		gcStats.totalmem -= gcStats.freedmem;
		gcStats.totalobj -= gcStats.freedobj;
		gcStats.allocobj = 0;
		gcStats.allocmem = 0;

		/* Wake up anyone waiting for the GC to finish */
		broadcastStaticCond(&gcman);
	}
}

/*
 * Start the GC process by scanning the root and thread stack objects.
 */
static
void
startGC(void)
{
	int i;
	refObject* robj;
	gc_unit* unit;
	gc_unit* nunit;

	gcStats.freedmem = 0;
	gcStats.freedobj = 0;
	gcStats.markedobj = 0;
	gcStats.markedmem = 0;

	/* disable the mutator to protect colour lists */
	LOCK();

	/* Walk the referenced objects */
	for (i = 0; i < REFOBJHASHSZ; i++) {
		for (robj = refObjects.hash[i]; robj != 0; robj = robj->next) {
			markObject(robj->mem);
		}
	}

	/* Walk all objects on the finalizer list */
	for (unit = gclists[finalise].cnext;
	     unit != &gclists[finalise]; unit = nunit) {
		nunit = unit->cnext;
		markObject(UTOMEM(unit));
	}

	/* Walk the thread objects */
	(*Kaffe_ThreadInterface.GcWalkThreads)();
}

/*
 * Finish off the GC process.  Any unreached (white) objects are moved
 * for finalising and the finaliser woken.
 * The reached (black) objects are moved onto the now empty white list
 * and recoloured white.
 */
static
void
finishGC(void)
{
	gc_unit* unit;
	gc_block* info;
	int idx;

	/* There shouldn't be any grey objects at this point */
	assert(gclists[grey].cnext == &gclists[grey]);

	/* 
	 * Any white objects should now be freed, but we cannot call
	 * gc_heap_free here because we might block in gc_heap_free, 
	 * which would leave the white list unprotected.
	 * So we move them to a 'mustfree' list from where we'll pull them
	 * off later.
	 */
	while (gclists[white].cnext != &gclists[white]) {
		unit = gclists[white].cnext;
		UREMOVELIST(unit);

		info = GCMEM2BLOCK(unit);
		idx = GCMEM2IDX(info, unit);

		assert(GC_GET_COLOUR(info, idx) == GC_COLOUR_WHITE);
		assert(GC_GET_STATE(info, idx) == GC_STATE_NORMAL);
		UAPPENDLIST(gclists[mustfree], unit);
	}

	/* 
	 * Now move the black objects back to the white queue for next time.
	 * Note that all objects that were eligible for finalization are now
	 * black - this is so because we marked and then walked them.
	 * We recognize them by their "INFINALIZE" state, however, and put
	 * them on the finalise list.
	 */
	while (gclists[black].cnext != &gclists[black]) {
		unit = gclists[black].cnext;
		UREMOVELIST(unit);

		info = GCMEM2BLOCK(unit);
		idx = GCMEM2IDX(info, unit);

		assert(GC_GET_COLOUR(info, idx) == GC_COLOUR_BLACK);

		if (GC_GET_STATE(info, idx) == GC_STATE_INFINALIZE) {
			UAPPENDLIST(gclists[finalise], unit);
		}
		else {
			UAPPENDLIST(gclists[white], unit);
		}
		GC_SET_COLOUR(info, idx, GC_COLOUR_WHITE);
	}
	/* 
	 * Now that all lists that the mutator manipulates are in a
	 * consistent state, we can reenable the mutator here 
	 */
	UNLOCK();

	/* 
	 * Now free the objects.  We can block here since we're the only
	 * thread manipulating the "mustfree" list.
	 */
	while (gclists[mustfree].cnext != &gclists[mustfree]) {
		unit = gclists[mustfree].cnext;
		info = GCMEM2BLOCK(unit);
		UREMOVELIST(unit);
FDBG(		printf("freeObject %p size %d\n", info, info->size);
		fflush(stdout);					)
		gcStats.freedmem += GCBLOCKSIZE(info);
		gcStats.freedobj += 1;
		OBJECTSTATSREMOVE(unit);
		gc_heap_free(unit);
	}


FTDBG(	printf("Freed %d objects of %dK\n", gcStats.freedobj,
		gcStats.freedmem/1024);					)

	/* If there's stuff to be finalised then we'd better do it */
	if (gclists[finalise].cnext != &gclists[finalise]) {
		lockStaticMutex(&finman);
		finalRunning = true;
		signalStaticCond(&finman);
		unlockStaticMutex(&finman);
	}

}

/*
 * The finaliser sits in a loop waiting to finalise objects.  When a
 * new finalised list is available, it is woken by the GC and finalises
 * the objects in turn.  An object is only finalised once after which
 * it is deleted.
 */
void
finaliserMan(void* arg)
{
	gc_block* info;
	gc_unit* unit;
	int idx;

	initStaticLock(&finman);
	lockStaticMutex(&finman);

	for (;;) {

		finalRunning = false;
		waitStaticCond(&finman, 0);
		assert(finalRunning == true);

		while (gclists[finalise].cnext != &gclists[finalise]) {
			LOCK();
			unit = gclists[finalise].cnext;
			UREMOVELIST(unit);
			UAPPENDLIST(gclists[grey], unit);

			info = GCMEM2BLOCK(unit);
			idx = GCMEM2IDX(info, unit);

			assert(GC_GET_STATE(info,idx) == GC_STATE_INFINALIZE);
			/* Objects are only finalised once */
			GC_SET_STATE(info, idx, GC_STATE_FINALIZED);
			GC_SET_COLOUR(info, idx, GC_COLOUR_GREY);
			UNLOCK();
			/* Call finaliser */
			unlockStaticMutex(&finman);
			(*gcFunctions[GC_GET_FUNCS(info,idx)].final)(UTOMEM(unit));
			lockStaticMutex(&finman);
		}

		/* Wake up anyone waiting for the finalizer to finish */
		broadcastStaticCond(&finman);
	}
}

/*
 * Explicity invoke the garbage collector and wait for it to complete.
 */
static
void
gcInvokeGC(void)
{
	lockStaticMutex(&gcman);
	if (gcRunning == false) {
		gcRunning = true;
		signalStaticCond(&gcman);
	}
	waitStaticCond(&gcman, 0);
	unlockStaticMutex(&gcman);
}

/*
 * Explicity invoke the finalizer.
 */
static
void
gcInvokeFinalizer(void)
{
	/* First invoke the GC */
	invokeGC();

	/* Run the finalizer (if might already be running as a result of
	 * the GC)
	 */
	lockStaticMutex(&finman);
	if (finalRunning == false) {
		finalRunning = true;
		signalStaticCond(&finman); 
	}
	waitStaticCond(&finman, 0);
	unlockStaticMutex(&finman);
}       

/*
 * Allocate a new object.  The object is attached to the white queue.
 * After allocation, if incremental collection is active we peform
 * a little garbage collection.  If we finish it, we wakeup the garbage
 * collector.
 */

void throwOutOfMemory(void) __NORETURN__;

static
void*
gcMalloc(size_t size, int fidx)
{
	static int gc_init = 0;
	gc_block* info;
	gc_unit* unit;
	void *mem;
	int i;

	/* Initialise GC */
	if (gc_init == 0) {
		gc_init = 1;
		initGc();
	}

	unit = gc_heap_malloc(size + sizeof(gc_unit));

	/* keep pointer to object */
	mem = UTOMEM(unit);
	if (unit == 0) {
		throwOutOfMemory();
	}
ADBG(	printf("gcMalloc: 0x%x (%d)\n", unit, size);			)

	info = GCMEM2BLOCK(unit);
	i = GCMEM2IDX(info, unit);

	gcStats.totalmem += GCBLOCKSIZE(info);
	gcStats.totalobj += 1;
	gcStats.allocmem += GCBLOCKSIZE(info);
	gcStats.allocobj += 1;

	GC_SET_FUNCS(info, i, fidx);

	OBJECTSTATSADD(unit);

	/* Determine whether we need to finalise or not */
	if (gcFunctions[fidx].final == GC_OBJECT_NORMAL || gcFunctions[fidx].final == GC_OBJECT_FIXED) {
		GC_SET_STATE(info, i, GC_STATE_NORMAL);
	}
	else {
		GC_SET_STATE(info, i, GC_STATE_NEEDFINALIZE);
	}

	/* If object is fixed, we give it the fixed colour and do not
	 * attach it to any lists.  This object is not part of the GC
	 * regieme and must be freed explicitly.
	 */
	if (gcFunctions[fidx].final == GC_OBJECT_FIXED) {
		GC_SET_COLOUR(info, i, GC_COLOUR_FIXED);
	}
	else {
		/*
		 * Note that as soon as we put the object on the white list,
		 * the gc might come along and free the object if it can't
		 * find any references to it.  This is why we need to keep
		 * a reference in `mem'.  Note that keeping a reference in
		 * `unit' will not do because markObject performs a UTOUNIT()!
		 */
		LOCK(); 
		GC_SET_COLOUR(info, i, GC_COLOUR_WHITE);
		UAPPENDLIST(gclists[white], unit);
		UNLOCK();
	}
	return (mem);
}

/*
 * Reallocate an object.
 */
static
void*
gcRealloc(void* mem, size_t size, int fidx)
{
	gc_block* info;
	int idx;
	void* newmem;
	gc_unit* unit;

	assert(fidx == GC_ALLOC_FIXED);

	/* If nothing to realloc from, just allocate */
	if (mem == NULL) {
		return (gcMalloc(size, fidx));
	}

	unit = UTOUNIT(mem);
	info = GCMEM2BLOCK(unit);
	idx = GCMEM2IDX(info, unit);

	/* Can only handled fixed objects at the moment */
	assert(GC_GET_COLOUR(info, idx) == GC_COLOUR_FIXED);

	/* If we'll fit into the current space, just send it back */
	if (info->size >= size + sizeof(gc_unit)) {
		return (mem);
	}

	/* Allocate new memory, copy data, and free the old */
	newmem = gcMalloc(size, fidx);
	memcpy(newmem, mem, info->size);
	gcFree(mem);

	return (newmem);
}

/*
 * Explicitly free an object.
 */
static
void
gcFree(void* mem)
{
	gc_block* info;
	int idx;
	gc_unit* unit;

	if (mem != 0) {
		unit = UTOUNIT(mem);
		info = GCMEM2BLOCK(unit);
		idx = GCMEM2IDX(info, unit);

		if (GC_GET_COLOUR(info, idx) == GC_COLOUR_FIXED) {

			OBJECTSTATSREMOVE(unit);

			/* Keep the stats correct */
			gcStats.totalmem -= GCBLOCKSIZE(info);
			gcStats.totalobj -= 1;

			gc_heap_free(unit);
		}
		else {
			/* We just ignore this - it'll get GCed */
		}
	}
}

/*
 * Add a presistent reference to an object.
 */
static
void
gcAddRef(void* mem)
{
	uint32 idx;
	refObject* obj;

	idx = REFOBJHASH(mem);
	for (obj = refObjects.hash[idx]; obj != 0; obj = obj->next) {
		/* Found it - just increase reference */
		if (obj->mem == mem) {
			obj->ref++;
			return;
		}
	}

	/* Not found - create a new one */
	obj = (refObject*)gcMalloc(sizeof(refObject), GC_ALLOC_REF);
	obj->mem = mem;
	obj->ref = 1;
	obj->next = refObjects.hash[idx];
	refObjects.hash[idx] = obj;
}

/*
 * Remove a persistent reference to an object.  If the count becomes
 * zero then the reference is removed.
 */
static
bool
gcRmRef(void* mem)
{
	uint32 idx;
	refObject** objp;
	refObject* obj;

	idx = REFOBJHASH(mem);
	for (objp = &refObjects.hash[idx]; *objp != 0; objp = &obj->next) {
		obj = *objp;
		/* Found it - just increase reference */
		if (obj->mem == mem) {
			obj->ref--;
			if (obj->ref == 0) {
				*objp = obj->next;
				gcFree(obj);
			}
			return (true);
		}
	}

	/* Not found!! */
	return (false);
}

static
void
finalizeObject(void* ob)
{
	Method* final;

	final = findMethod(OBJECT_CLASS((Hjava_lang_Object*)ob), final_name, void_signature);
	if (final != 0) {
		callMethodA(final, METHOD_INDIRECTMETHOD(final), (Hjava_lang_Object*)ob, 0, 0);
	}
	else {
		/* This shouldn't happen */
	}

	/* 
	 * make sure thread objects get detached 
	 */
	if (soft_instanceof(ThreadClass, ob)) {
		finalizeThread((Hjava_lang_Thread*)ob);
	}
}

#if defined(STATS)

/* --------------------------------------------------------------------- */
/* The following functions are strictly for statistics gathering	 */

static
void
objectStatsChange(gc_unit* unit, int diff)
{
	gc_block* info;
	int idx;
	int objdiff;
	int memdiff;

	info = GCMEM2BLOCK(unit);
	idx = GCMEM2IDX(info, unit);
	objdiff = diff * 1;
	memdiff = diff * GCBLOCKSIZE(info);

#define	CHECK_GC_TYPE(GCFUNCS,STAT)				\
	if (GC_GET_FUNCS(info, idx) == (GCFUNCS)) {		\
		objectStats.STAT##Nr += objdiff;		\
		objectStats.STAT##Mem += memdiff;		\
	} else

	CHECK_GC_TYPE(GC_ALLOC_NORMAL, object)
	CHECK_GC_TYPE(GC_ALLOC_FINALIZEOBJECT, object)
	CHECK_GC_TYPE(GC_ALLOC_REFARRAY, array)
	CHECK_GC_TYPE(GC_ALLOC_PRIMARRAY, array)
	CHECK_GC_TYPE(GC_ALLOC_CLASSOBJECT, class)
	CHECK_GC_TYPE(GC_ALLOC_METHOD, method)
	CHECK_GC_TYPE(GC_ALLOC_FIELD, field)
	CHECK_GC_TYPE(GC_ALLOC_STATICDATA, static)
	CHECK_GC_TYPE(GC_ALLOC_DISPATCHTABLE, dispatch)
	CHECK_GC_TYPE(GC_ALLOC_BYTECODE, bytecode)
	CHECK_GC_TYPE(GC_ALLOC_EXCEPTIONTABLE, exception)
	CHECK_GC_TYPE(GC_ALLOC_CONSTANT, constant)
	CHECK_GC_TYPE(GC_ALLOC_UTF8CONST, utf8const)
	CHECK_GC_TYPE(GC_ALLOC_INTERFACE, interface)
	CHECK_GC_TYPE(GC_ALLOC_JITCODE, jit)
	CHECK_GC_TYPE(GC_ALLOC_LOCK, lock)
	CHECK_GC_TYPE(GC_ALLOC_FIXED, fixed)
	CHECK_GC_TYPE(GC_ALLOC_REF, ref)
	CHECK_GC_TYPE(GC_ALLOC_THREADCTX, threadCtx)

	/* Else .... */
	{
		objectStats.otherNr += objdiff;
		objectStats.otherMem += memdiff;
	}

#undef	CHECK_GC_TYPE

}

void
objectStatsPrint(void)
{
	int cnt = 0;

#define	PRINT_GC_STAT(STAT)						    \
	fprintf(stderr, "%14.14s: Nr %6d  Mem %6dK",			    \
		#STAT, objectStats.STAT##Nr, objectStats.STAT##Mem / 1024); \
	cnt++;								    \
	if (cnt % 2 != 0) {						    \
		fprintf(stderr, "   ");					    \
	}								    \
	else {								    \
		fprintf(stderr, "\n");					    \
	}

	fprintf(stderr, "Memory statistics:\n");
	fprintf(stderr, "------------------\n");

	PRINT_GC_STAT(object);
	PRINT_GC_STAT(array);
	PRINT_GC_STAT(class);
	PRINT_GC_STAT(method);
	PRINT_GC_STAT(field);
	PRINT_GC_STAT(static);
	PRINT_GC_STAT(dispatch);
	PRINT_GC_STAT(bytecode);
	PRINT_GC_STAT(exception);
	PRINT_GC_STAT(constant);
	PRINT_GC_STAT(utf8const);
	PRINT_GC_STAT(interface);
	PRINT_GC_STAT(jit);
	PRINT_GC_STAT(fixed);
	PRINT_GC_STAT(lock);
	PRINT_GC_STAT(ref);
	PRINT_GC_STAT(threadCtx);
	PRINT_GC_STAT(other);

	if (cnt % 2 != 0) {
		fprintf(stderr, "\n");
	}
		
#undef	PRINT_GC_STAT
}
#endif

GarbageCollectorInterface Kaffe_GarbageCollectorInterface = {

	gcMalloc,
	gcRealloc,
	gcFree,

	gcInvokeGC,
	gcInvokeFinalizer,

	gcAddRef,
	gcRmRef,

};
