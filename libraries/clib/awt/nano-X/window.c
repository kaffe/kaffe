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
 *      Kaffe.org contributors, see ChangeLog for details.  All rights reserved.
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

#define TLK_GrSetReizable(wid, isResizable, x, y, width, height) 

void
Java_java_awt_Toolkit_wndSetFrameInsets ( JNIEnv* env, jclass clazz, jint top, jint left, jint bottom, jint right )
{
	nanoX.frameInsets.top  = top;
	nanoX.frameInsets.left = left;
	nanoX.frameInsets.bottom = bottom;
	nanoX.frameInsets.right = right;
	nanoX.frameInsets.guess = JNI_TRUE;
}

jobject /* synchronized */
Java_java_awt_Toolkit_wndCreateFrame(
		JNIEnv* env, jclass clazz, jstring jTitle, 
		jint x, jint y, jint width, jint height, 
		jint jCursor, jint nativeColorBack, jboolean isResizable )
{
	GR_WINDOW_ID wid;
	const char *title;

	if ( width <= 0 )  width = 1;
	if ( height <= 0 ) height = 1;

	wid = GrNewWindow(GR_ROOT_WINDOW_ID, x, y,
			width, height, 0, 
			nativeColorBack, GREEN);

	if ( wid != NULL ) {
		if ( jTitle != NULL ) {
			title = (*env)->GetStringUTFChars(env , jTitle, NULL);
			if ( title != NULL ) {
				GrSetWindowTitle(wid, (char *)title);
			}
		}
		registerSource( wid, GR_ROOT_WINDOW_ID, TLK_SOURCE_FRAME);
		GrSelectEvents(wid, TLK_GR_EVENT_MASK_ALL );
#ifdef DEBUG
		GrMapWindow(wid);
#endif
	}

	TLK_GrSetReizable(wid, isResizable, x, y, width, height);
	//printf("create 0x%x %d %d %d %d\n",wid,x,y,width,height);
	return (jobject)wid;
}

void
Java_java_awt_Toolkit_wndSetResizable( JNIEnv* env, jclass clazz, jobject _jwindow, jboolean isResizable, int x, int y, int width, int height )
{
	// NIY
	TLK_GrSetReizable(wid, isResizable, x, y, width, height);
}

void
Java_java_awt_Toolkit_wndSetBounds( JNIEnv* envP, jclass clazz, jobject _jwindow, jint x, jint y, jint width, jint height, jboolean isResizable )
{
	GR_WINDOW_ID wid;
	int index;

	wid = (GR_WINDOW_ID)_jwindow;
	index = getSourceIndex(wid);
	if ( index < 0 ) {
		SignalError("java.lang.InternalError", "illegal window id");
		return;
	}
	//printf("setB 0x%x %d %d %d %d\n",_jwindow,x,y,width,height);

	if ( width < 0 )  width = 1;
	if ( height < 0 ) height = 1;

	GrMoveWindow(wid, x, y);
	GrResizeWindow(wid, width, height);
	TLK_GrSetReizable(wid, isResizable, x, y, width, height);
	GrRaiseWindow(wid);
	GrFlush();
}

void
Java_java_awt_Toolkit_wndSetVisible( JNIEnv* env, jclass clazz, jobject _jwindow, jboolean showIt )
{
	int index;
	GR_WINDOW_ID wid;

	index = getSourceIndex((GR_WINDOW_ID)_jwindow);
	wid = (GR_WINDOW_ID)_jwindow;
	if (( index < 0 ) || (nanoX.windowsP[index].flags & TLK_SOURCE_DESTROYED)) {
#ifdef DEBUG
		SignalError("java.lang.InternalError", "now, debug!!");
#endif
		return;
	}

	if ( showIt ){
		nanoX.windowsP[index].flags |= TLK_SOURCE_MAPPED;
		GrMapWindow(wid);
		GrRaiseWindow(wid);
		GrFlush();
	} else {
		nanoX.windowsP[index].flags &= ~TLK_SOURCE_MAPPED;
		GrUnmapWindow(wid);
		GrFlush();
	}
}

void
Java_java_awt_Toolkit_wndSetTitle( JNIEnv* envP, jclass clazz, jobject _jwindow, jstring _jstr )
{
	int index;
	GR_WINDOW_ID wid;
	const char *buf;

	if ( _jstr == NULL ) {
		SignalError("java.lang.NullPointerException", "no string object");
		return;
	}
	index = getSourceIndex((GR_WINDOW_ID)_jwindow);
	if ( index < 0 ) {
		SignalError("java.lang.InternalError", "illegal window id");
	}
	wid = (GR_WINDOW_ID)_jwindow;

	buf = (*envP)->GetStringUTFChars(envP , _jstr, 0);

	if ( buf ) {
		GrSetWindowTitle(wid, buf);
	}
}

jobject
Java_java_awt_Toolkit_wndCreateWindow(JNIEnv* envP, jclass clazz, jobject _jowner_wid, jint x, jint y, jint width, jint height, jint jCursor, jint nativeBgColor )
{
	GR_WINDOW_ID wid, owner;
	if ( getSourceIndex((GR_WINDOW_ID)_jowner_wid) < 0 ) {
		SignalError("java.lang.InternalError", "illegal owner window id");
		return 0;
	}
	owner = (GR_WINDOW_ID)_jowner_wid;

	wid = GrNewWindow(owner, x, y,
			width, height, 0, 
			nativeBgColor, GREEN);

	if ( wid != NULL ) {
		registerSource( wid, owner, TLK_SOURCE_WINDOW);
		GrSelectEvents(wid, TLK_GR_EVENT_MASK_ALL);
	}
	TLK_GrSetReizable(wid, JNI_TRUE, x, y, width, height) 

	return (jobject)wid;
}

jobject
Java_java_awt_Toolkit_wndCreateDialog(JNIEnv* envP, jclass clazz, jobject _jowner_wid, jint x, jint y, jint width, jint height, jint jCursor, jint nativeBgColor, jboolean isReisizable )
{
	GR_WINDOW_ID wid, owner;
	if ( getSourceIndex((GR_WINDOW_ID)_jowner_wid) < 0 ) {
		SignalError("java.lang.InternalError", "illegal owner window id");
		return 0;
	}
	owner = (GR_WINDOW_ID)_jowner_wid;

	wid = GrNewWindow(owner, x, y,
			width, height, 0, 
			nativeBgColor, GREEN);

	if ( wid != NULL ) {
		registerSource( wid, owner, TLK_SOURCE_WINDOW);
		GrSelectEvents(wid, TLK_GR_EVENT_MASK_ALL);
	}
	TLK_GrSetReizable(wid, isResizable, x, y, width, height) 

	return (jobject)wid;
}

void
Java_java_awt_Toolkit_wndDestroyWindow( JNIEnv* envP, jclass clazz, jobject _jwindow )
{
	GR_WINDOW_ID wid;
	int index;

	wid = (GR_WINDOW_ID)_jwindow;
	index = getSourceIndex(wid);
	if ( index < 0 ) {
		SignalError("java.lang.InternalError", "illegal window id");
		return;
	}

	if (!(nanoX.windowsP[index].flags & TLK_SOURCE_DESTROYED)) {

		nanoX.windowsP[index].flags |= TLK_SOURCE_DESTROYED;
		nanoX.windowsP[index].flags &= ~TLK_SOURCE_MAPPED;

		GrDestroyWindow(wid);
		GrFlush();
	}
}

void
Java_java_awt_Toolkit_wndRequestFocus(JNIEnv* env, jclass clazz, jobject _jwindow)
{
	GR_WINDOW_ID wid;
	int index;

	wid = (GR_WINDOW_ID)_jwindow;
	index = getSourceIndex(wid);
	if ( index < 0 ) {
		SignalError("java.lang.InternalError", "illegal window id");
		return;
	}

	GrSetFocus(wid);
}

void
Java_java_awt_Toolkit_wndSetDialogInsets ( JNIEnv* env, jclass clazz, jint top, jint left, jint bottom, jint right )
{
	nanoX.dialogInsets.top  = top;
	nanoX.dialogInsets.left = left;
	nanoX.dialogInsets.bottom = bottom;
	nanoX.dialogInsets.right = right;
	nanoX.dialogInsets.guess = JNI_TRUE;
}

void
Java_java_awt_Toolkit_wndRepaint(JNIEnv* envP, jclass clazz, jobject _jwindow, jint x, jint y, jint width, jint height )
{
	GR_WINDOW_ID wid;
	int index;

	wid = (GR_WINDOW_ID)_jwindow;
	index = getSourceIndex(wid);
	if ( index < 0 ) {
		SignalError("java.lang.InternalError", "illegal window id");
	}

	GrClearWindow(wid, JNI_TRUE);
}

void
Java_java_awt_Toolkit_wndSetIcon(JNIEnv* env, jclass clazz, jobject _jwindow, jobject _jimages) 
{
	// NIY
}

void
Java_java_awt_Toolkit_wndToBack(JNIEnv* env, jclass clazz, jobject _jwindow)
{
	GR_WINDOW_ID wid;
	int index;

	wid = (GR_WINDOW_ID)_jwindow;
	index = getSourceIndex(wid);
	if ( index < 0 ) {
		SignalError("java.lang.InternalError", "illegal window id");
		return;
	}
	GrLowerWindow(wid);
}


void
Java_java_awt_Toolkit_wndToFront(JNIEnv* env, jclass clazz, jobject _jwindow)
{
	GR_WINDOW_ID wid;
	int index;

	wid = (GR_WINDOW_ID)_jwindow;
	index = getSourceIndex(wid);
	if ( index < 0 ) {
		SignalError("java.lang.InternalError", "illegal window id");
		return;
	}
	GrRaiseWindow(wid);
}


void
Java_java_awt_Toolkit_wndSetCursor ( JNIEnv* env, jclass clazz, jobject _jwindow, jint jCursor )
{
	// NIY
}
