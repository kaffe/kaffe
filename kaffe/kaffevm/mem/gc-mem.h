/* gc-mem.h
 * The heap manager.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __gc_mem_h
#define	__gc_mem_h

#define	GC_DEBUG 1

#define	MIN_HEAPSIZE	0
#define	MAX_HEAPSIZE	(32*1024*1024)
#define	ALLOC_HEAPSIZE	(1024*1024)

/* ------------------------------------------------------------------------ */

typedef struct _gcFuncs {
	void			(*walk)(void*, uint32);
	void			(*final)(void*);
} gcFuncs;

typedef struct _gc_freeobj {
	struct _gc_freeobj*	next;
} gc_freeobj;

typedef struct _gc_unit {
	struct _gc_unit*	cprev;
	struct _gc_unit*	cnext;
} gc_unit;

typedef struct _gc_block {
	uint32			magic;	/* Magic number */
	struct _gc_freeobj*	free;	/* Next free sub-block */
	struct _gc_block*	nfree;	/* Next block on sub-freelist */
	uint32			size;	/* Size of objects in this block */
	uint32			nr;	/* Nr of objects in block */
	uint32			avail;	/* Nr of objects available in block */
	struct _gc_block*	next;	/* Next block in freelist/hashtable */
	uint8*			funcs;	/* Function for objects */
	uint8*			state;	/* Colour & state of objects */
	uint8*			data;	/* Address of first object in */
} gc_block;

/* ------------------------------------------------------------------------ */

#define	GC_MAGIC		0xD0DECADE

#define	GCBLOCK2STATE(B, N)	(&(B)->state[(N)])
#define	GCBLOCK2MEM(B, N)	((gc_unit*)(&(B)->data[(N)*(B)->size]))
#define	GCBLOCK2FREE(B, N)	((gc_freeobj*)GCBLOCK2MEM(B, N))
#define	GCBLOCKSIZE(B)		(B)->size

#define	GCMEM2BLOCK(M)		((gc_block*)(((uintp)(M)) & -gc_pgsize))
#define	GCMEM2FREE(M)		((gc_freeobj*)(M))
#define	GCMEM2IDX(B, M)		(((uint8*)(M) - (B)->data) / (B)->size)

#define	GCBLOCKEND(B)		((gc_block*)(((uint8*)(B)) + (B)->size))

/* ------------------------------------------------------------------------ */

#define	GC_COLOUR_MASK		0x0F
#define	GC_COLOUR_INUSE		0x08
#define	GC_COLOUR_FREE		0x00
#define	GC_COLOUR_FIXED		0x01
#define	GC_COLOUR_WHITE		0x08
#define	GC_COLOUR_GREY		0x09
#define	GC_COLOUR_BLACK		0x0A

#define	GC_STATE_MASK		0xF0
#define	GC_STATE_NORMAL		0x00		/* Has no finalise method */
#define	GC_STATE_FINALIZED	0x00		/* Has been finalised */
#define	GC_STATE_NEEDFINALIZE	0x10		/* Needs finalising */
#define	GC_STATE_INFINALIZE	0x20		/* Starting finalisation */

#define	GC_SET_COLOUR(B, I, C) \
		(B)->state[I] = ((B)->state[I] & (~GC_COLOUR_MASK)) | (C)
#define	GC_GET_COLOUR(B, I)	((B)->state[I] & GC_COLOUR_MASK)

#define	GC_SET_STATE(B, I, C) \
		(B)->state[I] = ((B)->state[I] & (~GC_STATE_MASK)) | (C)
#define	GC_GET_STATE(B, I)	((B)->state[I] & GC_STATE_MASK)

#define	GC_SET_FUNCS(B, I, F)	(B)->funcs[I] = (F)
#define	GC_GET_FUNCS(B, I)	(B)->funcs[I]

/* ------------------------------------------------------------------------ */

#define	MIN_OBJECT_SIZE		8
#define	MAX_SMALL_OBJECT_SIZE	8192
#define	NR_FREELISTS		20
#define	GC_SMALL_OBJECT(S)	((S) <= max_small_object_size)

#define	MEMALIGN		8
#define	ROUNDUPALIGN(V)		(((uintp)(V) + MEMALIGN - 1) & -MEMALIGN)
#define	ROUNDDOWNALIGN(V)	((uintp)(V) & -MEMALIGN)
#define	ROUNDUPPAGESIZE(V)	(((uintp)(V) + gc_pgsize - 1) & -gc_pgsize)

#define	GC_OBJECT_HASHSIZE	1024
#define	GC_OBJECT_HASHIDX(B)	((((uintp)(B)) / gc_pgsize) & (GC_OBJECT_HASHSIZE-1))

/* ------------------------------------------------------------------------ */

extern void*	gc_heap_malloc(size_t);    
extern void	gc_heap_free(void*);

extern size_t	gc_pgsize;

#define	GC_OBJECT_SIZE(M)	GCMEM2BLOCK(M)->size

#endif
