/**
 * wnd.c - native toplevel window related functions
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include <limits.h>

#include <qapplication.h>
#include <qframe.h>
#include <qcursor.h>
#include <qwidget.h>

#include "toolkit.h"

#define QCOLOR(c) QColor(JRED(c), JGREEN(c), JBLUE(c))

extern QApplication *qapp;
#if 0
long StdEvents = ExposureMask | KeyPressMask | KeyReleaseMask |
		 PointerMotionMask | /* PointerMotionHintMask | */
		 ButtonPressMask | ButtonReleaseMask | ButtonMotionMask |
		 EnterWindowMask | LeaveWindowMask | StructureNotifyMask |
		 FocusChangeMask | VisibilityChangeMask;

long PopupEvents = ExposureMask |
		 PointerMotionMask | /* PointerMotionHintMask | */
		 ButtonPressMask | ButtonReleaseMask | ButtonMotionMask |
		 EnterWindowMask | LeaveWindowMask | StructureNotifyMask |
		 VisibilityChangeMask;
#endif
/*
 * X dosen't have "owned" popups (menus, choice windows etc.), all it has
 * are 'isTransientFor' properties, and these might not be honored by native
 * window managers in a way that the popup, when getting the focus, doesn't
 * visibly take the focus away from the owner ("shading" its titlebar). The
 * only reliable, non wm-dependent way to prevent this seems to be not to
 * select key events for the popup (i.e. not giving it the focus at all). But
 * we don't want to let this native layer deficiency (of a single platform)
 * show up on the Java side. We implement these temporary popup focus shifts
 * by means of focus forwarding in the native layer, which means we have to
 * take care of propper forwarding/restore during
 * setVisible/requestFocus/destroy, and
 * focus/key events. The owner never gives up the focus with respect to X,
 * but the native layer keeps track of all the involved retargeting of
 * Java events, thus giving Java the illusion of a real focus switch.
 * Since this comes with some "artificial" events we have to process in the
 * Java event queue, we use ClientMessages mapped to the corresponding Java
 * FocusEvents.
 */

/* also used in evt.c */
void forwardFocus ( int cmd, void* wnd )
{
#if 0
  XEvent event;

  event.xclient.type = ClientMessage; 
  event.xclient.message_type = FORWARD_FOCUS;
  event.xclient.format = 32;
  event.xclient.data.l[0] = cmd;
  event.xclient.window = wnd;
	  
  XSendEvent( X->dsp, wnd, False, StdEvents, &event);
#endif
}

static void 
retryFocus ( void* wnd, void* owner, int count )
{
#if 0
  XEvent event;

  event.xclient.type = ClientMessage; 
  event.xclient.message_type = RETRY_FOCUS;
  event.xclient.format = 32;
  event.xclient.data.l[0] = count;
  event.xclient.data.l[1] = owner;
  event.xclient.window = wnd;
	  
  XSendEvent( X->dsp, (Window)wnd, False, StdEvents, &event);
  XSync( X->dsp, False);
#endif
}

static int
getCursor ( jint jCursor )
{
  int cursor;

  if ( jCursor > 13 ) jCursor = 0;

  if ( !(cursor = X->cursors[jCursor]) ){
    QCursorShape shape;
    switch ( jCursor ) {

#if QT_VERSION < 300
      /* Qt2 */
      case  0: shape = ArrowCursor; break;     /*  0: DEFAULT_CURSOR     */
      case  1: shape = CrossCursor; break;     /*  1: CROSSHAIR_CURSOR   */
      case  2: shape = IbeamCursor; break;     /*  2: TEXT_CURSOR        */
      case  3: shape = WaitCursor; break;      /*  3: WAIT_CURSOR        */
      case  4: shape = SizeBDiagCursor; break; /*  4: SW_RESIZE_CURSOR */
      case  5: shape = SizeFDiagCursor; break; /*  5: SE_RESIZE_CURSOR   */
      case  6: shape = SizeFDiagCursor; break; /*  6: NW_RESIZE_CURSOR   */
      case  7: shape = SizeBDiagCursor; break; /*  7: NE_RESIZE_CURSOR   */
      case  8: shape = SizeVerCursor; break;   /*  8: N_RESIZE_CURSOR    */
      case  9: shape = SizeVerCursor; break;   /*  9: S_RESIZE_CURSOR    */
      case 10: shape = SizeHorCursor; break;   /* 10: W_RESIZE_CURSOR    */
      case 11: shape = SizeHorCursor; break;   /* 11: E_RESIZE_CURSOR    */
      case 12: shape = PointingHandCursor; break; /* 12: HAND_CURSOR        */
      case 13: shape = SizeAllCursor; break;   /* 13: MOVE_CURSOR        */
      default: shape = ArrowCursor;
#else
      /* Qt3 */
      case  0: shape = Qt::ArrowCursor; break;     /*  0: DEFAULT_CURSOR     */
      case  1: shape = Qt::CrossCursor; break;     /*  1: CROSSHAIR_CURSOR   */
      case  2: shape = Qt::IbeamCursor; break;     /*  2: TEXT_CURSOR        */
      case  3: shape = Qt::WaitCursor; break;      /*  3: WAIT_CURSOR        */
      case  4: shape = Qt::SizeBDiagCursor; break; /*  4: SW_RESIZE_CURSOR */
      case  5: shape = Qt::SizeFDiagCursor; break; /*  5: SE_RESIZE_CURSOR   */
      case  6: shape = Qt::SizeFDiagCursor; break; /*  6: NW_RESIZE_CURSOR   */
      case  7: shape = Qt::SizeBDiagCursor; break; /*  7: NE_RESIZE_CURSOR   */
      case  8: shape = Qt::SizeVerCursor; break;   /*  8: N_RESIZE_CURSOR    */
      case  9: shape = Qt::SizeVerCursor; break;   /*  9: S_RESIZE_CURSOR    */
      case 10: shape = Qt::SizeHorCursor; break;   /* 10: W_RESIZE_CURSOR    */
      case 11: shape = Qt::SizeHorCursor; break;   /* 11: E_RESIZE_CURSOR    */
      case 12: shape = Qt::PointingHandCursor; break; /* 12: HAND_CURSOR        */
      case 13: shape = Qt::SizeAllCursor; break;   /* 13: MOVE_CURSOR        */
      default: shape = Qt::ArrowCursor;
#endif
    }

    cursor = X->cursors[jCursor] = shape;
  }

  return cursor;
}


void Java_java_awt_Toolkit_wndSetTitle(JNIEnv* env, jclass clazz,
  void* wnd, jstring jTitle )
{
  fprintf(stderr,"wndSetTitle\n");
  if ( jTitle ) {
    char *buf = java2CString( env, X, jTitle);
    ((QWidget*)wnd)->setCaption(QString(buf));
  }
}


void Java_java_awt_Toolkit_wndSetResizable(JNIEnv* env, jclass clazz,
  void* wnd, jboolean isResizable, int x, int y, int width, int height)
{
  fprintf(stderr,"wndSetResizable\n");
  int min_width, max_width;
  int min_height, max_height;

  AWT_DBG(printf("setResizable: %p, %d, %d,%d,%d,%d\n",
    wnd, isResizable, x, y, width, height));

  if ( !isResizable ) {
    min_width = max_width = width;
    min_height = max_height = height;    
  }
  else {
    min_width = min_height = 0;
    max_width = max_height = 32767;
  }

  ((QWidget*)wnd)->setMinimumWidth(min_width);
  ((QWidget*)wnd)->setMaximumWidth(max_width);
  ((QWidget*)wnd)->setMinimumHeight(min_height);
  ((QWidget*)wnd)->setMaximumHeight(max_height);
}

#if 0
static Window
createWindow ( JNIEnv* env, jclass clazz, Window parent, Window owner,
  jstring jTitle, jint x, jint y, jint width, jint height,
  jint jCursor, jint clrBack, jboolean isResizable )
{
  unsigned long          valueMask;
  XSetWindowAttributes   attributes;
  Window                 wnd;
  Atom                   protocol[2];
  int                    i;

  /* note that we don't select on key / focus events for popus
     (owner, no title) */
  attributes.event_mask = (owner && !jTitle) ? PopupEvents :  StdEvents;
  attributes.background_pixel = clrBack;
  attributes.bit_gravity = ForgetGravity;
  attributes.cursor = getCursor( jCursor);
  valueMask = CWEventMask | CWBackPixel | CWBitGravity | CWCursor;

  if ( !jTitle ) {
	attributes.override_redirect = True;
	attributes.save_under = True;
	valueMask |= CWOverrideRedirect | CWSaveUnder;
  }
  else {
	attributes.backing_store = WhenMapped;
	valueMask |= CWBackingStore;
  }

  if ( width <= 0 )  width = 1;
  if ( height <= 0 ) height = 1;

  DBG( AWT_WND, printf("XCreateWindow %d,%d,%d,%d\n", x, y, width, height));
  wnd = XCreateWindow( X->dsp, parent, x, y, width, height, 0,
					   CopyFromParent, InputOutput, CopyFromParent,
					   valueMask, &attributes);
  DBG( AWT_WND, printf(" -> %lx\n", wnd));

  if ( wnd ) {
	i=0;
	protocol[i++] = WM_DELETE_WINDOW;
	protocol[i++] = WM_TAKE_FOCUS;
	XSetWMProtocols( X->dsp, wnd, protocol, i);

	if ( owner ){
	  XSetTransientForHint( X->dsp, wnd, owner ); // ???
	}

	if ( !isResizable )
	  Java_java_awt_Toolkit_wndSetResizable( env, clazz,
			 (void*)wnd, isResizable, x, y, width, height);

	if ( jTitle )
	  Java_java_awt_Toolkit_wndSetTitle( env, clazz, (void*)wnd, jTitle);

	return wnd;
  }
  else {
	return 0;
  }
}
#endif
/*
 * We register here (and not in evt.c, during evtRegisterSource) because
 * we want to be able to store attributes (flags, owner), without being
 * forced to use fancy intermediate cache mechanisms (or intermediate
 * register states). Storing own Window attributes (flags, owners) in
 * the native layer is essential for mechanisms like the popup focus
 * forwarding, but can be used in other places (like inset detection
 * and mapping) as well, to avoid costly round-trips or additional state
 * passed in from the Java side
 */
static int registerSource(Toolkit* X, QWidget* wnd, QWidget* owner,
  int flags)
{
  int i = getFreeSourceIdx( X, wnd);

  if ( i >= 0 ) {
	X->windows[i].w = wnd;
	X->windows[i].flags = flags;
	X->windows[i].owner = owner;
	return i;
  }
  else {
	DBG( AWT_EVT, printf("window table out of space: %d", X->nWindows));
	return -1;
  }
}


void* Java_java_awt_Toolkit_wndCreateFrame(JNIEnv* env, jclass clazz,
  jstring jTitle, jint x, jint y, jint width, jint height,
  jint jCursor, jint clrBack, jboolean isResizable)
{
//  Window w = createWindow( env, clazz, DefaultRootWindow( X->dsp), 0, jTitle,
//	   x, y, width, height, jCursor, clrBack, isResizable);

  fprintf(stderr,"wndCreateFrame\n");
  //QMainWindow* mw = new QMainWindow();
  QFrame *mw = new QFrame();
  AWT_DBG(printf("createFrame( %p, %d,%d,%d,%d,..) -> %p\n", 
    jTitle, x, y, width, height, mw));

  if ( width <= 0 )  width = 1;
  if ( height <= 0 ) height = 1;

  mw->setGeometry(x, y, width, height);

  ((QWidget *)mw)->setCursor(QCursor(getCursor(jCursor)));

  mw->setBackgroundColor(QCOLOR(clrBack));

  Java_java_awt_Toolkit_wndSetResizable( env, clazz, (void*)mw,
    isResizable, x, y, width, height);

  Java_java_awt_Toolkit_wndSetTitle(env, clazz, (void*)mw, jTitle);

  //mw->show(); // this should be removed

  int idx = registerSource( X, (QWidget*)mw, 0, WND_FRAME);
  AWT_DBG(printf("registerSource: mw=%p idx=%d\n",mw,idx));
  return (void*)mw;
}


void* Java_java_awt_Toolkit_wndCreateWindow(JNIEnv* env, jclass clazz,
  void* owner, jint x, jint y, jint width, jint height,
  jint jCursor, jint clrBack )
{
  fprintf(stderr,"wndCreateWindow\n");
/*  Window w = createWindow( env, clazz, X->root, (Window)owner, NULL,
             x, y, width, height, jCursor, clrBack, JNI_TRUE); */

  QFrame* mw = new QFrame((QWidget*)owner);
  mw->setFrameStyle(QFrame::Panel|QFrame::Raised);
  QPoint p = mw->mapToParent(mw->mapFromGlobal(QPoint(x,y)));
  x = p.x();
  y = p.y();
  // Owner

  AWT_DBG(printf("createWindow( %p, %d,%d,%d,%d,..) -> %p\n", 
          owner,x,y,width,height, mw));

  if ( width <= 0 )  width = 1;
  if ( height <= 0 ) height = 1;

  mw->setGeometry(x, y, width, height);

  ((QWidget *)mw)->setCursor (QCursor(getCursor(jCursor)));

  mw->setBackgroundColor(QCOLOR(clrBack));

  //mw->show(); // this should be removed

  int idx = registerSource( X, (QWidget*)mw, (QWidget*)owner, WND_WINDOW);
  AWT_DBG(printf("registerSource: mw=%p idx=%d\n",mw,idx));
  return (void*)mw;
}


void* Java_java_awt_Toolkit_wndCreateDialog(JNIEnv* env, jclass clazz,
  void* owner, jstring jTitle, jint x, jint y, jint width, jint height,
  jint jCursor, jint clrBack, jboolean isResizable )
{
/*
  Window w = createWindow( env, clazz, DefaultRootWindow( X->dsp),
    (Window)owner, jTitle, x, y, width, height,
    jCursor, clrBack, isResizable); */

  fprintf(stderr,"wndCreateDialog\n");
  QFrame* mw = new QFrame();
  mw->setFrameStyle(QFrame::Panel|QFrame::Raised);
  QPoint p = mw->mapToParent(mw->mapFromGlobal(QPoint(x,y)));
  x = p.x();
  y = p.y();

  // Owner

  AWT_DBG( printf("createDialog( %p,%p, %d,%d,%d,%d,..) -> %p\n", 
    owner, jTitle, x, y, width, height, mw));

  if ( width <= 0 )  width = 1;
  if ( height <= 0 ) height = 1;

  mw->setGeometry(x, y, width, height);

  ((QWidget *)mw)->setCursor(QCursor( getCursor(jCursor)));

  mw->setBackgroundColor(QCOLOR(clrBack));

  Java_java_awt_Toolkit_wndSetResizable( env, clazz, (void*)mw,
    isResizable, x, y, width, height);

  Java_java_awt_Toolkit_wndSetTitle(env, clazz, (void*)mw, jTitle);

  //mw->show(); // this should be removed

  int idx = registerSource( X, (QWidget*)mw, (QWidget*)owner, WND_DIALOG);
  AWT_DBG(printf("registerSource: mw=%p idx=%d\n",mw,idx));
  return (void*)mw;
}


void Java_java_awt_Toolkit_wndDestroyWindow(JNIEnv* env, jclass clazz,
  void* wnd)
{
  int i = getSourceIdx( X, wnd);

  AWT_DBG(printf("destroy window: %p (%d)\n", wnd, i));

  if ( (i >= 0) && !(X->windows[i].flags & WND_DESTROYED) ) {
    if ( wnd == X->focusFwd ) {
      /*
       * reset focus forwading NOW, we can't do it from a clientMessage,
       * since any real focusLost notification would be scrubbed by the
       * subsequent XDestroyWindow
       */
      resetFocusForwarding( X);

      if ( X->windows[i].owner && (X->windows[i].owner == X->focus) ) {
        /*
         * This was a explicit dispose(), and the owner still is the real
         * focus window.
         * Let's make it think it got back the focus
         */
        /* forwardFocus( FWD_REVERT, X->windows[i].owner); */
      }
    }

    X->windows[i].flags |= WND_DESTROYED;
    X->windows[i].flags &= ~WND_MAPPED;

    ((QWidget*)wnd)->close(TRUE);
    // XDestroyWindow( X->dsp, wnd);
  }
}


void Java_java_awt_Toolkit_wndRequestFocus(JNIEnv* env, jclass clazz,
  void* wnd)
{
  int i = getSourceIdx( X, wnd);

  AWT_DBG( printf("request focus: %lx (%d)\n", wnd, i));

  if ( (i < 0) || (X->windows[i].flags & WND_DESTROYED) )
    return;

  ((QWidget*)wnd)->setActiveWindow();
  ((QWidget*)wnd)->raise();
  ((QWidget*)wnd)->setFocus();
#if 0
  if ( (X->windows[i].owner) && (X->windows[i].flags & WND_WINDOW) ) {
    if ( X->focus != X->windows[i].owner ) {
      /* if our owner doesn't have it yet, make him the focus window */
      XSetInputFocus( X->dsp, X->windows[i].owner, RevertToParent, CurrentTime);
    }

    /*
     * This marks the beginning of a focus forward to a owned window
     * (which isn't allowed to get the real focus because it would
     * "shade" the titlebar of the owner)
     */
    forwardFocus( FWD_SET, wnd);
  }
  else {
    if ( (X->windows[i].flags & WND_MAPPED) == 0 ){
      /* If it's not mapped yet, try again later. Somebody might
       * have been too fast with requesting the focus of a not yet visible
       * window, resulting in BadMatch errors
       */
      retryFocus( wnd, X->windows[i].owner, 5);
    }
    else if ( (X->focusFwd) && (wnd == X->focus) ) {
      /* We still have it in real life, but we have to pretend we re-gained
       * it from our ownee. Reset forwarding here (instead of in the
       * ClientMessage), because a subsequent destroy of the ownee otherwise
       * might cause another revert (with the Java keyTgtRequest already
       * eaten up by this FWD_REVERT)
       */
      resetFocusForwarding( X);
      forwardFocus( FWD_REVERT, wnd);
    }
    else {
      /* we don't reset X->focusFwd here, that's done in the event handler */
      XSetInputFocus( X->dsp, (Window)wnd, RevertToParent, CurrentTime);
    }
  }
#endif
}


/*
 * We shift possible Frame / Dialog deco adaptions back to Java, because (in
 * order to get more compatible with JDK behavior) we not only have to apply
 * offsets in wndSetWindowBounds, but also when getting back via ComponentEvents
 * (ConfigureNotify), and during recursive child painting (to adapt the toplevel Graphics).
 * Since we don't have the window type info (frame / dialog / window)
 * in the native lib, this has to be done in the Java layer. In order to do it
 * all on one side, this means to shift the size adaption out of wndSetWindowBounds, too.
 *
 * Therefor Default frame/dialog Insets become the pivotal info, being passed into
 * the native lib as a (configurable 'Defaults') guess, turned into exact values
 * (of Frame.frameInsets, Dialog.dialogInsets) by means of native-to-Java callbacks,
 * computed during initial Frame/Dialog creation
 */

void
Java_java_awt_Toolkit_wndSetFrameInsets ( JNIEnv* env, jclass clazz,
										  jint top, jint left, jint bottom, jint right )
{
  X->frameInsets.top  = top;
  X->frameInsets.left = left;
  X->frameInsets.bottom = bottom;
  X->frameInsets.right = right;
  X->frameInsets.guess = 1;
}

void
Java_java_awt_Toolkit_wndSetDialogInsets ( JNIEnv* env, jclass clazz,
										  jint top, jint left, jint bottom, jint right )
{
  X->dialogInsets.top  = top;
  X->dialogInsets.left = left;
  X->dialogInsets.bottom = bottom;
  X->dialogInsets.right = right;
  X->dialogInsets.guess = 1;
}

void Java_java_awt_Toolkit_wndSetBounds(JNIEnv* env, jclass clazz, void* wnd,
  jint x, jint y, jint width, jint height, jboolean isResizable)
{
  AWT_DBG(printf("setBounds: %p %d,%d,%d,%d\n", wnd, x, y,
    width, height));

  if(width < 0) width = 1;
  if(height < 0) height = 1;

  ((QWidget*)wnd)->setGeometry(x, y, width, height);

  /*
   * Our perspective of 'isResizable' is somewhat relaxed: we just disable
   * "user-resizing" (like it is stated in the spec), i.e. a program is
   * allowed to do setBounds on non-resizable Frames/Dialogs. This makes
   * initial decoration size compensation easy, but it requires that we
   * update the "lock" after a resize operation
   */
  if ( !isResizable ) {
    Java_java_awt_Toolkit_wndSetResizable( env, clazz,
      (void*)wnd, FALSE, x, y, width, height);
  }
}

void Java_java_awt_Toolkit_wndRepaint(JNIEnv* env, jclass clazz, void* wnd,
  jint x, jint y, jint width, jint height )
{
  AWT_DBG(printf("wndRepaint: %lx %d,%d,%d,%d\n", wnd, x, y, width, height));

  ((QWidget*)wnd)->repaint(x, y, width, height);  // or use update?
}

void Java_java_awt_Toolkit_wndSetIcon(JNIEnv* env, jclass clazz, void* wnd,
  void* img )
{
}


void Java_java_awt_Toolkit_wndSetVisible(JNIEnv* env, jclass clazz,
  void* wnd, jboolean showIt)
{
  int     i = getSourceIdx( X, wnd);
  void*  owner;

  AWT_DBG( printf("setVisible: %lx (%d) %d\n", wnd, i, showIt));

  if ( (i < 0) || (X->windows[i].flags & WND_DESTROYED) )
    return;

  owner = X->windows[i].owner;

  if ( showIt ){
    X->windows[i].flags |= WND_MAPPED;
    ((QWidget*)wnd)->show();
    ((QWidget*)wnd)->setActiveWindow();
    ((QWidget*)wnd)->raise();
    ((QWidget*)wnd)->repaint();
//    XMapRaised( X->dsp, wnd);
//    XSync( X->dsp, False);

    /*
     * Don't automatically forward the focus for owned popups, the standard
     * JDK behavior is NOT to set the focus on them unless explicitly
     * requested. It would break Swing heavyweight popups (JWindowPopup,
     * both keyboard input and the famous "first on Solaris" jitter)
     */
  }
  else {
    if ( wnd == X->focusFwd ) {
      forwardFocus( FWD_CLEAR, owner);
      forwardFocus( FWD_REVERT, owner);
    }

    X->windows[i].flags &= ~WND_MAPPED;
    ((QWidget*)wnd)->hide();
    // XUnmapWindow( X->dsp, wnd);
    // XSync( X->dsp, False);
  }
}


void Java_java_awt_Toolkit_wndToBack(JNIEnv* env, jclass clazz, void* wnd)
{
  AWT_DBG(printf("toBack: %p\n", wnd));
  ((QWidget*)wnd)->lower();
}


void Java_java_awt_Toolkit_wndToFront(JNIEnv* env, jclass clazz, void* wnd)
{
  AWT_DBG(printf("toFront: %p\n", wnd));
  ((QWidget*)wnd)->raise();
}


void Java_java_awt_Toolkit_wndSetCursor(JNIEnv* env, jclass clazz,
  void* wnd, jint jCursor)
{
  AWT_DBG(printf("setCursor: %lx, %d\n", (QWidget *)wnd, jCursor));
  ((QWidget*)wnd)->setCursor(QCursor( getCursor(jCursor)));
//  XDefineCursor( X->dsp, (Window)wnd, getCursor( jCursor));
}
