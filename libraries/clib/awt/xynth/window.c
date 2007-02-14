
/*
 * Copyright (c) 2006 - 2007
 *	Alper Akcan <alper@kaffe.org>, All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "toolkit.h"

void xynth_kaffe_atevent (s_window_t *window, s_event_t *event);

void Java_java_awt_Toolkit_wndSetFrameInsets (JNIEnv *env UNUSED, jclass clazz UNUSED, jint top, jint left, jint bottom, jint right)
{
	DEBUGF("Enter");
	xynth->frame_insets.top  = top;
	xynth->frame_insets.left = left;
	xynth->frame_insets.bottom = bottom;
	xynth->frame_insets.right = right;
	xynth->frame_insets.guess = 1;
	DEBUGF("Leave");
}

void Java_java_awt_Toolkit_wndSetDialogInsets (JNIEnv *env UNUSED, jclass clazz UNUSED, jint top, jint left, jint bottom, jint right)
{
	DEBUGF("Enter");
	xynth->dialog_insets.top  = top;
	xynth->dialog_insets.left = left;
	xynth->dialog_insets.bottom = bottom;
	xynth->dialog_insets.right = right;
	xynth->dialog_insets.guess = 1;
	DEBUGF("Leave");
}

jobject Java_java_awt_Toolkit_wndCreateFrame (JNIEnv *env, jclass clazz UNUSED, jstring jTitle, jint x, jint y, jint width, jint height, jint jCursor, jint clrBack, jboolean isResizable)
{
	char *str;
	s_window_t *window;
	jobject jwindow;
	DEBUGF("Enter");
	str = java2CString(env, jTitle);
	s_window_init(&window);
	s_window_new(window, WINDOW_CHILD, xynth->root);
	s_window_set_coor(window, WINDOW_NOFORM, x, y, width, height);
	s_window_set_title(window, str);
	s_fillbox(window->surface, 0, 0, window->surface->width, window->surface->height, clrBack);
	s_window_atevent(window, xynth_kaffe_atevent);
	s_window_main(window);
	jwindow = JCL_NewRawDataObject(env, window);
	source_idx_register(xynth, UNVEIL_WINDOW(jwindow), xynth->root);
	DEBUGF("title: %s, x: %d, y: %d, w: %d, h: %d, cursor: %d, clrback: %d, resize: %d", str, x, y, width, height, clrBack, isResizable);
	AWT_FREE(str);
	DEBUGF("Leave");
	return jwindow;
}

jobject Java_java_awt_Toolkit_wndCreateWindow (JNIEnv *env, jclass clazz UNUSED, jobject nowner, jint x, jint y, jint width, jint height, jint jCursor, jint clrBack)
{
	jobject jwindow;
	s_window_t *owner;
	s_window_t *window;
	DEBUGF("Enter");
	owner = UNVEIL_WINDOW(nowner);
	s_window_init(&window);
	s_window_new(window, WINDOW_CHILD | WINDOW_NOFORM, owner);
	s_window_set_coor(window, WINDOW_NOFORM, x, y, width, height);
	s_fillbox(window->surface, 0, 0, window->surface->width, window->surface->height, clrBack);
	s_window_atevent(window, xynth_kaffe_atevent);
	s_window_main(window);
	jwindow = JCL_NewRawDataObject(env, window);
	source_idx_register(xynth, UNVEIL_WINDOW(jwindow), owner);
	DEBUGF("Leave");
	return jwindow;
}

jobject Java_java_awt_Toolkit_wndCreateDialog (JNIEnv *env, jclass clazz UNUSED, jobject nowner, jstring jTitle, jint x, jint y, jint width, jint height, jint jCursor, jint clrBack, jboolean isResizable)
{
	char *str;
	jobject jwindow;
	s_window_t *owner;
	s_window_t *window;
	DEBUGF("Enter");
	str = java2CString(env, jTitle);
	owner = UNVEIL_WINDOW(nowner);
	s_window_init(&window);
	s_window_new(window, WINDOW_CHILD, owner);
	s_window_set_coor(window, WINDOW_NOFORM, x, y, width, height);
	s_window_set_title(window, str);
	s_fillbox(window->surface, 0, 0, window->surface->width, window->surface->height, clrBack);
	s_window_atevent(window, xynth_kaffe_atevent);
	s_window_main(window);
	jwindow = JCL_NewRawDataObject(env, window);
	source_idx_register(xynth, UNVEIL_WINDOW(jwindow), owner);
	AWT_FREE(str);
	DEBUGF("Leave");
	return jwindow;
}

void Java_java_awt_Toolkit_wndSetVisible (JNIEnv *env UNUSED, jclass clazz UNUSED, jobject nwnd, jboolean showIt)
{
	int i;
	s_window_t *window;
	DEBUGF("Enter");
	window = UNVEIL_WINDOW(nwnd);
	i = source_idx_get(xynth, window);
	if (i < 0) {
		DEBUGF("Could not find idx for window");
		return;
	}
	DEBUGF("window->id: %d", window->id);
	if (showIt) {
		s_window_show(window);
	} else {
		s_window_hide(window);
	}
	DEBUGF("Leave");
}

void Java_java_awt_Toolkit_wndRequestFocus (JNIEnv *env UNUSED, jclass clazz UNUSED, jobject nwnd)
{
	int i;
	s_window_t *window;
	DEBUGF("Enter");
	window = UNVEIL_WINDOW(nwnd);
	i = source_idx_get(xynth, window);
	if (i < 0) {
		DEBUGF("Could not find idx for window");
		return;
	}
	s_window_show(window);
	DEBUGF("Leave");
}

void Java_java_awt_Toolkit_wndSetCursor (JNIEnv *env UNUSED, jclass clazz UNUSED, jobject nwnd, jint jCursor)
{
	s_window_t *window;
	DEBUGF("Enter");
	window = UNVEIL_WINDOW(nwnd);
	DEBUGF("Leave");
}

void Java_java_awt_Toolkit_wndDestroyWindow (JNIEnv *env UNUSED, jclass clazz UNUSED, jobject nwnd)
{
	int i;
	s_window_t *window;
	DEBUGF("Enter");
	window = UNVEIL_WINDOW(nwnd);
	i = source_idx_get(xynth, window);
	if (i < 0) {
		DEBUGF("Could not find idx for window");
		return;
	}
	s_window_quit(window);
	DEBUGF("Leave");
}
