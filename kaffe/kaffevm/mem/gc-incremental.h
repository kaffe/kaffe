/* gc-incremental.h
 * The incremental garbage collector.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __gc_incremental_h
#define __gc_incremental_h

#include "gc-mem.h"

extern int gc_mode;

#define	GC_DISABLED		0
#define	GC_ENABLED		1

/* Structure of a root object - essentially an indirect reference
 * to another data structure.
 */
typedef struct {
	void*			mem;
	unsigned int		len;
	gcFuncs*		funcs;
} indirect;

/* ------------------------------------------------------------------------ */

#define	GC_OBJECT_NORMAL	((void*)0)
#define	GC_OBJECT_FIXED		((void*)1)

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

#define	LOCK()			(*Kaffe_LockInterface.spinon)(0);
#define	UNLOCK()		(*Kaffe_LockInterface.spinoff)(0);

extern struct _gcStats {
        uint32  totalmem;
        uint32  totalobj;
        uint32  freedmem;
        uint32  freedobj;
        uint32  markedobj;
        uint32  markedmem;
        uint32  allocobj;
        uint32  allocmem;
} gcStats;

#if defined(GC_INCREMENTAL)
#define	GC_WRITE(_o, _p)	soft_addreference(_o, _p)
#else
#define	GC_WRITE(_o, _p)
#endif

extern void walkConservative(void*, uint32);
extern void markObject(void* mem);
extern void objectStatsPrint(void);

#endif
