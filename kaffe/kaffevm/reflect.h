/*
 * reflect.h
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *      The Kaffe.org's developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
#ifndef __KAFFE_REFLECT_H
#define __KAFFE_REFLECT_H

#include "java_lang_reflect_Constructor.h"
#include "java_lang_reflect_Field.h"
#include "java_lang_reflect_Method.h"

Hjava_lang_reflect_Constructor*  KaffeVM_makeReflectConstructor(struct Hjava_lang_Class* clazz, int slot);
Hjava_lang_reflect_Method*       KaffeVM_makeReflectMethod(struct Hjava_lang_Class* clazz, int slot);
Hjava_lang_reflect_Field*        KaffeVM_makeReflectField(struct Hjava_lang_Class* clazz, int slot);

#endif
