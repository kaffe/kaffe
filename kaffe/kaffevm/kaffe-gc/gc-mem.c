/* gc-mem.c
 * The heap manager.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "debug.h"
/* undefine this to revert to old tile scheme */
#define	PREDEFINED_NUMBER_OF_TILES

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "gtypes.h"
#include "baseClasses.h"
#include "support.h"
#include "stats.h"
#include "locks.h"
#include "thread.h"
#include "gc.h"
#include "gc-mem.h"
#include "gc-incremental.h"
#include "jni.h"
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif
#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif
#if defined(HAVE_SYS_MMAN_H)
#include <sys/mman.h>
#endif

#ifndef MAX
#define MAX(A,B) ((A) > (B) ? (A) : (B))
#endif

static gc_block *gc_last_block;
static gc_block *gc_reserve_pages;
static iStaticLock	gc_heap_lock;

#if defined(KAFFE_STATS)
static counter gcpages;
#endif
static gc_block* gc_small_block(size_t);
static gc_block* gc_large_block(size_t);

static gc_block* gc_primitive_alloc(size_t);

/**
 * A preallocated block for small objects.
 *
 * @list list of gc_blocks available for objects of the same size
 * @sz   the size of the objects that can be stored in @list. 
 */
typedef struct {
	gc_block* list;
	uint16	  sz;
} gc_freelist;

/**
 * Array of preallocated blocks.
 *  
 */
static gc_freelist freelist[NR_FREELISTS+1]
#ifdef PREDEFINED_NUMBER_OF_TILES
	= {
#define	S(sz)	{ NULL, sz }
	S(16),
	S(24),
	S(32),
	S(40),
	S(48),
	S(56),
	S(64),
	S(80),
	S(96),
	S(112),
	S(128),
	S(160),
	S(192),
	S(224),
	S(240),
	S(496),
	S(1000),
	S(2016),
	S(4040),
	{ (gc_block *)-1, 0 }
}
#endif /* PREDEFINED_NUMBER_OF_TILES */
;

/**
 * Maps a given size to a freelist entry. 
 *
 */
static struct {
	uint16	list;
} sztable[MAX_SMALL_OBJECT_SIZE+1];
static int max_freelist;

static size_t max_small_object_size;
static size_t gc_heap_allocation_size;	/* amount of memory by which to grow heap */
static size_t gc_heap_initial_size;	/* amount of memory to initially allocate */
static size_t gc_heap_total;		/* current size of the heap */
static size_t gc_heap_limit;		/* maximum size to which heap should grow */
static uintp gc_heap_base;              /* start of the heap */
static uintp gc_heap_range;             /* last gc-able address - gc_heap_base */

#ifndef gc_pgsize
static size_t gc_pgsize;
static int gc_pgbits;
#endif

#ifdef KAFFE_VMDEBUG
int gc_system_alloc_cnt;
#endif

/**
 * rounds @V up to the next page size.
 *
 */
#define	ROUNDUPPAGESIZE(V)	(((uintp)(V) + gc_pgsize - 1) & -gc_pgsize)

static char * gc_block_base = NULL;
static size_t gc_num_blocks = 0;
static size_t gc_num_live_pages = 0;

#define KGC_BLOCKS		((gc_block *) gc_block_base)

/**
 * Evaluates to the first usable address in gc_block @B.
 *
 */ 
#define GCBLOCK2BASE(B)		(((char *)gc_heap_base) \
					 + gc_pgsize * ((B) - KGC_BLOCKS))

/**
 * Evaluates to the size of the object that contains address @M.
 *
 */
#define	KGC_OBJECT_SIZE(M)	gc_mem2block(M)->size

#define ASSERT_ONBLOCK(OBJ, BLK) assert(gc_mem2block(OBJ) == BLK)

#if !(defined(NDEBUG) || !defined(KAFFE_VMDEBUG))
/* Magic constant used to mark blocks under gc's management */
static const uint32 gc_magic = 0xD0DECADE;

/* Set the magic marker of a block */
static inline void
gc_set_magic_marker(gc_block *b)
{
  b->magic = gc_magic;
}

/* Check the magic marker of a block */
static inline bool
gc_check_magic_marker(gc_block *b)
{
  return b->magic == gc_magic;
}

/*
 * analyze the slack incurred by small objects
 */
static int totalslack;
static int totalsmallobjs;

static void 
printslack(void)
{
	dprintf(
		"allocated %d small objects, total slack %d, slack/per "
		"object %8.2f\n", 
		totalsmallobjs, totalslack, totalslack/(double)totalsmallobjs);
}

/*
 * check whether the heap is still in a consistent state
 */
static void
gc_heap_check(void)
{
	int i; 

	gc_block *chk_blk = gc_last_block;

	while (chk_blk->pprev != NULL)
	  {
	    if (chk_blk->pprev != NULL && chk_blk->pprev->pnext != chk_blk)
	      {
		dprintf("Major failure in the Garbage Collector. Primitive block list trashed\n");
		abort();
	      }
	    chk_blk = chk_blk->pprev;
	  }

	while (chk_blk != gc_last_block)
	  {
	    if (chk_blk->pnext != NULL && chk_blk->pnext->pprev != chk_blk)
	      {
		dprintf("Major failure in the Garbage Collector (2). Primitive block list trashed\n");
		abort();
	      }
	    chk_blk = chk_blk->pnext;
	  }
	
	for (i = 0; i < NR_FREELISTS; i++) {
		gc_block* blk = freelist[i].list;
		if (blk == 0 || blk == (gc_block*)-1) {
			continue;
		} else {
			gc_freeobj* mem = blk->free;

			assert(GCBLOCKINUSE(blk));
			assert(blk->avail < blk->nr);
			assert(blk->funcs == (uint8*)GCBLOCK2BASE(blk));
			assert(blk->state == (uint8*)(blk->funcs + blk->nr));
			assert(blk->data  == (uint8*)ROUNDUPALIGN(blk->state + blk->nr));

			while (mem) {
				ASSERT_ONBLOCK(mem, blk);
				mem = mem->next;
			}
		}
	}
}

#endif /* !(defined(NDEBUG) || !defined(KAFFE_VMDEBUG)) */


static inline bool
gc_heap_is_unlimited(void)
{
  return gc_heap_limit != UNLIMITED_HEAP;
}

/*
 * Initialise allocator.
 */
void
gc_heap_initialise(void)
{
  initStaticLock(&gc_heap_lock);

#ifndef gc_pgsize
	gc_pgsize = getpagesize();
	for (gc_pgbits = 0;
	     (size_t)(1 << gc_pgbits) != gc_pgsize && gc_pgbits < 64;
	     gc_pgbits++)
		;
	assert(gc_pgbits < 64);
#endif

	gc_heap_allocation_size = Kaffe_JavaVMArgs.allocHeapSize;
	gc_heap_initial_size = Kaffe_JavaVMArgs.minHeapSize;
	gc_heap_limit = Kaffe_JavaVMArgs.maxHeapSize;

	/*
	 * Perform some sanity checks.
	 */
	if ((gc_heap_initial_size > gc_heap_limit) 
	    && !gc_heap_is_unlimited()) {
		dprintf(
		    "Initial heap size (%dK) > Maximum heap size (%dK)\n",
		    (int) (gc_heap_initial_size/1024), (int)(gc_heap_limit/1024));
		KAFFEVM_EXIT(-1);
	}

#ifndef PREDEFINED_NUMBER_OF_TILES
    {
	int i;
	int l;
	int b;
	int t;

	/* old scheme, where number of tiles was approximated by a series
	 * of powers of two
	 */
#define	OBJSIZE(NR) \
	((gc_pgsize-ROUNDUPALIGN(1)-(NR*(2+sizeof(void*))))/NR)

	/* For a given number of tiles in a block, work out the size of
	 * the allocatable units which'll fit in them and build a translation
	 * table for the sizes.
	 */
	i = 1;
	max_small_object_size = ROUNDDOWNALIGN(OBJSIZE(i));
	l = max_small_object_size;
	for (;;) {
		b = ROUNDDOWNALIGN(OBJSIZE(i));
		if (b >= MIN_OBJECT_SIZE) {
			for (t = l; t > b; t--) {
				sztable[t].list = l;
			}
			l = t;
			i <<= 1;
		}
		else {
			for (t = l; t > MIN_OBJECT_SIZE; t--) {
				sztable[t].list = l;
			}
			for (t = 0; t <= MIN_OBJECT_SIZE; t++) {
				sztable[t].list = MIN_OBJECT_SIZE;
			}
			break;
		}
	}

	/* Translate table into list numbers */
	i = -1;
	b = -1;
	for (l = 0; l <= max_small_object_size; l++) {
		if (sztable[l].list != b) {
			b = sztable[l].list;
			i++;
			freelist[i].sz = b;
		}
		sztable[l].list = i;
	}
	max_freelist = i;
    }
#else
	/* PREDEFINED_NUMBER_OF_TILES */
	{
		/*
		 * Use the preinitialized freelist table to initialize
		 * the sztable.
		 */
		int sz = 0;
		uint16 flidx = 0;
		while (freelist[flidx].list == 0) {
			for (; sz <= freelist[flidx].sz; sz++)
				sztable[sz].list = flidx;
			flidx++;
		}
		max_small_object_size = sz - 1;
		max_freelist = flidx;
	}
#endif

DBG(SLACKANAL,
	atexit(printslack);
    );

#undef	OBJSIZE

	/* Round 'gc_heap_allocation_size' up to pagesize */
	gc_heap_allocation_size = ROUNDUPPAGESIZE(gc_heap_allocation_size);

	/* Round 'gc_heap_initial_size' up to pagesize */
	gc_heap_initial_size = ROUNDUPPAGESIZE(gc_heap_initial_size);

	/* allocate heap of initial size from system */
	gc_heap_grow(gc_heap_initial_size);
}

/**
 * Allocate a piece of memory.
 */
void*
gc_heap_malloc(size_t sz)
{
	size_t lnr;
	gc_freeobj* mem = NULL;
	gc_block** mptr;
	gc_block* blk;
	size_t nsz;
#if defined(KAFFE_STATS)
	static timespent heap_alloc_time;
#endif

	lockStaticMutex(&gc_heap_lock);
	startTiming(&heap_alloc_time, "gc_heap_malloc");

DBG(SLACKANAL,
	if (KGC_SMALL_OBJECT(sz)) {
		totalslack += (freelist[sztable[sz].list].sz - sz);
		totalsmallobjs++;
	}
    );

DBG(GCDIAG, 
	gc_heap_check();
    );

	if (KGC_SMALL_OBJECT(sz)) {

		/* Translate size to object free list */
		lnr = sztable[sz].list;
		nsz = freelist[lnr].sz;

		/* No available objects? Allocate some more */
		mptr = &freelist[lnr].list;
		if (*mptr != 0) {
			blk = *mptr;
			assert(blk->free != 0);
DBG(GCALLOC,		dprintf("gc_heap_malloc: freelist %ld at %p free %p\n", 
				(long) sz, *mptr, blk->free););
		}
		else {
			blk = gc_small_block(nsz);
			if (blk == 0) {
				goto out;
			}
			blk->next = *mptr;
			*mptr = blk;

DBG(GCALLOC,		dprintf("gc_heap_malloc: small block %ld at %p free %p\n", 
				(long) sz, *mptr, blk->free););
		}

		/* Unlink free one and return it */
		mem = blk->free;

		DBG(GCDIAG,
		    assert(gc_check_magic_marker(blk));
		    ASSERT_ONBLOCK(mem, blk);
		    if (mem->next) ASSERT_ONBLOCK(mem->next, blk));

		blk->free = mem->next;

		KGC_SET_STATE(blk, GCMEM2IDX(blk, mem), KGC_STATE_NORMAL);

		/* Once we use all the sub-blocks up, remove the whole block
		 * from the freelist.
		 */
		assert(blk->nr >= blk->avail);
		assert(blk->avail > 0);
		blk->avail--;
		if (blk->avail == 0) {
			*mptr = blk->next;
		}
	}
	else {
		nsz = sz;
		blk = gc_large_block(nsz);
		if (blk == 0) {
			goto out;
		}
		mem = GCBLOCK2FREE(blk, 0);
		KGC_SET_STATE(blk, 0, KGC_STATE_NORMAL);
DBG(GCALLOC,	dprintf("gc_heap_malloc: large block %ld at %p\n", 
			(long) sz, mem);	);
		blk->avail--;
		assert(blk->avail == 0);
	}

	/* Clear memory */
	memset(mem, 0, nsz);

	assert(KGC_OBJECT_SIZE(mem) >= sz);

	out:
	stopTiming(&heap_alloc_time);
	unlockStaticMutex(&gc_heap_lock);

	return (mem);
}

/**
 * Free a piece of memory.
 */
void
gc_heap_free(void* mem)
{
	gc_block* info;
	gc_freeobj* obj;
	int lnr;
	int msz;
	int idx;
#if defined(KAFFE_STATS)
	static timespent heap_free_time;
#endif

	info = gc_mem2block(mem);
	idx = GCMEM2IDX(info, mem);

	DBG(GCDIAG,
	    gc_heap_check();
	    assert(gc_check_magic_marker(info));
	    assert(KGC_GET_COLOUR(info, idx) != KGC_COLOUR_FREE));

	KGC_SET_COLOUR(info, idx, KGC_COLOUR_FREE);

DBG(GCFREE,
	dprintf("gc_heap_free: memory %p size %d\n", mem, info->size);	);

	lockStaticMutex(&gc_heap_lock);
	startTiming(&heap_free_time, "gc_heap_free");

	if (KGC_SMALL_OBJECT(info->size)) {
		lnr = sztable[info->size].list;
	
		info->avail++;
		DBG(GCDIAG,
		    /* write pattern in memory to see when live objects were
		     * freed - Note that (f4f4f4f4 == -185273100)
		     */
		    memset(mem, 0xf4, info->size));
		obj = GCMEM2FREE(mem);
		obj->next = info->free;
		info->free = obj;

		ASSERT_ONBLOCK(obj, info);

		/* If we free all sub-blocks, free the block */
		assert(info->avail <= info->nr);
		if (info->avail == info->nr) {
			/*
			 * note that *finfo==0 is ok if we free a block
			 * whose small object is so large that it can
			 * only contain one object.
			 */
			gc_block** finfo = &freelist[lnr].list;
			for (;*finfo;) {
				if (*finfo == info) {
					(*finfo) = info->next;
					break;
				}
				finfo = &(*finfo)->next;
			}

			info->size = gc_pgsize;
			gc_primitive_free(info);

		} else if (info->avail==1) {
			/*
			 * If this block contains no free sub-blocks yet, attach
			 * it to freelist. 
			 */
			gc_block **finfo = &freelist[lnr].list;

			info->next = *finfo; 
			*finfo = info;
		}

	}
	else {
		/* Calculate true size of block */
		msz = info->size + 2 + ROUNDUPALIGN(1);
		msz = ROUNDUPPAGESIZE(msz);
		info->size = msz;
		gc_primitive_free(info);
	}

	stopTiming(&heap_free_time);
	unlockStaticMutex(&gc_heap_lock);

DBG(GCDIAG,
	gc_heap_check();
    );

}

/*
 * Allocate a new block of GC'ed memory.  The block will contain 'nr' objects
 * each of 'sz' bytes.
 */
static
gc_block*
gc_small_block(size_t sz)
{
	gc_block* info;
	int i;
	int nr;

	assert(sz >= sizeof(gc_block *));

	info = gc_primitive_alloc(gc_pgsize);
	if (info == 0) {
		return (NULL);
	}

	/* Calculate number of objects in this block */
	nr = (gc_pgsize-ROUNDUPALIGN(1))/(sz+2);

	/* Setup the meta-data for the block */
	DBG(GCDIAG, gc_set_magic_marker(info));

	info->size = sz;
	info->nr = nr;
	info->avail = nr;
	info->funcs = (uint8*)GCBLOCK2BASE(info);
	info->state = (uint8*)(info->funcs + nr);
	info->data = (uint8*)ROUNDUPALIGN(info->state + nr);

	DBG(GCDIAG, memset(info->data, 0, sz * nr));

	/* Build the objects into a free list */
	for (i = nr-1; i-- > 0;) {
		GCBLOCK2FREE(info, i)->next = GCBLOCK2FREE(info, i+1);
		KGC_SET_COLOUR(info, i, KGC_COLOUR_FREE);
		KGC_SET_STATE(info, i, KGC_STATE_NORMAL);
	}
	GCBLOCK2FREE(info, nr-1)->next = NULL;
	KGC_SET_COLOUR(info, nr-1, KGC_COLOUR_FREE);
	KGC_SET_STATE(info, nr-1, KGC_STATE_NORMAL);
	info->free = GCBLOCK2FREE(info, 0);
DBG(SLACKANAL,
	int slack = ((char *)info) 
		+ gc_pgsize - (char *)(GCBLOCK2MEM(info, nr));
	totalslack += slack;
    );
	return (info);
}

/*
 * Allocate a new block of GC'ed memory.  The block will contain one object
 */
static
gc_block*
gc_large_block(size_t sz)
{
	gc_block* info;
	size_t msz;
	size_t block_count;

	/* Add in management overhead */
	msz = sz+2+ROUNDUPALIGN(1);
	/* Round size up to a number of pages */
	msz = ROUNDUPPAGESIZE(msz);

	info = gc_primitive_alloc(msz);
	if (info == 0) {
		return (NULL);
	}

	/* number of pages allocated for block */
	block_count = msz >> gc_pgbits;
	
	DBG(GCPRIM, dprintf ("large block covers %zx pages\n", block_count); );

	/* Setup the meta-data for the block */
	DBG(GCDIAG, gc_set_magic_marker(info));

	info->size = sz;
	info->nr = 1;
	info->avail = 1;
	info->funcs = (uint8*)GCBLOCK2BASE(info);
	info->state = (uint8*)(info->funcs + 1);
	info->data = (uint8*)ROUNDUPALIGN(info->state + 1);
	info->free = NULL;

	DBG(GCDIAG, memset(info->data, 0, sz));

	GCBLOCK2FREE(info, 0)->next = NULL;

	/* now initialize the other blocks that were allocated */
	while (--block_count > 0) {

		info[block_count].size  = sz;
		info[block_count].nr    = 1;
		info[block_count].avail = 0;
		info[block_count].funcs = info[0].funcs;
		info[block_count].state = info[0].state;
		info[block_count].data  = info[0].data;
		info[block_count].free  = NULL;
	}

	/*
	 * XXX gc_large_block only called during a block allocation.
	 * The following is just going to get overwritten. (Right?)
	 */
	KGC_SET_COLOUR(info, 0, KGC_COLOUR_FREE);
	KGC_SET_STATE(info, 0, KGC_STATE_NORMAL);

	return (info);
}

/*
 * Primitive block management:  Allocating and freeing whole pages.
 *
 * Each primitive block of the heap consists of one or more contiguous
 * pages. Pages of unused primitive blocks are marked unreadable when
 * kaffe is compiled with debugging enabled. Whether a block is in use
 * can be determined by its nr field: when it's in use, its nr field
 * will be > 0.
 *
 * All primitive blocks are chained through their pnext / pprev fields,
 * no matter whether or not they are in use. This makes the necessary
 * check for mergable blocks as cheap as possible. Merging small blocks
 * is necessary so that single unused primitive blocks in the heap are
 * always as large as possible. The first block in the list is stored
 * in gc_block_base, the last block in the list is gc_last_block.
 *
 * In order to speed up the search for the primitive block that fits
 * a given allocation request best, small primitive blocks are stored
 * in several lists (one per size). If no primitive blocks of a given
 * size are left, a larger one is splitted instead. 
 */
#define KGC_PRIM_LIST_COUNT 20

static gc_block *gc_prim_freelist[KGC_PRIM_LIST_COUNT+1];

#ifndef PROT_NONE
#define PROT_NONE 0
#endif

#if !defined(HAVE_MPROTECT) || !defined(HAVE_COMPATIBLE_MPROTECT)
#define mprotect(A,L,P)
#define ALL_PROT
#define NO_PROT
#else
/* In a sense, this is backwards. */
#define ALL_PROT PROT_READ|PROT_WRITE|PROT_EXEC
#define NO_PROT  PROT_NONE
#endif

/* Mark a primitive block as used */
static inline void 
gc_block_add(gc_block *b)
{
  b->nr = 1;
#if defined(KAFFE_VMDEBUG)
  mprotect(GCBLOCK2BASE(b), b->size, ALL_PROT);
#endif
}

/* Mark a primitive block as unused */
static inline void 
gc_block_rm(gc_block *b)
{
  b->nr = 0;
#if defined(KAFFE_VMDEBUG)
  mprotect(GCBLOCK2BASE(b), b->size, NO_PROT);
#endif
}

/* Get the end a gc block
 *
 * This is OK, gc_prim_(alloc|free) never assume GCBLOCKEND is really
 * a valid block
 */
static inline gc_block*
gc_block_end(gc_block *b)
{
  return b + ((b->size+gc_pgsize-1)>>gc_pgbits);
}

/* return the prim list blk belongs to */
static inline gc_block **
gc_get_prim_freelist (gc_block *blk)
{
	size_t sz = blk->size >> gc_pgbits;

	if (sz <= KGC_PRIM_LIST_COUNT)
	{
	        assert (sz > 0);
		return &gc_prim_freelist[sz-1];
	}

	return &gc_prim_freelist[KGC_PRIM_LIST_COUNT];
}

/* add a primitive block to the correct freelist */
static inline void
gc_add_to_prim_freelist(gc_block *blk)
{
	gc_block **list = gc_get_prim_freelist (blk);

	/* insert the block int the list, sorting by ascending addresses */
	while (*list && blk > *list)
	{
		list = &(*list)->next;
	}

	if (*list) {
		(*list)->free = (gc_freeobj *)&blk->next;
	}

	blk->next = *list;
	*list = blk;
	blk->free = (gc_freeobj *)list;
}

/* remove a primitive block from its freelist */
static inline void
gc_remove_from_prim_freelist(gc_block *blk)
{
	*( (gc_block **) blk->free ) = blk->next;

	if (blk->next) {
		blk->next->free = blk->free;
	}
}
 
/*
 * Allocate a block of memory from the free list or, failing that, the
 * system pool.
 */
static
gc_block*
gc_primitive_alloc(size_t sz)
{
	size_t diff = 0;
	gc_block* best_fit = NULL;
	size_t i = sz >> gc_pgbits;

	assert(sz % gc_pgsize == 0);

	DBG(GCPRIM, dprintf("\ngc_primitive_alloc: got to allocate 0x%x bytes\n", (unsigned int)sz); );

	/* try freelists for small primitive blocks first */
	if (i <= KGC_PRIM_LIST_COUNT) {
		for (i-=1; i<KGC_PRIM_LIST_COUNT; i++) {
			if (gc_prim_freelist[i]) {
				best_fit = gc_prim_freelist[i]; 
				diff = gc_prim_freelist[i]->size - sz;
				break;
			}
		}
	}

	/* if that fails, try the big remaining list */
	if (!best_fit) {
		gc_block *ptr;
		for (ptr = gc_prim_freelist[KGC_PRIM_LIST_COUNT]; ptr != 0; ptr=ptr->next) {

			/* Best fit */
			if (sz == ptr->size) {
				diff = 0;
				best_fit = ptr;
				break;
			} else if (sz < ptr->size) {
				size_t left = ptr->size - sz;
		
				if (best_fit==NULL || left<diff) {
					diff = left;
					best_fit = ptr;
				}		
			}
		}
	}

	/* if we found a block, remove it from the list and check if splitting is necessary */
	if (best_fit) {
		gc_remove_from_prim_freelist (best_fit);

		DBG(GCPRIM, dprintf ("gc_primitive_alloc: found best_fit %p diff 0x%x (0x%x - 0x%x)\n",
				     best_fit, (unsigned int)diff, best_fit->size, (unsigned int)sz); );
		assert ( diff % gc_pgsize == 0 );

		if (diff > 0) {
			gc_block *nptr;

			best_fit->size = sz;
		
			nptr = gc_block_end(best_fit);
			nptr->size = diff;
			gc_block_rm (nptr);

			DBG(GCPRIM, dprintf ("gc_primitive_alloc: splitted remaining 0x%x bytes @ %p\n", (unsigned int)diff, nptr); );

			DBG(GCDIAG, gc_set_magic_marker(nptr));

			/* maintain list of primitive blocks */
			nptr->pnext = best_fit->pnext;
			nptr->pprev = best_fit;

			best_fit->pnext = nptr;

			if (nptr->pnext) {
				nptr->pnext->pprev = nptr;
			} else {
				gc_last_block = nptr;
			}

			/* and add nptr to one of the freelists */
			gc_add_to_prim_freelist (nptr);
		}

DBG(GCPRIM,	dprintf("gc_primitive_alloc: 0x%x bytes from freelist @ %p\n", best_fit->size, best_fit); );
		gc_block_add(best_fit);
		return (best_fit);
	}
DBG(GCPRIM,	dprintf("gc_primitive_alloc: no suitable block found!\n"); );

	/* Nothing found on free list */
	return (NULL);
}

/*
 * merge a primitive block with its successor.
 */
static inline void
gc_merge_with_successor (gc_block *blk)
{
	gc_block *next_blk = blk->pnext;

	assert (next_blk);

	blk->size += next_blk->size;
	blk->pnext = next_blk->pnext;

	/*
	 * if the merged block has a successor, update its pprev field.
	 * otherwise, the merged block is the last block in the primitive
	 * chain.
	 */
	if (blk->pnext) {
		blk->pnext->pprev = blk;
	} else {
		gc_last_block = blk;
	}
}


/*
 * Return a block of memory to the free list.
 */
void
gc_primitive_free(gc_block* mem)
{
	gc_block *blk;

	assert(mem->size % gc_pgsize == 0);
	assert(GCBLOCKINUSE(mem));

	/* Remove from object hash */
	gc_block_rm(mem);

	DBG(GCPRIM, dprintf ("\ngc_primitive_free: freeing block %p (%x bytes, %x)\n", mem, mem->size, mem->size >> gc_pgbits); );

	/*
	 * Test whether this block is mergable with its successor.
	 * We need to do the gc_block_end check, since the heap may not be a continuous
	 * memory area and thus two consecutive blocks need not be mergable. 
	 */
	
        blk = mem->pnext;
	if ((blk != NULL) &&
	    !GCBLOCKINUSE(blk) &&
	    gc_block_end(mem)==blk) {
		DBG(GCPRIM, dprintf ("gc_primitive_free: merging %p with its successor (%p, %u)\n", mem, blk, blk->size););

		gc_remove_from_prim_freelist(blk);

		gc_merge_with_successor (mem);
	}

	blk = mem->pprev;
	if ((blk != NULL) &&
	    !GCBLOCKINUSE(blk) &&
	    gc_block_end(blk)==mem) {
		DBG(GCPRIM, dprintf ("gc_primitive_free: merging %p with its predecessor (%p, %u)\n", mem, blk, blk->size); );

		gc_remove_from_prim_freelist(blk);
		  
		mem = blk;

		gc_merge_with_successor (mem);
	}
	
	gc_add_to_prim_freelist (mem);

	DBG(GCPRIM, dprintf ("gc_primitive_free: added 0x%x bytes @ %p to freelist %u @ %p\n", mem->size, mem,
			     (unsigned int)(gc_get_prim_freelist(mem)-&gc_prim_freelist[0]), gc_get_prim_freelist(mem)); );
}

/*
 * Try to reserve some memory for OOM exception handling.  Gc once at
 * the beginning.  We start out looking for an arbitrary number of
 * pages, and cut our expectations in half until we are able to
 * meet them.
 */
void
gc_primitive_reserve(size_t numpages)
{
	gc_block *r = NULL;
	size_t size = numpages * gc_pgsize;

	if (gc_reserve_pages != NULL)
	        return;
	
	while (size >= gc_pgsize && !(r = gc_primitive_alloc(size))) {
		if (size == gc_pgsize) {
			break;
		}
		size /= 2;
	}
	/* As it is done just at the initialization it is expected to have at 
	 * least one page free */
	assert(r != NULL);
	gc_reserve_pages = r;
}

/*
 * We return the reserve to the heap if it has not been already used.
 * This function returns true if some reserve was still available.
 */
bool
gc_primitive_use_reserve()
{
        if (gc_reserve_pages == NULL)
	        return false;
	gc_primitive_free(gc_reserve_pages);
	gc_reserve_pages = NULL;

	return true;
}


/*
 * System memory management:  Obtaining additional memory from the
 * OS.  This looks more complicated than it is, since it does not require
 * sbrk.
 */
/* Get some page-aligned memory from the system. */
static uintp
pagealloc(size_t size)
{
	void* ptr;

#define	CHECK_OUT_OF_MEMORY(P)	if ((P) == 0) return 0;

#if defined(HAVE_SBRK) && !defined(DARWIN)

	/* Our primary choice for basic memory allocation is sbrk() which
	 * should avoid any unsee space overheads.
	 */
	for (;;) {
		int missed;
		ptr = sbrk((intp)size);
		if (ptr == (void*)-1) {
			ptr = NULL;
			break;
		}
		if ((uintp)ptr % gc_pgsize == 0) {
			break;
		}
		missed = gc_pgsize - ((uintp)ptr % gc_pgsize);
		DBG(GCSYSALLOC,
		    dprintf("unaligned sbrk %p, missed %d bytes\n",
			    ptr, missed));
		sbrk((intp)(-size + missed));
	}
	CHECK_OUT_OF_MEMORY(ptr);

#elif defined(HAVE_MEMALIGN)

        ptr = memalign(gc_pgsize, size);
	CHECK_OUT_OF_MEMORY(ptr);

#elif defined(HAVE_VALLOC)

        ptr = valloc(size);
	CHECK_OUT_OF_MEMORY(ptr);

#else

	/* Fallback ...
	 * Allocate memory using malloc and align by hand.
	 */
	size += gc_pgsize;

        ptr = malloc(size);
	CHECK_OUT_OF_MEMORY(ptr);
	ptr = (void*)((((uintp)ptr) + gc_pgsize - 1) & (uintp)-gc_pgsize);

#endif
	mprotect(ptr, size, ALL_PROT);

	addToCounter(&gcpages, "gcmem-system pages", 1, size);
	return ((uintp) ptr);
}

/* Free memory allocated with pagealloc */
#ifdef HAVE_SBRK
static void pagefree(uintp base UNUSED, size_t size)
{
	sbrk((intp)-size);
}
#else
static void pagefree(uintp base, size_t size UNUSED)
{
	/* it must have been allocated with memalign, valloc or malloc */
	free((void *)base);
}
#endif

/*
 * Determine if ptr points inside the array of gc_block structures.
 *
 * @param ptr the pointer to check for
 * @param base a pointer to the start of the array
 * @param count the number of elements in the array
 */
static int
inside(void* ptr, gc_block* base, int count) {
        return ((gc_block*)ptr >= base && (gc_block*)ptr < base + count);
}


/*
 * Allocate size bytes of heap memory, and return the corresponding
 * gc_block *.
 */
static void *
gc_block_alloc(size_t size)
{
	int size_pg = (size>>gc_pgbits);
	uintp heap_addr;
	static uintp last_addr;

	if (!gc_block_base) {
		gc_num_blocks = (size+gc_pgsize-1)>>gc_pgbits;

		gc_block_base = malloc(gc_num_blocks * sizeof(gc_block));
		if (!gc_block_base) return NULL;
		memset(gc_block_base, 0, gc_num_blocks * sizeof(gc_block));
	}

	DBG(GCSYSALLOC, dprintf("pagealloc(%ld)", (long) size));
	heap_addr = pagealloc(size);
	DBG(GCSYSALLOC, dprintf(" => %p\n", (void *) heap_addr));

	if (!heap_addr) return NULL;
	
	if (!gc_heap_base) {
		gc_heap_base = heap_addr;
	}

	if (gc_mem2block((void *) (heap_addr + size))
	    > ((gc_block *)gc_block_base) + gc_num_blocks
	    || heap_addr < gc_heap_base) {
		char * old_blocks = gc_block_base;
		int onb = gc_num_blocks;
		unsigned int min_nb;	/* minimum size of array to hold heap_addr */
#if defined(KAFFE_STATS)
		static timespent growtime;
#endif

		startTiming(&growtime, "gctime-blockrealloc");
		/* Pick a new size for the gc_block array.  Remember,
		   malloc does not simply grow a memory segment.

		   We can extrapolate how many gc_blocks we need for
		   the entire heap based on how many heap pages
		   currently fit in the gc_block array.  But, we must
		   also make sure to allocate enough blocks to cover
		   the current allocation */
		gc_num_blocks = (gc_num_blocks * ((gc_heap_total + size) >> gc_pgbits))
			/ gc_num_live_pages;
		if (heap_addr < gc_heap_base) 
			min_nb = gc_num_blocks
			  + ((gc_heap_base - heap_addr) >> gc_pgbits);
		else
			min_nb = ((heap_addr + size) - gc_heap_base) >>
			  gc_pgbits;
		gc_num_blocks = MAX(gc_num_blocks, min_nb);
		DBG(GCSYSALLOC,
		    dprintf("growing block array from %d to %d elements\n",
			    onb, gc_num_blocks));

		KTHREAD(spinon)(NULL);
		gc_block_base = realloc(old_blocks,
					gc_num_blocks * sizeof(gc_block));
		if (!gc_block_base) {
			/* In some implementations, realloc is not smart enough to acquire new block
			 * in a non-contiguous region. Even if internally it calls some malloc procedure
			 * it fails evenly. A countermeasure is to use slow real malloc if realloc fails
			 * and only if that call also fails we put throw a OOM.
			 */
			DBG(GCSYSALLOC, dprintf("realloc has failed. Trying malloc.\n"));
			gc_block_base = malloc(gc_num_blocks * sizeof(gc_block));
			if (!gc_block_base) {
				/* roll back this call */
				DBG(GCSYSALLOC, dprintf("failed to grow the block list\n"));
				pagefree(heap_addr, size);
				gc_block_base = old_blocks;
				gc_num_blocks = onb;
				KTHREAD(spinoff)(NULL);
				return NULL;
			}
			memcpy(gc_block_base, old_blocks, onb * sizeof(gc_block));
			free(old_blocks);
		}

		DBG(GCSYSALLOC, dprintf("old block_base = %p, new block_base = %p\n", old_blocks, gc_block_base));
		/* If the array's address has changed, we have to fix
		   up the pointers in the gc_blocks, as well as all
		   external pointers to the gc_blocks.  We can only
		   fix gc_prim_freelist and the size-freelist array.
		   There should be no gc_block *'s on any stack
		   now. */ 
		if (gc_block_base != old_blocks) {
			int i;
			gc_block *b = (gc_block *) gc_block_base;
			uintp delta = gc_block_base - old_blocks;
#define R(type,X) if (X) X = (type*)((uintp)X + delta)

			DBG(GCSYSALLOC,
			    dprintf("relocating gc_block array\n"));
			for (i = 0; i < onb; i++) 
			  {
			    R(gc_block, b[i].next);
			    R(gc_block, b[i].pprev);
			    R(gc_block, b[i].pnext);
                            if (inside(b[i].free, (gc_block*)old_blocks, onb))
				R(gc_freeobj, b[i].free);
			  }

			memset(b + onb, 0,
			       (gc_num_blocks - onb) * sizeof(gc_block));

			for (i = 0; i<=KGC_PRIM_LIST_COUNT; i++)
				R(gc_block, gc_prim_freelist[i]);

			for (i = 0; freelist[i].list != (void*)-1; i++) 
				R(gc_block, freelist[i].list);

			R(gc_block, gc_reserve_pages);
			R(gc_block, gc_last_block);
#undef R
		}
		KTHREAD(spinoff)(NULL);
		stopTiming(&growtime);
	}
	gc_num_live_pages += size_pg;
	last_addr = MAX(last_addr, heap_addr + size);
	gc_heap_range = last_addr - gc_heap_base;
	DBG(GCSYSALLOC, dprintf("%ld unused bytes in heap addr range\n",
				(long) (gc_heap_range - gc_heap_total)));
#if defined(KAFFE_VMDEBUG)
	mprotect((void *) heap_addr, size, NO_PROT);
#endif
	return gc_mem2block((void *) heap_addr);
}

/**
 * Grows the heap.
 *
 * @param sz minimum number of bytes to grow.
 * @return 0 in case of an error, otherwise != 0
 */
void *
gc_heap_grow(size_t sz)
{
	gc_block* blk;

	if (KGC_SMALL_OBJECT(sz)) {
		sz = gc_pgsize;
	} else {
		sz = sz + 2 + ROUNDUPALIGN(1);
		sz = ROUNDUPPAGESIZE(sz);
	}

	if (sz < gc_heap_allocation_size) {
		sz = gc_heap_allocation_size;
	}

	assert(sz % gc_pgsize == 0);

	lockStaticMutex(&gc_heap_lock);

	if (gc_heap_total == gc_heap_limit) {
		unlockStaticMutex(&gc_heap_lock);
		return (NULL);
	} else if (gc_heap_total + sz > gc_heap_limit && !gc_heap_is_unlimited()) {
		/* take as much memory as we can */
		sz = gc_heap_limit - gc_heap_total;
		assert(sz % gc_pgsize == 0);
		DBG(GCSYSALLOC, dprintf("allocating up to limit\n"));
	}
#ifdef KAFFE_VMDEBUG
	gc_system_alloc_cnt++;
#endif

	blk = gc_block_alloc(sz);

	DBG(GCSYSALLOC,
	    dprintf("gc_system_alloc: %ld byte at %p\n", (long) sz, blk); );

	if (blk == NULL) {
		unlockStaticMutex(&gc_heap_lock);
		return (NULL);
	}

	gc_heap_total += sz;
	assert(gc_heap_total <= gc_heap_limit || gc_heap_is_unlimited());

	/* Place block into the freelist for subsequent use */
	DBG(GCDIAG, gc_set_magic_marker(blk));
	blk->size = sz;

	/* maintain list of primitive blocks */
	if (gc_last_block) {
		gc_last_block->pnext = blk;
		blk->pprev = gc_last_block;
	}
	
	gc_last_block = blk;

	/* Free block into the system */
	blk->nr = 1;
	gc_primitive_free(blk);

	unlockStaticMutex(&gc_heap_lock);

	return (blk);
}

/**
 * Evaluates to the gc_block that contains address @mem.
 *
 */
gc_block *
gc_mem2block(const void * mem) 
{
  return (KGC_BLOCKS + ( ( ((uintp) (mem)) - gc_heap_base) >> gc_pgbits));
}

/**
 * Gets current heap size.
 */
size_t
gc_get_heap_total(void)
{
  return gc_heap_total;
}

/**
 * Gets maximum size to which heap should grow.
 */
size_t
gc_get_heap_limit(void)
{
  return gc_heap_limit;
}

/**
 * Gets start of the heap.
 */
uintp
gc_get_heap_base(void)
{
  return gc_heap_base;
}

/**
 * Gets last gc-able address - start of the heap.
 */
uintp
gc_get_heap_range(void)
{
  return gc_heap_range;
}
