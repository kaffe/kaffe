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

#include "jni.h"

Toolkit nanoX;

jint 
Java_java_awt_Toolkit_tlkProperties ( JNIEnv* env, jclass clazz )
{
	return TLK_EXTERNAL_DECO | TLK_IS_BLOCKING | TLK_NEEDS_FLUSH;
}

static void 
KaffeNanoXErrorHandler(GR_EVENT *eventP)
{
  return;
}

jboolean
Java_java_awt_Toolkit_tlkInit( JNIEnv* env, jclass clazz, jstring name )
{

	GrSetErrorHandler(KaffeNanoXErrorHandler);

	if (GrOpen() < 0) {
#ifdef DEBUG
		fprintf(stderr, "nxtest: cannot open graphics\n");
#endif
		return JNI_FALSE;
	}

/*
	if (KbdOpen() < 0) {
		fprintf(stderr, "nxtest: cannot open kbd named pipe\n");
		//return JNI_FALSE;
	}
*/
	nanoX.maxWindowN = MAX_WINDOW_N;
	nanoX.windowN = 0;
	nanoX.windowsP = TLK_CALLOC( nanoX.maxWindowN, sizeof(TLKWindow));

	return JNI_TRUE;
}

void
Java_java_awt_Toolkit_tlkTerminate( JNIEnv* env, jclass clazz )
{
}

jint
Java_java_awt_Toolkit_tlkGetScreenWidth( JNIEnv* env, jclass clazz )
{
	GR_SCREEN_INFO info; 

	GrGetScreenInfo(&info);
	return info.ws_width;
}

jint
Java_java_awt_Toolkit_tlkGetScreenHeight( JNIEnv* env, jclass clazz )
{
	GR_SCREEN_INFO info; 

	GrGetScreenInfo(&info);
	return info.ws_height;
}

jint
Java_java_awt_Toolkit_tlkGetResolution( JNIEnv* env, jclass clazz )
{
	GR_SCREEN_INFO info; 

	GrGetScreenInfo(&info);
	return (jint)(info.xdpcm * 254 / 100);
}

jstring
Java_java_awt_Toolkit_tlkVersion ( JNIEnv* env, jclass clazz )
{
	return (*env)->NewStringUTF( env, "nano-X AWT Backend 0.1");
}

void
Java_java_awt_Toolkit_tlkDisplayBanner ( JNIEnv* env, jclass clazz, jstring banner )
{
}

void
Java_java_awt_Toolkit_tlkBeep( JNIEnv* env, jclass clazz )
{
	GrBell();
}

void
Java_java_awt_Toolkit_tlkSync ( JNIEnv* env, jclass clazz )
{
	GrFlush();
}

void
Java_java_awt_Toolkit_tlkFlush ( JNIEnv* env, jclass clazz )
{
	GrFlush();
}
