/* registers.c
 * Manage the machine registers.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "gtypes.h"
#include "bytecode.h"
#include "slots.h"
#include "icode.h"
#include "registers.h"
#include "seq.h"
#include "labels.h"
#include "codeproto.h"
#include "errors.h"
#include "machine.h"
#include "md.h"

/*
 * Define the registers.
 */
static kregs reginfo[MAXREG+1] = {
  REGISTER_SET
  { /* BAD */	NULL, 0, 0, 0, 0, 0 }
};

/**
 * Get the registers.
 */
kregs* 
KaffeVM_jitGetRegInfo(void)
{
  return reginfo;
}

/* This is horrible but necessary at the moment.  Sometime we need to
 * make transient changes to the registers which we will forget in 
 * a short while.  This can have a bad effect on read-once register so
 * we disable them termporaily.
 */
static int enable_readonce = Rreadonce;

/**
 * Get the value of enable_readonce.
 */
int
KaffeVM_jitGetEnableReadonce(void)
{
  return enable_readonce;
}

/**
 * Set the value of enable_readonce.
 *
 * @param enable new value
 */
void
KaffeVM_jitSetEnableReadonce(int enable)
{
  enable_readonce = enable;
}

/* Count for each register use - gives an idea of which register is
 * to be reused.
 */
static int usecnt = 0;

/*
 * Set this if you prefer a given register for a slot.
 */
static int idealReg = NOREG;

/**
 * Set preferred register for a slot
 *
 * @param reg the preferred register
 */
void 
KaffeVM_jitSetIdealReg(int reg)
{
  idealReg = reg;
}

/*
 * Initiate registers.
 */
void
initRegisters(void)
{
	int i;

	/* Free all registers */
	for (i = 0; i < MAXREG; i++) {
		reginfo[i].slot = NOSLOT;
		reginfo[i].used = 0;
	}
}

/*
 * Spill a register using the correct spill function.
 */
void
spill(SlotInfo* s)
{
#if defined(HAVE_spill_long)
	if (reginfo[s->regno].ctype & Rlong) {
		spill_long(s);
	}
	else
#endif
#if defined(HAVE_spill_int)
	if (reginfo[s->regno].ctype & Rint) {
		spill_int(s);
	}
	else
#endif
#if defined(HAVE_spill_ref)
	if (reginfo[s->regno].ctype & Rref) {
		spill_ref(s);
	}
	else
#endif
#if defined(HAVE_spill_double)
	if (reginfo[s->regno].ctype & Rdouble) {
		spill_double(s);
	}
	else
#endif
#if defined(HAVE_spill_float)
	if (reginfo[s->regno].ctype & Rfloat) {
		spill_float(s);
	}
	else
#endif
	{
		KAFFEVM_ABORT();
	}
}

/*
 * Reload a register using the correct reload function.
 */
void
reload(SlotInfo* s)
{
#if defined(HAVE_reload_long)
	if (reginfo[s->regno].ctype & Rlong) {
		reload_long(s);
	}
	else
#endif
#if defined(HAVE_reload_int)
	if (reginfo[s->regno].ctype & Rint) {
		reload_int(s);
	}
	else
#endif
#if defined(HAVE_reload_ref)
	if (reginfo[s->regno].ctype & Rref) {
		reload_ref(s);
	}
	else
#endif
#if defined(HAVE_reload_double)
	if (reginfo[s->regno].ctype & Rdouble) {
		reload_double(s);
	}
	else
#endif
#if defined(HAVE_reload_float)
	if (reginfo[s->regno].ctype & Rfloat) {
		reload_float(s);
	}
	else
#endif
	{
		KAFFEVM_ABORT();
	}
}

void
slot_kill_readonce(SlotInfo *s)
{
#if defined(HAVE_kill_readonce_register)
    	if (((s->modified & rwrite) != 0) &&
	    ((reginfo[s->regno].flags & enable_readonce) != 0)) {
	    	HAVE_kill_readonce_register (s);
		s->modified &= ~rwrite;
	}
#endif
}


/*
 * Translate a slot number into a register.
 *  Perform the necessary spills and reloads to make this happen.
 */
int
slowSlotRegister(SlotInfo* slot, int type, int use)
{
	SlotInfo* pslot;
	int creg, reg;
	kregs* regi;
	uint32 used;

	/* Allocate a register - pick the least recently used */
	reg = idealReg;
	used = 0xFFFFFFFF;
	for (creg = 0; creg < MAXREG; creg++) {
		regi = &reginfo[creg];
		if ((regi->type & Reserved) == 0
		    && (regi->type & type) == type
		    && regi->used < used) {
			used = regi->used;
			reg = creg;
		}
	}
	assert(reg != NOREG);
	regi = &reginfo[reg];
        assert((regi->type & (Reserved|Rglobal)) == 0);
        assert((regi->type & type) == type);

	pslot = regi->slot;

	/* Spill the register to its old slot if still in use */
	if (pslot != NOSLOT) {
		/* NB. We always spill readonce registers (which isn't very
		 * efficient but okay for now).
		 */
		if ((pslot->modified & rwrite) || (regi->flags & enable_readonce)) {
			spill(pslot);
			pslot->modified = 0;
		}
		slot_invalidate(pslot);
		if (regi->ctype == Rlong || regi->ctype == Rdouble) {
			slot_invalidate(&pslot[1]);
		}
	}

	/* If we have a register assigned to this slot then it must
	 * be the wrong type.  If we're not just writing, clobber the
	 * register so that it may be reloaded below.  Otherwise simply
	 * invalidate it so we know it is free later.
	 */
	if (use == rwrite) {
		slot_kill_readonce(slot);
		register_invalidate(slot->regno);
		slot_invalidate(slot);
		if (type == Rlong || type == Rdouble) {
			register_invalidate(slot[1].regno);
			slot_invalidate(&slot[1]);
		}
	}
	else {
		clobberRegister(slot->regno);
		if (type == Rlong || type == Rdouble) {
			clobberRegister(slot[1].regno);
		}
	}

	/* Setup the new slot/register mapping and delete old one */
	regi->slot = slot;
	regi->ctype = type;
	slot->regno = reg;

	/* Reload a slot if we are not writing to it */
	if (use & rread) {
		reload(slot);
	}
	slot->modified = 0;

	/* Mark as used */
	slot->modified |= use;
	regi->used = ++usecnt;

	/* If register is destroyed by reading, then destroy it */
	if ((use & rread) && (regi->flags & enable_readonce)) {
		register_invalidate(slot->regno);
		slot_invalidate(slot);
	}

	/* Return register */
	return (regi->regno);
}

/*
 * A slot and register have alreay been allocated.  Just need to update
 *  the state.
 */
int
fastSlotRegister(SlotInfo* slot, int type, int use)
{
	int reg;
	kregs* regi;

	/* If register already allocated, use it */
	reg = slot->regno;
	regi = &reginfo[reg];

	/* Update the current mode of the register.  */
	regi->ctype = type;

	/* Mark as used */
	slot->modified |= use;
	regi->used = ++usecnt;

	if ((use & rwrite)) {
		slot->modified &= ~rnowriteback;
	}

	/* If register is destroyed by reading, then destroy it */
	if ((use & rread) && (regi->flags & enable_readonce)) {
		register_invalidate(slot->regno);
		slot_invalidate(slot);
	}

	/* Return register */
	return (regi->regno);
}

/*
 * Clobber a register.
 * This will spill the register if its inuse and give it an undefined
 *  value.
 */
void
clobberRegister(int reg)
{
	SlotInfo* pslot;
	kregs* regi;

	if (reg != NOREG) {
		regi = &reginfo[reg];
		pslot = regi->slot;
		if (pslot != NOSLOT) {
			if ((pslot->modified & rwrite) != 0 || (regi->flags & enable_readonce)) {
				spill(pslot);
				pslot->modified = 0;
			}
			slot_invalidate(pslot);
		}
		register_invalidate(reg);
		regi->used = ++usecnt;
	}
}

/*
 * Force a slot to correspond to a specific register.
 *  Register should have been saved elsewhere.
 */
void
forceRegister(SlotInfo* slot, int reg, int type)
{
	/* Invalidate the current register in this slot - don't spill
	 * it 'cause we will be rebinding the slot.
	 */
	if (slot->regno != NOREG) {
		register_invalidate(slot->regno);
	}

	/* Clobber the register we are about to reassign - this may already
	 * be free (or have been clobbered before).
	 */
	clobberRegister(reg);

	/* Setup the slot with the desirable register */
	slot->regno = reg;
	slot->modified = rwrite;
	reginfo[reg].slot = slot;
	reginfo[reg].used = ++usecnt;

	assert((reginfo[reg].type & type) == type);
	reginfo[reg].ctype = type;

	/* Finally, we assume the copy is done elsewhere */
}

/*
 * Returns the absolute offset of a slot in the current frame.  If
 * the slot is in a register, it is spilled.
 */
int
slowSlotOffset(SlotInfo* slot, int type, int use UNUSED)
{
	/* If slot is in a register, clobber the register to force it back
	 * into memory.
 	 */
	clobberRegister(slot[0].regno);
	if (type == Rlong || type == Rdouble) {
		clobberRegister(slot[1].regno);
	}

	return (SLOT2FRAMEOFFSET(slot));
}
