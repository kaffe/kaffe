/* slots.c
 * Slots.
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
#include "slots.h"
#include "seq.h"
#include "md.h"
#include "registers.h"
#include "gc.h"
#include "machine.h"
#include "debug.h"

/**
 * number of elements in basicslots.
 */
int maxslot;

/**
 * array containing all slots of the method being translated.
 */
SlotInfo* basicslots;

/**
 * same as basicslots ???
 */
SlotInfo* slotinfo;

/**
 * array of locals of the method being translated.
 *
 * the first localsz entries correspond to the local variables as per vm spec,
 * the last stacksz entries correspond to the entries of the operand stack
 *
 * The bottom of the operand stack is the last entry of this array and it
 * grows downwards (towards smaller indices).
 */
SlotInfo* localinfo;

/**
 * array of temp slots being used by the generated native code.
 */
SlotInfo* tempinfo;

/**
 * the slot that contains the stack limit.
 *
 * only used when STACK_LIMIT is #defined by the backend. 
 */
SlotInfo stack_limit[1];

static SlotData* basicdatas;
static int lastnrslots = 0;
#if defined(STACK_LIMIT)
static SlotData stack_limit_data;
#endif

/**
 * Initializes the slots for the jitted method.
 *
 * @param islots number of slots that are needed.
 */
void
initSlots(int islots)
{
	int i;
	int nrslots = islots;

	/* Allocate extra slots for temps */
	nrslots += MAXTEMPS;

	/* Make sure we have enough slots space */
	if (nrslots > lastnrslots) {
		basicslots = gc_realloc(basicslots,
					nrslots * sizeof(SlotInfo),
					KGC_ALLOC_JIT_SLOTS);
		basicdatas = gc_realloc(basicdatas,
					nrslots * sizeof(SlotData),
					KGC_ALLOC_JIT_SLOTS);
		lastnrslots = nrslots;
	}
	/* Set 'maxslot' to the maximum slot usable (excluding returns) */
	maxslot = nrslots;

	/* Setup various slot pointers */
	slotinfo = &basicslots[0];
	localinfo = &slotinfo[0];
	tempinfo = &localinfo[islots];

        /* Setup all slots */
        for (i = 0; i < nrslots; i++) {
		basicslots[i].slot = &basicdatas[i];
		basicdatas[i].regno = NOREG;
		basicdatas[i].modified = 0;
		basicdatas[i].rnext = NULL;
		basicdatas[i].rseq = NULL;
		basicdatas[i].wseq = NULL;
		basicdatas[i].offset = SLOT2FRAMEOFFSET(&basicslots[i]);
		basicdatas[i].global = GL_NOGLOBAL;
        }

#if defined(STACK_LIMIT)
	/* And setup the stack offset */
	stack_limit->slot = &stack_limit_data;
	stack_limit_data.regno = NOREG;
	stack_limit_data.modified = 0;
	stack_limit_data.rnext = NULL;
	stack_limit_data.rseq = NULL;
	stack_limit_data.wseq = NULL;
	stack_limit_data.offset = SLOT2FRAMEOFFSET(stack_limit);
	stack_limit_data.global = GL_NOGLOBAL;
#endif
}

/**
 * Setup for the beginning of a basic block.
 *
 * Sets the rseq and wseq fields of all slots to 0.
 */
void
setupSlotsForBasicBlock(void)
{
	int i;
	SlotData* sdata;

	for (i = 0; i < maxslot; i++) {
		sdata = slotinfo[i].slot;
		sdata->rseq = NULL;
		sdata->wseq = NULL;
	}

#if defined(STACK_LIMIT)
	sdata = stack_limit->slot;
	sdata->rseq = NULL;
	sdata->wseq = NULL;
#endif
}

/**
 * Record that a few slots are not used any longer. 
 *
 * @param data array of slots that are not used further in the basic block.
 * @param nr   number of slots in data
 */
void
lastuse_slot(SlotInfo* data, int nr)
{
	SlotData* sdata;

	for (; nr > 0; nr--, data++) {
		sdata = data->slot;
		if (sdata->rseq != NULL && !isGlobal(sdata)) {
			sdata->rseq->lastuse |= 1 << sdata->rseqslot;
		}
		sdata->wseq = NULL;
		sdata->rseq = NULL;
	}
}
