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


#if !defined (USE_POLLING_AWT)
#include "jsyscall.h"
#include "../../../../kaffe/kaffevm/locks.h"

#if defined(UNIX_JTHREADS)
void jthreadedBlockEAGAIN(int fd);   /* move to SysCallInterface ? */
int jthreadedFileDescriptor(int fd);
#endif

#endif /* !USE_POLLING_AWT */

void forwardFocus ( int cmd, Window wnd );  /* from wnd.c */


/*******************************************************************************
 *
 */

static int nextEvent ( JNIEnv* env, jclass clazz, Toolkit *X, int blockIt )
{
  if ( X->preFetched )
	return 1;

#if !defined(USE_POLLING_AWT) && !defined(SUN_AWT_FIX)
  /*
   * We can't use QueuedAfterFlush since it seems to rely on blocked IO. At least
   * XFree86-3.3.2 subsequently hangs in _XFlushInt. This is the weak point of
   * our multithreaded X support, which depends on the Xlib in use
   */
  if ( blockIt ) {                /* this is from a getNextEvent */
	while ( X->pending <= 0 ) {
	  XFlush( X->dsp);
	  if ( (X->pending = XEventsQueued( X->dsp, QueuedAlready)) == 0 ) {
		X->blocking = 1;

		/* Note that we might get blocked here, but we still have the Toolkit.class
		 * lock. Since the new locking interface requires locks to occur symmetrical
		 * inside of the same stack frame, we have to backup to some VM specific
		 * functionality, here.
		 */
#if defined (UNIX_JTHREADS)
		UNBLOCK_EXECUTE( clazz, (jthreadedBlockEAGAIN( ConnectionNumber( X->dsp))));

#elif defined (UNIX_PTHREADS)
		/*
		 * Even with pthreads, don't simply do a XNextEvent, because we most probably
		 * would get "unexpected async replies" X errors when doing roundtrips (e.g.
		 * direct or indirect XSyncs) from other threads while still being blocked in
		 * XNextEvent, here. The only thing done outside the Toolkit lock should be
		 * to check availability of X input
		 */
		{
		  int stat;

		  do {
		    UNBLOCK_EXECUTE( clazz, (stat =select( ConnectionNumber(X->dsp)+1, &X->rfds, NULL,NULL,NULL)));
		  }  while ( stat != 1 );
		}
#endif

		/*
		 * getting here just means we got input, it doesn't mean we have events. It also
		 * doesn't mean there currently is no other thread doing X requests
		 */
		X->blocking = 0;
		X->pending = XEventsQueued( X->dsp, QueuedAfterReading);
	  }
	}
  }
  else {                         /* this is from a peekEvent */
	if ( X->blocking ){          /* nothing to expect, there is a pending getNextEvent */
	  return 0;
	}

	if ( X->pending <= 0 ) {
	  XFlush( X->dsp);
	  if ( (X->pending = XEventsQueued( X->dsp, QueuedAlready)) == 0 ){
		return 0;
	  }
	}
  }
#else /* USE_POLLING_AWT */
  /*
   * We need to use this one for Solaris.  We have problems trying to unblock the
   * AWT thread off the X server connection.
   */
  if ( X->pending <= 0 ) {
	if ( (X->pending = XEventsQueued( X->dsp, QueuedAfterFlush)) == 0 )
	  return 0;
  }
#endif /* !USE_POLLING_AWT */

  XNextEvent( X->dsp, &X->event);
  X->pending--;

  return 1;
}


/* X-to-Java key modifier mapping
 *                                     altGr   : PC
 *     shift       -    ctrl     alt    meta   : Java
 *     Shift    Lock    Ctrl    Mod1    Mod3   : X      symbol
 *  ----------------------------------------------------------
 *         1       1       2       8       4   : Java   value
 *         1       2       4       8      32   : X
 */

static __inline__ int keyMod ( int keyState )
{
  int mod = 0;
  if ( keyState & 3 ) mod |= 1; /* shift or lock */
  if ( keyState & 4 ) mod |= 2;
  if ( keyState & 8 ) mod |= 8;
  return mod;
}


jclass     AWTEvent;
jclass     ComponentEvent;
jclass     MouseEvent;
jclass     FocusEvent;
jclass     WindowEvent;
jclass     KeyEvent;
jclass     PaintEvent;
jclass     WMEvent;

jmethodID  getComponentEvent;
jmethodID  getMouseEvent;
jmethodID  getFocusEvent;
jmethodID  getWindowEvent;
jmethodID  getKeyEvent;
jmethodID  getPaintEvent;
jmethodID  getWMEvent;

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
#define    UPDATE               801

#define    FOCUS_GAINED        1004
#define    FOCUS_LOST          1005

#define    WM_KILLED           1905

#if defined(DEBUG)
static char *eventStr ( int evtId )
{
  switch (evtId) {
  case COMPONENT_RESIZED: return "ComponentResized";

  case WINDOW_CLOSING: return "WindowClosing";
  case WINDOW_CLOSED: return "WindowClosed";
  case WINDOW_ICONIFIED: return "WindowIconified";
  case WINDOW_DEICONIFIED: return "WindowDeiconified";

  case KEY_PRESSED: return "KeyPressed";
  case KEY_RELEASED: return "KeyReleased";

  case MOUSE_PRESSED: return "MousePressed";
  case MOUSE_RELEASED: return "MouseReleased";
  case MOUSE_MOVED: return "MouseMoved";
  case MOUSE_ENTERED: return "MouseEntered";
  case MOUSE_EXITED: return "MouseExited";

  case PAINT: return "Paint";
  case UPDATE: return "Update";
	
  case FOCUS_GAINED: return "FocusGained";
  case FOCUS_LOST: return "FocusLost";

  case WM_KILLED: return "WMKilled";

  default: return "<unknown>";
  }
};
#endif

static jobject
skip ( JNIEnv* env, Toolkit* X )
{
  return NULL;
}


static jobject
keyNotify ( JNIEnv* env, Toolkit* X )
{
  KeySym          keysym;
  XComposeStatus  ioStatus;
  int             n, keyCode, keyChar, mod, idx;

  /*
   * We should eventually support input methods here.
   * Note that 'keysym' is queried separately (with a standard state), to
   * ensure the "one physical key -> one keycode" invariant
   */
  n = XLookupString( &X->event.xkey, X->buf, X->nBuf, 0, &ioStatus);
  keysym = XKeycodeToKeysym( X->dsp, X->event.xkey.keycode, 0);

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
	keyCode = LKeyCode[keysym & 0xff];
  }

  X->evtId = (X->event.xany.type == KeyPress)? KEY_PRESSED : KEY_RELEASED;
  mod = keyMod( X->event.xkey.state);

  if ( X->fwdIdx >= 0 ) {
	/*
	 * watch out - we still might have key events for a already
	 * unregistered forwardee in the queue (since we fake X, we can't rely on it
	 * to remove key events for a destroyed forwardee)
	 */
	if ( !checkSource( X, X->fwdIdx) )
	  return 0;
	idx = X->fwdIdx;
  }
  else {
	idx = X->srcIdx;
  }

  return (*env)->CallStaticObjectMethod( env, KeyEvent, getKeyEvent,
										 idx, X->evtId, keyCode, keyChar, mod);
}


static jobject
buttonNotify ( JNIEnv* env, Toolkit* X )
{
  if ( X->event.xany.type == ButtonPress ) {
	X->evtId = MOUSE_PRESSED;

	if ( (X->windows[X->srcIdx].w == X->focus) && (X->fwdIdx >= 0) )
	  forwardFocus( FWD_REVERT, X->event.xany.window);
  }
  else {
	X->evtId = MOUSE_RELEASED;
  }

  return (*env)->CallStaticObjectMethod( env, MouseEvent, getMouseEvent,
										 X->srcIdx, X->evtId,
										 X->event.xbutton.button,
										 X->event.xbutton.x, X->event.xbutton.y);
}


static jobject
motionNotify ( JNIEnv* env, Toolkit* X )
{
  return (*env)->CallStaticObjectMethod( env, MouseEvent, getMouseEvent,
										 X->srcIdx, (X->evtId = MOUSE_MOVED),
										 0, X->event.xmotion.x, X->event.xmotion.y);
}


static jobject
mouseNotify ( JNIEnv* env, Toolkit* X )
{
  X->evtId = (X->event.xany.type == EnterNotify) ? MOUSE_ENTERED : MOUSE_EXITED;

  return (*env)->CallStaticObjectMethod( env, MouseEvent, getMouseEvent,
										 X->srcIdx, X->evtId,
										 0, X->event.xcrossing.x, X->event.xcrossing.y);
}


static jobject
focusNotify ( JNIEnv* env, Toolkit* X )
{
  int et = X->event.xany.type;
  int idx = (X->focusFwd) ? X->fwdIdx : X->srcIdx;

  /*
   * get rid of all these fancy intermediate focus events (the real thing should
   * come last), but be aware of that we might get events for already unregistered windows
   * (in case the app isn't particulary careful with disposing windows), ending up in
   * ArrayOutOfBoundsExceptions in the getFocusEvent
   */
  while ( XCheckMaskEvent( X->dsp, FocusChangeMask, &X->event) ){
	X->pending--;
	if ( getSourceIdx( X, X->event.xfocus.window) >= 0 ) {
	  et = X->event.xany.type;
	  idx = (X->focusFwd) ? X->fwdIdx : X->srcIdx;
	}
  }

  if ( et == FocusIn ) {
	X->evtId = FOCUS_GAINED;
	X->focus = X->event.xany.window;
  }
  else {
	X->evtId = FOCUS_LOST;
	X->focus = 0;
  }

  /*
   * No matter what the focus change is - if we get a REAL focus notification,
   * it means that we will end focus forwarding (which is done on the owner-basis,
   * not by means of a global grab mode)
   */
  resetFocusForwarding( X);

  if ( checkSource( X, idx) ){
	return (*env)->CallStaticObjectMethod( env, FocusEvent, getFocusEvent, idx,
										   X->evtId, JNI_FALSE);
  }
  else {
	return 0;
  }
}


static jobject
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
										 X->srcIdx, (X->evtId = UPDATE),
										 x, y, w, h);
}


static jobject
destroyNotify ( JNIEnv* env, Toolkit* X )
{
  /*
   * We should get this just for windows which have been destroyed from an
   * external client, since removeNotify() calls evtUnregisterSource() (i.e.
   * removes windows properly from the dispatch table)
   */
  X->windows[X->srcIdx].flags &= ~WND_MAPPED;

  return (*env)->CallStaticObjectMethod( env, WMEvent, getWMEvent,
										 X->srcIdx, (X->evtId = WM_KILLED));
}


static jobject
mapNotify ( JNIEnv* env, Toolkit* X )
{
  int id = 0;

  if ( X->event.xany.type == MapNotify ) {
	if ( (X->windows[X->srcIdx].flags & WND_MAPPED) == 0 ){
	  id = WINDOW_DEICONIFIED;
	  X->windows[X->srcIdx].flags |= WND_MAPPED;
	}
  }
  else {
	if ( (X->windows[X->srcIdx].flags & WND_MAPPED) != 0 ){
	  id = WINDOW_ICONIFIED;
	  X->windows[X->srcIdx].flags &= ~WND_MAPPED;
	}
  }

  if ( id ) {
	return (*env)->CallStaticObjectMethod( env, WindowEvent, getWindowEvent,
										   X->srcIdx, id);
  }
  else {
	// we do the ComponentEvent show/hide in Java
	return 0;
  }
}


static jobject
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


static jobject
clientMessage ( JNIEnv* env, Toolkit* X )
{
  if ( X->windows[X->srcIdx].flags & WND_DESTROYED ){
	/* we lost him, Jim */
	return 0;
  }

  if ( X->event.xclient.message_type == WM_PROTOCOLS ) {
	if ( X->event.xclient.data.l[0] == WM_DELETE_WINDOW ) {
	  return (*env)->CallStaticObjectMethod( env, WindowEvent, getWindowEvent,
											 X->srcIdx, (X->evtId = WINDOW_CLOSING));
	}
	else if ( X->event.xclient.data.l[0] == WM_TAKE_FOCUS ) {
	  XSetInputFocus( X->dsp, X->event.xany.window, RevertToParent, CurrentTime);
	}
  }

  /*
   * This is a workaround for the common problem of requesting the focus for not
   * yet mapped windows (resulting in BadMatch errors)
   */
  else if ( X->event.xclient.message_type == RETRY_FOCUS ) {
	if ( X->windows[X->srcIdx].flags & WND_MAPPED ) {
	  XSetInputFocus( X->dsp, (Window)X->event.xclient.window, RevertToParent, CurrentTime);
	  if ( X->event.xclient.data.l[1] ) {
		/* we have a pending forward request, too */
		forwardFocus( FWD_SET, X->event.xclient.data.l[1]);
	  }
	}
  }

  /*
   * This is a workaround for X not having "owned" windows (popups), which do
   * not "shade" the titlebar of their owner (i.e. don't indicate a change in
   * the activeWindow). The only way to implement this reliable (w/o playing
   * around with a window manager) is to not let these popups select on
   * key events at all. But rather than to expose this to the Java side (like
   * the JDK does), we hide this in the native layer
   */
  else if ( X->event.xclient.message_type == FORWARD_FOCUS ) {
	switch ( X->event.xclient.data.l[0] ){
	case FWD_SET:
	  DBG( AWT_EVT, printf("FWD_SET: %lx (%d) %lx\n", X->event.xany.window, X->srcIdx, X->windows[X->srcIdx].owner));

	  if ( (X->srcIdx != X->fwdIdx) && (X->focus == X->windows[X->srcIdx].owner) ){
		X->fwdIdx = X->srcIdx;
		X->focusFwd = X->event.xany.window;
		return (*env)->CallStaticObjectMethod( env, FocusEvent, getFocusEvent,
											   X->srcIdx, FOCUS_GAINED, JNI_TRUE);
	  }
	  else {
		return 0;
	  }

	case FWD_CLEAR:
	  DBG( AWT_EVT, printf("FWD_CLEAR: %lx (%d) %lx\n", X->event.xany.window, X->srcIdx, X->windows[X->srcIdx].owner));

	  if ( X->fwdIdx >= 0 ) {
		resetFocusForwarding( X);
		return (*env)->CallStaticObjectMethod( env, FocusEvent, getFocusEvent,
											   X->srcIdx, FOCUS_LOST, JNI_FALSE);
	  }
	  else {
		return 0;
	  }

	case FWD_REVERT:
	  DBG( AWT_EVT, printf("FWD_REVERT: %lx\n", X->event.xany.window));
	  if ( X->event.xany.window == X->focus ) {
		resetFocusForwarding( X);
		return (*env)->CallStaticObjectMethod( env, FocusEvent, getFocusEvent,
											   X->srcIdx, FOCUS_GAINED, JNI_FALSE);
	  }
	}
  }
  return 0;
}

static jobject
reparentNotify ( JNIEnv* env, Toolkit* X )
{
  Window    window, parent, root;
  jclass    clazz = 0;
  jmethodID setDecoInsets = 0;
  int       left, top, right, bottom;
  int       x, y, w, h, bw, d;
  int       xc, yc, wc, hc;
  DecoInset *in = 0;
  XSizeHints wmHints;
  long      supHints;

  if ( X->frameInsets.guess || X->dialogInsets.guess ) {
	window = X->event.xreparent.window;
	parent = X->event.xreparent.parent;

	XGetGeometry( X->dsp, parent, &root, &x, &y, &w, &h, &bw, &d);
	XGetGeometry( X->dsp, window, &root, &xc, &yc, &wc, &hc, &bw, &d);

	left   = X->event.xreparent.x;
	top    = X->event.xreparent.y;
	right  = w - wc - left;
	bottom = h - hc - top;

	if ( (X->windows[X->srcIdx].flags & WND_DIALOG) && X->dialogInsets.guess ) {
	  in = &(X->dialogInsets);
	  if ( (left != in->left) || (top != in->top) ||
		   (right != in->right) || (bottom != in->bottom) ){
		clazz = (*env)->FindClass( env, "java/awt/Dialog");
		setDecoInsets = (*env)->GetStaticMethodID( env, clazz, "setDecoInsets","(IIIII)V");
	  }
	  in->guess = 0;
	}
	else if ( X->frameInsets.guess ) {
	  in = &(X->frameInsets);
	  if ( (left != in->left) || (top != in->top) ||
		   (right != in->right) || (bottom != in->bottom) ){
		clazz = (*env)->FindClass( env, "java/awt/Frame");
		setDecoInsets = (*env)->GetStaticMethodID( env, clazz, "setDecoInsets","(IIIII)V");
	  }
	  in->guess = 0;
	}

	if ( clazz ) {
	  wc -= (left + right) - (in->left + in->right);
	  hc -= (top + bottom) - (in->top + in->bottom);

	  XCheckTypedWindowEvent( X->dsp, window, ConfigureNotify, &X->event);
	  XCheckTypedWindowEvent( X->dsp, window, Expose, &X->event);
	  XResizeWindow( X->dsp, window, wc, hc);

	  in->left = left;
	  in->top = top;
	  in->right = right;
	  in->bottom = bottom;

	  (*env)->CallStaticVoidMethod( env, clazz, setDecoInsets, 
									in->top, in->left, in->bottom, in->right, X->srcIdx);

	  /* check if this was a resize locked window (which has to be locked again) */
	  XGetWMNormalHints( X->dsp, window, &wmHints, &supHints);
	  if ( wmHints.min_width == wmHints.max_width ){
		wmHints.min_width = wmHints.max_width = wc;
		wmHints.min_height = wmHints.max_height = hc;
		XSetWMNormalHints( X->dsp, window, &wmHints);
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
  reparentNotify,       /* ReparentNotify          21 */
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
#if !defined(USE_POLLING_AWT)
  unsigned long mask;
  XSetWindowAttributes attrs;
#endif

  if ( ComponentEvent != NULL ){
	DBG( AWT_EVT, printf("evtInit called twice\n"));
	return NULL;
  }

  ComponentEvent = (*env)->FindClass( env, "java/awt/ComponentEvt");
  MouseEvent     = (*env)->FindClass( env, "java/awt/MouseEvt");
  FocusEvent     = (*env)->FindClass( env, "java/awt/FocusEvt");
  WindowEvent    = (*env)->FindClass( env, "java/awt/WindowEvt");
  KeyEvent       = (*env)->FindClass( env, "java/awt/KeyEvt");
  PaintEvent     = (*env)->FindClass( env, "java/awt/PaintEvt");
  WMEvent        = (*env)->FindClass( env, "java/awt/WMEvent");

  getComponentEvent = (*env)->GetStaticMethodID( env, ComponentEvent, "getEvent", 
												 "(IIIIII)Ljava/awt/ComponentEvt;");
  getMouseEvent     = (*env)->GetStaticMethodID( env, MouseEvent, "getEvent",
												 "(IIIII)Ljava/awt/MouseEvt;");
  getFocusEvent     = (*env)->GetStaticMethodID( env, FocusEvent, "getEvent",
												 "(IIZ)Ljava/awt/FocusEvt;");
  getWindowEvent    = (*env)->GetStaticMethodID( env, WindowEvent, "getEvent",
												 "(II)Ljava/awt/WindowEvt;");
  getKeyEvent       = (*env)->GetStaticMethodID( env, KeyEvent, "getEvent",
												 "(IIIII)Ljava/awt/KeyEvt;");
  getPaintEvent     = (*env)->GetStaticMethodID( env, PaintEvent, "getEvent",
												 "(IIIIII)Ljava/awt/PaintEvt;");
  getWMEvent        = (*env)->GetStaticMethodID( env, WMEvent, "getEvent",
												 "(II)Ljava/awt/WMEvent;");

#if !defined(USE_POLLING_AWT)
  /*
   * we need a target for evtWakeup(), which is used to unblock nextEvent() in
   * case we post a event to the localqueue
   */
  mask = CWEventMask;
  attrs.override_redirect = True;
  attrs.event_mask = StructureNotifyMask;

  X->wakeUp = XCreateWindow( X->dsp, X->root, -1000, -1000, 1, 1, 0, CopyFromParent,
							   InputOutput, CopyFromParent, mask, &attrs);

  /*
   * make X connection non-blocking (to get SIGIOs)
   * NOTE: this requires all Xlib calls doing IO via the X conn to be synced! In addition,
   * no Xlib function doing blocked reads on the connection should be called without
   * ensuring that there is input available.
   */
#if defined (UNIX_JTHREADS)
  jthreadedFileDescriptor( ConnectionNumber(X->dsp));
#elif defined (UNIX_PTHREADS)
  FD_ZERO( &X->rfds);
  FD_SET( ConnectionNumber(X->dsp), &X->rfds);
#endif

#endif /* !USE_POLLING_AWT */

  Component = (*env)->FindClass( env, "java/awt/Component");
  return (*env)->NewObjectArray( env, X->nWindows, Component, NULL);
}


jobject
Java_java_awt_Toolkit_evtGetNextEvent ( JNIEnv* env, jclass clazz )
{
  jobject jEvt = NULL;

  DBG( AWT_EVT, printf("getNextEvent..\n"));

  while ( nextEvent( env, clazz, X, True) &&
		  ((getSourceIdx( X, X->event.xany.window) >= 0) || 
		   (X->event.xany.window == X->cbdOwner)) ) {
	X->preFetched = 0;
	if ( (jEvt = (processEvent[X->event.xany.type])( env, X)) ){
	  break;
	}
  }

  DBG( AWT_EVT, printf("..getNextEvent: %d (%s) %d, %p, %lx\n",
				 X->evtId, eventStr( X->evtId), X->srcIdx, jEvt, X->event.xany.window));

  return jEvt;
}

jobject
Java_java_awt_Toolkit_evtPeekEvent ( JNIEnv* env, jclass clazz )
{
  jobject jEvt = NULL;

  DBG( AWT_EVT, printf("peekEvent..\n"));

  if ( nextEvent( env, clazz, X, False) && ((getSourceIdx( X, X->event.xany.window) >= 0)) ) {
	if ( (jEvt = (processEvent[X->event.xany.type])( env, X)) )
	  X->preFetched = 1;
  }

  DBG( AWT_EVT, printf("..peekEvent: %s %p, %lx\n", eventStr(X->evtId), jEvt, X->event.xany.window));
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
 * two different queues - the native one, and the queue for self-posted events.
 * We could also do a round trip here, but this would wake up the event dispatcher
 * immediately (i.e. before this func returns), which is probably a crude side effect
 * of postEvent().
 * Using a ClientMessage might be a bit dangerous if Xlib tries to be too smart by
 * just dispatching this to the local queue (without passing to the server)
 */
void
Java_java_awt_Toolkit_evtWakeup ( JNIEnv* env, jclass clazz )
{
  XEvent event;

  DBG( AWT_EVT, printf("evtWakeup\n"));
  DBG_ACTION( awt, XSynchronize( X->dsp, False));

  event.xclient.type = ClientMessage; 
  event.xclient.message_type = WAKEUP;
  event.xclient.format = 8;
  event.xclient.window = X->wakeUp;

  XSendEvent( X->dsp, X->wakeUp, False, 0, &event);
  XFlush( X->dsp);

  DBG_ACTION( awt, XSynchronize( X->dsp, True));
}

/*
 * This is a helper for (IS_DISPATCH_EXCLUSIVE | NATIVE_DISPATCHER_LOOP) windowing
 * systems, where we need to execute certain requests (e.g. window creation) to
 * the native event thread. Not required here
 */
void
Java_java_awt_Toolkit_evtSendWMEvent ( JNIEnv* env, jclass clazz, jobject wmEvt )
{
}


jint
Java_java_awt_Toolkit_evtRegisterSource ( JNIEnv* env, jclass clazz, Window wnd )
{
  /*
   * We have done that already during the various wndCreateXX()
   */
  int i = getSourceIdx( X, wnd);

  DBG( AWT_EVT, printf("registerSource( %lx) -> %d\n", wnd, i));

  return i;
}


jint
Java_java_awt_Toolkit_evtUnregisterSource ( JNIEnv* env, jclass clazz, Window wnd )
{
  int i = getSourceIdx( X, wnd);

  if ( i >= 0 ){
	X->windows[i].w = -1;
	X->windows[i].flags = 0;
	X->windows[i].owner = 0;
  }

  if ( X->lastWindow == wnd )
	X->lastWindow = 0;

  DBG( AWT_EVT, printf("unregisterSource( %lx) -> %d\n", wnd, i));

  return i;
}
