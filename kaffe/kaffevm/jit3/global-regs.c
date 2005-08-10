/* global-regs.c
 * Setup global registers.
 *
 * Copyright (c) 1996-1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003, 2004, 2005
 *	Kaffe.org contributors. See ChangeLog for details. All rights reserved.
 *  
 * Cross-language profiling changes contributed by
 * the Flux Research Group, Department of Computer Science,
 * University of Utah, http://www.cs.utah.edu/flux/
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "config.h"
#include "code-analyse.h"
#include "gc.h"
#include "global-regs.h"
#include "itypes.h"
#include "machine.h"
#include "md.h"
#include "registers.h"
#include "slots.h"

#if defined(NR_GLOBALS)

static
void
bindSlotToGlobal(int lcl, int r, int type)
{
	kregs* reg;
	SlotData* dslot;
	int gtype;

	reg = &reginfo[r];
	dslot = localinfo[lcl].slot;

	reg->ctype = type;
	reg->type |= Rglobal;
	reg->refs++;
	reg->slot = dslot;
	dslot->regno = r;

	gtype = GL_ISGLOBAL;

	/* If this is an argument then pre-load */
	if (lcl < maxArgs) {
		gtype |= GL_PRELOAD;
	}
	/* If this slot is never written note that the global is
	 * read only.
	 */
	if (codeInfo->localuse[lcl].write == -1) {
		gtype |= GL_RONLY;
	}
	setGlobal(dslot, gtype);
}

static
int
sortSlots(const void* s1, const void* s2)
{
	localUse* lcluse1;
	localUse* lcluse2;

	lcluse1 = &codeInfo->localuse[*((SlotInfo* const*)s1) - localinfo];
	lcluse2 = &codeInfo->localuse[*((SlotInfo* const*)s2) - localinfo];

	if (lcluse1->use == lcluse2->use) {
		return (0);
	}
	else if (lcluse1->use > lcluse2->use) {
		return (-1);
	}
	else {
		return (1);
	}
}

/*
 * Setup global registers
 */
void
setupGlobalRegisters(void)
{
	SlotInfo** slots;
	int j;
	int max;

	/* If we don't have any code info we can't do any global
	 * optimization
	 */
	if ((codeInfo == 0) || (codeInfo->localuse == 0)) {
		return;
	}

	/* Allocate an array for the slot pointers and copy them in */
	slots = gc_malloc((1+maxLocal) * sizeof(SlotInfo*), KGC_ALLOC_JIT_SLOTS);
	for (j = 0; j < maxLocal; j++) {
		slots[j] = &localinfo[j];
	}

	/* Sort the slots to the most used is first */
	qsort(slots, maxLocal, sizeof(SlotInfo*), sortSlots);

	/* Allocate the slots to globals */
	max = NR_GLOBALS;
	for (j = 0; j < maxLocal && max > 0; j++) {
		int k;
		int i;
		localUse* lcl;

		i = slots[j] - localinfo;
		lcl = &codeInfo->localuse[i];

		for (k = 0; k < MAXREG; k++) {
			kregs* reg;

			reg = &reginfo[k];
			if ((reg->flags & Rglobal) == 0) {
				/* Not a global */
			}
			else if ((reg->type & Rglobal) != 0) {
				/* Already used */
			}
			else if (lcl->type == TINT && (reg->type & Rint) != 0) {
				bindSlotToGlobal(i, k, Rint);
				max--;
				break;
			}
			else if (lcl->type == TOBJ && (reg->type & Rref) != 0) {
				bindSlotToGlobal(i, k, Rref);
				max--;
				break;
			}
		}
	}

	gc_free(slots);
}

#endif
