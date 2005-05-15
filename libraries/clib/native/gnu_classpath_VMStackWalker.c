/*
 * gnu_classpath_VMStackWalker.c
 *
 * Copyright (c) 2005
 *      Kaffe.org contributors.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "gnu_classpath_VMStackWalker.h"
#include "kaffe_lang_ThreadStack.h"

JNIEXPORT jobjectArray JNICALL 
Java_gnu_classpath_VMStackWalker_getClassContext(JNIEnv* env UNUSED, jclass clazz UNUSED)
{
	return kaffe_lang_ThreadStack_getClassStack();
}

