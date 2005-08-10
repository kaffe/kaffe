/* registers.h
 * Instruction macros to extract information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __registers_h
#define __registers_h

#include "slots.h"

/* Register types */
#define	Reserved		0x80
#define	Rglobal			0x40
#define	Rint			0x01
#define	Rlong			0x02
#define	Rfloat			0x04
#define	Rdouble			0x08
#define	Rref			0x10
#define	Rsubint			0x20

/* Register flags */
#define	Rreadonce		0x01
#define	Rnosaveoncall		0x02

/**
 * Information about the state of a particular register.
 *
 * slot  - head of the list of slots whose values are in this register
 * ctype - value type this register currently contains
 * type  - all value types this register can contain
 * flags - flags of the register
 * used  - the last time this register was assigned to a slot
 * regno - backend id of this register (whatever the backend wants it to be)
 * refs  - number of slots whose values are in this register
 */
typedef struct _kregs {
	SlotData*		slot;
	uint8			ctype;
	uint8			type;
	uint8			flags;
	uint32			used;
	uint8			regno;
	uint8			refs;
} kregs;

extern kregs reginfo[];
extern int enable_readonce;

#define	MAXREG			NR_REGISTERS
#define	NOREG			MAXREG

/*
 * If we have stack limit support use the STACK_LIMIT macro to get
 * the slot offset.  If we don't we just return the value 0 since
 * we don't be using stack_limit anyway.
 */
#if defined(STACK_LIMIT)
#define	STACK_LIMIT_OFFSET()	STACK_LIMIT()
#else
#define	STACK_LIMIT_OFFSET()	0
#endif

#define	SLOT2FRAMEOFFSET(_s)				\
	(						\
		((_s) == stack_limit) ?			\
		 STACK_LIMIT_OFFSET() :			\
		((_s) - slotinfo < maxArgs) ? 		\
		 SLOT2ARGOFFSET((_s) - slotinfo) :	\
		 SLOT2LOCALOFFSET((_s) - slotinfo)	\
	)

void	initRegisters(void);
int	slotRegister(SlotData*, int, int, int);
void	clobberRegister(int);
void	forceRegister(SlotData*, int, int);
int	slotOffsetNoSpill(SlotData*, int);
int	slotOffset(SlotData*, int, int);
void	slot_invalidate(SlotData*);
void	spillAndUpdate(SlotData*, jboolean);
void	preloadRegister(SlotData*, int, int);

#define	rreload			0
#define	rread			1
#define	rwrite			2

/* JIT2 compatibility */
#define	_slowSlotRegister(A,B,C) slotRegister(A,B,C,NOREG)
#define	_slotInRegister(S,T)	_inRegister(S,T)
#define	slotInRegister(I,T)	inRegister(I,T)
#define	slowSlotOffset(S,T,U)	slotOffset(S,T,U)
#define	_slowSlotOffset(S,T,U)	slotOffset(S,T,U)

/**
 * Macros to deal with slots of type jint.
 *
 */
#define	rreg_int(i)		slotRegister(seq_slot(s, i), Rint, rread, NOREG)
#define	wreg_int(i)		slotRegister(seq_slot(s, i), Rint, rwrite, NOREG)
#define	rwreg_int(i)		slotRegister(seq_slot(s, i), Rint, rread|rwrite, NOREG)
#define	rslot_int(i)		slotOffset(seq_slot(s, i), Rint, rread)
#define	wslot_int(i)		slotOffset(seq_slot(s, i), Rint, rwrite)
#define	rreg_ideal_int(i,r)	slotRegister(seq_slot(s, i), Rint, rread, r)

/**
 * Macros to deal with slots of type ref.
 *
 */
#define	rreg_ref(i)		slotRegister(seq_slot(s, i), Rref, rread, NOREG)
#define	wreg_ref(i)		slotRegister(seq_slot(s, i), Rref, rwrite, NOREG)
#define	rwreg_ref(i)		slotRegister(seq_slot(s, i), Rref, rread|rwrite, NOREG)
#define	rslot_ref(i)		slotOffset(seq_slot(s, i), Rref, rread)
#define	wslot_ref(i)		slotOffset(seq_slot(s, i), Rref, rwrite)

/**
 * Macros to deal with slots of type long.
 *
 */
#define	rreg_long(i)		slotRegister(seq_slot(s, i), Rlong, rread, NOREG)
#define	wreg_long(i)		slotRegister(seq_slot(s, i), Rlong, rwrite, NOREG)
#define	rwreg_long(i)		slotRegister(seq_slot(s, i), Rlong, rread|rwrite, NOREG)
#define	rslot_long(i)		slotOffset(seq_slot(s, i), Rlong, rread)
#define	wslot_long(i)		slotOffset(seq_slot(s, i), Rlong, rwrite)
#define	rreg_ideal_long(i,r)	slotRegister(seq_slot(s, i), Rlong, rread, r)

/**
 * Macros to deal with slots of type float.
 *
 */
#define	rreg_float(i)		slotRegister(seq_slot(s, i), Rfloat, rread, NOREG)
#define	wreg_float(i)		slotRegister(seq_slot(s, i), Rfloat, rwrite, NOREG)
#define	rwreg_float(i)		slotRegister(seq_slot(s, i), Rfloat, rread|rwrite, NOREG)
#define	rslot_float(i)		slotOffset(seq_slot(s, i), Rfloat, rread)
#define	wslot_float(i)		slotOffset(seq_slot(s, i), Rfloat, rwrite)
#define	rreg_ideal_float(i,r)	slotRegister(seq_slot(s, i), Rfloat, rread, r)

/**
 * Macros to deal with slots of type double.
 *
 */
#define	rreg_double(i)		slotRegister(seq_slot(s, i), Rdouble, rread, NOREG)
#define	wreg_double(i)		slotRegister(seq_slot(s, i), Rdouble, rwrite, NOREG)
#define	rwreg_double(i)		slotRegister(seq_slot(s, i), Rdouble, rread|rwrite, NOREG)
#define	rslot_double(i)		slotOffset(seq_slot(s, i), Rdouble, rread)
#define	wslot_double(i)		slotOffset(seq_slot(s, i), Rdouble, rwrite)
#define	rreg_ideal_double(i,r)	slotRegister(seq_slot(s, i), Rdouble, rread, r)

/**
 * Macros to deal with slots of type subint.
 *
 */
#define	rreg_subint(i)		slotRegister(seq_slot(s, i), Rsubint, rread, NOREG)
#define	wreg_subint(i)		slotRegister(seq_slot(s, i), Rsubint, rwrite, NOREG)
#define	rwreg_subint(i)		slotRegister(seq_slot(s, i), Rsubint, rread|rwrite, NOREG)
#define	rslot_subint(i)		slotOffset(seq_slot(s, i), Rsubint, rread)
#define	wslot_subint(i)		slotOffset(seq_slot(s, i), Rsubint, rwrite)
#define	rreg_ideal_subint(i,r)	slotRegister(seq_slot(s, i), Rsubint, rread, r)

#define	sreg(I)			reginfo[(seq_slot(s,I)->regno)].regno
#define	sreg_int(i)		sreg(i)
#define	sreg_ref(i)		sreg(i)
#define	sreg_long(i)		sreg(i)
#define	sreg_float(i)		sreg(i)
#define	sreg_double(i)		sreg(i)

#if 0
#define	lreg_int(i)		wreg_int(i)
#define	lreg_ref(i)		wreg_ref(i)
#define	lreg_long(i)		wreg_long(i)
#define	lreg_float(i)		wreg_float(i)
#define	lreg_double(i)		wreg_double(i)
#endif
#define	lreg_int(i)		slotRegister(seq_slot(s, i), Rint, rreload, NOREG)     
#define	lreg_ref(i)		slotRegister(seq_slot(s, i), Rref, rreload, NOREG)     
#define	lreg_long(i)		slotRegister(seq_slot(s, i), Rlong, rreload, NOREG)     
#define	lreg_float(i)		slotRegister(seq_slot(s, i), Rfloat, rreload, NOREG)     
#define	lreg_double(i)		slotRegister(seq_slot(s, i), Rdouble, rreload, NOREG)     

#define	const_int(I)		s->u[I].value.i
#define const_long(I)		s->u[I].value.l
#define const_label(I)		s->u[I].labconst
#define const_method(I)		s->u[I].methconst
#define	const_float(I)		s->u[I].value.f
#define	const_double(I)		s->u[I].value.d

/* Reserve a register */
#define register_reserve(r)     (reginfo[r].type |= Reserved)
#define register_unreserve(r)   (reginfo[r].type &= ~Reserved)

#define	calleeSave(R)		(reginfo[R].flags & Rnosaveoncall)

/* Is a slot already in a register? */
#define	_inRegister(S,T)	((reginfo[(S)->regno].ctype & (T)) != (0))
#define	inRegister(I,T)		(_inRegister(seq_slot(s,I), T))

/* Is a register global? */
#define	isRegisterGlobal(R)	((reginfo[(R)].type & Rglobal) != 0)

extern void initGlobalRegisters(int);
extern void setGlobalRegister(int);

extern void sanityCheck(void);
extern void reload(SlotData*);

extern void slot_kill_readonce(SlotData*);

#if defined(HAVE_kill_readonce_register)
extern void HAVE_kill_readonce_register(SlotData *s);
#endif

#endif
