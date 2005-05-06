/* labels.c
 * Manage the code labels and links.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "gtypes.h"
#include "classMethod.h"
#include "kaffe/jmalloc.h"
#include "labels.h"
#include "code-analyse.h"
#include "itypes.h"
#include "seq.h"
#include "constpool.h"
#include "gc.h"
#include "md.h"
#include "support.h"
#include "errors.h"
#include "machine.h"
#include "stats.h"

static label* firstLabel;
static label* lastLabel;
static label* currLabel;

#if defined(KAFFE_VMDEBUG)
static uint32 labelCount;
#endif

void
KaffeJIT_resetLabels(void)
{
	currLabel = firstLabel;
}


/*
 * Set epilogue label destination.
 */
void
KaffeJIT_setEpilogueLabel(uintp to)
{
	label* l;

	for (l = firstLabel; l != currLabel; l = l->next) {
		if ((l->type & Ltomask) == Lepilogue) {
			l->type = Linternal | (l->type & ~Ltomask);
			l->to = to;
		}
	}
}


/*
 * Link labels.
 * This function uses the saved label information to link the new code
 * fragment into the program.
 */
void
KaffeJIT_linkLabels(codeinfo* codeInfo, uintp codebase)
{
	long dest = 0;
	int* place;
	label* l;

	for (l = firstLabel; l != currLabel; l = l->next) {

		/* Ignore this label if it hasn't been used */
		if (l->type == Lnull) {
			continue;
		}

		/* Find destination of label */
		switch (l->type & Ltomask) {
		case Lexternal:
			dest = l->to;	/* External code reference */
			break;
		case Lconstant:		/* Constant reference */
			dest = ((constpool*)l->to)->at;
			break;
		case Linternal:		/* Internal code reference */
		/* Lepilogue is changed to Linternal in KaffeJIT_setEpilogueLabel() */
			dest = l->to + codebase;
			break;
		case Lcode:		/* Reference to a bytecode */
			assert(INSNPC(l->to) != (uintp)-1);
			dest = INSNPC(l->to) + codebase;
			break;
		case Lgeneral:		/* Dest not used */
			break;
		default:
			goto unhandled;
		}

		/* Find the source of the reference */
		switch (l->type & Lfrommask) {
		case Labsolute:		/* Absolute address */
			break;
		case Lrelative:		/* Relative address */
			dest -= l->from + codebase;
			break;
		default:
			goto unhandled;
		}

		/* Find place to store result */
		place = (int*)(l->at + codebase);

		switch (l->type & Ltypemask) {
		case Lquad:
			*(uint64*)place = dest;
			break;
		case Llong:
			*(uint32*)place = dest;
			break;

		/* Machine specific labels go in this magic macro */
			EXTRA_LABELS(place, dest, l);

		default:
		unhandled:
			printf("Label type 0x%x not supported (%p).\n", l->type, l);
			KAFFEVM_ABORT();
		}
#if 0
		/*
		 * If we were saving relocation information we must save all
		 * labels which are 'Labsolute', that is they hold an absolute
		 * address for something.  Note that this doesn't catch
		 * everything, specifically it doesn't catch string objects
		 * or references to classes.
		 */
		if ((l->type & Labsolute) != 0) {
			l->snext = savedLabel;
			savedLabel = l;
		}
#endif
	}
}

/*
 * Allocate a new label.
 */
label*
KaffeJIT_newLabel(void)
{
	int i;
	label* ret = currLabel;

	if (ret == 0) {
		/* Allocate chunk of label elements */
		ret = KCALLOC(ALLOCLABELNR, sizeof(label));
		addToCounter(&jitmem, "jitmem-temp", 1, 
			ALLOCLABELNR * sizeof(label));

		/* Attach to current chain */
		if (lastLabel == 0) {
			firstLabel = ret;
		}
		else {
			lastLabel->next = ret;
		}
		lastLabel = &ret[ALLOCLABELNR-1];

		/* Link elements into list */
		for (i = 0; i < ALLOCLABELNR-1; i++) {
#if defined(KAFFE_VMDEBUG)
			sprintf(ret[i].name, "L%d", labelCount + i);
#endif

			ret[i].next = &ret[i+1];
		}
		ret[ALLOCLABELNR-1].next = NULL;
	}
	currLabel = ret->next;
#if defined(KAFFE_VMDEBUG)
	labelCount += 1;
#endif
	return (ret);
}

label*
KaffeJIT_getInternalLabel(label **lptr, uintp pc)
{
	label *curr, *retval = NULL;

	assert(lptr != NULL);
	
	if( *lptr == NULL )
	{
		/* Start at the head of the list. */
		*lptr = firstLabel;
	}
	curr = *lptr;
	while( curr && (curr != currLabel) && !retval )
	{
		switch( curr->type & Ltomask )
		{
		case Linternal:
			if( curr->to == pc )
			{
				*lptr = curr->next;
				retval = curr;
			}
			break;
		case Lcode:
		  /* Commented out since codeInfo works
		   * differently on jit and jit3.

			if( INSNPC(curr->to) == pc )
			{
				*lptr = curr->next;
				retval = curr;
			}
		  */
			break;
		}
		curr = curr->next;
	}
	return( retval );
}
