/**
 * evt.c - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */


#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "toolkit.h"
#include "keysyms.h"

/*******************************************************************************
 *
 */

__inline__ int getSourceIdx ( Toolkit* X, Window w )
{
  int      n;
  register int i;

  if ( w == X->lastWindow ){
	return X->srcIdx;
  }
  else {
	for ( i = (unsigned long) w, n=0; n < X->nWindows; i++, n++ ) {
	  i %= X->nWindows;
	  if ( X->windows[i] == w ){
		X->srcIdx = i;
		X->lastWindow = w;
		return X->srcIdx;
	  }
	  
	  if ( X->windows[i] == 0 )
		return -1;
	}
	return -1;
  }
}

__inline__ int nextEvent ( Toolkit *X )
{
  if ( X->preFetched )
	return 1;

  if ( X->pending <= 0 )
	X->pending = XEventsQueued( X->dsp, QueuedAfterFlush);

  if ( !X->pending ) {
	return 0;
  }
  else {
	XNextEvent( X->dsp, &X->event);
	X->pending--;
	return 1;
  }
}


/* X-to-Java key modifier mapping
 *                                     altGr   : PC
 *     shift       -    ctrl     alt    meta   : Java
 *     Shift    Lock    Ctrl    Mod1    Mod3   : X      symbol
 *  ----------------------------------------------------------
 *         1       1       2       8       4   : Java   value
 *         1       2       4       8      32   : X
 */

__inline__ int keyMod ( int keyState )
{
  int mod = 0;
  if ( keyState & 3 ) mod |= 1; /* shift or lock */
  if ( keyState & 4 ) mod |= 2;
  if ( keyState & 8 ) mod |= 8;
  return mod;
}


jclass     ComponentEvent;
jclass     MouseEvent;
jclass     FocusEvent;
jclass     WindowEvent;
jclass     KeyEvent;
jclass     PaintEvent;

jmethodID  getComponentEvent;
jmethodID  getMouseEvent;
jmethodID  getFocusEvent;
jmethodID  getWindowEvent;
jmethodID  getKeyEvent;
jmethodID  getPaintEvent;

#define    COMPONENT_RESIZED    101

#define    WINDOW_CLOSING       201
#define    WINDOW_CLOSED        202
#define    WINDOW_ICONIFIED     203
#define    WINDOW_DEICONIFIED   204

#define    KEY_PRESSED          401
#define    KEY_RELEASED         402

#define    MOUSE_PRESSED        501
#define    MOUSE_RELEASED       502
#define    MOUSE_MOVED          503
#define    MOUSE_ENTERED        504
#define    MOUSE_EXITED         505

#define    PAINT                800

#define    FOCUS_GAINED        1004
#define    FOCUS_LOST          1005


jobject
skip ( JNIEnv* env, Toolkit* X )
{
  return NULL;
}


jobject
keyNotify ( JNIEnv* env, Toolkit* X )
{
  KeySym          keysym;
  XComposeStatus  ioStatus;
  int             n, keyCode, keyChar, mod;

  /*
   * This is just preliminary, it does not use input methods yet. Even keysym
   * mapping is not fully supported
   */
  n = XLookupString( &X->event.xkey, X->buf, X->nBuf, &keysym, &ioStatus);

  if ( (keysym >= 0xff00) || (n == 0) ) {
	keyCode = FKeyCode[keysym & 0xff];
	/*
	 * There are some "control keys" that should generate KEY_TYPED events
	 * (enter, cancel, backspace, del, tab). This is flagged by a negative keyCode
	 * value and leads to non-zero keyChars
	 */
	if ( keyCode < 0 ){
	  keyChar = keyCode = -keyCode;
	}
	else {  /* a "pure" function key */
	  keyChar = 0;
	}

  }
  else {
	keyChar = (unsigned char)X->buf[0];
	keyCode = keyChar & ~0x20;
  }

  X->evtId = (X->event.xany.type == KeyPress)? KEY_PRESSED : KEY_RELEASED;
  mod = keyMod( X->event.xkey.state);

  return (*env)->CallStaticObjectMethod( env, KeyEvent, getKeyEvent,
										 X->srcIdx, X->evtId, keyCode, keyChar, mod);
}


jobject
buttonNotify ( JNIEnv* env, Toolkit* X )
{
  X->evtId = ( X->event.xany.type == ButtonPress ) ?  MOUSE_PRESSED : MOUSE_RELEASED;

  return (*env)->CallStaticObjectMethod( env, MouseEvent, getMouseEvent,
										 X->srcIdx, X->evtId,
										 X->event.xbutton.button,
										 X->event.xbutton.x, X->event.xbutton.y);
}


jobject
motionNotify ( JNIEnv* env, Toolkit* X )
{
  return (*env)->CallStaticObjectMethod( env, MouseEvent, getMouseEvent,
										 X->srcIdx, (X->evtId = MOUSE_MOVED),
										 0, X->event.xmotion.x, X->event.xmotion.y);
}


jobject
mouseNotify ( JNIEnv* env, Toolkit* X )
{
  X->evtId = (X->event.xany.type == EnterNotify) ? MOUSE_ENTERED : MOUSE_EXITED;

  return (*env)->CallStaticObjectMethod( env, MouseEvent, getMouseEvent,
										 X->srcIdx, X->evtId,
										 0, X->event.xcrossing.x, X->event.xcrossing.y);
}


jobject
focusNotify ( JNIEnv* env, Toolkit* X )
{
  /*
   * get rid of all these fancy intermediate focus events (the real thing should
   * come last)
   */
  while ( XCheckMaskEvent( X->dsp, FocusChangeMask, &X->event) ){
	X->pending--;
	X->srcIdx = getSourceIdx( X, X->event.xfocus.window);
  }

  X->evtId = (X->event.xany.type == FocusIn) ? FOCUS_GAINED : FOCUS_LOST;

  return (*env)->CallStaticObjectMethod( env, FocusEvent, getFocusEvent, X->srcIdx, X->evtId);
}


jobject
expose ( JNIEnv* env, Toolkit* X )
{
  Window  wnd = X->event.xany.window;
  int     x   = X->event.xexpose.x;
  int     y   = X->event.xexpose.y;
  int     w   = X->event.xexpose.width;
  int     h   = X->event.xexpose.height;
  int     xw, yh, exw, eyh;

  while ( XCheckWindowEvent( X->dsp, wnd, ExposureMask | StructureNotifyMask, &X->event) ){
	X->pending--;

	if ( X->event.xany.type == Expose ) {
	  xw = x + w;
	  yh = y + h;

	  exw = X->event.xexpose.x + X->event.xexpose.width;
	  eyh = X->event.xexpose.y + X->event.xexpose.height;

	  if ( X->event.xexpose.x < x ) x = X->event.xexpose.x;
	  if ( X->event.xexpose.y < y ) y = X->event.xexpose.y;
	
	  w = (exw > xw) ? exw - x : xw - x;
	  h = (eyh > yh) ? eyh - y : yh - y;
	}
	else {
	  X->preFetched = 1;
	  break;
	}
  }

  return (*env)->CallStaticObjectMethod( env, PaintEvent, getPaintEvent,
										 X->srcIdx, (X->evtId = PAINT),
										 x, y, w, h);
}


jobject
destroyNotify ( JNIEnv* env, Toolkit* X )
{
  return (*env)->CallStaticObjectMethod( env, WindowEvent, getWindowEvent,
										 X->srcIdx, (X->evtId = WINDOW_CLOSED));
}


jobject
mapNotify ( JNIEnv* env, Toolkit* X )
{
  X->evtId = (X->event.xany.type == MapNotify) ? WINDOW_DEICONIFIED : WINDOW_ICONIFIED;

  if ( X->event.xany.window == X->newWindow )  /* 'BadMatch' focus set workaround */
	X->newWindow = 0;

  return (*env)->CallStaticObjectMethod( env, WindowEvent, getWindowEvent,
										 X->srcIdx, X->evtId);
}


jobject
configureNotify ( JNIEnv* env, Toolkit* X )
{
  Window  child;
  int     x, y, w, h;

  /*
   * some window managers are rather loquacious when doing opaque moves
   */

  while ( XCheckTypedWindowEvent( X->dsp, X->event.xany.window, ConfigureNotify, &X->event) ){
	X->pending--;
  }

  if ( (X->event.xconfigure.x == 0) && (X->event.xconfigure.y == 0) ) {
	XTranslateCoordinates( X->dsp, X->event.xconfigure.window,
						   DefaultRootWindow( X->dsp),
						   X->event.xconfigure.x, X->event.xconfigure.y,
						   &X->event.xconfigure.x, &X->event.xconfigure.y, &child);
  }

  x = X->event.xconfigure.x;
  y = X->event.xconfigure.y;
  w = X->event.xconfigure.width;
  h = X->event.xconfigure.height;
  X->evtId = COMPONENT_RESIZED;

  return (*env)->CallStaticObjectMethod( env, ComponentEvent, getComponentEvent,
										 X->srcIdx, X->evtId, x, y, w, h);
}


jobject
clientMessage ( JNIEnv* env, Toolkit* X )
{
  if ( X->event.xclient.message_type == WM_PROTOCOLS ) {
	if ( X->event.xclient.data.l[0] == WM_DELETE_WINDOW ) {
	  return (*env)->CallStaticObjectMethod( env, WindowEvent, getWindowEvent,
											 X->srcIdx, (X->evtId = WINDOW_CLOSING));
	}
	else if ( X->event.xclient.data.l[0] == WM_TAKE_FOCUS ) {
	  XSetInputFocus( X->dsp, X->event.xany.window, RevertToNone, CurrentTime);
	}
  }
  /*
   * this is a workaround for the common problem of requesting the focus for not
   * yet mapped windows (resulting in BadMatch errors)
   */
  else if ( X->event.xclient.message_type == RETRY_FOCUS ) {
	if ( (X->event.xclient.window == X->newWindow) && (X->event.xclient.data.l[0] > 0) ){
	  X->event.xclient.data.l[0]--;
	  XSendEvent( X->dsp, X->event.xany.window, False, StdEvents, &X->event);
	}
	else {
	  if ( !X->newWindow ){
		XSetInputFocus( X->dsp, (Window)X->event.xclient.window, RevertToNone, CurrentTime);
	  }
	}
  }
  return NULL;
}


typedef jobject (*EventFunc)(JNIEnv*,Toolkit*);

static EventFunc  processEvent[LASTEvent] = {
  skip, skip,           /* 0, 1 are reserved by X    */
  keyNotify,            /* KeyPress                 2 */
  keyNotify,            /* KeyRelease               3 */
  buttonNotify,         /* ButtonPress              4 */
  buttonNotify,         /* ButtonRelease            5 */
  motionNotify,         /* MotionNotify             6 */
  mouseNotify,          /* EnterNotify              7 */
  mouseNotify,          /* LeaveNotify              8 */
  focusNotify,          /* FocusIn                  9 */
  focusNotify,          /* FocusOut                10 */
  skip,                 /* KeymapNotify            11 */
  expose,               /* Expose                  12 */
  skip,                 /* GraphicsExpose          13 */
  skip,                 /* NoExpose                14 */
  skip,                 /* VisibilityNotify        15 */
  skip,                 /* CreateNotify            16 */
  destroyNotify,        /* DestroyNotify           17 */
  mapNotify,            /* UnmapNotify             18 */
  mapNotify,            /* MapNotify               19 */
  skip,                 /* MapRequest              20 */
  skip,                 /* ReparentNotify          21 */
  configureNotify,      /* ConfigureNotify         22 */
  skip,                 /* ConfigureRequest        23 */
  skip,                 /* GravityNotify           24 */
  skip,                 /* ResizeRequest           25 */
  skip,                 /* CirculateNotify         26 */
  skip,                 /* CirculateRequest        27 */
  skip,                 /* PropertyNotify          28 */
  selectionClear,       /* SelectionClear          29 */
  selectionRequest,     /* SelectionRequest        30 */
  skip,                 /* SelectionNotify         31 */
  skip,                 /* ColormapNotify          32 */
  clientMessage,        /* ClientMessage           33 */
  skip                  /* MappingNotify           34 */
};


/*******************************************************************************
 *
 */

jobject
Java_java_awt_Toolkit_evtInit ( JNIEnv* env, jclass clazz )
{
  jclass Component;

  ComponentEvent = (*env)->FindClass( env, "java/awt/event/ComponentEvent");
  MouseEvent     = (*env)->FindClass( env, "java/awt/event/MouseEvent");
  FocusEvent     = (*env)->FindClass( env, "java/awt/event/FocusEvent");
  WindowEvent    = (*env)->FindClass( env, "java/awt/event/WindowEvent");
  KeyEvent       = (*env)->FindClass( env, "java/awt/event/KeyEvent");
  PaintEvent     = (*env)->FindClass( env, "java/awt/event/PaintEvent");

  getComponentEvent = (*env)->GetStaticMethodID( env, ComponentEvent, "getComponentEvent", 
												 "(IIIIII)Ljava/awt/event/ComponentEvent;");
  getMouseEvent     = (*env)->GetStaticMethodID( env, MouseEvent, "getMouseEvent",
												 "(IIIII)Ljava/awt/event/MouseEvent;");
  getFocusEvent     = (*env)->GetStaticMethodID( env, FocusEvent, "getFocusEvent",
												 "(II)Ljava/awt/event/FocusEvent;");
  getWindowEvent    = (*env)->GetStaticMethodID( env, WindowEvent, "getWindowEvent",
												 "(II)Ljava/awt/event/WindowEvent;");
  getKeyEvent       = (*env)->GetStaticMethodID( env, KeyEvent, "getKeyEvent",
												 "(IIIII)Ljava/awt/event/KeyEvent;");
  getPaintEvent     = (*env)->GetStaticMethodID( env, PaintEvent, "getPaintEvent",
												 "(IIIIII)Ljava/awt/event/PaintEvent;");

  X->nWindows = 47;
  X->windows = calloc( X->nWindows, sizeof(Window));

  Component = (*env)->FindClass( env, "java/awt/Component");
  return (*env)->NewObjectArray( env, X->nWindows, Component, NULL);
}


jobject
Java_java_awt_Toolkit_evtGetNextEvent ( JNIEnv* env, jclass clazz )
{
  jobject jEvt = NULL;

  while ( nextEvent( X) &&
		  ((getSourceIdx( X, X->event.xany.window) >= 0) || 
		   (X->event.xany.window == X->cbdOwner)) ) {
	X->preFetched = 0;
	if ( (jEvt = (processEvent[X->event.xany.type])( env, X)) ){
	  break;
	}
  }

  return jEvt;
}

jobject
Java_java_awt_Toolkit_evtPeekEvent ( JNIEnv* env, jclass clazz )
{
  jobject jEvt = NULL;

  X->peek = 1;
  if ( (jEvt = Java_java_awt_Toolkit_evtGetNextEvent( env, clazz)) ) {
	X->preFetched = 1;
  }
  X->peek = 0;

  return jEvt;
}


/*
 * We can't do a XPeekIfEvent here because it would block until such an event
 * comes in
 */
jobject
Java_java_awt_Toolkit_evtPeekEventId ( JNIEnv* env, jclass clazz, jint id )
{
  jobject jEvt;

  if ( (jEvt = Java_java_awt_Toolkit_evtPeekEvent( env, clazz)) && (X->evtId == id) ){
	return jEvt;
  }

  return NULL;
}


/*
 * This is just used to wakeup the getNextEvent call if we are multithreaded,
 * and we post an event from outside the dispatcher thread (which might be blocked
 * in a native getNextEvent). This is a tribute to the fact that we actually have
 * two different queues - the native one, and the queue for self-posted events
 */
void
Java_java_awt_Toolkit_evtWakeup ( JNIEnv* env, jclass clazz )
{
  XEvent event;

  event.xclient.message_type = WAKEUP;
  event.xclient.format = 8;

  XSendEvent( X->dsp, X->event.xany.window, False, -1, &event);
}

jint
Java_java_awt_Toolkit_evtRegisterSource ( JNIEnv* env, jclass clazz, void* wnd )
{
  register int i;
  int      n;

  /*
   * we don't use a double hashing here because collisions are very unlikely
   * (window IDs usually already are hashed, so it does not make sense to
   * hash them again - we just could make it worse
   */
  for ( i = (unsigned long)wnd, n=0; n < X->nWindows; i++, n++ ) {
	i %= X->nWindows;
	if ( (int)X->windows[i] <= 0 ) {
	  X->windows[i] = (Window)wnd;
	  return i;
	}
  }
	
  fprintf( stderr, "window table out of space: %d", X->nWindows);
  return -1;
}


jint
Java_java_awt_Toolkit_evtUnregisterSource ( JNIEnv* env, jclass clazz, void* wnd )
{
  int i = getSourceIdx( X, (Window)wnd);

  if ( i >= 0 )
	X->windows[i] = -1;

  return i;
}
