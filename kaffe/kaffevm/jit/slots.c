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
#include "registers.h"
#include "md.h"
#include "gc.h"

SlotInfo* basicslots;
SlotInfo* slotinfo;
int maxslot;

SlotInfo* localinfo;
SlotInfo* tempinfo;

extern int maxLocal;

/*
 * Initiate slots.
 */
void
initSlots(int nrslots)
{
	int i;
	static int lastnrslots = 0;

	/* Allocate extra slots for temps */
	nrslots += MAXTEMPS;

	/* Make sure we have enough slots space */
	if (nrslots > lastnrslots) {
		basicslots = gc_realloc_fixed(basicslots, nrslots * sizeof(SlotInfo));
		lastnrslots = nrslots;
	}
	/* Set 'maxslot' to the maximum slot usable (excluding returns) */
	maxslot = nrslots;

        /* Free all slots */
        for (i = 0; i < nrslots; i++) {
		basicslots[i].regno = NOREG;
		basicslots[i].modified = 0;
        }

	/* Setup various slot pointers */
	slotinfo = &basicslots[0];
	localinfo = 0;
	tempinfo = 0;
}
