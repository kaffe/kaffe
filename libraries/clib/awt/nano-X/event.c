/**
 * nano-X AWT backend for Kaffe.
 *
 * Copyright (c) 2001
 *	Exor International Inc. All rights reserved.
 *
 * Copyright (c) 2001
 *	Sinby Corporatin, All rights reserved.
 *
 * Copyright (c) 2005
 *	Kaffe.org contributors, see ChangeLog for details.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-setjmp.h"

#include "toolkit.h"

#include <jni.h>

void handlerInit( JNIEnv *, jclass );

jobject
Java_java_awt_Toolkit_evtInit( JNIEnv* env, jclass clazz )
{
	jclass Component;

	handlerInit( env, clazz );

	Component = (*env)->FindClass( env, "java/awt/Component");
	if ( Component == NULL ) {
		SignalError(
			"java.lang.NoClassDefFoundError", 
			"can't find java.awt.Component class");
		return 0;
	}

	return (*env)->NewObjectArray( env, nanoX.maxWindowN, Component, NULL);
}

jint /* synchronized */
Java_java_awt_Toolkit_evtRegisterSource( JNIEnv* env, jclass clazz, jobject wid )
{
	int i;

	i = getSourceIndex((GR_WINDOW_ID)wid);
	if ( i < 0 ) {
		SignalError("java.lang.InternalError", "can't register source");
		return -1;
	}
	nanoX.windowsP[i].flags |= TLK_WINDOW_REGISTER;

	return i;
}

jint
Java_java_awt_Toolkit_evtUnregisterSource ( JNIEnv* env, jclass clazz, jobject _jwindow)
{
	int index;

	index = getSourceIndex((GR_WINDOW_ID)_jwindow);

	if ( index >= 0 ) {
		nanoX.windowsP[index].wid = 0;
		nanoX.windowsP[index].owner = 0;
		nanoX.windowsP[index].flags = 0;
	}

	return index;
}

void
Java_java_awt_Toolkit_evtWakeup( JNIEnv* env, jclass clazz )
{
	GrFlush();
}

void
Java_java_awt_Toolkit_evtSendWMEvent(JNIEnv* env, jclass clazz, jobject wmEvt )
{
}
