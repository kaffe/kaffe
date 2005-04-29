/**
 * evt.c - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2002, 2003, 2004, 2005
 *	Kaffe.org contributors, see ChangeLog for details.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */
#ifdef QPE
#  include <qpe/qpeapplication.h>
#else
#  include <qapplication.h>
#endif
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


#ifdef QPE
extern QPEApplication *qapp;
#else
extern QApplication *qapp;
#endif

QQueue<EventPacket> g_event_queue;

EventDispatcher::EventDispatcher(QWidget *parent, const char *name) {
  if(parent)
    parent->installEventFilter(this);
}

bool EventDispatcher::eventFilter(QObject* o, QEvent* e) {
  DBG(AWT_EVT, qqDebug("event type=%d widget=%p\n", e->type(), o));
  QEvent* newEvent = NULL;
  EventPacket* packet = NULL;
  bool processed = false;

  if (X->srcIdx == 0 || getSourceIdx(X, o) == 0xffffffff)
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
      DBG(AWT_EVT, qqDebug("Event Show: srcIdx=%d\n", X->srcIdx));

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
      DBG(AWT_EVT, qqDebug("Event Hid: srcIdx=%d\n", X->srcIdx));

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
      DBG(AWT_EVT, qqDebug("Event Focus: srcIdx=%d\n", X->srcIdx));
      QFocusEvent* tmpFocusEvent = (QFocusEvent*)e;
      newEvent = (QEvent*) new QFocusEvent(tmpFocusEvent->type());
      ((QFocusEvent*)newEvent)->setReason(tmpFocusEvent->reason());
      processed = true;
      break;
    }

    case QEvent::KeyPress:
    case QEvent::KeyRelease: {
      DBG(AWT_EVT, qqDebug("Event Key: srcIdx=%d\n", X->srcIdx));
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
      DBG(AWT_EVT, qqDebug("Event MouseButton: srcIdx=%d\n", X->srcIdx));
      QMouseEvent* tmpMouseEvent = (QMouseEvent*)e;
      newEvent = (QEvent*) new QMouseEvent(tmpMouseEvent->type(),
        tmpMouseEvent->pos(), tmpMouseEvent->button(),
        tmpMouseEvent->state());
      processed = true;
      break;
    }
    case QEvent::Paint: {
      DBG(AWT_EVT, qqDebug("Event Paint: srcIdx=%d\n", X->srcIdx));
      QPaintEvent* tmpPaintEvent = (QPaintEvent*)e;
      newEvent = (QEvent*) new QPaintEvent(tmpPaintEvent->rect(),
        tmpPaintEvent->erased());
      processed = true;
      break;
    }
    case QEvent::Move: {
      DBG(AWT_EVT, qqDebug("Event Move: srcIdx=%d\n", X->srcIdx));
      QPoint data(((QWidget*)o)->width(), ((QWidget*)o)->height());
      QMoveEvent* tmpMoveEvent = (QMoveEvent*)e;
      // Hide width/height in oldPos of newEvent
      newEvent = (QEvent*) new QMoveEvent(tmpMoveEvent->pos(),
        data);      
      processed = true;
      break;
    }
    case QEvent::Resize: {
      DBG(AWT_EVT, qqDebug("Event Resize: srcIdx=%d\n", X->srcIdx));
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


/**
 * Qt-X-Java key modifier mapping
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

jobject Java_java_awt_Toolkit_evtInit(JNIEnv* env, jclass clazz)
{
  jclass Component;

  DBG(AWT_EVT, qqDebug("evtInit\n"));

  if ( ComponentEvent != NULL ){
    DBG(AWT_EVT, qqDebug("evtInit called twice\n"));
    return NULL;
  }

  if(qapp == NULL) {
    DBG(AWT_EVT, qqDebug("evtInit: qapp not initialized!\n"));
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
      DBG(AWT_EVT, qqDebug("processing Destroy: SrcIdx=%d\n", index));
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
      DBG(AWT_EVT, qqDebug("processing Reparent: SrcIdx=%d\n", index));
      return NULL;
    }

    case QEvent::FocusIn:
    case QEvent::FocusOut: {
      DBG(AWT_EVT, qqDebug("processing %s: SrcIdx=%d\n",(event->type() == QEvent::FocusIn)?"FocusIn":"FocusOut",index));
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
      DBG(AWT_EVT, qqDebug("processing %s: SrcIdx=%d\n",(event->type() == QEvent::KeyPress)?"KeyPress":"KeyRelease",index));
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
      
      DBG(AWT_EVT, qqDebug("KeyEvent: idx=%d keyCode=%d keyChar=%c mod=%d\n", index, keyCode,keyChar,mod));

      return env->CallStaticObjectMethod( KeyEvent, getKeyEvent,
                                          index, evtId, keyCode, keyChar, mod);

    }
    
    case QEvent::MouseButtonPress: {
      DBG(AWT_EVT, qqDebug("processing MouseButtonPress Event SrcIdx=%d\n", index));
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
      DBG(AWT_EVT, qqDebug("processing MouseButtonRelease Event SrcIdx=%d\n", index));
      QMouseEvent* mouseEvent = (QMouseEvent*)event;
      // check the diff between event.xbutton.button and QMouseEvent::button()
      return env->CallStaticObjectMethod(MouseEvent, getMouseEvent,
        index, MOUSE_RELEASED, mapButton(mouseEvent->button()),
        mouseEvent->x(), mouseEvent->y());
    }
    case QEvent::MouseMove: {
      DBG(AWT_EVT, qqDebug("processing MouseMove Event SrcIdx=%d\n", index));
      QMouseEvent* mouseEvent = (QMouseEvent*)event;
      return env->CallStaticObjectMethod(MouseEvent, getMouseEvent,
        index, MOUSE_MOVED, 0,
        mouseEvent->x(), mouseEvent->y());
    }
    case QEvent::Paint: {
      DBG(AWT_EVT, qqDebug("processing Paint Event SrcIdx=%d\n", index));
      QPaintEvent* paintEvent = (QPaintEvent*)event;
      QRect rect=paintEvent->rect();
      return env->CallStaticObjectMethod(PaintEvent, getPaintEvent,
        index, UPDATE, rect.x(), rect.y(), rect.width(), rect.height());
    }
    case QEvent::Move: {
      DBG(AWT_EVT, qqDebug("processing Move Event SrcIdx=%d\n", index));
      QMoveEvent* moveEvent = (QMoveEvent*)event;
      QPoint pos, data;
      pos = moveEvent->pos();
      data = moveEvent->oldPos();
      return env->CallStaticObjectMethod( ComponentEvent, getComponentEvent,
        index, COMPONENT_RESIZED, pos.x(), pos.y(), data.x(), data.y());
    }
    case QEvent::Resize: {
      DBG(AWT_EVT, qqDebug("processing Resize Event SrcIdx=%d\n", index));
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

  DBG(AWT_EVT, qqDebug("getNextEvent..\n"));
  pollJavaClipboard(env);

  if (g_event_queue.count()) {
    packet = g_event_queue.dequeue();
    event = packet->getEvent();
    index = packet->getIndex();
    if (event) {
      jEvt = processEvent(env, X, event, index);
      delete event;
    }
    delete packet;
  }

  qapp->processEvents(10);

  return jEvt;
}

jobject
Java_java_awt_Toolkit_evtPeekEvent ( JNIEnv* env, jclass clazz )
{
  jobject jEvt = NULL;

  DBG(AWT_EVT, qqDebug("peekEvent..\n"));
#if 0
  if ( nextEvent( env, clazz, X, False) && ((getSourceIdx( X, (void*)X->event.xany.window) >= 0)) ) {
	if ( (jEvt = (processEvent[X->event.xany.type])( env, X)) )
	  X->preFetched = 1;
  }

  DBG( AWT_EVT, qFatal("..peekEvent: %s %p, %lx\n", eventStr(X->evtId), jEvt, X->event.xany.window));
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
  DBG(AWT_EVT, qqDebug("evtWakeup\n"));
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

  DBG(AWT_EVT, qqDebug("registerSource( %p) -> %d\n", wnd, i));

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

  DBG(AWT_EVT, qqDebug("unregisterSource( %p) -> %d\n", wnd, i));

  return i;
}

