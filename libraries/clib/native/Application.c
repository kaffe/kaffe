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
#include <jni.h>
#include <native.h>

extern void exitThread(void);
extern HArrayOfObject* java_lang_SecurityManager_getClassContext0(void);

void
Java_kaffe_lang_Application_exit0(JNIEnv* env, jobject application)
{
	exitThread();
}

/*
 * Generate an array of classes representing the classes on the current
 * threads stack.
 */
HArrayOfObject*
kaffe_lang_Application_classStack0(void)
{
	return (java_lang_SecurityManager_getClassContext0());
}
