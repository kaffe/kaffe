/* gc-incremental.c
 * The garbage collector.
 * The name is misleading.  GC is non-incremental at this point.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#define	STATS

#include "config.h"
#include "debug.h"
#include "config-std.h"
#include "config-mem.h"
#include "gtypes.h"
#include "gc.h"
#include "locks.h"
#include "thread.h"
#include "errors.h"
#include "exception.h"
#include "external.h"
#include "lookup.h"
#include "soft.h"
#include "md.h"
#include "jni.h"
#include "access.h"
#include "stringSupport.h"

static gcList gclists[5];
static int mustfree = 4;		/* temporary list */
static int white = 3;
static int grey = 2;
static int black = 1;
static int finalise = 0;

static int gcRunning = 0;
static bool finalRunning = false;
static timespent gc_time;
static timespent sweep_time;

#if defined(STATS)

/* Class and object space statistics */
static struct {
	int	classNr;
	int	objectNr;
	int	stringNr;
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
	int	stringMem;
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
static void objectStatsPrint(void);

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
iLock gc_lock;			/* allocator mutex */

int gc_mode = GC_DISABLED;	/* GC will be enabled after the first
				 * thread is setup.
				 */
static void gcFree(void*);

static void finalizeObject(void*);

void walkMemory(void*);

static void walkNull(void*, uint32);
static void walkClass(void*, uint32);
static void walkMethods(Method*, int);
static void walkObject(void*, uint32);
static void walkRefArray(void*, uint32);
#define walkPrimArray walkNull

/* Standard GC function sets */
static gcFuncs gcFunctions[] = {
  { walkString,       GC_OBJECT_NORMAL, destroyString },   /* JAVASTRING */
  { walkNull,	      GC_OBJECT_NORMAL, 0 },  		   /* NOWALK */
  { walkObject,	      GC_OBJECT_NORMAL, 0 },  		   /* NORMALOBJECT */
  { walkPrimArray,    GC_OBJECT_NORMAL, 0 },  		   /* PRIMARRAY */
  { walkRefArray,     GC_OBJECT_NORMAL, 0 },  		   /* REFARRAY */
  { walkClass,        GC_OBJECT_NORMAL, destroyClass },    /* CLASSOBJECT */
  { walkObject,       finalizeObject,   0 },   		   /* FINALIZEOBJECT */
  { walkNull,	      GC_OBJECT_FIXED,  0 }, 		   /* BYTECODE */
  { walkNull, 	      GC_OBJECT_FIXED,  0 },   		   /* EXCEPTIONTABLE */
  { walkNull,         GC_OBJECT_FIXED,  0 },  		   /* JITCODE */
  { walkNull,	      GC_OBJECT_FIXED,  0 },   		   /* STATICDATA */
  { walkNull, 	      GC_OBJECT_FIXED,  0 },		   /* CONSTANT */
  { walkNull,	      GC_OBJECT_FIXED,  0 },  		   /* FIXED */
  { walkNull, 	      GC_OBJECT_FIXED,  0 }, 		   /* DISPATCHTABLE */
  { walkNull, 	      GC_OBJECT_FIXED,  0 },		   /* METHOD */
  { walkNull, 	      GC_OBJECT_FIXED,  0 },		   /* FIELD */
  { walkNull,	      GC_OBJECT_FIXED,  0 },		   /* UTF8CONST */
  { walkNull, 	      GC_OBJECT_FIXED,  0 },		   /* INTERFACE */
  { walkNull,	      GC_OBJECT_FIXED,  0 },		   /* LOCK */
  { walkNull,	      GC_OBJECT_FIXED,  0 },		   /* THREADCTX */
  { walkNull,	      GC_OBJECT_FIXED,  0 },		   /* REF */
};

#define	REFOBJALLOCSZ	128
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
#define	REFOBJHASH(V)		((((uintp)(V))/(2*sizeof(uintp)))%REFOBJHASHSZ)

/* For statistics gathering, record how many objects and how 
 * much memory was marked.
 */
#define RECORD_MARKED(nr_of_objects, size)	\
        gcStats.markedobj += nr_of_objects;	\
	gcStats.markedmem += size;

struct _gcStats gcStats;
extern size_t gc_heap_total;
extern size_t gc_heap_limit;
extern Hjava_lang_Class* ThreadClass;

static void startGC(void);
static void finishGC(void);
static void markObjectDontCheck(gc_unit *unit, gc_block *info, int idx);

/*
 * Initalise the Garbage Collection system.
 */
static
void
initGc(void)
{
	initStaticLock(&gc_lock);
	URESETLIST(gclists[white]);
	URESETLIST(gclists[grey]);
	URESETLIST(gclists[black]);
	URESETLIST(gclists[finalise]);
	URESETLIST(gclists[mustfree]);
}

/* Return true if gc_unit is pointer to an allocated object */
static inline int
gc_heap_isobject(gc_block *info, gc_unit *unit)
{
	uintp p = (uintp) UTOMEM(unit) - gc_heap_base;
	int idx;

	if (!(p & (MEMALIGN - 1)) && p < gc_heap_range && info->inuse) {
		/* Make sure 'unit' refers to the beginning of an
		 * object.  We do this by making sure it is correctly
		 * aligned within the block.
		 */
		idx = GCMEM2IDX(info, unit);
		if (idx < info->nr && GCBLOCK2MEM(info, idx) == unit
		    && (GC_GET_COLOUR(info, idx) & GC_COLOUR_INUSE) ==
		    GC_COLOUR_INUSE) {
			return 1;
		}
	}
	return 0;
}

/*
 * Mark the memory as reached if it really is an object.
 */
void
markObject(void* mem)
{
	gc_block* info;
	gc_unit* unit;

	/*
	 * First we check to see if the memory 'mem' is in fact the
	 * beginning of an object.  If not we just return.
	 */

	/* Get block info for this memory - if it exists */
	info = GCMEM2BLOCK(mem);
	unit = UTOUNIT(mem);
	if (gc_heap_isobject(info, unit)) {
		markObjectDontCheck(unit, info, GCMEM2IDX(info, unit));
	}
}

#define MARK_OBJECT_PRECISE(obj) do { \
  const void *objp = obj; \
  if (objp) { \
    gc_unit *unit = UTOUNIT(objp); \
    gc_block *info = GCMEM2BLOCK(unit); \
    int idx = GCMEM2IDX(info, unit); \
    markObjectDontCheck(unit, info, idx); \
  } \
} while (0)

static void
markObjectDontCheck(gc_unit *unit, gc_block *info, int idx)
{
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

DBG(GCWALK,	
	dprintf("walkConservative: %x-%x\n", base, base+size);
    )

	RECORD_MARKED(1, size)

	if (size > 0) {
		for (mem = ((int8*)base) + (size & -ALIGNMENTOF_VOIDP) - sizeof(void*); (void*)mem >= base; mem -= ALIGNMENTOF_VOIDP) {
			void *p = *(void **)mem;
			if (p) {
				markObject(p);
			}
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
	Hjava_lang_Class *clazz;
	int *layout;
	int8* mem;
	int i, l, nbits;

	/* 
	 * Note that there is a window after the object is allocated but
	 * before dtable is set.  In this case, we don't have to walk anything.
	 */
	if (obj->dtable == 0)
		return;

	/* retrieve the layout of this object from its class */
	clazz = obj->dtable->class;

	/* class without a loader, i.e., system classes are anchored so don't
	 * bother marking them.
	 */
	if (clazz->loader != 0) {
		MARK_OBJECT_PRECISE(clazz);
	}

	layout = clazz->gc_layout;
	nbits = CLASS_FSIZE(clazz)/ALIGNMENTOF_VOIDP;

DBG(GCWALK,
	dprintf("walkObject `%s' ", CLASS_CNAME(clazz));
	BITMAP_DUMP(layout, nbits)
	dprintf(" (nbits=%d) %x-%x\n", nbits, base, base+size);
    )

	assert(CLASS_FSIZE(clazz) > 0);
	assert(size > 0);

	RECORD_MARKED(1, size)

	mem = (int8 *)base;

	/* optimize this! */
	while (nbits > 0) {
		/* get next integer from bitmap */
		l = *layout++;
		i = 0;
		while (i < BITMAP_BPI) {
			/* skip the rest if no refs left */
			if (l == 0) {	
				mem += (BITMAP_BPI - i) * ALIGNMENTOF_VOIDP;
				break;
			}

			if (l < 0) {
				/* we know this pointer points to gc'ed memory
				 * there is no need to check - go ahead and 
				 * mark it.  Note that p may or may not point
				 * to a "real" Java object.
				 */
				MARK_OBJECT_PRECISE(*(void **)mem);
			}
			i++;
			l <<= 1;
			mem += ALIGNMENTOF_VOIDP;
		}
		nbits -= BITMAP_BPI;
	}

	/* Special magic to handle thread objects */
	if (soft_instanceof(ThreadClass, obj)) {
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
	RECORD_MARKED(1, size)
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

	assert(GC_GET_FUNCS(info, idx) < 
		sizeof(gcFunctions)/sizeof(gcFunctions[0]));
	(*gcFunctions[GC_GET_FUNCS(info, idx)].walk)(mem, GCBLOCKSIZE(info));
}

/*
 * Walk a java.lang.String object
 */
void               
walkString(void* str, uint32 size)
{
	/* That's all we have to do here */
	MARK_OBJECT_PRECISE(unhand((Hjava_lang_String*)str)->value);
}

/*
 * Destroy a string object.
 */
void               
destroyString(void* obj)
{
	Hjava_lang_String* str = (Hjava_lang_String*)obj;

	/* unintern this string if necessary */
	if (unhand(str)->interned == true) {
		stringUninternString(str);
	}
}

/*
 * Walk the methods of a class.
 */
static
void
walkMethods(Method* m, int nm)
{
	RECORD_MARKED(1, nm * sizeof(Method))		
	while (nm-- > 0) {
#if defined(TRANSLATOR)
		/* walk the block of jitted code conservatively.
		 * Is this really needed? 
		 * XXX: this will break encapsulation later on */
		if (METHOD_TRANSLATED(m) && (m->accflags & ACC_NATIVE) == 0) {
			void *mem = m->c.ncode.ncode_start;
			if (mem != 0) {
				uint32 len;
				len = GCBLOCKSIZE(GCMEM2BLOCK(UTOUNIT(mem)));
				walkConservative(mem, len);
			}
		}
#endif
		MARK_OBJECT_PRECISE(m->class);

		/* walk exception table in order to keep resolved catch types
		   alive */
		if (m->exception_table != 0) {
			jexceptionEntry* eptr = &m->exception_table->entry[0];
			int i;

			for (i = 0; i < m->exception_table->length; i++) {
				Hjava_lang_Class* c = eptr[i].catch_type;
				if (c != 0 && c != UNRESOLVABLE_CATCHTYPE) {
					MARK_OBJECT_PRECISE(c);
				}
			}
		}

		/* NB: need to mark ncode only if it points to a trampoline */
		if(!METHOD_TRANSLATED(m) && (m->ncode != 0)) {
			markObject(m->ncode);		/* XXX */
		}
		m++;
	}
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
	constants* pool;
	int idx;

	RECORD_MARKED(1, size)

	class = (Hjava_lang_Class*)base;

	if (class->state >= CSTATE_PREPARED) {
		MARK_OBJECT_PRECISE(class->superclass);
	}

	/* walk constant pool - only resolved classes and strings count */
	pool = CLASS_CONSTANTS(class);
	for (idx = 0; idx < pool->size; idx++) {
		switch (pool->tags[idx]) {
		case CONSTANT_ResolvedClass:
			MARK_OBJECT_PRECISE(CLASS_CLASS(idx, pool));
			break;
		case CONSTANT_ResolvedString:
			MARK_OBJECT_PRECISE((void*)pool->data[idx]);
			break;
		}
	}

	/* walk fields */
	if (CLASS_FIELDS(class) != 0) {
		RECORD_MARKED(1, CLASS_NFIELDS(class) * sizeof(Field));

		/* walk all fields to keep their types alive */
		fld = CLASS_FIELDS(class);
		for (n = 0; n < CLASS_NFIELDS(class); n++) {
			if (FIELD_RESOLVED(fld)) {
				MARK_OBJECT_PRECISE(fld->type);
			} /* else it's an Utf8Const that is not subject to gc */
			fld++;
		}

		/* walk static fields that contain references */
		fld = CLASS_SFIELDS(class);
		for (n = 0; n < CLASS_NSFIELDS(class); n++) {
			if (FIELD_RESOLVED(fld) && FIELD_ISREF(fld)) {
				MARK_OBJECT_PRECISE(*(void**)FIELD_ADDRESS(fld));
			}
			fld++;
		}
	}

	/* The interface table for array classes points to static memory,
	 * so we must not mark it.  */
	if (!CLASS_IS_ARRAY(class)) {
		/* mark interfaces referenced by this class */
		for (n = 0; n < class->total_interface_len; n++) {
			MARK_OBJECT_PRECISE(class->interfaces[n]);
		}
	} else {
		/* array classes should keep their element type alive */
		MARK_OBJECT_PRECISE(CLASS_ELEMENT_TYPE(class));
	} 

	/* CLASS_METHODS only points to the method array for non-array and
	 * non-primitive classes */
	if (!CLASS_IS_PRIMITIVE(class) && !CLASS_IS_ARRAY(class) && CLASS_METHODS(class) != 0) {
		walkMethods(CLASS_METHODS(class), CLASS_NMETHODS(class));
	}
	MARK_OBJECT_PRECISE(class->loader);

	/* Walk the static data elements */
	if (class->state >= CSTATE_DOING_PREPARE) {
        	fld = CLASS_SFIELDS(class);
        	n = CLASS_NSFIELDS(class);
        	for (; --n >= 0; fld++) {
			if (FIELD_ISREF(fld)) {
				MARK_OBJECT_PRECISE(*(void**)FIELD_ADDRESS(fld));
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

	RECORD_MARKED(1, size)

	arr = (Hjava_lang_Object*)base;
	if (arr->dtable == 0) {			/* see walkObject */
		return;
	}

	ptr = OBJARRAY_DATA(arr);
	/* mark class only if not a system class (which would be anchored
	 * anyway.)  */
	if (arr->dtable->class->loader != 0) {
		MARK_OBJECT_PRECISE(arr->dtable->class);
	}

	for (i = ARRAY_SIZE(arr); --i>= 0; ) {
		Hjava_lang_Object* el = *ptr++;
		MARK_OBJECT_PRECISE(el);
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

	/* Wake up anyone waiting for the GC to finish every time we're done */
	for(;; broadcastStaticCond(&gcman)) {

		gcRunning = 0;
		while (gcRunning == 0) {
			waitStaticCond(&gcman, 0);
		}
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
    )
			continue;
                }

		/*
		 * Now try to decide whether we should postpone the gc and get
		 * some memory from the system instead.
		 *
		 * If we already use the maximum amount of memory, we must gc.
		 *
		 * Otherwise, wait until the newly allocated memory is at 
		 * least 1/3 of the total memory in use.  Assuming that the
		 * gc will collect all newly allocated memory, this would 
		 * asymptotically converge to a memory usage of approximately
		 * 3/2 the amount of long-lived and fixed data combined.
		 *
		 * Feel free to tweak this parameter.
		 */
		if (gcRunning == 1 && gc_heap_total < gc_heap_limit && 
		    gcStats.allocmem * 3 < gcStats.totalmem * 1) {
DBG(GCSTAT,
			dprintf("skipping collection since alloc/total "
				"%dK/%dK = %.2f < 1/3\n",
				gcStats.allocmem/1024, 
				gcStats.totalmem/1024,
				gcStats.allocmem/(double)gcStats.totalmem);
    )
			continue;
		}
		startGC();

		for (unit = gclists[grey].cnext; unit != &gclists[grey]; unit = gclists[grey].cnext) {
			walkMemory(UTOMEM(unit));
		}
		/* Now walk any white objects which will be finalized.  They
		 * may get reattached, so anything they reference must also
		 * be live just in case.
		 */
		for (unit = gclists[white].cnext; unit != &gclists[white]; unit = nunit) {
			nunit = unit->cnext;
			info = GCMEM2BLOCK(unit);
			idx = GCMEM2IDX(info, unit);
			if (GC_GET_STATE(info, idx) == GC_STATE_NEEDFINALIZE) {
				/* this assert is somewhat expensive */
				DBG(GCDIAG,
				    assert(gc_heap_isobject(info, unit)));
				GC_SET_STATE(info, idx, GC_STATE_INFINALIZE);
				markObjectDontCheck(unit, info, idx);
			}
		}
		/* We may now have more grey objects, so walk them */
		for (unit = gclists[grey].cnext; unit != &gclists[grey]; unit = gclists[grey].cnext) {
			walkMemory(UTOMEM(unit));
		}

		finishGC();

		if (Kaffe_JavaVMArgs[0].enableVerboseGC > 0) {
			/* print out all the info you ever wanted to know */
			fprintf(stderr, 
			    "<GC: heap %dK, total before %dK,"
			    " after %dK (%d/%d objs)\n %2.1f%% free,"
			    " alloced %dK (#%d), marked %dK, "
			    "swept %dK (#%d)\n"
			    " %d objs (%dK) awaiting finalization>\n",
			gc_heap_total/1024, 
			gcStats.totalmem/1024, 
			(gcStats.totalmem-gcStats.freedmem)/1024, 
			gcStats.totalobj,
			gcStats.totalobj-gcStats.freedobj,
			(1.0 - ((gcStats.totalmem-gcStats.freedmem)/
				(double)gc_heap_total)) * 100.0,
			gcStats.allocmem/1024,
			gcStats.allocobj,
			gcStats.markedmem/1024, 
			gcStats.freedmem/1024,
			gcStats.freedobj,
			gcStats.finalobj,
			gcStats.finalmem/1024);
		}
		if (Kaffe_JavaVMArgs[0].enableVerboseGC > 1) {
			OBJECTSTATSPRINT();
		}
		gcStats.totalmem -= gcStats.freedmem;
		gcStats.totalobj -= gcStats.freedobj;
		gcStats.allocobj = 0;
		gcStats.allocmem = 0;
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

	/* measure time */
	startTiming(&gc_time, "gc-scan");

	/* Walk the referenced objects */
	for (i = 0; i < REFOBJHASHSZ; i++) {
		for (robj = refObjects.hash[i]; robj != 0; robj = robj->next) {
			MARK_OBJECT_PRECISE(robj->mem);
		}
	}

	/* Walk all objects on the finalizer list */
	for (unit = gclists[finalise].cnext;
	     unit != &gclists[finalise]; unit = nunit) {
		nunit = unit->cnext;
		MARK_OBJECT_PRECISE(UTOMEM(unit));
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
		gcStats.freedmem += GCBLOCKSIZE(info);
		gcStats.freedobj += 1;
		UAPPENDLIST(gclists[mustfree], unit);
		OBJECTSTATSREMOVE(unit);
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
			gcStats.finalmem += GCBLOCKSIZE(info);
			gcStats.finalobj += 1;
			UAPPENDLIST(gclists[finalise], unit);
		}
		else {
			UAPPENDLIST(gclists[white], unit);
		}
		GC_SET_COLOUR(info, idx, GC_COLOUR_WHITE);
	}

	/* this is where we'll stop locking out other threads 
	 * measure gc time until here.  This is not quite accurate, as
	 * it excludes the time to sweep objects, but lacking
	 * per-thread timing it's a reasonable thing to do.
	 */
	stopTiming(&gc_time);

	/* 
	 * Now that all lists that the mutator manipulates are in a
	 * consistent state, we can reenable the mutator here 
	 */
	UNLOCK();

	/* 
	 * Now free the objects.  We can block here since we're the only
	 * thread manipulating the "mustfree" list.
	 */
	lockStaticMutex(&gc_lock);
	startTiming(&sweep_time, "gc-sweep");

	while (gclists[mustfree].cnext != &gclists[mustfree]) {
		void (*destroy)(void *);
		unit = gclists[mustfree].cnext;

		/* invoke destroy function before freeing the object */
		info = GCMEM2BLOCK(unit);
		idx = GCMEM2IDX(info, unit);
		destroy = gcFunctions[GC_GET_FUNCS(info,idx)].destroy;
		if (destroy != 0) {
			destroy(UTOMEM(unit));
		}

		UREMOVELIST(unit);
		gc_heap_free(unit);
	}
	stopTiming(&sweep_time);
	unlockStaticMutex(&gc_lock);

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
			gcStats.finalmem -= GCBLOCKSIZE(info);
			gcStats.finalobj -= 1;

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
gcInvokeGC(int mustgc)
{
	lockStaticMutex(&gcman);
	if (gcRunning == 0) {
		gcRunning = mustgc ? 2 : 1;
		signalStaticCond(&gcman);
	}
	waitStaticCond(&gcman, 0);
	unlockStaticMutex(&gcman);
}

/*
 * GC and invoke the finalizer.  Used to run finalizers on exit.
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
	void * volatile mem;	/* needed on SGI, see comment below */
	int i;
	size_t bsz;

	/* Initialise GC */
	if (gc_init == 0) {
		gc_init = 1;
		initGc();
	}

	assert(size != 0);
	lockStaticMutex(&gc_lock);
	unit = gc_heap_malloc(size + sizeof(gc_unit));

	/* keep pointer to object */
	mem = UTOMEM(unit);
	if (unit == 0) {
		unlockStaticMutex(&gc_lock);
		throwOutOfMemory();
	}

	info = GCMEM2BLOCK(mem);
	i = GCMEM2IDX(info, unit);

	bsz = GCBLOCKSIZE(info);
	gcStats.totalmem += bsz;
	gcStats.totalobj += 1;
	gcStats.allocmem += bsz;
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
	 * regime and must be freed explicitly.
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
		 * In addition, on some architectures (SGI), we must tell the
		 * compiler to not delay computing mem by defining it volatile.
		 */
		LOCK(); 
		GC_SET_COLOUR(info, i, GC_COLOUR_WHITE);
		UAPPENDLIST(gclists[white], unit);
		UNLOCK();
	}
	unlockStaticMutex(&gc_lock);
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
	int osize;

	assert(fidx == GC_ALLOC_FIXED);

	/* If nothing to realloc from, just allocate */
	if (mem == NULL) {
		return (gcMalloc(size, fidx));
	}

	lockStaticMutex(&gc_lock);
	unit = UTOUNIT(mem);
	info = GCMEM2BLOCK(unit);
	idx = GCMEM2IDX(info, unit);
	osize = GCBLOCKSIZE(info);

	/* Can only handled fixed objects at the moment */
	assert(GC_GET_COLOUR(info, idx) == GC_COLOUR_FIXED);
	info = 0;
	unlockStaticMutex(&gc_lock);

	/* If we'll fit into the current space, just send it back */
	if (osize >= size + sizeof(gc_unit)) {
		return (mem);
	}

	/* Allocate new memory, copy data, and free the old */
	newmem = gcMalloc(size, fidx);
	memcpy(newmem, mem, osize);
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
		lockStaticMutex(&gc_lock);
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
			assert(!!!"Attempt to explicitly free nonfixed object");
		}
		unlockStaticMutex(&gc_lock);
	}
}

/*
 * Add a presistent reference to an object.
 */
static
void
gcAddRef(const void* mem)
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
gcRmRef(const void* mem)
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
	Hjava_lang_Object* obj = (Hjava_lang_Object*)ob;
	Hjava_lang_Class* objclass = OBJECT_CLASS(obj);
	Method* final = objclass->finalizer;

	assert(final != 0);
	callMethodA(final, METHOD_INDIRECTMETHOD(final), obj, 0, 0);
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

	CHECK_GC_TYPE(GC_ALLOC_JAVASTRING, string)
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

static void
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
	PRINT_GC_STAT(string);
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
