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

#define	NOSLOT			NULL

#define	MAXTEMPS		16

/* Return slots */
#define	returnInt		0
#define	returnRef		0
#define	returnLong		0 /* 1 */
#define	returnFloat		2
#define	returnDouble		2 /* 3 */

/* Compile-time information about a slot. */

#define	SI_SLOT			0
#define	SI_CONST		1

struct SlotInfo {
	/* If regno != NOREG, then the register that currently contains it.
	 * If regno == NOREG, then the value is on the stack,
	 * in the slot's home location (SLOT2FRAMEOFFSET). */
	uint16			regno;
	uint8			modified;
	uint8			info;
};
typedef struct SlotInfo SlotInfo;

#define REGSLOT SlotInfo

extern SlotInfo* slotinfo;
extern SlotInfo* localinfo;
extern SlotInfo* tempinfo;
extern int tmpslot;
extern int stackno;
extern int maxslot;

void initSlots(int);

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

#define	slot_alloctmp(N)	(N) = &tempinfo[tmpslot],		\
				tmpslot += 1
#define	slot_alloc2tmp(N)	(N) = &tempinfo[tmpslot],		\
				tmpslot += 2
#define	slot_freetmp(N)		slot_invalidate(N)
#define	slot_free2tmp(N)	slot_invalidate(N);			\
				slot_invalidate((N)+1)
#define	slot_invalidate(N)	(N)->regno = NOREG;			\
				(N)->modified = 0
#define	slot_nowriteback(N)	_slot_nowriteback(N)
#define	slot_nowriteback2(N)	_slot_nowriteback(N);			\
				_slot_nowriteback((N)+1)


#endif
