/* seq.c
 * Pseudo instruction sequences.
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
#include "kaffe/jmalloc.h"
#include "seq.h"
#include "gc.h"
#include "support.h"
#include "stats.h"

sequence* firstSeq;
sequence* lastSeq;
sequence* currSeq;

/*
 * Reset the sequence list.
 */
void
initSeq(void)
{
	currSeq = firstSeq;
}

/*
 * Allocate a new sequence element.
 */
sequence*
nextSeq(void)
{
	int i;
	sequence* ret = currSeq;

	if (ret == 0) {
		/* Allocate chunk of sequence elements */
		ret = KCALLOC(ALLOCSEQNR, sizeof(sequence));
		addToCounter(&jitmem, "jitmem-temp", 1,
			ALLOCSEQNR * sizeof(sequence));

		/* Attach to current chain */
		if (lastSeq == NULL) {
			firstSeq = ret;
		}
		else {
			lastSeq->next = ret;
		}
		lastSeq = &ret[ALLOCSEQNR-1];

		/* Link elements into list */
		for (i = 0; i < ALLOCSEQNR-1; i++) {
			ret[i].next = &ret[i+1];
		}
		ret[ALLOCSEQNR-1].next = NULL;
	}
	currSeq = ret->next;
	return (ret);
}
