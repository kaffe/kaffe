/*
 * kaffe.lang.Application.c
 *
 * Copyright (c) 1996, 1997, 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "kaffe_lang_Application.h"
#include "kaffe_lang_ThreadStack.h"
#include <native.h>

extern void exitThread(void);

void
Java_kaffe_lang_Application_exit0(JNIEnv* env, jobject application)
{
	exitThread();
}

/*
 * Generate an array of classes representing the classes on the current
 * threads stack.
 */
jarray
Java_kaffe_lang_Application_classStack0(JNIEnv *env, jclass cls)
{
	return (kaffe_lang_ThreadStack_getClassStack());
}
