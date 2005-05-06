/* registers.c
 * Manage the machine registers.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#define SCHK(s)

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "gtypes.h"
#include "bytecode.h"
#include "slots.h"
#include "md.h"
#include "icode.h"
#include "registers.h"
#include "seq.h"
#include "icode.h"
#include "labels.h"
#include "codeproto.h"
#include "gc.h"
#include "funcs.h"
#include "debug.h"

#include "kaffe_jni.h"

static void spill(SlotData*);

/**
 * Define the registers.
 *
 * initialized with the REGISTER_SET macro to be defined by the backend
 *
 * There are two ways of how registers are identified. One is the jitter id,
 * which is an index into this array. The other one is the backend id, which
 * is the value of the regno field of the reginfo struct.
 */
kregs reginfo[] = {
	REGISTER_SET
	{ /* BAD */	NULL, 0, 0, 0, 0, 0, 0 }
};

/**
 * This is horrible but necessary at the moment.  Sometime we need to
 * make transient changes to the registers which we will forget in
 * a short while.  This can have a bad effect on read-once register so
 * we disable them termporaily.
 */
int enable_readonce = Rreadonce;

/**
 * Number of register assignments done so far.
 * 
 * Gives an idea of which register is to be reused.
 */
static int usecnt = 0;

/**
 * Initialize the registers.
 *
 * 
 */
void
initRegisters(void)
{
	int i;

	/* Free all registers */
	for (i = 0; i < MAXREG; i++) {
		reginfo[i].slot = NOSLOT;
		reginfo[i].used = 0;
		reginfo[i].refs = 0;
		reginfo[i].type &= ~Rglobal;
	}
}

/**
 * Spill a slot.
 *
 * @param sd the slot to spill
 * @param clean if true, sd->modified is set to 0
 */
void
spillAndUpdate(SlotData* sd, jboolean clean)
{
	if (sd->modified != 0) {
		spill(sd);
		if (clean) {
			sd->modified = 0;
		}
	}
	if ((reginfo[sd->regno].flags & enable_readonce) != 0) {
		slot_invalidate(sd);
	}
}

/*
 * Spill a register using the correct spill function.
 */
static
void
spill(SlotData* s)
{
#if defined(HAVE_spill_long)
	if (reginfo[s->regno].ctype & Rlong) {
		spill_long(s);
	}
	else
#endif
#if defined(HAVE_spill_int)
	if (reginfo[s->regno].ctype & (Rint|Rsubint)) {
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

/**
 * Reload a register using the correct reload function.
 *
 * @param s the slot that is to be reloaded
 */
void
reload(SlotData* s)
{
#if defined(HAVE_reload_long)
	if (reginfo[s->regno].ctype & Rlong) {
		reload_long(s);
	}
	else
#endif
#if defined(HAVE_reload_int)
	if (reginfo[s->regno].ctype & (Rint|Rsubint)) {
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

/**
 * ??
 *
 * @param s 
 */
void
slot_kill_readonce(SlotData *s)
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
 * Move one register from to another.
 */
static
int
move_register(int toreg, int fromreg)
{
#if defined(HAVE_move_register_long)
	if (reginfo[toreg].type & Rlong) {
		HAVE_move_register_long(reginfo[toreg].regno, reginfo[fromreg].regno);
		return (1);
	}
	else
#endif
#if defined(HAVE_move_register_int)
	if (reginfo[toreg].type & (Rint|Rsubint)) {
		HAVE_move_register_int(reginfo[toreg].regno, reginfo[fromreg].regno);
		return (1);
	}
	else
#endif
#if defined(HAVE_move_register_ref)
	if (reginfo[toreg].type & Rref) {
		HAVE_move_register_ref(reginfo[toreg].regno, reginfo[fromreg].regno);
		return (1);
	}
	else
#endif
#if defined(HAVE_move_register_double)
	if (reginfo[toreg].type & Rdouble) {
		HAVE_move_register_double(reginfo[toreg].regno, reginfo[fromreg].regno);
		return (1);
	}
	else
#endif
#if defined(HAVE_move_register_float)
	if (reginfo[toreg].type & Rfloat) {
		HAVE_move_register_float(reginfo[toreg].regno, reginfo[fromreg].regno);
		return (1);
	}
	else
#endif
	{
		return (0);
	}
}

static
int
allocRegister(int idealreg, int type)
{
	int reg;
	uint32 used;
	int creg;
	kregs* regi;
	int vreg = 0;

	reg = idealreg;
	if (reg == NOREG) {
		/* Allocate a register - pick the the least recently used */
		used = 0xFFFFFFFF;
		for (creg = 0; creg < MAXREG; creg++) {
			regi = &reginfo[creg];
			if( regi->type & (Rglobal | Reserved) )
			{
				/* Not available. */
			}
			else if( (regi->type & type) == type )
			{
				/* Right type. */
#if defined(ALLOC_REGISTERS_SEQUENTIALLY)
				if( regi->used < used )
				{
					used = regi->used;
					reg = creg;
				}
#else
				if( (!vreg &&
				     (regi->flags & Rnosaveoncall) &&
				     (regi->used == used)) ||
				    (regi->used < used) )
				{
					if( !(regi->flags & Rnosaveoncall) )
						vreg = 1;
					else
						vreg = 0;
					used = regi->used;
					reg = creg;
				}
#endif
			}
		}
	}

	assert(reg != NOREG);

	return (reg);
}

/**
 * Assign a register to a slot. 
 *
 * @param slot the slot to be moved into a register.
 * @param type type of the slot
 * @param use  whether the slot is loaded for reading, writing or both. 
 * @param idealreg if the slot is to be moved into a particular register,
 *                 this is the jitter id of it (NOREG if any register is ok).
 * @return backend id of assigned register
 *
 * Perform the necessary spills and reloads to make this happen.
 */
int
slotRegister(SlotData* slot, int type, int use, int idealreg)
{
	int reg;
	kregs* regi;
	int needreload;
	int oldmod;

SCHK(	sanityCheck();						)

	reg = slot->regno;
	regi = &reginfo[reg];

	/* Do global register stuff before anything else.
	 * Note that we ignore the ideal register for globals since we
	 * cannot change the register assignment for globals.
	 */
	if (isGlobal(slot)) {
		/* If requested type cannot be honoured, we return NOREG
		 * and let the caller deal with it.
		 */
		if ((regi->type & type) == 0) {
			return (NOREG);
		}

		/* If we're reading or the register is only in my use
		 * then we can use it directly.
		 */
		if (use == rread || regi->refs == 1) {
			/* Nothing to do */
		}
		/* We're writing to a shared global - we must make it
		 * unshared.
		 */
		else {
			/* Simple - just clobber the register to
			 * force the shares back to memory.
			 */
			clobberRegister(reg);
		}
		slot->modified |= use;
		return (reg);
	}

	/* If we're reading and this is the right type or register then
	 * use it.
	 */
	if ((reg == idealreg || idealreg == NOREG) && use == rread && (regi->type & type) != 0) {
		regi->ctype = regi->type & type;
	}
	/* If we're writing and we're not sharing this register and it's
	 * the right type then use it.
	 */
	else if ((reg == idealreg || idealreg == NOREG) && regi->refs == 1 && (regi->type & type) != 0) {
		slot_kill_readonce(regi->slot);
		regi->ctype = regi->type & type;
	}
	/* Otherwise reallocate */
	else {
		/* Get a register and clobber what it for reuse */
		reg = allocRegister(idealreg, type);
		clobberRegister(reg);

SCHK(		sanityCheck();					)

		/* If we're modifying this slot, clear the modified bits
		 * so when we clobber it, it won't get written back.
		 */
		if (use == rwrite) {
			slot[0].modified = 0;
			if (type == Rlong || type == Rdouble) {
				slot[1].modified = 0;
			}
		}

		/* Work out whether we'll need to reload the register */
		if ((use & rread) != 0) {
			needreload = 1;
		}
		else {
			needreload = 0;
		}

		/* If register types are compatible then we can just
		 * move the value from one register to the other (if supported).
		 * Note that since we've not writing it back we must preserve
		 * the modified status on the slot which is cleared by
		 * the slot_invalidate call.
		 */
		if (reginfo[slot[0].regno].type == reginfo[reg].type &&
		    needreload != 0 && move_register(reg, slot[0].regno) != 0) {
			oldmod = slot[0].modified;
			slot_invalidate(&slot[0]);
			slot[0].modified = oldmod;
			needreload = 0;
		}
		else {
			/*
			 * Spill anything which might be in the slot if its
			 * dirty.  Then invalidate the slot so we can reuse.
			 */
			if (slot[0].regno != NOREG) {
				if (slot[0].modified != 0) {
					spill(&slot[0]);
				}
				slot_invalidate(&slot[0]);
			}
			if (type == Rlong || type == Rdouble) {
				if (slot[1].regno != NOREG) {
					if (slot[1].modified != 0) {
						spill(&slot[1]);
					}
					slot_invalidate(&slot[1]);
				}
			}
		}

SCHK(		sanityCheck();					)

		/* Setup the new slot/register mapping and delete old one */
		regi = &reginfo[reg];
		assert(regi->slot == 0);
		regi->slot = slot;
		regi->ctype = regi->type & type;
		assert(regi->ctype != 0);
		regi->refs = 1;
		slot->regno = reg;

		/* Reload a slot if we are not writing to it */
		if (needreload != 0) {
			assert((reginfo[reg].type & Rglobal) == 0);
			reload(slot);
		}
	}

	/* Mark as used */
	slot->modified |= use;
	regi->used = ++usecnt;

	/* If register is destroyed by reading, then destroy it */
	if ((use & rread) && (regi->flags & enable_readonce)) {
		assert(!isGlobal(slot));
		slot_invalidate(slot);
	}

SCHK(	sanityCheck();						)

	/* Return register */
	return (regi->regno);
}

/**
 * Clobber a register.
 *
 * @param reg jitter id of the register to clobber.
 *
 * This will spill the register if its inuse and give it an undefined
 * value.
 */
void
clobberRegister(int reg)
{
	SlotData* pslot;
	SlotData* nslot;
	kregs* regi;

SCHK(	sanityCheck();						)

	if (reg != NOREG) {
		regi = &reginfo[reg];
		pslot = regi->slot;
		while (pslot != NOSLOT) {
			assert(pslot->regno == reg);
			if ((pslot->modified & rwrite) != 0 || (regi->flags & enable_readonce)) {
				spill(pslot);
				pslot->modified = 0;
			}
			nslot = pslot->rnext;
			if (!isGlobal(pslot)) {
				slot_invalidate(pslot);
			}
			pslot = nslot;
		}

		/* Fake a used counter to this doesn't get recycled quickly */
		regi->used = ++usecnt;
	}

SCHK(	sanityCheck();						)
}

/**
 * Force a slot to correspond to a specific register.
 *
 * @param slot the slot the register is to be assigned to.
 * @param reg  the jitter id of the register that is to be assigned to the slot.
 * @param type type of register needed.
 *
 * Register should have been saved elsewhere.
 */
void
forceRegister(SlotData* slot, int reg, int type)
{
	kregs* regi;

SCHK(	sanityCheck();						)

	if (slot->regno != reg) {
		assert(!isGlobal(slot));
		assert((reginfo[reg].type & Rglobal) == 0);
		/*assert((reginfo[slot->regno].type & Rglobal) == 0);*/
		/* Invalidate the current register in this slot - don't spill
		 * it 'cause we will be rebinding the slot.
		 */
		slot_invalidate(slot);

		/* Clobber the register we are about to reassign - this
		 * may already be free (or have been clobbered before).
		 */
		clobberRegister(reg);
	}

	/* Setup the slot with the desirable register */
	regi = &reginfo[reg];
	slot->regno = reg;
	slot->modified = rwrite;
	regi->slot = slot;
	regi->used = ++usecnt;
	regi->refs = 1;

DBG(REGFORCE,
    dprintf ("forceRegister() set forced %d %p\n", reg, slot);
    );

	regi->ctype = regi->type & type;
	assert(regi->ctype != 0);

	/* Finally, we assume the copy is done elsewhere */

SCHK(	sanityCheck();						)
}

/**
 * Returns the absolute offset of a slot in the current frame.
 *
 * @param slot the slot whose offset is to be returned.
 * @param type type of the slot
 * @param use  ??? not used ???
 *
 * If the slot is in a register, it is spilled.
 */
int
slotOffset(SlotData* slot, int type, int use UNUSED)
{
SCHK(	sanityCheck();						)

	/* If slot is in a register, clobber the register to force it back
	 * into memory.
 	 */
	clobberRegister(slot[0].regno);
	if (type == Rlong || type == Rdouble) {
		clobberRegister(slot[1].regno);
	}

SCHK(	sanityCheck();						)

	return (slotOffsetNoSpill(slot, type));
}

/**
 * Returns the absolute offset of a slot in the current frame.
 *
 * @param slot the slot whose offset is to be returned
 * @param type type of the slot
 * @return the offset of the slot
 */
int
slotOffsetNoSpill(SlotData* slot, int type)
{
	int off0;
	int off1;

	off0 = slot[0].offset;
	if (type == Rlong || type == Rdouble) {
		off1 = slot[1].offset;
		/* For longs and doubles, which take two slots, we return
		 * the offset to the lowest in memory since that's where
		 * the data will be read/written.  Since the slots might
		 * go up or down depending we need to find both offsets
		 * and return the lowest one.
		 */
		if (off1 < off0) {
			return (off1);
		}
	}
	return (off0);
}

/**
 * Invalidates a slot.
 *
 * @param sdata the slot to invalidate
 */
void
slot_invalidate(SlotData* sdata)
{
	kregs* regi;
	SlotData** ptr;
	int reg;

SCHK(	sanityCheck();						)

	reg = sdata->regno;

	if (reg != NOREG) {
		regi = &reginfo[reg];
		if (regi->refs == 1) {
			regi->slot = NOSLOT;
			regi->used = 0;
		}
		else {
			for (ptr = &regi->slot; *ptr != NULL; ptr = &(*ptr)->rnext) {
				if (*ptr == sdata) {
					*ptr = sdata->rnext;
					sdata->rnext = NULL;
					goto found;
				}
			}
			assert("slot_invalidate: slot not found on register" == 0);

			found:;
		}
		regi->refs--;
		sdata->regno = NOREG;
	}
	sdata->modified = 0;

SCHK(	sanityCheck();						)
}

/**
 * Preload a register at the beginning of a function.
 *
 * @param slot the slot that is to be preloaded
 * @param type the type of the slot
 * @param idealreg the register that should be assigned to the slot.
 * 
 * This is used to load up the arguments which are passed
 * in registers.
 */
void
preloadRegister(SlotData* slot, int type, int idealreg)
{
	int r;
	if (isGlobal(slot)) {
		r = move_register(slot->regno, idealreg);
		assert(r != 0);
		/* Since we've got the global loaded from the argument
		 * register there is no need to preload it.
		 */
		slot->global &= ~GL_PRELOAD;
	}
	else {
		slotRegister(slot, type, rwrite, idealreg);
	}
#if defined(TRACE_METHOD_ARGS)
	if (Kaffe_JavaVMArgs.enableVerboseCall != 0) {
		spill(slot);
	}
#endif
}

void
sanityCheck(void)
{
	int i;
	int c;
	SlotData* s;

	for (i = 0; i < MAXREG; i++) {
		c = 0;
		for (s = reginfo[i].slot; s != NOSLOT; s = s->rnext) {
			assert(s->regno == i);
			c++;
		}
		assert(reginfo[i].refs == c);
	}
}
