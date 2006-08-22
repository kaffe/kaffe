
/*
 * Copyright (c) 2006
 *	Alper Akcan <distchx@yahoo.com>, All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "toolkit.h"

jint Java_java_awt_Toolkit_tlkProperties (JNIEnv *env UNUSED, jclass clazz UNUSED)
{
	jint props;
	DEBUGF("Enter");
	props = TLK_EXTERNAL_DECO;
	DEBUGF("Leave");
	return props;
}

jboolean Java_java_awt_Toolkit_tlkInit (JNIEnv *env UNUSED, jclass clazz UNUSED, jstring name UNUSED)
{
	DEBUGF("Enter");
	xynth = AWT_MALLOC(sizeof(xynth_toolkit_t));
	xynth->eventq = (xynth_eventq_t *) AWT_MALLOC(sizeof(xynth_eventq_t));
	s_list_init(&(xynth->eventq->list));
	s_thread_mutex_init(&(xynth->eventq->mut));
	s_client_init(&(xynth->root));
	s_window_new(xynth->root, WINDOW_MAIN, NULL);
	s_thread_create(s_client_main, xynth->root);
	xynth->nwindows = XYNTH_NWINDOWS;
	xynth->windows = AWT_CALLOC(xynth->nwindows, sizeof(window_rec_t));
	DEBUGF("Leave");
	return JNI_TRUE;
}

jint Java_java_awt_Toolkit_tlkGetScreenWidth (JNIEnv *env UNUSED, jclass clazz UNUSED)
{
	DEBUGF("Enter");
	DEBUGF("Leave");
	return xynth->root->surface->linear_buf_width;
}

jint Java_java_awt_Toolkit_tlkGetScreenHeight (JNIEnv *env UNUSED, jclass clazz UNUSED)
{
	DEBUGF("Enter");
	DEBUGF("Leave");
	return xynth->root->surface->linear_buf_height;
}

jint Java_java_awt_Toolkit_tlkGetResolution (JNIEnv *env UNUSED, jclass clazz UNUSED)
{
	DEBUGF("Enter");
	DEBUGF("Leave");
	return 0;
}

jstring Java_java_awt_Toolkit_tlkVersion (JNIEnv *env, jclass clazz UNUSED)
{
	DEBUGF("Enter");
	DEBUGF("Leave");
	return (*env)->NewStringUTF(env, "Xynth Windowing System");
}

void Java_java_awt_Toolkit_tlkDisplayBanner (JNIEnv* env UNUSED, jclass clazz UNUSED, jstring banner UNUSED)
{
	DEBUGF("Enter");
	DEBUGF("Leave");
}

void Java_java_awt_Toolkit_tlkBeep (JNIEnv *env UNUSED, jclass clazz UNUSED)
{
	DEBUGF("Enter");
	DEBUGF("Leave");
}
