/* gc-mem.h
 * The heap manager.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */
/*
 * NB: this file contains only types and declaration relating to heap
 * management.  There's nothing relating to gc in here.
 */
#ifndef __gc_mem_h
#define	__gc_mem_h

#ifndef gc_pgsize
extern size_t gc_pgsize;
extern int gc_pgbits;
#endif

extern size_t gc_heap_total;
extern size_t gc_heap_allocation_size;
extern size_t gc_heap_limit;

#ifdef KAFFE_VMDEBUG
extern int gc_system_alloc_cnt;
#endif

/* ------------------------------------------------------------------------ */

typedef struct _gc_freeobj {
	struct _gc_freeobj*	next;
} gc_freeobj;

/* ------------------------------------------------------------------------ */

#define	MIN_OBJECT_SIZE		8
#define	MAX_SMALL_OBJECT_SIZE	8192
#define	NR_FREELISTS		20
#define	GC_SMALL_OBJECT(S)	((S) <= max_small_object_size)

#define	MEMALIGN		8
#define	ROUNDUPALIGN(V)		(((uintp)(V) + MEMALIGN - 1) & -MEMALIGN)
#define	ROUNDDOWNALIGN(V)	((uintp)(V) & -MEMALIGN)
#define	ROUNDUPPAGESIZE(V)	(((uintp)(V) + gc_pgsize - 1) & -gc_pgsize)

/* ------------------------------------------------------------------------ */

extern void*	gc_heap_malloc(size_t);    
extern void	gc_heap_free(void*);

#define	GC_OBJECT_SIZE(M)	GCMEM2BLOCK(M)->size

typedef struct _gc_block {
#ifdef KAFFE_VMDEBUG
	uint32			magic;	/* Magic number */
#endif
	struct _gc_freeobj*	free;	/* Next free sub-block */
	struct _gc_block*	next;	/* Next block in prim/small freelist */
	struct _gc_block*	nfree;	/* Next block on sub-freelist */
	uint32			inuse;	/* 1bit! Is block allocated? */
	uint32			size;	/* Size of objects in this block */
	uint16			nr;	/* Nr of objects in block */
	uint16			avail;	/* Nr of objects available in block */
	uint8*			funcs;	/* Function for objects */
	uint8*			state;	/* Colour & state of objects */
	uint8*			data;	/* Address of first object in */
} gc_block;

extern gc_block	* gc_primitive_reserve(void);
extern void	gc_primitive_free(gc_block* mem);

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

#define ASSERT_ONBLOCK(OBJ, BLK) assert(GCMEM2BLOCK(OBJ) == BLK)

#endif
