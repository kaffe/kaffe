/*
 * verify-uninit.c
 *
 * Copyright 2004
 *   Kaffe.org contributors. See ChangeLog for details. All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Code for handing of uninitialized type in the verifier.
 */

#include "verify-uninit.h"

/*
 * checkUninit()
 *     To be called when dealing with (get/put)field access.  Makes sure that get/putfield and
 *     invoke* instructions have access to the instance fields of the object in question.
 */
bool
checkUninit(Hjava_lang_Class* this, Type* type)
{
	if (type->tinfo & TINFO_UNINIT) {
		if (type->tinfo & TINFO_UNINIT_SUPER) {
			UninitializedType* uninit = type->data.uninit;
			Type t;
			t.tinfo = TINFO_CLASS;
			t.data.class = this;
			
			if (!sameType(&uninit->type, &t)) {
				return false;
			}
		}
		else {
			return false;
		}
	}
	
	return true;
}

/*
 * pushUninit()
 *    Adds an unitialized type to the list of uninitialized types.
 *
 *    uninits is the front of the list to be added onto.
 */
UninitializedType*
pushUninit(UninitializedType* uninits, const Type* type)
{
	UninitializedType* uninit = checkPtr(gc_malloc(sizeof(UninitializedType), KGC_ALLOC_VERIFIER));
	uninit->type = *type;
	uninit->prev = NULL;
	
	if (!uninits) {
		uninit->next = NULL;
		return uninit;
	}
	
	uninit->prev = NULL;
	uninit->next = uninits;
	uninits->prev = uninit;
	return uninit;
}

/*
 * popUninit()
 *     Pops an uninitialized type off of the operand stack
 */
void
popUninit(const Method* method, UninitializedType* uninit, BlockInfo* binfo)
{
	uint32 n;
	
	for (n = 0; n < method->localsz; n++) {
		if (binfo->locals[n].tinfo & TINFO_UNINIT &&
		    ((UninitializedType*)binfo->locals[n].data.class) == uninit) {
			binfo->locals[n] = uninit->type;
		}
	}
	
	for (n = 0; n < binfo->stacksz; n++) {
		if (binfo->opstack[n].tinfo & TINFO_UNINIT &&
		    ((UninitializedType*)binfo->opstack[n].data.class) == uninit) {
			binfo->opstack[n] = uninit->type;
		}
	}
	
	if (uninit->prev) {
		uninit->prev->next = uninit->next;
	}
	if (uninit->next) {
		uninit->next->prev = uninit->prev;
	}
	
	gc_free(uninit);
}

/*
 * freeUninits
 *    frees a list of unitialized types
 */
void
freeUninits(UninitializedType* uninits)
{
	UninitializedType* tmp;
	while (uninits) {
		tmp = uninits->next;
		gc_free(uninits);
		uninits = tmp;
	}
}
