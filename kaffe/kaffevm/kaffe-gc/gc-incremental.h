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

#define KGC_COLOUR_MASK          0x0F
#define KGC_COLOUR_INUSE         0x08            /* a mask, not a value */
#define KGC_COLOUR_FREE          0x00
#define KGC_COLOUR_FIXED         0x01
#define KGC_COLOUR_WHITE         0x08            /* hit by _INUSE mask */
#define KGC_COLOUR_GREY          0x09            /* hit by _INUSE mask */
#define KGC_COLOUR_BLACK         0x0A            /* hit by _INUSE mask */

#define KGC_STATE_MASK           0xF0
#define KGC_STATE_NORMAL         0x00            /* Has no finalise method */
#define KGC_STATE_FINALIZED      0x00            /* Has been finalised */
#define KGC_STATE_NEEDFINALIZE   0x10            /* Needs finalising */
#define KGC_STATE_INFINALIZE     0x20            /* Starting finalisation */

#define KGC_SET_COLOUR(B, I, C) \
                (B)->state[I] = ((B)->state[I] & (~KGC_COLOUR_MASK)) | (C)
#define KGC_GET_COLOUR(B, I)     ((B)->state[I] & KGC_COLOUR_MASK)

#define KGC_SET_STATE(B, I, C) \
                (B)->state[I] = ((B)->state[I] & (~KGC_STATE_MASK)) | (C)
#define KGC_GET_STATE(B, I)      ((B)->state[I] & KGC_STATE_MASK)

#define KGC_SET_FUNCS(B, I, F)   (B)->funcs[I] = (F)
#define KGC_GET_FUNCS(B, I)      (B)->funcs[I]

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
				(OBJ)->cprev = NULL; \
				(OBJ)->cnext = NULL

#define	UTOMEM(OBJ)		((void*)(((gc_unit*)(uintp)(OBJ))+1))
#define	UTOUNIT(OBJ)		(((gc_unit*)(uintp)(OBJ))-1)

/* ------------------------------------------------------------------------ */

#define	STOPWORLD()		KTHREAD(suspendall)()
#define	RESUMEWORLD()		KTHREAD(unsuspendall)()

void KaffeGC_WalkConservative(Collector* gcif, const void* base, uint32 size);
void KaffeGC_WalkMemory(Collector* gcif, void* mem);

#endif
