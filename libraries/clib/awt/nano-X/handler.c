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
#include "keymap.h"

#include "jni.h"

jobject handlerIllegal(JNIEnv *,GR_EVENT*);
jobject handlerExposure(JNIEnv *,GR_EVENT*);
jobject handlerButtonDown(JNIEnv *,GR_EVENT*);
jobject handlerButtonUp(JNIEnv *,GR_EVENT*);
jobject handlerMouseEnter(JNIEnv *,GR_EVENT*);
jobject handlerMouseExit(JNIEnv *,GR_EVENT*);
jobject handlerMouseMotion(JNIEnv *,GR_EVENT*);
jobject handlerMousePosition(JNIEnv *,GR_EVENT*);
jobject handlerKeyDown(JNIEnv *,GR_EVENT*);
jobject handlerKeyUp(JNIEnv *,GR_EVENT*);
jobject handlerFocusIn(JNIEnv *,GR_EVENT*);
jobject handlerFocusOut(JNIEnv *,GR_EVENT*);
jobject handlerUpdate(JNIEnv *,GR_EVENT*);
jobject handlerCloseReq(JNIEnv *,GR_EVENT*);
jobject handlerClientDataReq(JNIEnv *,GR_EVENT*);
void sendClipboardData(JNIEnv *, GR_EVENT_CLIENT_DATA_REQ *);

#define TLK_GR_EVENT_MAX_EVENT_N (GR_EVENT_TYPE_SELECTION_CHANGED+1)
typedef jobject (*EventFunc)(JNIEnv*,GR_EVENT*);

EventFunc  processEvent[TLK_GR_EVENT_MAX_EVENT_N] = {
	handlerIllegal,  // NONE
	handlerExposure,
	handlerButtonDown,
	handlerButtonUp,
	handlerMouseEnter,
	handlerMouseExit,
	handlerMouseMotion,
	handlerMousePosition,
	handlerKeyDown,
	handlerKeyUp,
	handlerFocusIn,
	handlerFocusOut,
	handlerIllegal, // FDINPUT
	handlerUpdate,
	handlerIllegal, // GR_EVENT_TYPE_CHLD_UPDATE	14	/* never sent*/
	handlerCloseReq,
	handlerIllegal, // Timeout
	handlerIllegal, // ScreenSaver
	handlerClientDataReq,
	handlerIllegal, // ClientData,
	handlerIllegal, // SelectionChanged
};

static jclass     AWTEvent;
static jmethodID  getID;

static jclass     PaintEvent;
static jmethodID  getPaintEvent;

static jclass     FocusEvent;
static jmethodID  getFocusEvent;

static jclass     MouseEvent;
static jmethodID  getMouseEvent;

static jclass     KeyEvent;
static jmethodID  getKeyEvent;

static jclass     WindowEvent;
static jmethodID  getWindowEvent;

jobject
handlerIllegal(JNIEnv *envP, GR_EVENT *eventP)
{
	SignalError("java.lang.InternalError", "illegal event");
}

jobject
handlerExposure(JNIEnv *envP, GR_EVENT *eventP)
{
	assert(eventP->exposure.type == GR_EVENT_TYPE_EXPOSURE);

	/* create object by VM */
	return (*envP)->CallStaticObjectMethod( envP, PaintEvent, getPaintEvent,
		getCachedSourceIndex(eventP->exposure.wid),
		TLK_AWT_EVENT_UPDATE,
		eventP->exposure.x,
		eventP->exposure.y,
		eventP->exposure.width,
		eventP->exposure.height);
}

jobject
handlerButtonDown(JNIEnv *envP, GR_EVENT *eventP)
{
	assert(eventP->mouse.type == GR_EVENT_TYPE_BUTTON_DOWN);

	/* create object by VM */
	return (*envP)->CallStaticObjectMethod( envP, MouseEvent, getMouseEvent,
		getCachedSourceIndex(eventP->mouse.wid),
		TLK_AWT_EVENT_MOUSE_PRESSED,
		0, // when
		eventP->mouse.x,
		eventP->mouse.y);
}

jobject
handlerButtonUp(JNIEnv *envP, GR_EVENT *eventP)
{
	assert(eventP->mouse.type == GR_EVENT_TYPE_BUTTON_UP);

	/* create object by VM */
	return (*envP)->CallStaticObjectMethod( envP, MouseEvent, getMouseEvent,
		getCachedSourceIndex(eventP->mouse.wid),
		TLK_AWT_EVENT_MOUSE_RELEASED,
		0, // when
		eventP->mouse.x,
		eventP->mouse.y);
}

jobject
handlerMouseEnter(JNIEnv *envP, GR_EVENT *eventP)
{
	assert(eventP->mouse.type == GR_EVENT_TYPE_MOUSE_ENTER);

	/* create object by VM */
	return (*envP)->CallStaticObjectMethod( envP, MouseEvent, getMouseEvent,
		getCachedSourceIndex(eventP->mouse.wid),
		TLK_AWT_EVENT_MOUSE_ENTERED,
		0, // when
		eventP->mouse.x,
		eventP->mouse.y);
}

jobject
handlerMouseExit(JNIEnv *envP, GR_EVENT *eventP)
{
	assert(eventP->mouse.type == GR_EVENT_TYPE_MOUSE_EXIT);

	/* create object by VM */
	return (*envP)->CallStaticObjectMethod( envP, MouseEvent, getMouseEvent,
		getCachedSourceIndex(eventP->mouse.wid),
		TLK_AWT_EVENT_MOUSE_EXITED,
		0, // when
		eventP->mouse.x,
		eventP->mouse.y);
}

jobject
handlerMouseMotion(JNIEnv *envP, GR_EVENT *eventP)
{
	assert(eventP->mouse.type == GR_EVENT_TYPE_MOUSE_MOTION);

	/* create object by VM */
	return (*envP)->CallStaticObjectMethod( envP, MouseEvent, getMouseEvent,
		getCachedSourceIndex(eventP->mouse.wid),
		TLK_AWT_EVENT_MOUSE_MOVED,
		0, // when
		eventP->mouse.x,
		eventP->mouse.y);
}

jobject
handlerMousePosition(JNIEnv *envP, GR_EVENT *eventP)
{
	GR_EVENT event;

	assert(eventP->mouse.type == GR_EVENT_TYPE_MOUSE_POSITION);

retry:
	GrPeekEvent(&event);
	if ( event.mouse.type == GR_EVENT_TYPE_MOUSE_POSITION ) {
		GrCheckNextEvent(eventP);
		goto retry;
	}
	/* create object by VM */
	return (*envP)->CallStaticObjectMethod( envP, MouseEvent, getMouseEvent,
		getCachedSourceIndex(eventP->mouse.wid),
		TLK_AWT_EVENT_MOUSE_MOVED,
		0, // when
		eventP->mouse.x,
		eventP->mouse.y);
}

static __inline__ int 
keyMod( GR_KEYMOD modifiers)
{
	int jmod = 0;
	if ( modifiers & MWKMOD_SHIFT) {
		jmod |= TLK_AWT_KM_SHIFT_MASK;
	}
	if ( modifiers & MWKMOD_CTRL) {
		jmod |= TLK_AWT_KM_CTRL_MASK;
	}
	if ( modifiers & MWKMOD_META) {
		jmod |= TLK_AWT_KM_META_MASK;
	}
	if ( modifiers & MWKMOD_ALT) {
		jmod |= TLK_AWT_KM_ALT_MASK;
	}

	return jmod;
}

jobject
handlerKeyDown(JNIEnv *envP, GR_EVENT *eventP)
{
	unsigned int ch, code;

	assert(eventP->keystroke.type == GR_EVENT_TYPE_KEY_DOWN);

	if (  eventP->keystroke.ch >= MWKEY_FIRST ) {
		code = keymap[eventP->keystroke.ch & 0xFF];
		ch = 0;
	} else {
		code = eventP->keystroke.ch; 
		ch = eventP->keystroke.ch; 
	}
	return (*envP)->CallStaticObjectMethod(envP, KeyEvent, getKeyEvent,
		getCachedSourceIndex(eventP->keystroke.wid),
		TLK_AWT_EVENT_KEY_PRESSED,
		code,
		ch,
		keyMod(eventP->keystroke.modifiers));
}

jobject
handlerKeyUp(JNIEnv *envP, GR_EVENT *eventP)
{
	unsigned int ch, code;

	assert(eventP->keystroke.type == GR_EVENT_TYPE_KEY_UP);

	if (  eventP->keystroke.ch >= MWKEY_FIRST ) {
		code = keymap[eventP->keystroke.ch & 0xFF] >> 8;
		ch = keymap[eventP->keystroke.ch & 0xFF] & 0xFF;
	} else {
		code = eventP->keystroke.ch; 
		ch = eventP->keystroke.ch; 
	}
	return (*envP)->CallStaticObjectMethod(envP, KeyEvent, getKeyEvent,
		getCachedSourceIndex(eventP->keystroke.wid),
		TLK_AWT_EVENT_KEY_RELEASED,
		code,
		ch,
		keyMod(eventP->keystroke.modifiers));
}

jobject
handlerFocusIn(JNIEnv *envP, GR_EVENT *eventP)
{
	assert(eventP->general.type == GR_EVENT_TYPE_FOCUS_IN);
	return (*envP)->CallStaticObjectMethod( envP, FocusEvent, getFocusEvent, 
		getCachedSourceIndex(eventP->general.wid),
		TLK_AWT_EVENT_FOCUS_LOST,
		JNI_TRUE);
}

jobject
handlerFocusOut(JNIEnv *envP, GR_EVENT *eventP)
{
	assert(eventP->general.type == GR_EVENT_TYPE_FOCUS_OUT);
	return (*envP)->CallStaticObjectMethod( envP, FocusEvent, getFocusEvent, 
		getCachedSourceIndex(eventP->general.wid),
		TLK_AWT_EVENT_FOCUS_LOST,
		JNI_TRUE);
}

jobject
handlerUpdate(JNIEnv *envP, GR_EVENT *eventP)
{
	assert(eventP->update.type == GR_EVENT_TYPE_UPDATE);

	if ( eventP->update.utype == 6 ) {
		return NULL;
	}

	/* create object by VM */
	return (*envP)->CallStaticObjectMethod( envP, PaintEvent, getPaintEvent,
		getCachedSourceIndex(eventP->update.wid),
		TLK_AWT_EVENT_UPDATE,
		eventP->update.x,
		eventP->update.y,
		eventP->update.width,
		eventP->update.height);
}

jobject
handlerCloseReq(JNIEnv *envP, GR_EVENT *eventP)
{
	assert(eventP->general.type == GR_EVENT_TYPE_CLOSE_REQ);

	return (*envP)->CallStaticObjectMethod( envP, WindowEvent, getWindowEvent,
		getCachedSourceIndex(eventP->general.wid),
		TLK_AWT_EVENT_WINDOW_CLOSING);
}

jobject
handlerClientDataReq(JNIEnv *envP, GR_EVENT *eventP)
{
	assert(eventP->clientdatareq.type == GR_EVENT_TYPE_CLIENT_DATA_REQ);

	sendClipboardData(envP, (GR_EVENT_CLIENT_DATA_REQ *)eventP);

	return NULL;
}

jobject
Java_java_awt_Toolkit_evtGetNextEvent( JNIEnv* envP, jclass clazz )
{
	GR_EVENT event;

	GrCheckNextEvent(&event);
	if ( event.type == GR_EVENT_TYPE_NONE ) {
		return NULL;
	}

	assert( event.type <= GR_EVENT_TYPE_SELECTION_CHANGED);
	//printf("event.type %d\n",event.type);

	return processEvent[event.type](envP, &event);
}

jobject
Java_java_awt_Toolkit_evtPeekEvent( JNIEnv* envP, jclass clazz )
{
	GR_EVENT event;

	GrPeekEvent(&event);
	if ( event.type == GR_EVENT_TYPE_NONE ) {
		return NULL;
	}

	assert( event.type <= GR_EVENT_TYPE_SELECTION_CHANGED );

	return processEvent[event.type](envP, &event);
}

jobject
Java_java_awt_Toolkit_evtPeekEventId ( JNIEnv* envP, jclass clazz, jint id )
{
	jobject jevent;

	jint jevent_id;

	jevent = Java_java_awt_Toolkit_evtPeekEvent(envP, clazz);
	if ( jevent == NULL ) {
		return NULL;
	}
	jevent_id = (*envP)->CallStaticIntMethod(envP, AWTEvent, getID, jevent);

	return (jevent_id == id)?(jevent):NULL;
}

void 
handlerInit( JNIEnv* envP, jclass clazz )
{
/*
	AWTEvent = (*envP)->FindClass(envP, "java/awt/AWTEvent");
	getID = (*envP)->GetStaticMethodID(envP, PaintEvent, "getID", "(Ljava/awt/AWTEvent;)I");
*/

	PaintEvent = (*envP)->FindClass( envP, "java/awt/PaintEvt");
	getPaintEvent = (*envP)->GetStaticMethodID( envP, PaintEvent, "getEvent", "(IIIIII)Ljava/awt/PaintEvt;");

	FocusEvent = (*envP)->FindClass( envP, "java/awt/FocusEvt");
	getFocusEvent = (*envP)->GetStaticMethodID( envP, FocusEvent, "getEvent", "(IIZ)Ljava/awt/FocusEvt;");

	MouseEvent = (*envP)->FindClass( envP, "java/awt/MouseEvt");
	getMouseEvent = (*envP)->GetStaticMethodID( envP, MouseEvent, "getEvent", "(IIIII)Ljava/awt/MouseEvt;");

	KeyEvent = (*envP)->FindClass( envP, "java/awt/KeyEvt");
	getKeyEvent = (*envP)->GetStaticMethodID( envP, KeyEvent, "getEvent", "(IIIII)Ljava/awt/KeyEvt;");
	
	WindowEvent = (*envP)->FindClass( envP, "java/awt/WindowEvt");
	getWindowEvent = (*envP)->GetStaticMethodID( envP, WindowEvent, "getEvent", "(II)Ljava/awt/WindowEvt;");
}
