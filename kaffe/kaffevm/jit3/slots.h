/* slots.h
 * Slots.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __slots_h
#define __slots_h

#define	Tcomplex		0
#define	Tnull			Tcomplex
#define	Tconst			1
#define	Tcopy			2
#define	Tcomm			3
#define	Tstore			4
#define	Tload			5

#define	NOSLOT			0

#define	MAXTEMPS		16

/* Return slots */
#define	returnInt		0
#define	returnRef		0
#define	returnLong		0 /* 1 */
#define	returnFloat		2
#define	returnDouble		2 /* 3 */

/* Spill/Reload types */
#define	SR_BASIC		0
#define	SR_FUNCTION		1
#define	SR_SUBBASIC		2
#define	SR_SYNC			3
#define	SR_START		4
#define	SR_EXCEPTION		5

struct SlotData {
	uint16			regno;
	int			offset;
	struct _sequence*	wseq;
	struct _sequence*	rseq;
	uint8			rseqslot;
	uint8			modified;
	struct SlotData*	rnext;
	int			global;
};
typedef struct SlotData SlotData;

struct SlotInfo {
	SlotData*		slot;
};
typedef struct SlotInfo SlotInfo;

/* Global slots flags */
#define	GL_NOGLOBAL		0x00
#define	GL_PRELOAD		0x01
#define	GL_NOLOAD		0x02
#define	GL_RONLY		0x04

#define	isGlobal(S)		((S)->global != GL_NOGLOBAL)
#define	isGlobalPreload(S)	(((S)->global & GL_PRELOAD) != 0)
#define	isGlobalReadonly(S)	(((S)->global & GL_RONLY) != 0)

#define	REGSLOT	SlotData

extern SlotInfo* basicslots;
extern SlotInfo* slotinfo;
extern SlotInfo* localinfo;
extern SlotInfo* tempinfo;
extern SlotInfo stack_limit[1];
extern int tmpslot;
extern int stackno;
extern int maxslot;

void initSlots(int);
void setupSlotsForBasicBlock(void);
void lastuse_slot(SlotInfo* data, int nr);

/* Slot access macros */
#define	stack(N)		(&localinfo[stackno+(N)])
#define	stack_ref		stack
#define	stack_float		stack
#define	stack_long		stack
#define	stack_double		stack_long

#define	rstack			stack
#define	rstack_ref		stack_ref
#define	rstack_float		stack_float
#define	rstack_double		stack_double
#define	rstack_long		stack_long
#define	wstack			stack
#define	wstack_ref		stack_ref
#define	wstack_float		stack_float
#define	wstack_long		stack_long
#define	wstack_double		stack_double

#define	local(N)		(&localinfo[(N)])
#define	local_ref		local
#define	local_float		local
#define	local_long		local
#define	local_double		local_long

#define	slot_alloctmp(N)	(N) = &tempinfo[tmpslot];		\
				tmpslot += 1
#define	slot_alloc2tmp(N)	(N) = &tempinfo[tmpslot];		\
				tmpslot += 2
#define	slot_freetmp(N)		lastuse_slot(N, 1)
#define	slot_free2tmp(N)	lastuse_slot(N, 2)

#define	slot_nowriteback(N)	/* does nothing */
#define	slot_nowriteback2(N)	/* does nothing */

#endif
