/*
 * verify-sigstack.c
 *
 * Copyright 2004
 *   Kaffe.org contributors. See ChangeLog for details. All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Code for handing of signature stack in the verifier.
 */

#include "errors.h"
#include "gc.h"
#include "verify-sigstack.h"

/*
 * pushSig()
 *     Pushes a new signature on the Stack
 */
SigStack*
pushSig(SigStack* sigs, const char* sig)
{
	SigStack* new_sig = checkPtr(gc_malloc(sizeof(SigStack), KGC_ALLOC_VERIFIER));
	new_sig->sig = sig;
	new_sig->next = sigs;
	return new_sig;
}


/*
 * freeSigStack()
 *     Frees the memory consumed by a stack of names and signatures.
 */
void
freeSigStack(SigStack* sigs)
{
	SigStack* tmp;
	while(sigs != NULL) {
		tmp = sigs->next;
		gc_free(sigs);
		sigs = tmp;
	}
}
