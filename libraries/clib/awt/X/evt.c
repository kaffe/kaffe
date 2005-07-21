/**
 * evt.c - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */


#include "config.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include "toolkit.h"
#include "keysyms.h"
#include "jcl.h"


#if !defined (USE_POLLING_AWT)
#include "jsyscall.h"
#include "locks.h"

#if defined(UNIX_JTHREADS)
void jthreadedBlockEAGAIN(int fd);   /* move to SysCallInterface ? */
int jthreadedFileDescriptor(int fd);
#endif

#endif /* !USE_POLLING_AWT */


/*******************************************************************************
 *
 */

static int nextEvent ( JNIEnv* env UNUSED, jclass clazz UNUSED, Toolkit *tk, int blockIt UNUSED )
{
  if ( tk->preFetched )
	return 1;

#if !defined(USE_POLLING_AWT) && !defined(SUN_AWT_FIX)
  /*
   * We can't use QueuedAfterFlush since it seems to rely on blocked IO. At least
   * XFree86-3.3.2 subsequently hangs in _XFlushInt. This is the weak point of
   * our multithreaded X support, which depends on the Xlib in use
   */
  if ( blockIt ) {                /* this is from a getNextEvent */
	while ( tk->pending <= 0 ) {
	  XFlush( tk->dsp);
	  if ( (tk->pending = XEventsQueued( tk->dsp, QueuedAlready)) == 0 ) {
		tk->blocking = 1;

		/* Note that we might get blocked here, but we still have the Toolkit.class
		 * lock. Since the new locking interface requires locks to occur symmetrical
		 * inside of the same stack frame, we have to backup to some VM specific
		 * functionality, here.
		 */
#if defined (UNIX_JTHREADS)
		UNBLOCK_EXECUTE( clazz, (jthreadedBlockEAGAIN( ConnectionNumber( tk->dsp))));

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
		    UNBLOCK_EXECUTE( clazz, (stat =select( ConnectionNumber(tk->dsp)+1, &tk->rfds, NULL,NULL,NULL)));
		  }  while ( stat != 1 );
		}
#endif

		/*
		 * getting here just means we got input, it doesn't mean we have events. It also
		 * doesn't mean there currently is no other thread doing X requests
		 */
		tk->blocking = 0;
		tk->pending = XEventsQueued( tk->dsp, QueuedAfterReading);
	  }
	}
  }
  else {                         /* this is from a peekEvent */
	if ( tk->blocking ){          /* nothing to expect, there is a pending getNextEvent */
	  return 0;
	}

	if ( tk->pending <= 0 ) {
	  XFlush( tk->dsp);
	  if ( (tk->pending = XEventsQueued( tk->dsp, QueuedAlready)) == 0 ){
		return 0;
	  }
	}
  }
#else /* USE_POLLING_AWT */
  /*
   * We need to use this one for Solaris.  We have problems trying to unblock the
   * AWT thread off the X server connection.
   */
  if ( tk->pending <= 0 ) {
	if ( (tk->pending = XEventsQueued( tk->dsp, QueuedAfterFlush)) == 0 )
	  return 0;
  }
#endif /* !USE_POLLING_AWT */

  XNextEvent( tk->dsp, &tk->event);
  tk->pending--;

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

static inline int keyMod ( int keyState )
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

#if defined(KAFFE_VMDEBUG) && !defined(NDEBUG)
static const char *eventStr ( int evtId )
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
#endif /* defined(KAFFE_VMDEBUG) && !defined(NDEBUG) */

static jobject
skip ( JNIEnv* env UNUSED, Toolkit* tk UNUSED )
{
  return NULL;
}


static jobject
keyNotify ( JNIEnv* env, Toolkit* tk )
{
  KeySym          keysym;
  KeySym          keysym2;
  XComposeStatus  ioStatus;
  int             n, keyCode, keyChar, mod, idx, nchar;

  /*
   * We should eventually support input methods here.
   * Note that 'keysym' is queried separately (with a standard state), to
   * ensure the "one physical key -> one keycode" invariant
   */
  n = XLookupString( &tk->event.xkey, tk->buf, tk->nBuf, &keysym2, &ioStatus);
  keysym = XKeycodeToKeysym( tk->dsp, tk->event.xkey.keycode, 0);


  /* Bug fix: the keypad numbers where not handled correctly.
   * In X, numlock is a modifier, and XKeycodeToKeysym do
   * not do any modifier interpretation (in order to
   * build the correct Java KeyEvent).
   * But, as a result, since there is no NumLock modifier
   * in Java, the information was lost, and the keypad could
   * not work with NumLock selected.
   * The "solution" is to use the returned keysym from XLookupString
   * (which interpret the modifiers) if and only if it 
   * the original keysym correspond to the keypad; this should
   * code will all the case where the numlock alter the interpretation
   * of the keypad; also, if the keysums are the xk_xp_<num> 
   * we set the keychar to the correspoding digit.
   */

  if ((keysym >= XK_KP_Space) && (keysym <= XK_KP_9)) {
      keysym = keysym2;
      
      switch (keysym)
	{
	case XK_KP_Multiply:
	  nchar = '*';
	  break;
	case XK_KP_Add:
	  nchar = '+';
	  break;
	case XK_KP_Separator:
	  nchar = ',';
	  break;
	case XK_KP_Subtract:
	  nchar = '-';
	  break;
	case XK_KP_Decimal:
	  nchar = '.';
	  break;
	case XK_KP_Divide:
	  nchar = '/';
	  break;
	case XK_KP_0:
	  nchar = '0';
	  break;
	case XK_KP_1:
	  nchar = '1';
	  break;
	case XK_KP_2:
	  nchar = '2';
	  break;
	case XK_KP_3:
	  nchar = '3';
	  break;
	case XK_KP_4:
	  nchar = '4';
	  break;
	case XK_KP_5:
	  nchar = '5';
	  break;
	case XK_KP_6:
	  nchar = '6';
	  break;
	case XK_KP_7:
	  nchar = '7';
	  break;
	case XK_KP_8:
	  nchar = '8';
	  break;
	case XK_KP_9:
	  nchar = '9';
	  break;
	default:
	  nchar = -1;
	  break;
	}
  }
  else
    nchar = -1;

  if ( (keysym >= 0xff00) || (n == 0) ) {
	keyCode = FKeyCode[keysym & 0xff];

	/*
	 * There are some "control keys" that should generate KEY_TYPED events
	 * (enter, cancel, backspace, del, tab). This is flagged by a negative keyCode
	 * value and leads to non-zero keyChars
	 */
	if ( keyCode < 0 ){
	  keyChar = keyCode = -keyCode;
	} else if (nchar >= 0) {
	  keyChar = nchar;
	}
	else {  /* a "pure" function key */
	  keyChar = 0;
	}

  }
  else {
	keyChar = (unsigned char)X->buf[0];
	keyCode = LKeyCode[keysym & 0xff];
  }

  tk->evtId = (tk->event.xany.type == KeyPress)? KEY_PRESSED : KEY_RELEASED;
  mod = keyMod( tk->event.xkey.state);

  if ( tk->fwdIdx >= 0 ) {
	/*
	 * watch out - we still might have key events for a already
	 * unregistered forwardee in the queue (since we fake X, we can't rely on it
	 * to remove key events for a destroyed forwardee)
	 */
	if ( !checkSource( tk, tk->fwdIdx) )
	  return 0;
	idx = tk->fwdIdx;
  }
  else {
	idx = tk->srcIdx;
  }

  return (*env)->CallStaticObjectMethod( env, KeyEvent, getKeyEvent,
					 idx, tk->evtId, keyCode, keyChar, mod);
}


static jobject
buttonNotify ( JNIEnv* env, Toolkit* tk )
{
  if ( tk->event.xany.type == ButtonPress ) {
	tk->evtId = MOUSE_PRESSED;

	if ( (tk->windows[tk->srcIdx].w == tk->focus) && (tk->fwdIdx >= 0) )
	  forwardFocus( FWD_REVERT, tk->event.xany.window);
  }
  else {
	tk->evtId = MOUSE_RELEASED;
  }

  return (*env)->CallStaticObjectMethod( env, MouseEvent, getMouseEvent,
					 tk->srcIdx, tk->evtId,
					 tk->event.xbutton.button,
					 tk->event.xbutton.x, tk->event.xbutton.y);
}


static jobject
motionNotify ( JNIEnv* env, Toolkit* tk )
{
  return (*env)->CallStaticObjectMethod( env, MouseEvent, getMouseEvent,
					 tk->srcIdx, (tk->evtId = MOUSE_MOVED),
					 0, tk->event.xmotion.x, tk->event.xmotion.y);
}


static jobject
mouseNotify ( JNIEnv* env, Toolkit* tk )
{
  tk->evtId = (tk->event.xany.type == EnterNotify) ? MOUSE_ENTERED : MOUSE_EXITED;

  return (*env)->CallStaticObjectMethod( env, MouseEvent, getMouseEvent,
					 tk->srcIdx, tk->evtId,
					 0, tk->event.xcrossing.x, tk->event.xcrossing.y);
}


static jobject
focusNotify ( JNIEnv* env, Toolkit* tk )
{
  int et = tk->event.xany.type;
  int idx = (tk->focusFwd) ? tk->fwdIdx : tk->srcIdx;

  /*
   * get rid of all these fancy intermediate focus events (the real thing should
   * come last), but be aware of that we might get events for already unregistered windows
   * (in case the app isn't particulary careful with disposing windows), ending up in
   * ArrayOutOfBoundsExceptions in the getFocusEvent
   */
  while ( XCheckMaskEvent( tk->dsp, FocusChangeMask, &tk->event) ){
	tk->pending--;
	if ( getSourceIdx( tk, tk->event.xfocus.window) >= 0 ) {
	  et = tk->event.xany.type;
	  idx = (tk->focusFwd) ? tk->fwdIdx : tk->srcIdx;
	}
  }

  if ( et == FocusIn ) {
	tk->evtId = FOCUS_GAINED;
	tk->focus = tk->event.xany.window;
  }
  else {
	tk->evtId = FOCUS_LOST;
	tk->focus = 0;
  }

  /*
   * No matter what the focus change is - if we get a REAL focus notification,
   * it means that we will end focus forwarding (which is done on the owner-basis,
   * not by means of a global grab mode)
   */
  resetFocusForwarding( tk);

  if ( checkSource( tk, idx) ){
	return (*env)->CallStaticObjectMethod( env, FocusEvent, getFocusEvent, idx,
										   tk->evtId, JNI_FALSE);
  }
  else {
	return 0;
  }
}


static jobject
expose ( JNIEnv* env, Toolkit* tk )
{
  Window  wnd = tk->event.xany.window;
  int     x   = tk->event.xexpose.x;
  int     y   = tk->event.xexpose.y;
  int     w   = tk->event.xexpose.width;
  int     h   = tk->event.xexpose.height;
  int     xw, yh, exw, eyh;

  while ( XCheckWindowEvent( tk->dsp, wnd, ExposureMask | StructureNotifyMask, &tk->event) ){
	tk->pending--;

	if ( tk->event.xany.type == Expose ) {
	  xw = x + w;
	  yh = y + h;

	  exw = tk->event.xexpose.x + tk->event.xexpose.width;
	  eyh = tk->event.xexpose.y + tk->event.xexpose.height;

	  if ( tk->event.xexpose.x < x ) x = tk->event.xexpose.x;
	  if ( tk->event.xexpose.y < y ) y = tk->event.xexpose.y;
	
	  w = (exw > xw) ? exw - x : xw - x;
	  h = (eyh > yh) ? eyh - y : yh - y;
	}
	else {
	  tk->preFetched = 1;
	  break;
	}
  }

  return (*env)->CallStaticObjectMethod( env, PaintEvent, getPaintEvent,
					 tk->srcIdx, (tk->evtId = UPDATE),
					 x, y, w, h);
}


static jobject
destroyNotify ( JNIEnv* env, Toolkit* tk )
{
  /*
   * We should get this just for windows which have been destroyed from an
   * external client, since removeNotify() calls evtUnregisterSource() (i.e.
   * removes windows properly from the dispatch table)
   */
  tk->windows[tk->srcIdx].flags &= ~WND_MAPPED;

  return (*env)->CallStaticObjectMethod( env, WMEvent, getWMEvent,
					 tk->srcIdx, (tk->evtId = WM_KILLED));
}


static jobject
mapNotify ( JNIEnv* env, Toolkit* tk )
{
  int id = 0;

  if ( tk->event.xany.type == MapNotify ) {
	if ( (tk->windows[tk->srcIdx].flags & WND_MAPPED) == 0 ){
	  id = WINDOW_DEICONIFIED;
	  tk->windows[tk->srcIdx].flags |= WND_MAPPED;
	}
  }
  else {
	if ( (tk->windows[tk->srcIdx].flags & WND_MAPPED) != 0 ){
	  id = WINDOW_ICONIFIED;
	  tk->windows[tk->srcIdx].flags &= ~WND_MAPPED;
	}
  }

  if ( id ) {
    return (*env)->CallStaticObjectMethod( env, WindowEvent, getWindowEvent,
					   tk->srcIdx, id);
  }
  else {
	  /* we do the ComponentEvent show/hide in Java */
	return 0;
  }
}


static jobject
configureNotify ( JNIEnv* env, Toolkit* tk )
{
  Window  child;
  int     x, y, w, h;

  /*
   * some window managers are rather loquacious when doing opaque moves
   */

  while ( XCheckTypedWindowEvent( tk->dsp, tk->event.xany.window, ConfigureNotify, &tk->event) ){
	tk->pending--;
  }

  if ( (tk->event.xconfigure.x == 0) && (tk->event.xconfigure.y == 0) ) {
	XTranslateCoordinates( tk->dsp, tk->event.xconfigure.window,
			       DefaultRootWindow( tk->dsp),
			       tk->event.xconfigure.x, tk->event.xconfigure.y,
			       &tk->event.xconfigure.x, &tk->event.xconfigure.y, &child);
  }

  x = tk->event.xconfigure.x;
  y = tk->event.xconfigure.y;
  w = tk->event.xconfigure.width;
  h = tk->event.xconfigure.height;
  X->evtId = COMPONENT_RESIZED;

  return (*env)->CallStaticObjectMethod( env, ComponentEvent, getComponentEvent,
					 tk->srcIdx, tk->evtId, x, y, w, h);
}


static jobject
clientMessage ( JNIEnv* env, Toolkit* tk )
{
  if ( tk->windows[tk->srcIdx].flags & WND_DESTROYED ){
	/* we lost him, Jim */
	return 0;
  }

  if ( tk->event.xclient.message_type == WM_PROTOCOLS ) {
	if ( tk->event.xclient.data.l[0] == WM_DELETE_WINDOW ) {
	  return (*env)->CallStaticObjectMethod( env, WindowEvent, getWindowEvent,
											 tk->srcIdx, (tk->evtId = WINDOW_CLOSING));
	}
	else if ( tk->event.xclient.data.l[0] == WM_TAKE_FOCUS ) {
	  XSetInputFocus( tk->dsp, tk->event.xany.window, RevertToParent, CurrentTime);
	}
  }

  /*
   * This is a workaround for the common problem of requesting the focus for not
   * yet mapped windows (resulting in BadMatch errors)
   */
  else if ( tk->event.xclient.message_type == RETRY_FOCUS ) {
	if ( tk->windows[tk->srcIdx].flags & WND_MAPPED ) {
	  XSetInputFocus( tk->dsp, tk->event.xclient.window, RevertToParent, CurrentTime);
	  if ( tk->event.xclient.data.l[1] ) {
		/* we have a pending forward request, too */
		forwardFocus( FWD_SET, tk->event.xclient.data.l[1]);
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
  else if ( tk->event.xclient.message_type == FORWARD_FOCUS ) {
	switch ( tk->event.xclient.data.l[0] ){
	case FWD_SET:
	  DBG( AWT_EVT, printf("FWD_SET: %lx (%d) %lx\n", tk->event.xany.window, tk->srcIdx, tk->windows[tk->srcIdx].owner));

	  if ( (tk->srcIdx != tk->fwdIdx) && (tk->focus == tk->windows[tk->srcIdx].owner) ){
		tk->fwdIdx = tk->srcIdx;
		tk->focusFwd = tk->event.xany.window;
		return (*env)->CallStaticObjectMethod( env, FocusEvent, getFocusEvent,
						       tk->srcIdx, FOCUS_GAINED, JNI_TRUE);
	  }
	  else {
		return 0;
	  }

	case FWD_CLEAR:
	  DBG( AWT_EVT, printf("FWD_CLEAR: %lx (%d) %lx\n", tk->event.xany.window, tk->srcIdx, tk->windows[tk->srcIdx].owner));

	  if ( tk->fwdIdx >= 0 ) {
		resetFocusForwarding( tk);
		return (*env)->CallStaticObjectMethod( env, FocusEvent, getFocusEvent,
						       tk->srcIdx, FOCUS_LOST, JNI_FALSE);
	  }
	  else {
		return 0;
	  }

	case FWD_REVERT:
	  DBG( AWT_EVT, printf("FWD_REVERT: %lx\n", tk->event.xany.window));
	  if ( tk->event.xany.window == tk->focus ) {
		resetFocusForwarding( tk);
		return (*env)->CallStaticObjectMethod( env, FocusEvent, getFocusEvent,
						       tk->srcIdx, FOCUS_GAINED, JNI_FALSE);
	  }
	}
  }
  return 0;
}

static jobject
reparentNotify ( JNIEnv* env, Toolkit* tk )
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

  if ( tk->frameInsets.guess || tk->dialogInsets.guess ) {
	window = tk->event.xreparent.window;
	parent = tk->event.xreparent.parent;

	XGetGeometry( tk->dsp, parent, &root, &x, &y, &w, &h, &bw, &d);
	XGetGeometry( tk->dsp, window, &root, &xc, &yc, &wc, &hc, &bw, &d);

	left   = tk->event.xreparent.x;
	top    = tk->event.xreparent.y;
	right  = w - wc - left;
	bottom = h - hc - top;

	if ( (tk->windows[tk->srcIdx].flags & WND_DIALOG) && tk->dialogInsets.guess ) {
	  in = &(tk->dialogInsets);
	  if ( (left != in->left) || (top != in->top) ||
		   (right != in->right) || (bottom != in->bottom) ){
		clazz = (*env)->FindClass( env, "java/awt/Dialog");
		setDecoInsets = (*env)->GetStaticMethodID( env, clazz, "setDecoInsets","(IIIII)V");
	  }
	  in->guess = 0;
	}
	else if ( tk->frameInsets.guess ) {
	  in = &(tk->frameInsets);
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

	  XCheckTypedWindowEvent( tk->dsp, window, ConfigureNotify, &X->event);
	  XCheckTypedWindowEvent( tk->dsp, window, Expose, &X->event);
	  XResizeWindow( tk->dsp, window, wc, hc);

	  in->left = left;
	  in->top = top;
	  in->right = right;
	  in->bottom = bottom;

	  (*env)->CallStaticVoidMethod( env, clazz, setDecoInsets, 
									in->top, in->left, in->bottom, in->right, tk->srcIdx);

	  /* check if this was a resize locked window (which has to be locked again) */
	  XGetWMNormalHints( tk->dsp, window, &wmHints, &supHints);
	  if ( wmHints.min_width == wmHints.max_width ){
		wmHints.min_width = wmHints.max_width = wc;
		wmHints.min_height = wmHints.max_height = hc;
		XSetWMNormalHints( tk->dsp, window, &wmHints);
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
Java_java_awt_Toolkit_evtInit ( JNIEnv* env, jclass clazz UNUSED )
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
  assert(ComponentEvent != NULL);
  MouseEvent     = (*env)->FindClass( env, "java/awt/MouseEvt");
  assert(MouseEvent != NULL);
  FocusEvent     = (*env)->FindClass( env, "java/awt/FocusEvt");
  assert(FocusEvent != NULL);
  WindowEvent    = (*env)->FindClass( env, "java/awt/WindowEvt");
  assert(WindowEvent != NULL);  
  KeyEvent       = (*env)->FindClass( env, "java/awt/KeyEvt");
  assert(KeyEvent != NULL);
  PaintEvent     = (*env)->FindClass( env, "java/awt/PaintEvt");
  assert(PaintEvent != NULL);
  WMEvent        = (*env)->FindClass( env, "java/awt/WMEvent");
  assert(WMEvent != NULL);

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
Java_java_awt_Toolkit_evtWakeup ( JNIEnv* env UNUSED, jclass clazz UNUSED )
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
Java_java_awt_Toolkit_evtSendWMEvent ( JNIEnv* env UNUSED, jclass clazz UNUSED, jobject wmEvt UNUSED )
{
}


jint
Java_java_awt_Toolkit_evtRegisterSource ( JNIEnv* env UNUSED, jclass clazz UNUSED, jobject nativeWnd )
{
  Window wnd = UNVEIL_WND(nativeWnd);

  /*
   * We have done that already during the various wndCreateXX()
   */
  int i = getSourceIdx( X, wnd);

  DBG( AWT_EVT, printf("registerSource( %lx) -> %d\n", wnd, i));

  return i;
}


jint
Java_java_awt_Toolkit_evtUnregisterSource ( JNIEnv* env UNUSED, jclass clazz UNUSED, jobject nativeWnd )
{
  Window wnd = UNVEIL_WND(nativeWnd);
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
