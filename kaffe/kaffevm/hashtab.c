/*
 * hashtab.c
 *
 * "Intern" hash table library
 *
 * Copyright (c) 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 */

#include "config.h"
#include "debug.h"
#include "config-std.h"
#include "config-mem.h"
#include "gtypes.h"
#include "gc.h"
#include "hashtab.h"
#include "kaffe/jmalloc.h"

/* Initial size */
#define INITIAL_SIZE		1024

/* When to increase size of hash table */
#define NEED_RESIZE(tab)	(4 * (tab)->count >= 3 * (tab)->size)

/* Generate step from hash. Note that "step" must always be
   relatively prime to tab->size. */
#define LIST_STEP(hash)		(8 * (hash) + 7)

/* Hashtable structure */
struct _hashtab {
	const void	**list; 	/* List of pointers to whatever */
	int		count;  	/* Number of slots used in the list */
	int		size;   	/* Total size list; always a power of 2 */
	compfunc_t	comp;   	/* Comparison function */
	hashfunc_t	hash;   	/* Hash function */
	allocfunc_t	alloc;  	/* Allocation function */
	freefunc_t	dealloc;	/* Free function */
};

/* Internal functions */
static int		hashFindSlot(hashtab_t, const void *ptr);
static hashtab_t	hashResize(hashtab_t tab);

/* Indicates a deleted pointer */
static const void	*const DELETED = (const void *)&DELETED;

/*
 * Create a new hashtable
 */
hashtab_t
hashInit(hashfunc_t hash, compfunc_t comp, allocfunc_t alloc, freefunc_t dealloc)
{
	hashtab_t tab;

	/* Use specified alloc function if one is given, fall back to KFREE */
	if (alloc == 0) {
		tab = KCALLOC(1, sizeof(*tab));
	} else {
		tab = alloc(sizeof(*tab));
	}
	if (tab == 0) {
		return (NULL);
	}
	tab->hash = hash;
	tab->comp = comp;
	tab->alloc = alloc;
	tab->dealloc = dealloc;
	/* start out with initial size */
	return (hashResize(tab));
}

/*
 * Destroy a hash table.
 */
void
hashDestroy(hashtab_t tab)
{
	int k;

	/* Remove all entries in the table */
	for (k = 0; k < tab->size; k++) {
		if (tab->list[k] != NULL && tab->list[k] != DELETED) {
			hashRemove(tab, tab->list[k]);
		}
	}

	/* Nuke the table */
	if (tab->dealloc) {
		tab->dealloc(tab->list);
		tab->dealloc(tab);
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
const void *
hashAdd(hashtab_t tab, const void *ptr)
{
	int	i;
	const void	*rtn;

	if (NEED_RESIZE(tab)) {
		if (hashResize(tab) == 0) {
			/* XXX OutOfMemoryError? */
			return (NULL);
		}
	}
	i = hashFindSlot(tab, ptr);
	assert(i != -1);
	if (tab->list[i] == NULL || tab->list[i] == DELETED) {
		tab->list[i] = ptr;
		tab->count++;
	}
	rtn = tab->list[i];

	return(rtn);
}

/*
 * Remove a pointer. If the pointer is not itself in the table,
 * we don't remove it.
 */
void
hashRemove(hashtab_t tab, const void *ptr)
{
	int i;

	i = hashFindSlot(tab, ptr);
	assert(i != -1);
	if (tab->list[i] != NULL
	    && tab->list[i] != DELETED
	    && tab->list[i] == ptr) {
		tab->count--;
		tab->list[i] = DELETED;
	}
}

/*
 * Find a matching pointer in the table.
 */
const void *
hashFind(hashtab_t tab, const void *ptr)
{
	int i;
	const void *rtn;

	i = hashFindSlot(tab, ptr);
	assert(i != -1);
	rtn = (tab->list[i] == DELETED) ?
		NULL : tab->list[i];

	return(rtn);
}

/*
 * Find if an equal pointer is already in the table. If found,
 * return it; otherwise return a free slot for it.
 */
static int
hashFindSlot(hashtab_t tab, const void *ptr)
{
	const int hash = (*tab->hash)(ptr);
	const int startIndex = hash & (tab->size - 1);
	const int step = LIST_STEP(hash);
	int i, deletedIndex = -1;

	/* Sanity check */
	if (ptr == NULL || ptr == DELETED) {
		return(-1);
	}

	/* Find slot */
	i = startIndex;
	for (;;) {
		const void **const ptr2 = &tab->list[i];

		if (*ptr2 == NULL) {
			return (deletedIndex >= 0) ? deletedIndex : i;
		}
		if (*ptr2 == DELETED) {
			if (deletedIndex == -1) {
				deletedIndex = i;
			}
		} else if (*ptr2 == ptr || (*tab->comp)(ptr, *ptr2) == 0) {
			return(i);
		}
		i = (i + step) & (tab->size - 1);

		/* Check for looping all the way through the table */
		if (i == startIndex) {
                        if (deletedIndex >= 0) {
                                return(deletedIndex);
                        }
			assert(!"hashFindSlot: no slot!");
		}
	}
}

/*
 * Make the table bigger.
 * Return the new table or null if the allocation failed.
 *
 * It is okay to remove entries from the table while the allocation
 * function is invoked.
 */
static hashtab_t
hashResize(hashtab_t tab)
{
	const int newSize = (tab->size > 0) ? (tab->size * 2) : INITIAL_SIZE;
	const void **newList;
	const void **oldList;
	int i;

	/* Get a bigger list */
	if (tab->alloc) {
		newList = tab->alloc(newSize * sizeof(*newList));
	} else {
		newList = KCALLOC(newSize, sizeof(*newList));
	}

	/* It is possible that the table no longer needs resizing, for 
	 * instance because a garbage collection happened and removed
	 * entries, for instance when uninterning strings.
	 */
	if (!NEED_RESIZE(tab)) {
		if (tab->dealloc) {
			tab->dealloc(newList);
		} else {
			KFREE(newList);
		}
		return (tab);
	}

	if (newList == NULL) {
		return (NULL);
	}

	/* Rehash old list contents into new list */
	for (i = tab->size - 1; i >= 0; i--) {
		const void *ptr = tab->list[i];

		if (ptr != NULL && ptr != DELETED) {
			const int hash = (*tab->hash)(ptr);
			const int step = LIST_STEP(hash);
			int j;

			for (j = hash & (newSize - 1);
			    newList[j] != NULL;
			    j = (j + step) & (newSize - 1));
			newList[j] = ptr;
		}
	}

	/* Update table. The operation is atomic as the lock
	 * is held by the owner of the hashtable.
	 */
	oldList = tab->list;
	tab->list = newList;
	tab->size = newSize;

	/* Free the old table */
	if (tab->dealloc) {
		tab->dealloc(oldList);
	} else {
		KFREE(oldList);
	}
	return (tab);
}
