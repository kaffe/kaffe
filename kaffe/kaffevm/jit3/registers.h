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

/* Register types */
#define	Reserved		0x80
#define	Rglobal			0x40
#define	Rint			0x01
#define	Rlong			0x02
#define	Rfloat			0x04
#define	Rdouble			0x08
#define	Rref			0x10

/* Register flags */
#define	Rreadonce		0x01
#define	Rnosaveoncall		0x02

/* Information about the state of a particular register. */
typedef struct _kregs {
	/* The slot whose value is in this register. */
	SlotData*		slot;
	uint8			ctype;
	uint8			type;
	uint8			flags;
	uint32			used;
	uint8			regno;
	uint8			refs;
} kregs;

extern kregs reginfo[];
extern int usecnt;

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
int	slotOffset(SlotData*, int, int);
void	slot_invalidate(SlotData*);
void	spillAndUpdate(SlotData*, jboolean);


#define	rread			1
#define	rwrite			2

#define	_slowSlotRegister(A,B,C) slotRegister(A,B,C,NOREG)

#define	rreg_int(i)		slotRegister(seq_slot(s, i), Rint, rread, NOREG)
#define	wreg_int(i)		slotRegister(seq_slot(s, i), Rint, rwrite, NOREG)
#define	rwreg_int(i)		slotRegister(seq_slot(s, i), Rint, rread|rwrite, NOREG)
#define	rslot_int(i)		slotOffset(seq_slot(s, i), Rint, rread)
#define	wslot_int(i)		slotOffset(seq_slot(s, i), Rint, rwrite)

#define	rreg_ref(i)		slotRegister(seq_slot(s, i), Rref, rread, NOREG)
#define	wreg_ref(i)		slotRegister(seq_slot(s, i), Rref, rwrite, NOREG)
#define	rwreg_ref(i)		slotRegister(seq_slot(s, i), Rref, rread|rwrite, NOREG)
#define	rslot_ref(i)		slotOffset(seq_slot(s, i), Rref, rread)
#define	wslot_ref(i)		slotOffset(seq_slot(s, i), Rref, rwrite)

#define	rreg_long(i)		slotRegister(seq_slot(s, i), Rlong, rread, NOREG)
#define	wreg_long(i)		slotRegister(seq_slot(s, i), Rlong, rwrite, NOREG)
#define	rwreg_long(i)		slotRegister(seq_slot(s, i), Rlong, rread|rwrite, NOREG)
#define	rslot_long(i)		slotOffset(seq_slot(s, i), Rlong, rread)
#define	wslot_long(i)		slotOffset(seq_slot(s, i), Rlong, rwrite)

#define	rreg_float(i)		slotRegister(seq_slot(s, i), Rfloat, rread, NOREG)
#define	wreg_float(i)		slotRegister(seq_slot(s, i), Rfloat, rwrite, NOREG)
#define	rwreg_float(i)		slotRegister(seq_slot(s, i), Rfloat, rread|rwrite, NOREG)
#define	rslot_float(i)		slotOffset(seq_slot(s, i), Rfloat, rread)
#define	wslot_float(i)		slotOffset(seq_slot(s, i), Rfloat, rwrite)

#define	rreg_double(i)		slotRegister(seq_slot(s, i), Rdouble, rread, NOREG)
#define	wreg_double(i)		slotRegister(seq_slot(s, i), Rdouble, rwrite, NOREG)
#define	rwreg_double(i)		slotRegister(seq_slot(s, i), Rdouble, rread|rwrite, NOREG)
#define	rslot_double(i)		slotOffset(seq_slot(s, i), Rdouble, rread)
#define	wslot_double(i)		slotOffset(seq_slot(s, i), Rdouble, rwrite)

#define	sreg(I)			(seq_slot(s,I)->regno)
#define	sreg_int(i)		sreg(i)
#define	sreg_ref(i)		sreg(i)
#define	sreg_long(i)		sreg(i)
#define	sreg_float(i)		sreg(i)
#define	sreg_double(i)		sreg(i)

#define	lreg_int(i)		wreg_int(i)
#define	lreg_ref(i)		wreg_ref(i)
#define	lreg_long(i)		wreg_long(i)
#define	lreg_float(i)		wreg_float(i)
#define	lreg_double(i)		wreg_double(i)

#define	const_int(I)		s->u[I].value.i
#define const_long(I)		s->u[I].value.l
#define const_label(I)		s->u[I].labconst
#define const_method(I)		s->u[I].methconst
#define	const_float(I)		s->u[I].value.d
#define	const_double(I)		s->u[I].value.d

#define	calleeSave(R)		(reginfo[R].flags & Rnosaveoncall)

/* Is a slot already in a register? */
#define	inRegister(i)		(seq_slot(s,i)->regno != NOREG)

extern void initGlobalRegisters(int);
extern void setGlobalRegister(int);

void sanityCheck(void);
void reload(SlotData*);

#endif
