/* seq.c
 * Pseudo instruction sequences.
 *
 * Copyright (c) 1996, 1997, 2003
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "gtypes.h"
#include "seq.h"
#include "gc.h"

static sequencechunk* sequencechunks;
sequence* firstSeq;
sequence* lastSeq;
sequence* currSeq;
sequence* activeSeq;

/**
 * Reset the sequence list.
 */
void
initSeq(void)
{
	currSeq = firstSeq;
	while( (sequencechunks != NULL) && (sequencechunks->next != NULL) )
	{
		sequencechunk *sc = sequencechunks;

		sequencechunks = sc->next;
		gc_free(sc);
	}
	if( sequencechunks != NULL )
	{
		lastSeq = &sequencechunks->data[ALLOCSEQNR - 1];
		lastSeq->next = NULL;
	}
}

/**
 * Allocate a new sequence element.
 */
sequence*
nextSeq(void)
{
	sequence* ret;

	ret = currSeq;
	if (ret == 0) {
		sequencechunk *sc;
		
		int i;
		/* Allocate chunk of sequence elements */
		sc = gc_malloc(sizeof(sequencechunk), KGC_ALLOC_JIT_SEQ);
		assert(sc != NULL);

		sc->next = sequencechunks;
		sequencechunks = sc;
		
		ret = &sc->data[0];
		
		/* Attach to current chain */
		if (lastSeq == 0) {
			firstSeq = ret;
		}
		else {
			lastSeq->next = ret;
		}
		lastSeq = &sc->data[ALLOCSEQNR-1];

		/* Link elements into list */
		for (i = 0; i < ALLOCSEQNR-1; i++) {
			sc->data[i].next = &sc->data[i+1];
		}
	}
	currSeq = ret->next;
	ret->lastuse = 0;
	ret->refed = 1;
	ret->jflags = willcatch;
	activeSeq = ret;
	return (ret);
}
