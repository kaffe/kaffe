/*
 * verify-uninit.c
 *
 * Copyright 2004
 *   Kaffe.org contributors. See ChangeLog for details. All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Interface to the code for handing of uninitialized type in the verifier.
 */

#include "verify.h"

/*
 * checkUninit()
 *     To be called when dealing with (get/put)field access.  Makes sure that get/putfield and
 *     invoke* instructions have access to the instance fields of the object in question.
 */
extern bool checkUninit(Hjava_lang_Class* this, Type* type);

/*
 * pushUninit()
 *    Adds an unitialized type to the list of uninitialized types.
 *
 *    uninits is the front of the list to be added onto.
 */
extern UninitializedType* pushUninit(UninitializedType* uninits, const Type* type);

/*
 * popUninit()
 *     Pops an uninitialized type off of the operand stack
 */
extern void popUninit(const Method* method, UninitializedType* uninit, BlockInfo* binfo);

/*
 * freeUninits
 *    frees a list of unitialized types
 */

extern void freeUninits(UninitializedType* uninits);
