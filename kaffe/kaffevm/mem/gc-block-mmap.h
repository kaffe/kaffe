/* gc-block.h
 * Store descriptions of gc-blocks in a seperate array.  Use mmap to
 * grow array of heap blocks and array of struct gc_blocks in lock step.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __gc_block_h
#define	__gc_block_h

#include "mem/gc-incremental.h"

extern size_t gc_heap_base;
extern size_t gc_block_base;

typedef struct _gc_block {
#ifdef DEBUG
	uint32			magic;	/* Magic number */
#endif
	struct _gc_freeobj*	free;	/* Next free sub-block */
	struct _gc_block*	nfree;	/* Next block on sub-freelist */
	uint32			size;	/* Size of objects in this block */
	uint16			nr;	/* Nr of objects in block */
	uint16			avail;	/* Nr of objects available in block */
	struct _gc_block*	next;	/* Next block in freelist/hashtable */
	uint8*			funcs;	/* Function for objects */
	uint8*			state;	/* Colour & state of objects */
	uint8*			data;	/* Address of first object in */
} gc_block;

/* ------------------------------------------------------------------------ */

#define	GC_MAGIC		0xD0DECADE
#define GCBLOCK_LIVE		((gc_block *) -1) /* block->next when alloced*/
#define GC_BLOCKS		((gc_block *) gc_block_base)

#define	GCBLOCK2STATE(B, N)	(&(B)->state[(N)])
#define	GCBLOCK2MEM(B, N)	((gc_unit*)(&(B)->data[(N)*(B)->size]))
#define	GCBLOCK2FREE(B, N)	((gc_freeobj*)GCBLOCK2MEM(B, N))
#define	GCBLOCKSIZE(B)		(B)->size

#define	GCMEM2FREE(M)		((gc_freeobj*)(M))
#define	GCMEM2IDX(B, M)		(((uint8*)(M) - (B)->data) / (B)->size)
#define GCMEM2BLOCK(M)		(GC_BLOCKS + ((((uintp) M) - gc_heap_base) \
					      >> gc_pgbits))
/* find first usable address in block */ 
#define GCBLOCK2BASE(B)	(((char *)gc_heap_base)		\
			 + gc_pgsize * ((B) - GC_BLOCKS))


/* This is OK, gc_prim_(alloc|free) never assume GCBLOCKEND is really
   a valid block */
#define GCBLOCKEND(B)		((B) + (((B)->size+gc_pgsize-1)>>gc_pgbits))
#define GCBLOCK_OVH		0

extern uintp gc_heap_range;	/* last gcable address - gc_heap_base */

static inline int
gc_heap_isobject(gc_block *info, gc_unit *unit)
{
	uintp p = (uintp) UTOMEM(unit) - gc_heap_base;
	int idx;

	if (!(p & (MEMALIGN - 1)) && p < gc_heap_range
	    && info->next == GCBLOCK_LIVE) {
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

extern void gc_block_init(void);
extern void gc_block_add(gc_block *);
extern void gc_block_rm(gc_block *);
extern void *gc_block_alloc(size_t sz);
#endif /* _gc_block_h */
