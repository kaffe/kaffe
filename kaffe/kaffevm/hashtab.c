/*
 * hashtab.c
 *
 * "Intern" hash table library
 *
 * Copyright (c) 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "gtypes.h"
#include "gc.h"
#include "jsyscall.h"
#include "locks.h"
#include "hashtab.h"

/* Initial size */
#define INITIAL_SIZE		1024

/* When to increase size of hash table */
#define NEED_RESIZE(tab)	(4 * (tab)->count >= 3 * (tab)->size)

/* Valid client-set flags */
#define HASH_CLIENT_FLAGS	(HASH_ADD_REFS|HASH_SYNCHRONIZE|HASH_REENTRANT)

/* Generate step from hash. Note that "step" must always be
   relatively prime to tab->size. */
#define LIST_STEP(hash)		(8 * (hash) + 7)

/* Hashtable structure */
struct _hashtab {
	const void	**list;	/* List of pointers to whatever */
	int		count;	/* Number of slots used in the list */
	int		size;	/* Total size list; always a power of 2 */
	int		flags;	/* Flags */
	iLock		lock;	/* Mutex for synchronizing threads */
	compfunc_t	comp;	/* Comparison function */
	hashfunc_t	hash;	/* Hash function */
	allocfunc_t	alloc;	/* Allocation function */
	freefunc_t	free;	/* Free function */
};

/* Internal functions */
static int		hashFindSlot(hashtab_t, const void *ptr);
static void		hashResize(hashtab_t tab);

/* Indicates a deleted pointer */
static const void	*const DELETED = (const void *)&DELETED;

/*
 * Create a new hashtable
 */
hashtab_t
hashInit(hashfunc_t hash, compfunc_t comp, allocfunc_t alloc, freefunc_t free, int flags)
{
	hashtab_t tab;

	/* Use specified alloc function if one is given, fall back to KFREE */
	if (alloc == 0) {
		tab = KCALLOC(1, sizeof(*tab));
	} else {
		tab = alloc(sizeof(*tab));
	}
	if (tab == 0) {
		assert(!"hashInit out of memory"); /* XXX OutOfMemoryError? */
	}
	tab->hash = hash;
	tab->comp = comp;
	tab->alloc = alloc;
	tab->free = free;
	tab->flags = (flags & HASH_CLIENT_FLAGS);
	if (tab->flags & HASH_SYNCHRONIZE) {
		initStaticLock(&tab->lock);
	}
	return(tab);
}

/*
 * Destroy a hash table.
 */
extern void
hashDestroy(hashtab_t tab)
{
	int k;

	/* Lock table */
	if (tab->flags & HASH_SYNCHRONIZE) {
		lockStaticMutex(&tab->lock);
	}

	/* Remove all entries in the table */
	for (k = 0; k < tab->size; k++) {
		if (tab->list[k] != NULL && tab->list[k] != DELETED) {
			hashRemove(tab, tab->list[k]);
		}
	}

	/* Nuke the table */
	/* XXX should free the lock here */
	if (tab->free) {
		tab->free(tab->list);
		tab->free(tab);
	} else {
		KFREE(tab->list);
		KFREE(tab);
	}
}

/*
 * Add an entry to the hash table. It's OK if the entry is already there,
 * or is equal to something that is already there. This returns the
 * matching pointer that is actually in the table.
 */
void *
hashAdd(hashtab_t tab, const void *ptr)
{
	int	index;
	void	*rtn;

	/* Lock table */
	if (tab->flags & HASH_SYNCHRONIZE) {
		lockStaticMutex(&tab->lock);
	}

	if (NEED_RESIZE(tab)) {
		hashResize(tab);
	}
	index = hashFindSlot(tab, ptr);
	assert(index != -1);
	if (tab->list[index] == NULL || tab->list[index] == DELETED) {
		if (tab->flags & HASH_ADD_REFS) {
			gc_add_ref(ptr);
		}
		tab->list[index] = ptr;
		tab->count++;
	}
	rtn = (void *) tab->list[index];

	/* Unlock table */
	if (tab->flags & HASH_SYNCHRONIZE) {
		unlockStaticMutex(&tab->lock);
	}
	return(rtn);
}

/*
 * Remove a pointer. If the pointer is not itself in the table,
 * we don't remove it.
 */
void
hashRemove(hashtab_t tab, const void *ptr)
{
	int index;

	/* Lock table */
	if (tab->flags & HASH_SYNCHRONIZE) {
		lockStaticMutex(&tab->lock);
	}

	index = hashFindSlot(tab, ptr);
	assert(index != -1);
	if (tab->list[index] != NULL
	    && tab->list[index] != DELETED
	    && tab->list[index] == ptr) {
		tab->count--;
		tab->list[index] = DELETED;
		if (tab->flags & HASH_ADD_REFS) {
			gc_rm_ref(ptr);
		}
	}

	/* Unlock table */
	if (tab->flags & HASH_SYNCHRONIZE) {
		unlockStaticMutex(&tab->lock);
	}
}

/*
 * Find a matching pointer in the table.
 */
void *
hashFind(hashtab_t tab, const void *ptr)
{
	int index;
	void *rtn;

	/* Lock table */
	if (tab->flags & HASH_SYNCHRONIZE) {
		lockStaticMutex(&tab->lock);
	}

	index = hashFindSlot(tab, ptr);
	assert(index != -1);
	rtn = (tab->list[index] == DELETED) ?
		NULL : (void *) tab->list[index];

	/* Unlock table */
	if (tab->flags & HASH_SYNCHRONIZE) {
		unlockStaticMutex(&tab->lock);
	}
	return(rtn);
}

/*
 * Find if an equal pointer is already in the table. If found,
 * return it; otherwise return a free slot for it.
 * Assumes the table is locked already (if HASH_SYNCHRONIZE).
 */
static int
hashFindSlot(hashtab_t tab, const void *ptr)
{
	const int hash = (*tab->hash)(ptr);
	const int startIndex = hash & (tab->size - 1);
	const int step = LIST_STEP(hash);
	int index, deletedIndex = -1;

	/* Sanity check */
	if (ptr == NULL || ptr == DELETED) {
		return(-1);
	}

	/* Find slot */
	index = startIndex;
	for (;;) {
		const void **const ptr2 = &tab->list[index];

		if (*ptr2 == NULL) {
			return (deletedIndex >= 0) ? deletedIndex : index;
		}
		if (*ptr2 == DELETED) {
			if (deletedIndex == -1) {
				deletedIndex = index;
			}
		} else if (*ptr2 == ptr || (*tab->comp)(ptr, *ptr2) == 0) {
			return(index);
		}
		index = (index + step) & (tab->size - 1);

		/* Check for looping all the way through the table */
		if (index == startIndex) {
                        if (deletedIndex >= 0) {
                                return(deletedIndex);
                        }
			assert(!"hashFindSlot: no slot!");
		}
	}
}

/*
 * Make the table bigger.
 * Assumes the table is locked already (if HASH_SYNCHRONIZE).
 */
static void
hashResize(hashtab_t tab)
{
	const int newSize = (tab->size > 0) ? (tab->size * 2) : INITIAL_SIZE;
	const void **newList;
	int index;

	/* Get a bigger list */
	if (tab->alloc) {
		newList = tab->alloc(newSize * sizeof(*newList));
	} else {
		newList = KCALLOC(newSize, sizeof(*newList));
	}

	/* If the hashtable is reentrant, it is possible that the table
	 * no longer needs resizing.  This could happen if the CALLOC
	 * caused a garbage collection which uninterned a lot of strings,
	 * for instance.
	 */
	if (tab->flags & HASH_REENTRANT) {
		if (!NEED_RESIZE(tab)) {
			if (tab->free) {
				tab->free(newList);
			} else {
				KFREE(newList);
			}
			return;
		}
	}

	if (newList == NULL) {
		assert(!"hashResize out of memory"); /* XXX OutOfMemoryError? */
	}

	/* Rehash old list contents into new list */
	for (index = tab->size - 1; index >= 0; index--) {
		const void *ptr = tab->list[index];

		if (ptr != NULL && ptr != DELETED) {
			const int hash = (*tab->hash)(ptr);
			const int step = LIST_STEP(hash);
			int index;

			for (index = hash & (newSize - 1);
			    newList[index] != NULL;
			    index = (index + step) & (newSize - 1));
			newList[index] = ptr;
		}
	}

	/* Update table */
	if (tab->free) {
		tab->free(tab->list);
	} else {
		KFREE(tab->list);
	}
	tab->list = newList;
	tab->size = newSize;
}

