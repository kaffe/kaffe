/* reference.h
 * Interface declaration of the Kaffe/Java reference internal API.
 *
 * Copyright (c) 2005
 *      The Kaffe.org's developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __KAFFEVM_REFERENCE_H
#define __KAFFEVM_REFERENCE_H

#include <jni.h>

typedef enum {
  KGC_PHANTOM_REFERENCE,
  KGC_WEAK_REFERENCE,
  KGC_SOFT_REFERENCE
} kgc_reference_type;

typedef enum {
  KGC_DEFAULT_FINALIZER,
  KGC_REFERENCE_FINALIZER,
  KGC_OBJECT_REFERENCE_FINALIZER
} kgc_finalizer_type;

typedef void (*KaffeVM_Finalizer)(jobject obj);

/**
 * This function must be called if somebody wants to register a new java object
 * reference. The reference is assumed inheriting from java.lang.ref.Reference.
 *
 * @param reference the reference object.
 * @param obj the object to track.
 * @param reftype the type of reference.
 */
void KaffeVM_registerObjectReference(jobject reference, jobject obj, kgc_reference_type reftype);

/**
 * Returns true if the given object is registered to be referenced.
 *
 * @param obj a valid object.
 */
bool KaffeVM_isReferenced(jobject obj);

/**
 * This changes the implementation of the finalizer for the given object.
 * This should be handled with care as reference and objects cannot be mixed.
 *
 * @param obj the object which we want to change the finalizer.
 * @param fintype the new finalizer type to use for this object.
 */
void KaffeVM_setFinalizer(jobject obj, kgc_finalizer_type fintype);

/**
 * This initialize the reference subsystem. No call to another function
 * of this subsystem should be invoked before this one.
 */
void KaffeVM_referenceInit(void);

#endif

