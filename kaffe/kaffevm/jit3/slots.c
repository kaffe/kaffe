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

int maxslot;
SlotInfo* basicslots;
SlotInfo* slotinfo;
SlotInfo* localinfo;
SlotInfo* tempinfo;
SlotInfo stack_limit[1];	/* Used to represent the stack limit */

static SlotData* basicdatas;
static int lastnrslots = 0;
#if defined(STACK_LIMIT)
static SlotData stack_limit_data;
#endif

/*
 * Initiate slots.
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
					GC_ALLOC_JITTEMP);
		basicdatas = gc_realloc(basicdatas,
					nrslots * sizeof(SlotData),
					GC_ALLOC_JITTEMP);
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
		basicdatas[i].rnext = 0;
		basicdatas[i].rseq = 0;
		basicdatas[i].wseq = 0;
		basicdatas[i].offset = SLOT2FRAMEOFFSET(&basicslots[i]);
		basicdatas[i].global = GL_NOGLOBAL;
        }

#if defined(STACK_LIMIT)
	/* And setup the stack offset */
	stack_limit->slot = &stack_limit_data;
	stack_limit_data.regno = NOREG;
	stack_limit_data.modified = 0;
	stack_limit_data.rnext = 0;
	stack_limit_data.rseq = 0;
	stack_limit_data.wseq = 0;
	stack_limit_data.offset = SLOT2FRAMEOFFSET(stack_limit);
	stack_limit_data.global = GL_NOGLOBAL;
#endif
}

/*
 * Setup for the beginning of a basic block.
 */
void
setupSlotsForBasicBlock(void)
{
	int i;
	SlotData* sdata;

	for (i = 0; i < maxslot; i++) {
		sdata = slotinfo[i].slot;
		sdata->rseq = 0;
		sdata->wseq = 0;
	}

#if defined(STACK_LIMIT)
	sdata = stack_limit->slot;
	sdata->rseq = 0;
	sdata->wseq = 0;
#endif
}

/*
 * Mark a number of slots as lastuse.
 */
void
lastuse_slot(SlotInfo* data, int nr)
{
	SlotData* sdata;

	for (; nr > 0; nr--, data++) {
		sdata = data->slot;
		if (sdata->modified & rforced) {
DBG(REGFORCE,
    dprintf ("lastuse_slot() forced %d %x\n", sdata->regno, sdata);
    )
		}
		if (sdata->rseq != 0 && !isGlobal(sdata)) {
			sdata->rseq->lastuse |= 1 << sdata->rseqslot;
		}
		sdata->wseq = 0;
		sdata->rseq = 0;
	}
}
