/*
 * jni_native.h
 * Java Native Interface - Handles native JNI call wrapping.
 *
 * Copyright (c) 2004
 *      The Kaffe.org's developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef KAFFE_JNI_NATIVE_H
#define KAFFE_JNI_NATIVE_H

#include "config.h"
#include "classMethod.h"
#include "jtypes.h"

void KaffeVM_JNI_wrapper(Method* xmeth, void* func);
void KaffeVM_KNI_wrapper(Method* xmeth, void* func);
jint KaffeVM_JNI_native(Method* meth);

#endif
