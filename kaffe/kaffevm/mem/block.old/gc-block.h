/* gc-block.h
 * Store descriptions of gc-blocks in blocks themselves.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __gc_block_h
#define	__gc_block_h

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

extern gc_block* gc_objecthash[];

/* ------------------------------------------------------------------------ */

#define	GC_MAGIC		0xD0DECADE
#define GC_OBJECT_HASHSIZE	1024
#define GC_OBJECT_HASHIDX(B)	((((uintp)(B)) >> gc_pgbits) & (GC_OBJECT_HASHSIZE-1))

#define	GCBLOCK2STATE(B, N)	(&(B)->state[(N)])
#define	GCBLOCK2MEM(B, N)	((gc_unit*)(&(B)->data[(N)*(B)->size]))
#define	GCBLOCK2FREE(B, N)	((gc_freeobj*)GCBLOCK2MEM(B, N))
#define	GCBLOCKSIZE(B)		(B)->size

#define	GCMEM2FREE(M)		((gc_freeobj*)(M))
#define	GCMEM2IDX(B, M)		(((uint8*)(M) - (B)->data) / (B)->size)

#define GCMEM2BLOCK(M)		((gc_block*)(((uintp)(M)) & -gc_pgsize))
#define GCBLOCK2BASE(B)		((char *) ((B) + 1))
#define GCBLOCKEND(B)		((gc_block*)(((uint8*)(B)) + (B)->size))
#define GCBLOCK_OVH		(sizeof(gc_block))

static inline int
gc_heap_isobject(gc_block *info, gc_unit *unit)
{
	gc_block *hptr;
	uintp hidx;
	int idx;

	if ((uintp) unit & (MEMALIGN - 1)) return 0;

	info = GCMEM2BLOCK(unit);
	hidx = GC_OBJECT_HASHIDX(info);
	for (hptr = gc_objecthash[hidx]; hptr != 0; hptr = hptr->next) {
		if (hptr == info) {
			idx = GCMEM2IDX(info, unit);
			if (idx < info->nr && GCBLOCK2MEM(info, idx) == unit && (GC_GET_COLOUR(info, idx) & GC_COLOUR_INUSE) == GC_COLOUR_INUSE) {
				/* Make sure 'unit' refers to the
				 * beginning of an object.  We do this
				 * by making sure it is correctly
				 * aligned within the block.
				 */
				idx = GCMEM2IDX(info, unit);
				if (idx < info->nr && GCBLOCK2MEM(info, idx) == unit && (GC_GET_COLOUR(info, idx) & GC_COLOUR_INUSE) == GC_COLOUR_INUSE) {
					return 1;
				}
			}
		}
	}
	return 0;
}

extern void gc_block_init(void);
extern void gc_block_add(gc_block *);
extern void gc_block_rm(gc_block *);
extern void *gc_block_alloc(size_t sz);
#endif /* _gc_block_h */
