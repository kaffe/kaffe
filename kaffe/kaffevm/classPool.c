/*
 * classPool.c
 * Methods dealing with the class entry pool
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "debug.h"
#include "config-std.h"
#include "config-mem.h"
#include "errors.h"
#include "itypes.h"
#include "access.h"
#include "file.h"
#include "thread.h"
#include "jthread.h"
#include "classMethod.h"
#include "md.h"
#include "support.h"
#include "stringSupport.h"
#include "stats.h"
#include "gc.h"
#include "locks.h"
#include "md.h"

#define	CLASSHASHSZ	256	/* Must be a power of two */
static iLock* classHashLock;
static classEntry* classEntryPool[CLASSHASHSZ];
#if defined(KAFFE_STATS)
statobject classStats;
void statClassPool(void);
#endif

/*
 * Lookup an entry for a given (name, loader) pair.
 * Return null if none is found.
 */
classEntry*
lookupClassEntryInternal(Utf8Const* name, Hjava_lang_ClassLoader* loader)
{
	classEntry* entry;

	entry = classEntryPool[utf8ConstHashValue(name) & (CLASSHASHSZ-1)];
	for (; entry != 0; entry = entry->next) {
		if (utf8ConstEqual(name, entry->name) && loader == entry->loader) {
			return (entry);
		}
	}
	return (0);
}

/*
 * Lookup an entry for a given (name, loader) pair.  
 * Create one if none is found.
 */
classEntry*
lookupClassEntry(Utf8Const* name, Hjava_lang_ClassLoader* loader,
		 errorInfo *einfo)
{
	classEntry* entry;
	classEntry** entryp;
	int iLockRoot;
	static int f = 0;

        if (f == 0) {
		f++;
		registerUserCounter(&classStats, "class-pool", statClassPool);
        }

	entry = lookupClassEntryInternal(name, loader);
	if (entry != 0)
		return (entry);

	/* Failed to find class entry - create a new one */
	entry = KMALLOC(sizeof(classEntry));
	if (entry == 0) {
		postOutOfMemory(einfo);
		return (0);
	}
	entry->name = name;
	entry->loader = loader;
	entry->class = 0;
	entry->next = 0;

	/* Lock the class table and insert entry into it (if not already
	   there) */
        lockStaticMutex(&classHashLock);

	entryp = &classEntryPool[utf8ConstHashValue(name) & (CLASSHASHSZ-1)];
	for (; *entryp != 0; entryp = &(*entryp)->next) {
		if (utf8ConstEqual(name, (*entryp)->name) && loader == (*entryp)->loader) {
			/* Someone else added it - discard ours and return
			   the new one. */
			unlockStaticMutex(&classHashLock);
			KFREE(entry);
			return (*entryp);
		}
	}

	/* Add ours to end of hash */
	*entryp = entry;
	addToCounter(&cpemem, "vmmem-class entry pool", 1, GCSIZEOF(entry));

	/* 
	 * This reference to the utf8 name will be released if and when this 
	 * class entry is freed in finalizeClassLoader.
	 */
	utf8ConstAddRef(entry->name);

        unlockStaticMutex(&classHashLock);

	return (entry);
}

#if defined(TRANSLATOR) && !defined(JIT3)
/*
 * Find method containing pc.
 * This is the old version that linearly scans the class entry pool.
 *
 * It is only used in JIT2.  JIT3 uses a faster implementation
 * in methodCache.c.  JIT2 could use that implementation too if calls 
 * to makeMethodActive are added.
 *
 * For now, let's leave this code here.  It could be used a potential
 * fallback for gc systems that would not support gcGetObjectBase().
 *
 * Note that the only reason this code is in this file is that
 * the classEntryPool is a private structure --- what we'd really need
 * here is an "map" function over the elements in the class entry pool.
 */
Method*
findMethodFromPC(uintp pc)
{
	classEntry* entry;
	Method* ptr;
	int ipool;
	int imeth;

	for (ipool = CLASSHASHSZ;  --ipool >= 0; ) {
		for (entry = classEntryPool[ipool];  entry != NULL; entry = entry->next) {
			if (entry->class != 0) {
				imeth = CLASS_NMETHODS(entry->class);
				ptr = CLASS_METHODS(entry->class);
				for (; --imeth >= 0;  ptr++) {
					uintp ncode;
					if (!METHOD_TRANSLATED(ptr)) {
						continue;
					}
					ncode = (uintp)METHOD_NATIVECODE(ptr);
					if (pc >= ncode && pc < (uintp)ptr->c.ncode.ncode_end) {
						return (ptr);
					}
				}
			}
		}
	}
	return (NULL);
}
#endif

/*
 * Remove all entries from the class entry pool that belong to a given
 * class.  Return the number of entries removed.
 */
int
removeClassEntries(Hjava_lang_ClassLoader* loader)
{
	classEntry** entryp;
	classEntry* entry;
	int ipool;
	int totalent = 0;
	int iLockRoot;

        lockStaticMutex(&classHashLock);
	for (ipool = CLASSHASHSZ;  --ipool >= 0; ) {
		entryp = &classEntryPool[ipool];
		for (;  *entryp != NULL; entryp = &(*entryp)->next) {
			entry = *entryp;
			if (entry->loader == loader) {
				/*
				 * If class gc is turned off, no classloader 
				 * should ever be finalized because they're all 
				 * kept alive by their respective classes.
				 */
				assert(entry->class == 0 || 
					Kaffe_JavaVMArgs[0].enableClassGC != 0);
DBG(CLASSGC,
				dprintf("removing %s l=%p/c=%p\n", 
				    entry->name->data, loader, entry->class);
    )
				/* release reference to name */
				utf8ConstRelease(entry->name);
				(*entryp) = entry->next;
				addToCounter(&cpemem, "vmmem-class entry pool",
					1, -(jlong)GCSIZEOF(entry));
				KFREE(entry);
				totalent++;
			}
			/* if this was the last item, break */
			if (*entryp == 0)
				break;
		}
	}
        unlockStaticMutex(&classHashLock);
	return (totalent);
}

/*
 * Finalize a classloader and remove its entries in the class entry pool.
 */
void
finalizeClassLoader(Hjava_lang_ClassLoader* loader)
{
        int rmoved;
 
DBG(CLASSGC,
        dprintf("Finalizing classloader @%p\n", loader);
    )
        rmoved = removeClassEntries(loader);
   
DBG(CLASSGC,
        dprintf("removed entries from class entry pool: %d\n", rmoved);
    )
}

/*
 * this is a diagnostic function that does a sanity check
 */
void
checkClass(Hjava_lang_Class *c, Hjava_lang_ClassLoader *loader)
{
	classEntry* entry;
	int ipool;

	for (ipool = CLASSHASHSZ;  --ipool >= 0; ) {
		entry = classEntryPool[ipool];
		for (; entry != NULL; entry = entry->next) {
			/* we may be leaking classPool entries because
			 * finalizers are never run.  Running
			 * finalizers can be hard, since we need to
			 * know what process a class came from.
			 */
			if (entry->class == c && entry->loader != loader) {
				fprintf(stderr, "class %s@%p ",
					describeObject(c), c);
				fprintf(stderr, " referenced by initiating"
					" loader %s@%p",
					describeObject(entry->loader),
					entry->loader);
				fprintf(stderr, " but not defining loader"
					" %s@%p\n",
					describeObject(loader),
					loader);
				ABORT();
			}
		}
	}
}


#if defined(KAFFE_STATS) || defined(KAFFE_PROFILER)
/**
 * Walk the class pool and invoke walker() for each classes
 */
void
walkClassPool(int (*walker)(Hjava_lang_Class *clazz, void *), void *param)
{
	int ipool;
	classEntry* entry;

	assert(walker != NULL);

	for (ipool = CLASSHASHSZ;  --ipool >= 0; ) {
		entry = classEntryPool[ipool];
		for (; entry != NULL; entry = entry->next) {
			if (entry->class) {
				walker(entry->class, param);
			}
		}
	}
}
#endif


#if defined(KAFFE_STATS)
static void
statClass(Hjava_lang_Class *clazz, int *total)
{
	Collector *c = main_collector;
	int misc = 0;
	int miscfixed = 0;
	int bytecodemem = 0, jitmem = 0;

#define SIZE_IFNONZERO(x)  ((x) ? GC_getObjectSize(c, (x)) : 0)
				
	/* compute sizes of miscellaneous data */
	/* we tally up the memory for KMALLOC'ed objects (they'll appear
	 * under "other-fixed".) and the memory for other misc objects
	 */
	misc += SIZE_IFNONZERO(clazz);		/* myself */
	misc += SIZE_IFNONZERO(clazz->dtable);
	misc += SIZE_IFNONZERO(CLASS_CONSTANTS(clazz)->data);
	misc += SIZE_IFNONZERO(CLASS_FIELDS(clazz));
	misc += SIZE_IFNONZERO(CLASS_STATICDATA(clazz));

	miscfixed += SIZE_IFNONZERO(clazz->if2itable);
	miscfixed += SIZE_IFNONZERO(clazz->itable2dtable);
	miscfixed += SIZE_IFNONZERO(clazz->gc_layout);
	miscfixed += SIZE_IFNONZERO(clazz->sourcefile);
	miscfixed += SIZE_IFNONZERO(clazz->implementors);
	if (!CLASS_IS_ARRAY(clazz)) {
		misc += SIZE_IFNONZERO(clazz->interfaces);
	}

	/* methods only if we have some */
	if (!CLASS_IS_PRIMITIVE(clazz) && !CLASS_IS_ARRAY(clazz) 
		&& CLASS_METHODS(clazz) != 0) 
	{
		Method *m = CLASS_METHODS(clazz);
		int i, n = CLASS_NMETHODS(clazz);
		misc += SIZE_IFNONZERO(CLASS_METHODS(clazz));
		for (i = 0; i < n; m++, i++) {
			miscfixed += SIZE_IFNONZERO(m->lines);
			miscfixed += SIZE_IFNONZERO(m->declared_exceptions);
			misc += SIZE_IFNONZERO(m->exception_table);
			/* bytecode or jitted code */
			if ((m->accflags & ACC_NATIVE) == 0) {
				if (METHOD_TRANSLATED(m)) {
					jitmem += SIZE_IFNONZERO(m->c.ncode.ncode_start);
				} else {
					bytecodemem += SIZE_IFNONZERO(m->c.bcode.code);
				}
			}
		}
	}

	fprintf(stderr, "%7d %7d %7d %7d %s\n",
		misc,
		miscfixed,
		jitmem,
		bytecodemem,
		CLASS_CNAME(clazz));

	total[0] += misc;
	total[1] += miscfixed;
	total[2] += jitmem;
	total[3] += bytecodemem;
}

void
statClassPool(void)
{
	int total[20];
		 
	memset(total, 0, sizeof total);
	fprintf(stderr, "#DUMPING CLASSPOOL MEMORY\n");
	fprintf(stderr, "%-7s %-7s %-7s %-7s %-7s\n",
		"#MISC", "MISCFIX", "JCODE", "BCODE", "CLASS");
	fprintf(stderr, "%-7s %-7s %-7s %-7s %-7s\n",
		"#------", "-------", "-------", "-------", "--------------\n");
	walkClassPool(statClass, total);
	fprintf(stderr, "%7.2f %7.2f %7.2f %7.2f KBytes\n",
		total[0]/1024.0, total[1]/1024.0, total[2]/1024.0,
		total[3]/1024.0);
	fflush(stderr);
}
#endif

