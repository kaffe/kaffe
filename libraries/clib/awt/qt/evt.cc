/**
 * evt.c - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include <qapplication.h>
#include <qevent.h>
#include <qqueue.h>
#include <qwidget.h>

#include "evt.h"
#include "toolkit.h"

#include "keysyms.h"

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

extern void pollJavaClipboard(JNIEnv *env);
extern jobject clearJavaClipboard(JNIEnv *env);

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

class EventPacket {
public:
  EventPacket(QEvent* evt, int idx) { event = evt; srcIdx = idx; }
  QEvent* getEvent() { return event; }
  int getIndex() { return srcIdx; }
protected:
  QEvent* event;
  int srcIdx;
};


// Murphy
extern QApplication *qapp;
QQueue<EventPacket> g_event_queue;

EventDispatcher::EventDispatcher(QWidget *parent, const char *name) {
  if(parent)
    parent->installEventFilter(this);
}

bool EventDispatcher::eventFilter(QObject* o, QEvent* e) {
//  AWT_DBG(printf("event type=%d widget=%p\n", e->type(), o));
  QEvent* newEvent = NULL;
  EventPacket* packet = NULL;
  bool processed = false;

//  getSourceIdx(X, o);

  if(X->srcIdx == 0)
    return QWidget::eventFilter(o, e);

  switch(e->type()) {
    case QEvent::Clipboard: {
      newEvent = new QEvent(e->type());
      processed = true;
      break;
    }

    case QEvent::Destroy:
    case QEvent::Reparent: {
      newEvent = new QEvent(e->type());
      processed = true;
      break;
    }

    case QEvent::Show: {
      AWT_DBG(printf("Event Show: srcIdx=%d\n", X->srcIdx));

#if (QT_VERSION < 300)
      newEvent = (QEvent*) new QShowEvent(true);
#else
      QShowEvent* tmpShowEvent = (QShowEvent*)e;
      newEvent = (QEvent*) new QShowEvent();
#endif

      processed = true;
      break;
    }
    
    case QEvent::Hide: {
      AWT_DBG(printf("Event Hid: srcIdx=%d\n", X->srcIdx));

#if (QT_VERSION < 300)
      newEvent = (QEvent*) new QHideEvent(true);
#else
      QHideEvent* tmpHideEvent = (QHideEvent*)e;
      newEvent = (QEvent*) new QHideEvent();
#endif

      processed = true;
      break;
    }
    
    case QEvent::FocusIn:
    case QEvent::FocusOut: {
      AWT_DBG(printf("Event Focus: srcIdx=%d\n", X->srcIdx));
      QFocusEvent* tmpFocusEvent = (QFocusEvent*)e;
      newEvent = (QEvent*) new QFocusEvent(tmpFocusEvent->type());
      ((QFocusEvent*)newEvent)->setReason(tmpFocusEvent->reason());
      processed = true;
      break;
    }

    case QEvent::KeyPress:
    case QEvent::KeyRelease: {
      AWT_DBG(printf("Event Key: srcIdx=%d\n", X->srcIdx));
      QKeyEvent* tmpKeyEvent = (QKeyEvent*)e;
      newEvent = (QEvent*) new QKeyEvent(tmpKeyEvent->type(),tmpKeyEvent->key(),
	tmpKeyEvent->ascii(), tmpKeyEvent->state(), tmpKeyEvent->text(),
	tmpKeyEvent->isAutoRepeat(), tmpKeyEvent->count());
      processed = true;
      break;
    }

    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove: {
      AWT_DBG(printf("Event MouseButton: srcIdx=%d\n", X->srcIdx));
      QMouseEvent* tmpMouseEvent = (QMouseEvent*)e;
      newEvent = (QEvent*) new QMouseEvent(tmpMouseEvent->type(),
        tmpMouseEvent->pos(), tmpMouseEvent->button(),
        tmpMouseEvent->state());
      processed = true;
      break;
    }
    case QEvent::Paint: {
      AWT_DBG(printf("Event Paint: srcIdx=%d\n", X->srcIdx));
      QPaintEvent* tmpPaintEvent = (QPaintEvent*)e;
      newEvent = (QEvent*) new QPaintEvent(tmpPaintEvent->rect(),
        tmpPaintEvent->erased());
      processed = true;
      break;
    }
    case QEvent::Move: {
      AWT_DBG(printf("Event Move: srcIdx=%d\n", X->srcIdx));
      QPoint data(((QWidget*)o)->width(), ((QWidget*)o)->height());
      QMoveEvent* tmpMoveEvent = (QMoveEvent*)e;
      // Hide width/height in oldPos of newEvent
      newEvent = (QEvent*) new QMoveEvent(tmpMoveEvent->pos(),
        data);      
      processed = true;
      break;
    }
    case QEvent::Resize: {
      AWT_DBG(printf("Event Resize: srcIdx=%d\n", X->srcIdx));
      QSize data(((QWidget*)o)->x(), ((QWidget*)o)->y());
      QResizeEvent* tmpResizeEvent = (QResizeEvent*)e;
      // Hide x/y in oldSize of newEvent
      newEvent = (QEvent*) new QResizeEvent(tmpResizeEvent->size(),
        data);
      processed = true;
      break;
    }
  }

  if(processed == true) {
    packet = new EventPacket(newEvent, getSourceIdx(X, o)/*X->srcIdx*/);
    g_event_queue.enqueue(packet);
  }

  return QWidget::eventFilter(o, e);
}

EventDispatcher* eventDispatcher;

void forwardFocus ( int cmd, void* wnd );  /* from wnd.c */


/*****************************************************************************
 *
 */


/* X-to-Java key modifier mapping
 *                                     altGr   : PC
 *     shift       -    ctrl     alt    meta   : Java
 *     Shift    Lock    Ctrl    Mod1    Mod3   : X      symbol
 *  ----------------------------------------------------------
 *         1       1       2       8       4   : Java   value
 *         1       2       4       8      32   : X
 * ShiftButton    NA ControlButton  AltButton  : QT
 */

static inline int keyMod ( int keyState )
{
  int mod = 0;
  if ( keyState & Qt::ShiftButton )   mod |= 1;
  if ( keyState & Qt::ControlButton ) mod |= 2;
  if ( keyState & Qt::AltButton )     mod |= 8;
  return mod;
}

static inline int mapButton( int button )
{
  if ( button & Qt::LeftButton) return 1;
  if ( button & Qt::RightButton) return 3;
  if ( button & Qt::MidButton) return 2;
  return 1;
}   

#if defined(KAFFE_VMDEBUG)
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
#if 0
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

  return env->CallStaticObjectMethod( KeyEvent, getKeyEvent,
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
}


static jobject
mouseNotify ( JNIEnv* env, Toolkit* X )
{
  X->evtId = (X->event.xany.type == EnterNotify) ? MOUSE_ENTERED : MOUSE_EXITED;

  return env->CallStaticObjectMethod(  MouseEvent, getMouseEvent,
    X->srcIdx, X->evtId,
    0, X->event.xcrossing.x, X->event.xcrossing.y);
}


static jobject
focusNotify ( JNIEnv* env, Toolkit* X )
{
  return 0;
#if 0
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
	return env->CallStaticObjectMethod( FocusEvent, getFocusEvent, idx,
										   X->evtId, JNI_FALSE);
  }
  else {
	return 0;
  }
#endif
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

  return env->CallStaticObjectMethod( WMEvent, getWMEvent,
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
	return env->CallStaticObjectMethod( WindowEvent, getWindowEvent,
		   X->srcIdx, id);
  }
  else {
	// we do the ComponentEvent show/hide in Java
	return 0;
  }
}

static jobject
clientMessage ( JNIEnv* env, Toolkit* X )
{
  return 0;
#if 0
  if ( X->windows[X->srcIdx].flags & WND_DESTROYED ){
	/* we lost him, Jim */
	return 0;
  }

  if ( X->event.xclient.message_type == WM_PROTOCOLS ) {
	if ( X->event.xclient.data.l[0] == WM_DELETE_WINDOW ) {
	  return env->CallStaticObjectMethod( WindowEvent, getWindowEvent,
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
		return env->CallStaticObjectMethod( FocusEvent, getFocusEvent,
											   X->srcIdx, FOCUS_GAINED, JNI_TRUE);
	  }
	  else {
		return 0;
	  }

	case FWD_CLEAR:
	  DBG( AWT_EVT, printf("FWD_CLEAR: %lx (%d) %lx\n", X->event.xany.window, X->srcIdx, X->windows[X->srcIdx].owner));

	  if ( X->fwdIdx >= 0 ) {
		resetFocusForwarding( X);
		return env->CallStaticObjectMethod( FocusEvent, getFocusEvent,
											   X->srcIdx, FOCUS_LOST, JNI_FALSE);
	  }
	  else {
		return 0;
	  }

	case FWD_REVERT:
	  DBG( AWT_EVT, printf("FWD_REVERT: %lx\n", X->event.xany.window));
	  if ( X->event.xany.window == X->focus ) {
		resetFocusForwarding( X);
		return env->CallStaticObjectMethod( FocusEvent, getFocusEvent,
											   X->srcIdx, FOCUS_GAINED, JNI_FALSE);
	  }
	}
  }
  return 0;
#endif
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
		clazz = env->FindClass( "java/awt/Dialog");
		setDecoInsets = env->GetStaticMethodID( clazz, "setDecoInsets","(IIIII)V");
	  }
	  in->guess = 0;
	}
	else if ( X->frameInsets.guess ) {
	  in = &(X->frameInsets);
	  if ( (left != in->left) || (top != in->top) ||
		   (right != in->right) || (bottom != in->bottom) ){
		clazz = env->FindClass( "java/awt/Frame");
		setDecoInsets = env->GetStaticMethodID( clazz, "setDecoInsets","(IIIII)V");
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

	  env->CallStaticVoidMethod( clazz, setDecoInsets, 
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
  keyNotify,            /* KeyPress                 2 */
  keyNotify,            /* KeyRelease               3 */
  mouseNotify,          /* EnterNotify              7 */
  mouseNotify,          /* LeaveNotify              8 */
  focusNotify,          /* FocusIn                  9 */
  focusNotify,          /* FocusOut                10 */
  destroyNotify,        /* DestroyNotify           17 */
  mapNotify,            /* UnmapNotify             18 */
  mapNotify,            /* MapNotify               19 */
  reparentNotify,       /* ReparentNotify          21 */
  selectionClear,       /* SelectionClear          29 */
  selectionRequest,     /* SelectionRequest        30 */
  clientMessage,        /* ClientMessage           33 */
};
#endif

/*******************************************************************************
 *
 */

jobject Java_java_awt_Toolkit_evtInit(JNIEnv* env, jclass clazz)
{
  jclass Component;

  AWT_DBG(printf("evtInit\n"));

  if ( ComponentEvent != NULL ){
    AWT_DBG(printf("evtInit called twice\n"));
    return NULL;
  }

  if(qapp == NULL) {
    AWT_DBG(printf("evtInit: qapp not initialized!\n"));
  }

  eventDispatcher = new EventDispatcher();
  qapp->installEventFilter(eventDispatcher);

  ComponentEvent = env->FindClass("java/awt/ComponentEvt");
  MouseEvent     = env->FindClass("java/awt/MouseEvt");
  FocusEvent     = env->FindClass("java/awt/FocusEvt");
  WindowEvent    = env->FindClass("java/awt/WindowEvt");
  KeyEvent       = env->FindClass("java/awt/KeyEvt");
  PaintEvent     = env->FindClass("java/awt/PaintEvt");
  WMEvent        = env->FindClass("java/awt/WMEvent");

  getComponentEvent = env->GetStaticMethodID( ComponentEvent, "getEvent", 
												 "(IIIIII)Ljava/awt/ComponentEvt;");
  getMouseEvent     = env->GetStaticMethodID( MouseEvent, "getEvent",
												 "(IIIII)Ljava/awt/MouseEvt;");
  getFocusEvent     = env->GetStaticMethodID( FocusEvent, "getEvent",
												 "(IIZ)Ljava/awt/FocusEvt;");
  getWindowEvent    = env->GetStaticMethodID( WindowEvent, "getEvent",
												 "(II)Ljava/awt/WindowEvt;");
  getKeyEvent       = env->GetStaticMethodID( KeyEvent, "getEvent",
												 "(IIIII)Ljava/awt/KeyEvt;");
  getPaintEvent     = env->GetStaticMethodID( PaintEvent, "getEvent",
												 "(IIIIII)Ljava/awt/PaintEvt;");
  getWMEvent        = env->GetStaticMethodID( WMEvent, "getEvent",
                              "(II)Ljava/awt/WMEvent;");

  Component = env->FindClass( "java/awt/Component");
  return env->NewObjectArray( X->nWindows, Component, NULL);
}

jobject processEvent(JNIEnv* env, Toolkit* X, QEvent* event, int index)
{

  switch(event->type()) {
    case QEvent::Clipboard: {
      return clearJavaClipboard(env);
    }
    
    case QEvent::Show:
    case QEvent::Hide: {
      int id = 0;

      if (event->type() == QEvent::Show ) {
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
	return env->CallStaticObjectMethod( WindowEvent, getWindowEvent,
		   X->srcIdx, id);
      }
      else {
	// we do the ComponentEvent show/hide in Java
	return 0;
      }
    }

      

    case QEvent::Destroy: {
      AWT_DBG(printf("processing Destroy: SrcIdx=%d\n", index));
      /*
       * We should get this just for windows which have been destroyed from an
       * external client, since removeNotify() calls evtUnregisterSource() (i.e.
       * removes windows properly from the dispatch table)
       */
      X->windows[X->srcIdx].flags &= ~WND_MAPPED;

      return env->CallStaticObjectMethod( WMEvent, getWMEvent,
        	                 index, WM_KILLED);

    }
    
    case QEvent::Reparent: {
      AWT_DBG(printf("processing Reparent: SrcIdx=%d\n", index));
      return NULL;
    }

    case QEvent::FocusIn:
    case QEvent::FocusOut: {
      AWT_DBG(printf("processing %s: SrcIdx=%d\n",(event->type() == QEvent::FocusIn)?"FocusIn":"FocusOut",index));
      int evtId;
      int et = event->type();
      if ( et == QEvent::FocusIn ) {
	evtId = FOCUS_GAINED;

      }
      else {
	evtId = FOCUS_LOST;
      }

      if (checkSource( X, index) ){
	return env->CallStaticObjectMethod( FocusEvent, getFocusEvent, 
                             index, evtId, JNI_FALSE);
      }
      else {
	return NULL;
      }
    }

    case QEvent::KeyPress:
    case QEvent::KeyRelease: { 
      AWT_DBG(printf("processing %s: SrcIdx=%d\n",(event->type() == QEvent::KeyPress)?"KeyPress":"KeyRelease",index));
      QKeyEvent *keyEvent = (QKeyEvent*)event;
      int             n, keyCode, keyChar, mod;
      
      /*
       * We should eventually support input methods here.
       * Note that 'keysym' is queried separately (with a standard state), to
       * ensure the "one physical key -> one keycode" invariant
      */
      int keysym;
      n = keyEvent->count();
      keysym = keyEvent->key();

      if ( (keysym >= 0x1000) || (n == 0) ) { 
        keyCode = QFKeyCode[keysym & 0xff];
        /*
         * There are some "control keys" that should generate KEY_TYPED events
         * (enter, cancel, backspace, del, tab). This is flagged by a negative
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
   	keyChar = keyEvent->ascii();//(unsigned char)X->buf[0];
   	keyCode = QLKeyCode[keysym & 0xff];
      }


      int evtId = (((QEvent*)keyEvent)->type() == QEvent::KeyPress)? KEY_PRESSED : KEY_RELEASED;
      mod = keyMod(keyEvent->state());
      
      AWT_DBG(printf("KeyEvent: idx=%d keyCode=%d keyChar=%c mod=%d\n", index, keyCode,keyChar,mod));

      return env->CallStaticObjectMethod( KeyEvent, getKeyEvent,
                                          index, evtId, keyCode, keyChar, mod);

    }
    
    case QEvent::MouseButtonPress: {
      AWT_DBG(printf("processing MouseButtonPress Event SrcIdx=%d\n", index));
      QMouseEvent* mouseEvent = (QMouseEvent*)event;
      // check the diff between event.xbutton.button and QMouseEvent::button()
      if (checkSource( X, index) ){
	env->CallStaticObjectMethod( FocusEvent, getFocusEvent, 
                             index, FOCUS_GAINED, JNI_FALSE);
      }
      else {
	return NULL;
      }
      return env->CallStaticObjectMethod(MouseEvent, getMouseEvent,
        index, MOUSE_PRESSED, mapButton(mouseEvent->button()),
        mouseEvent->x(), mouseEvent->y());
    }
    case QEvent::MouseButtonRelease: {
      AWT_DBG(printf("processing MouseButtonRelease Event SrcIdx=%d\n", index));
      QMouseEvent* mouseEvent = (QMouseEvent*)event;
      // check the diff between event.xbutton.button and QMouseEvent::button()
      return env->CallStaticObjectMethod(MouseEvent, getMouseEvent,
        index, MOUSE_RELEASED, mapButton(mouseEvent->button()),
        mouseEvent->x(), mouseEvent->y());
    }
    case QEvent::MouseMove: {
      AWT_DBG(printf("processing MouseMove Event SrcIdx=%d\n", index));
      QMouseEvent* mouseEvent = (QMouseEvent*)event;
      return env->CallStaticObjectMethod(MouseEvent, getMouseEvent,
        index, MOUSE_MOVED, 0,
        mouseEvent->x(), mouseEvent->y());
    }
    case QEvent::Paint: {
      AWT_DBG(printf("processing Paint Event SrcIdx=%d\n", index));
      QPaintEvent* paintEvent = (QPaintEvent*)event;
      QRect rect=paintEvent->rect();
      return env->CallStaticObjectMethod(PaintEvent, getPaintEvent,
        index, UPDATE, rect.x(), rect.y(), rect.width(), rect.height());
    }
    case QEvent::Move: {
      AWT_DBG(printf("processing Move Event SrcIdx=%d\n", index));
      QMoveEvent* moveEvent = (QMoveEvent*)event;
      QPoint pos, data;
      pos = moveEvent->pos();
      data = moveEvent->oldPos();
      return env->CallStaticObjectMethod( ComponentEvent, getComponentEvent,
        index, COMPONENT_RESIZED, pos.x(), pos.y(), data.x(), data.y());
    }
    case QEvent::Resize: {
      AWT_DBG(printf("processing Resize Event SrcIdx=%d\n", index));
      QResizeEvent* resizeEvent = (QResizeEvent*)event;
      QSize size, data;
      size = resizeEvent->size();
      data = resizeEvent->oldSize();
      return env->CallStaticObjectMethod( ComponentEvent, getComponentEvent,
        index, COMPONENT_RESIZED, data.width(), data.height(),
        size.width(), size.height());
    }
  }
  return NULL;
}

jobject Java_java_awt_Toolkit_evtGetNextEvent(JNIEnv* env, jclass clazz)
{
  jobject jEvt = NULL;
  EventPacket *packet;
  QEvent *event;
  int index;

  //AWT_DBG(printf("getNextEvent..\n"));
  pollJavaClipboard(env);

  if (g_event_queue.count()) {
    packet=g_event_queue.dequeue();
    event=packet->getEvent();
    index=packet->getIndex();
    jEvt = processEvent(env, X, event, index);
    delete event;
    delete packet;
  }

//  qapp->processOneEvent();
  qapp->processEvents(10);

  return jEvt;
}

jobject
Java_java_awt_Toolkit_evtPeekEvent ( JNIEnv* env, jclass clazz )
{
  jobject jEvt = NULL;

  AWT_DBG(printf("peekEvent..\n"));
#if 0
  if ( nextEvent( env, clazz, X, False) && ((getSourceIdx( X, (void*)X->event.xany.window) >= 0)) ) {
	if ( (jEvt = (processEvent[X->event.xany.type])( env, X)) )
	  X->preFetched = 1;
  }

  DBG( AWT_EVT, printf("..peekEvent: %s %p, %lx\n", eventStr(X->evtId), jEvt, X->event.xany.window));
#endif
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
void Java_java_awt_Toolkit_evtWakeup(JNIEnv* env, jclass clazz)
{
//  XEvent event;

  AWT_DBG(printf("evtWakeup\n"));

/*
  event.xclient.type = ClientMessage; 
  event.xclient.message_type = WAKEUP;
  event.xclient.format = 8;
  event.xclient.window = X->wakeUp;

  XSendEvent( X->dsp, X->wakeUp, False, 0, &event);
  XFlush( X->dsp);
*/
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


jint Java_java_awt_Toolkit_evtRegisterSource(JNIEnv* env, jclass clazz,
  void* wnd)
{
  /*
   * We have done that already during the various wndCreateXX()
   */
  int i = getSourceIdx( X, wnd);

  AWT_DBG(printf("registerSource( %p) -> %d\n", wnd, i));

  return i;
}


jint Java_java_awt_Toolkit_evtUnregisterSource(JNIEnv* env, jclass clazz,
  void* wnd)
{
  int i = getSourceIdx( X, wnd);

  if ( i >= 0 ){
	X->windows[i].w = (void*)0xffffffff;
	X->windows[i].flags = 0;
	X->windows[i].owner = 0;
  }

  if ( X->lastWindow == wnd )
	X->lastWindow = 0;

  AWT_DBG(printf("unregisterSource( %p) -> %d\n", wnd, i));

  return i;
}

