/*
 * methodCache.c
 *
 * A faster way to find a translated method given a pc.
 * When dispatching an exception, we must map return PCs to Method*
 * in order to find exception handlers.  In this file, we implement
 * a hashtable that maps the start PC of a method to the Method itself.
 *
 * We use the gc interface to find the start PC of a method given a PC
 * within the method.
 *
 * NB: This code is currently only used in JIT3.
 *
 * Copyright (c) 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Written by Godmar Back <gback@cs.utah.edu>
 */

#include "config.h"
#include "debug.h"
#include "config-std.h"
#include "config-signal.h"
#include "config-mem.h"
#include "jtypes.h"
#include "classMethod.h"
#include "errors.h"
#include "methodCache.h"

/*
 * The overhead of this table is about (3 words + gc overhead) * #active meths
 * For instance, for 1024 methods, this would be (3 + 2.5) * 1024 * 4 = 22KB.
 *
 * There's an additional cost of METHCACHEHASHSZ words (0.5 KB)
 */
#define METHCACHEHASHSZ    128
typedef struct _methCacheEntry {
        void 				*pc;
	Method 				*meth;
        struct _methCacheEntry*      	next;
} methCacheEntry;

static struct {
        methCacheEntry*  hash[METHCACHEHASHSZ];
} methCacheTable;

/* This is a bit homemade.  We need a 7-bit hash from the address here */
#define METHCACHEHASH(V)   \
    ((((uintp)(V) >> 2) ^ ((uintp)(V) >> 9))%METHCACHEHASHSZ)

#if defined(JIT3)
/*
 * Find method containing pc.
 *
 * Find the method by the pc under which it was stored.
 * This is what we want to optimize.
 *
 * Should not need locking under these assumptions:
 * 	- adding is atomic and 
 *	- removal is done while other threads are stopped.
 */
Method*
findMethodFromPC(uintp pc)
{        
	void *pc_key = GC_getObjectBase(main_collector, (void*)pc);

        if (!pc_key) {                
		return (0);        
	} else {
		methCacheEntry *entry;

		entry = methCacheTable.hash[METHCACHEHASH(pc_key)]; 

		for (; entry != 0; entry = entry->next) {
			if (entry->pc == pc_key) {
				return (entry->meth);
			}
		}

		/* not found */
		return (0);
        }
}
#endif  /* defined (JIT3) */

/*
 * Make a method active.  We can then find it when backtracing
 * during exceptions.
 */
bool
makeMethodActive(Method* meth)
{
	unsigned int idx;
	methCacheEntry *entry;
	void *pc_key = METHOD_CODE_START(meth);

#if defined(DUMPMETHODCACHESTATS)
	static int f = 0;
	if (!f) {
		static void dumpMethodCacheStats(void);
		atexit(dumpMethodCacheStats);
		f = 1;
	}
#endif	/* defined(DUMPMETHODCACHESTATS) */

#if defined(DEBUG)
	/* paranoia */
	assert(findActiveMethod(pc_key) == 0);
#endif

	entry = (methCacheEntry*)KMALLOC(sizeof(methCacheEntry));
	if (entry == 0) {
		return (false);
	}

	idx = METHCACHEHASH(pc_key);
	entry->pc = pc_key;
	entry->meth = meth;
	entry->next = methCacheTable.hash[idx];
	methCacheTable.hash[idx] = entry;
	return (true);
}

/*
 * Make a method inactive before the class is unloaded.
 */
void
makeMethodInactive(Method* meth)
{
	methCacheEntry **entry;
	void *pc_key = METHOD_CODE_START(meth);

	entry = &methCacheTable.hash[METHCACHEHASH(pc_key)]; 

	for (; *entry != 0; entry = &(*entry)->next) {
                if ((*entry)->pc == pc_key) {
			methCacheEntry *me = *entry;	
                        (*entry) = me->next;
			KFREE(me);
                        break;
                }
        }
}

#if defined(DUMPMETHODCACHESTATS)
/*
 * A function to dump the length of the lists in the method cache
 * hashtable.  This is just to manually evaluate how good METHCACHEHASH is.
 * It looks ``good enough'' to me for now.
 */
static void
dumpMethodCacheStats(void)
{
	int i;
	int min = -1, max, avg = 0, total = 0, empty = 0;
	for(i = 0; i < METHCACHEHASHSZ; i++) {
		methCacheEntry *entry = methCacheTable.hash[i];
		int l = 0;
		while (entry) {
			l++;
			total++;
			entry = entry->next;
		}
		fprintf(stderr, "[%3d] -> %d\n", i, l);

		if (l == 0) {
			empty++;
		}
		avg += l;
		if (min == -1) {
			min = max = l;
		}
		if (l < min) {
			min = l;
		}
		if (l > max) {
			max = l;
		}
	}
	avg /= METHCACHEHASHSZ;
	fprintf(stderr,
		"MethodCache: %d anchors, min list %d, max list %d, avg %d, "
		"%d empty lists, total of %d entries\n",
		METHCACHEHASHSZ, min, max, avg, empty, total);
}
#endif	/* defined(DUMPMETHODCACHESTATS) */

