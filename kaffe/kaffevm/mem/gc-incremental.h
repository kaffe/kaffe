/* gc-incremental.h
 * The incremental garbage collector.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */
/*
 * This file contains all definitions relating to gc and allocation types.
 */
#ifndef __gc_incremental_h
#define __gc_incremental_h

/*
 * Default values for initial and maximum heap size and heap increment.
 */
#define	MIN_HEAPSIZE	(5*1024*1024)
#define	MAX_HEAPSIZE	(64*1024*1024)
#define	ALLOC_HEAPSIZE	(1024*1024)

extern uintp gc_heap_base;
extern uintp gc_block_base;
extern uintp gc_heap_range;	/* last gc-able address - gc_heap_base */

/* ------------------------------------------------------------------------ */

/*
 * This record describes an allocation type.
 */
typedef struct _gcFuncs {
        walk_func_t             walk;
        final_func_t            final;
        destroy_func_t          destroy;
	const char*		description;
        int                     nr;		/* only used ifdef STATS */
        int                     mem;		/* only used ifdef STATS */
} gcFuncs;

typedef struct _gc_unit {
        struct _gc_unit*        cprev;
        struct _gc_unit*        cnext;
} gc_unit;

/* ------------------------------------------------------------------------ */

/* Structure of a root object - essentially an indirect reference
 * to another data structure.
 */
typedef struct {
	void*			mem;
	unsigned int		len;
	gcFuncs*		funcs;
} indirect;

/* ------------------------------------------------------------------------ */

#define GC_COLOUR_MASK          0x0F
#define GC_COLOUR_INUSE         0x08            /* a mask, not a value */
#define GC_COLOUR_FREE          0x00
#define GC_COLOUR_FIXED         0x01
#define GC_COLOUR_WHITE         0x08            /* hit by _INUSE mask */
#define GC_COLOUR_GREY          0x09            /* hit by _INUSE mask */
#define GC_COLOUR_BLACK         0x0A            /* hit by _INUSE mask */

#define GC_STATE_MASK           0xF0
#define GC_STATE_NORMAL         0x00            /* Has no finalise method */
#define GC_STATE_FINALIZED      0x00            /* Has been finalised */
#define GC_STATE_NEEDFINALIZE   0x10            /* Needs finalising */
#define GC_STATE_INFINALIZE     0x20            /* Starting finalisation */

#define GC_SET_COLOUR(B, I, C) \
                (B)->state[I] = ((B)->state[I] & (~GC_COLOUR_MASK)) | (C)
#define GC_GET_COLOUR(B, I)     ((B)->state[I] & GC_COLOUR_MASK)

#define GC_SET_STATE(B, I, C) \
                (B)->state[I] = ((B)->state[I] & (~GC_STATE_MASK)) | (C)
#define GC_GET_STATE(B, I)      ((B)->state[I] & GC_STATE_MASK)

#define GC_SET_FUNCS(B, I, F)   (B)->funcs[I] = (F)
#define GC_GET_FUNCS(B, I)      (B)->funcs[I]

/* ------------------------------------------------------------------------ */

typedef	gc_unit			gcList;

#define URESETLIST(LIST)	(LIST).cnext = &(LIST); \
				(LIST).cprev = &(LIST)

#define UAPPENDLIST(LIST, OBJ)	(OBJ)->cprev = (LIST).cprev; \
				(OBJ)->cnext = (LIST).cprev->cnext; \
				(LIST).cprev->cnext = (OBJ); \
				(LIST).cprev = (OBJ)

#define UREMOVELIST(OBJ)	(OBJ)->cprev->cnext = (OBJ)->cnext; \
				(OBJ)->cnext->cprev = (OBJ)->cprev; \
				(OBJ)->cprev = 0; \
				(OBJ)->cnext = 0

#define	UTOMEM(OBJ)		((void*)(((gc_unit*)(OBJ))+1))
#define	UTOUNIT(OBJ)		(((gc_unit*)(OBJ))-1)

/* ------------------------------------------------------------------------ */

#define	STOPWORLD()		jthread_suspendall()
#define	RESUMEWORLD()		jthread_unsuspendall()

extern struct _gcStats {
        uint32  totalmem;
        uint32  totalobj;
        uint32  freedmem;
        uint32  freedobj;
        uint32  markedobj;
        uint32  markedmem;
        uint32  allocobj;
        uint32  allocmem;
        uint32  finalobj;
        uint32  finalmem;
} gcStats;

#if defined(notyet) && defined(GC_INCREMENTAL)
#define	GC_WRITE(_o, _p)	soft_addreference(_o, _p)
#else
#define	GC_WRITE(_o, _p)
#endif

#endif
