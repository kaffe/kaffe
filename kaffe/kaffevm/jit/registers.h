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
#define	Rsubint			0x00 /* jit3: 0x20 */

/* Register flags */
/* NB. Currently we do not support preserving values in registers across
 *     function calls (which is a pity on the SPARC) because it causes
 *     problems with exception handling.  This needs to be fixed!
 */
#define	Rreadonce		0x01
#define	Rnosaveoncall		0x00 /* 0x02 */

/* Information about the state of a particular register. */
typedef struct _kregs {
	/* The slot whose value is in this register. */
	SlotInfo*		slot;
	uint8			ctype;
	uint8			type;
	uint8			flags;
	uint32			used;
	uint8			regno;
} kregs;

extern kregs* KaffeVM_jitGetRegInfo(void);

#define	register_invalidate(r)	KaffeVM_jitGetRegInfo()[r].slot = NOSLOT

/* reserve a register */
#define        register_reserve(r)     (KaffeVM_jitGetRegInfo()[r].type |= Reserved)
#define        register_unreserve(r)   (KaffeVM_jitGetRegInfo()[r].type &= ~Reserved)

#define	MAXREG			NR_REGISTERS
#define	NOREG			MAXREG

#define	SLOT2FRAMEOFFSET(_s)				\
	(((_s) - slotinfo < maxArgs) ? 			\
		SLOT2ARGOFFSET((_s) - slotinfo) :	\
		SLOT2LOCALOFFSET((_s) - slotinfo))

void	spill(SlotInfo*);
void	reload(SlotInfo*);

extern void slot_kill_readonce(SlotInfo*);

#if defined(HAVE_kill_readonce_register)
extern void HAVE_kill_readonce_register(SlotInfo *s);
#endif

void	initRegisters(void);
int	slowSlotRegister(SlotInfo*, int, int);
int	fastSlotRegister(SlotInfo*, int, int);
void	clobberRegister(int);
void	forceRegister(SlotInfo*, int, int);
int	slowSlotOffset(SlotInfo*, int, int);

extern void KaffeVM_jitSetIdealReg(int reg);
extern int KaffeVM_jitGetEnableReadonce(void);
extern void KaffeVM_jitSetEnableReadonce(int enable);

#define	rread			1
#define	rwrite			2
#define	rnowriteback		4

#define _slotInRegister(_s, _t)					\
	((KaffeVM_jitGetRegInfo()[(_s)->regno].type & (_t)) == (_t))

#define slotInRegister(_i, _t)					\
	_slotInRegister(seq_slot(s, _i), _t)

#define	_slowSlotRegister(_s, _t, _u)				\
	(_slotInRegister(_s, _t) ? 				\
	 fastSlotRegister(_s, _t, _u) :				\
	 slowSlotRegister(_s, _t, _u))

#define	rreg_int(i)		_slowSlotRegister(seq_slot(s, i), Rint, rread)
#define	wreg_int(i)		_slowSlotRegister(seq_slot(s, i), Rint, rwrite)
#define	rwreg_int(i)		_slowSlotRegister(seq_slot(s, i), Rint, rread|rwrite)
#define	rslot_int(i)		slowSlotOffset(seq_slot(s, i), Rint, rread)
#define	wslot_int(i)		slowSlotOffset(seq_slot(s, i), Rint, rwrite)

#define	rreg_ref(i)		_slowSlotRegister(seq_slot(s, i), Rref, rread)
#define	wreg_ref(i)		_slowSlotRegister(seq_slot(s, i), Rref, rwrite)
#define	rwreg_ref(i)		_slowSlotRegister(seq_slot(s, i), Rref, rread|rwrite)
#define	rslot_ref(i)		slowSlotOffset(seq_slot(s, i), Rref, rread)
#define	wslot_ref(i)		slowSlotOffset(seq_slot(s, i), Rref, rwrite)

#define	rreg_long(i)		_slowSlotRegister(seq_slot(s, i), Rlong, rread)
#define	wreg_long(i)		_slowSlotRegister(seq_slot(s, i), Rlong, rwrite)
#define	rwreg_long(i)		_slowSlotRegister(seq_slot(s, i), Rlong, rread|rwrite)
#define	rslot_long(i)		slowSlotOffset(seq_slot(s, i), Rlong, rread)
#define	wslot_long(i)		slowSlotOffset(seq_slot(s, i), Rlong, rwrite)

#define	rreg_float(i)		_slowSlotRegister(seq_slot(s, i), Rfloat, rread)
#define	wreg_float(i)		_slowSlotRegister(seq_slot(s, i), Rfloat, rwrite)
#define	rwreg_float(i)		_slowSlotRegister(seq_slot(s, i), Rfloat, rread|rwrite)
#define	rslot_float(i)		slowSlotOffset(seq_slot(s, i), Rfloat, rread)
#define	wslot_float(i)		slowSlotOffset(seq_slot(s, i), Rfloat, rwrite)

#define	rreg_double(i)		_slowSlotRegister(seq_slot(s, i), Rdouble, rread)
#define	wreg_double(i)		_slowSlotRegister(seq_slot(s, i), Rdouble, rwrite)
#define	rwreg_double(i)		_slowSlotRegister(seq_slot(s, i), Rdouble, rread|rwrite)
#define	rslot_double(i)		slowSlotOffset(seq_slot(s, i), Rdouble, rread)
#define	wslot_double(i)		slowSlotOffset(seq_slot(s, i), Rdouble, rwrite)

#define	sreg_int(i)		rreg_int(i)
#define	sreg_ref(i)		rreg_ref(i)
#define	sreg_long(i)		rreg_long(i)
#define	sreg_float(i)		rreg_float(i)
#define	sreg_double(i)		rreg_double(i)

#define	lreg_int(i)		wreg_int(i)
#define	lreg_ref(i)		wreg_ref(i)
#define	lreg_long(i)		wreg_long(i)
#define	lreg_float(i)		wreg_float(i)
#define	lreg_double(i)		wreg_double(i)

#define	const_int(i)		s->u[i].iconst
#define const_long(i)		s->u[i].lconst
#define const_label(i)		s->u[i].labconst
#define const_method(i)		s->u[i].methconst
#define	const_float(i)		s->u[i].fconst
#define	const_double(i)		s->u[i].fconst

#endif
