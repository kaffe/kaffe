/* slots.h
 * Management of Slots.
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

/* Spill/Reload types */
#define	SR_BASIC		0
#define	SR_FUNCTION		1
#define	SR_SUBBASIC		2
#define	SR_SYNC			3
#define	SR_START		4
#define	SR_EXCEPTION		5

/**
 * used to track a function's locals, arguments and temps.
 *
 * regno    - jitter id of the register holding the value of this slot
 * offset   - stack frame offset of this slot
 * wseq     - the last sequence writing this slot (inside the basic block currently translated)
 * rseq     - the last sequence reading this slot (inside the basic block currently translated)
 * rseqslot - this slot is the rseqslot'th parameter of its rseq
 * modified - flag indicating how this slot is modified (rread, rwrite or 0)
 * rnext    - pointer to the next slot whose value is stored in the same register
 * global   - flag indicating whether this slot contains a global value
 *
 * A SlotData represents both, a local variable of the execution frame as defined
 * in the JVM Spec, second edition, §3.6.1 and an entry of the operand stack
 * as in §3.6.2. As such, longs and doubles occupy two SlotDatas, all other
 * types only one.
 */
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

#define	slot_data(S)		((S).slot)

/* Global slots flags */
#define	GL_NOGLOBAL		0x00
#define	GL_ISGLOBAL		0x01
#define	GL_PRELOAD		0x02
#define	GL_RONLY		0x04

#define	setGlobal(S,T)		(S)->global = (T)
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

/**
 * Access an entry of the operand stack.
 *
 * @param N index of the entry (starting with 0).
 */
#define	stack(N)		(&localinfo[stackno+(N)])
#define	stack_ref		stack
#define	stack_float		stack
#define	stack_long		stack
#define	stack_double		stack_long

/**
 * Read an entry of the operand stack.
 * 
 */
#define	rstack			stack
#define	rstack_ref		stack_ref
#define	rstack_float		stack_float
#define	rstack_double		stack_double
#define	rstack_long		stack_long

/**
 * Write an entry of the operand stack.
 *
 */
#define	wstack			stack
#define	wstack_ref		stack_ref
#define	wstack_float		stack_float
#define	wstack_long		stack_long
#define	wstack_double		stack_double

/**
 * Access a local variable of the jitted method.
 *
 * @param N index of the local variable.
 */
#define	local(N)		(&localinfo[(N)])
#define	local_ref		local
#define	local_float		local
#define	local_long		local
#define	local_double		local_long

/** 
 * Allocate a temp slot for a byte, short, int, char, ref or float.
 * 
 * @param N variable to which the new temp slot is assigned
 */
#define	slot_alloctmp(N)	(N) = &tempinfo[tmpslot];		\
				tmpslot += 1

/**
 * Allocate a temp slot for a double or a long.
 *
 * @param N variable to which the new temp slot is assigned
 */
#define	slot_alloc2tmp(N)	(N) = &tempinfo[tmpslot];		\
				tmpslot += 2

/**
 * Frees a temp slot that was allocated with slot_alloctmp.
 *
 * @param N the slot that is to be freed.
 */
#define	slot_freetmp(N)		lastuse_slot(N, 1)

/**
 * Frees a temp slot that was allocated with slot_alloc2tmp.
 *
 * @param N the temp slot that is to be freed.
 */
#define	slot_free2tmp(N)	lastuse_slot(N, 2)

#define	slot_nowriteback(N)	/* does nothing */
#define	slot_nowriteback2(N)	/* does nothing */

#endif
