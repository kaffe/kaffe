/**
 * wnd.c - native toplevel window related functions
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2002, 2003, 2004
 *	Kaffe.org contributors, see ChangeLog for details.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include <limits.h>

#ifdef QPE
#  include <qpe/qpeapplication.h>
#else
#  include <qapplication.h>
#endif

#include <qframe.h>
#include <qcursor.h>
#include <qwidget.h>

#include "toolkit.h"

#define QCOLOR(c) QColor(JRED(c), JGREEN(c), JBLUE(c))

#ifdef QPE
extern QPEApplication *qapp;
#else
extern QApplication *qapp;
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
}

static void 
retryFocus ( void* wnd, void* owner, int count )
{
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
  DBG(AWT_WND, qqDebug("wndSetTitle\n"));
  if ( jTitle ) {
    char *buf = java2CString( env, X, jTitle);
    ((QWidget*)wnd)->setCaption(QString(buf));
  }
}


void Java_java_awt_Toolkit_wndSetResizable(JNIEnv* env, jclass clazz,
  void* wnd, jboolean isResizable, int x, int y, int width, int height)
{
  DBG(AWT_WND, qqDebug("wndSetResizable\n"));
  int min_width, max_width;
  int min_height, max_height;

  DBG(AWT_WND, qqDebug("setResizable: %p, %d, %d,%d,%d,%d\n",
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
	DBG(AWT_EVT, qqDebug("window table out of space: %d", X->nWindows));
	return -1;
  }
}


void* Java_java_awt_Toolkit_wndCreateFrame(JNIEnv* env, jclass clazz,
  jstring jTitle, jint x, jint y, jint width, jint height,
  jint jCursor, jint clrBack, jboolean isResizable)
{
  DBG(AWT_WND, qqDebug("wndCreateFrame\n"));
  QFrame *mw = new QFrame();
  DBG(AWT_WND, qqDebug("createFrame( %p, %d,%d,%d,%d,..) -> %p\n", 
    jTitle, x, y, width, height, mw));

  if ( width <= 0 )  width = 1;
  if ( height <= 0 ) height = 1;

  mw->setGeometry(x, y, width, height);

  ((QWidget *)mw)->setCursor(QCursor(getCursor(jCursor)));

  mw->setBackgroundColor(QCOLOR(clrBack));

  Java_java_awt_Toolkit_wndSetResizable( env, clazz, (void*)mw,
    isResizable, x, y, width, height);

  Java_java_awt_Toolkit_wndSetTitle(env, clazz, (void*)mw, jTitle);

  int idx = registerSource( X, (QWidget*)mw, 0, WND_FRAME);
  DBG(AWT_WND, qqDebug("registerSource: mw=%p idx=%d\n",mw,idx));
  return (void*)mw;
}


void* Java_java_awt_Toolkit_wndCreateWindow(JNIEnv* env, jclass clazz,
  void* owner, jint x, jint y, jint width, jint height,
  jint jCursor, jint clrBack )
{
  DBG(AWT_WND, qqDebug("wndCreateWindow\n"));

  QFrame* mw = new QFrame((QWidget*)owner);
  mw->setFrameStyle(QFrame::Panel|QFrame::Raised);
  QPoint p = mw->mapToParent(mw->mapFromGlobal(QPoint(x,y)));
  x = p.x();
  y = p.y();
  // Owner

  DBG(AWT_WND, qqDebug("createWindow( %p, %d,%d,%d,%d,..) -> %p\n", 
          owner,x,y,width,height, mw));

  if ( width <= 0 )  width = 1;
  if ( height <= 0 ) height = 1;

  mw->setGeometry(x, y, width, height);

  ((QWidget *)mw)->setCursor (QCursor(getCursor(jCursor)));

  mw->setBackgroundColor(QCOLOR(clrBack));

  int idx = registerSource( X, (QWidget*)mw, (QWidget*)owner, WND_WINDOW);
  DBG(AWT_WND, qqDebug("registerSource: mw=%p idx=%d\n",mw,idx));
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

  DBG(AWT_WND, qqDebug("wndCreateDialog\n"));
  QFrame* mw = new QFrame();
  mw->setFrameStyle(QFrame::Panel|QFrame::Raised);
  QPoint p = mw->mapToParent(mw->mapFromGlobal(QPoint(x,y)));
  x = p.x();
  y = p.y();

  // Owner

  DBG(AWT_WND, qqDebug("createDialog( %p,%p, %d,%d,%d,%d,..) -> %p\n", 
    owner, jTitle, x, y, width, height, mw));

  if ( width <= 0 )  width = 1;
  if ( height <= 0 ) height = 1;

  mw->setGeometry(x, y, width, height);

  ((QWidget *)mw)->setCursor(QCursor( getCursor(jCursor)));

  mw->setBackgroundColor(QCOLOR(clrBack));

  Java_java_awt_Toolkit_wndSetResizable( env, clazz, (void*)mw,
    isResizable, x, y, width, height);

  Java_java_awt_Toolkit_wndSetTitle(env, clazz, (void*)mw, jTitle);

  int idx = registerSource( X, (QWidget*)mw, (QWidget*)owner, WND_DIALOG);
  DBG(AWT_WND, qqDebug("registerSource: mw=%p idx=%d\n",mw,idx));
  return (void*)mw;
}


void Java_java_awt_Toolkit_wndDestroyWindow(JNIEnv* env, jclass clazz,
  void* wnd)
{
  int i = getSourceIdx( X, wnd);

  DBG(AWT_WND, qqDebug("destroy window: %p (%d)\n", wnd, i));

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
  }
}


void Java_java_awt_Toolkit_wndRequestFocus(JNIEnv* env, jclass clazz,
  void* wnd)
{
  int i = getSourceIdx( X, wnd);

  DBG(AWT_WND, qqDebug("request focus: %lx (%d)\n", wnd, i));

  if ( (i < 0) || (X->windows[i].flags & WND_DESTROYED) )
    return;

  ((QWidget*)wnd)->setActiveWindow();
  ((QWidget*)wnd)->raise();
  ((QWidget*)wnd)->setFocus();
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
  DBG(AWT_WND, qqDebug("setBounds: %p %d,%d,%d,%d\n", wnd, x, y,
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
  DBG(AWT_WND, qqDebug("wndRepaint: %lx %d,%d,%d,%d\n", wnd, x, y, width, height));

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

  DBG(AWT_WND, qqDebug("setVisible: %lx (%d) %d\n", wnd, i, showIt));

  if ( (i < 0) || (X->windows[i].flags & WND_DESTROYED) )
    return;

  owner = X->windows[i].owner;

  if ( showIt ){
    X->windows[i].flags |= WND_MAPPED;
    ((QWidget*)wnd)->show();
    ((QWidget*)wnd)->setActiveWindow();
    ((QWidget*)wnd)->raise();
    ((QWidget*)wnd)->repaint();
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
  }
}


void Java_java_awt_Toolkit_wndToBack(JNIEnv* env, jclass clazz, void* wnd)
{
  DBG(AWT_WND, qqDebug("toBack: %p\n", wnd));
  ((QWidget*)wnd)->lower();
}


void Java_java_awt_Toolkit_wndToFront(JNIEnv* env, jclass clazz, void* wnd)
{
  DBG(AWT_WND, qqDebug("toFront: %p\n", wnd));
  ((QWidget*)wnd)->raise();
}


void Java_java_awt_Toolkit_wndSetCursor(JNIEnv* env, jclass clazz,
  void* wnd, jint jCursor)
{
  DBG(AWT_WND, qqDebug("setCursor: %lx, %d\n", (QWidget *)wnd, jCursor));
  ((QWidget*)wnd)->setCursor(QCursor( getCursor(jCursor)));
}
