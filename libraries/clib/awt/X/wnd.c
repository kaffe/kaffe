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
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <X11/Xutil.h>
#include "toolkit.h"


long StdEvents = ExposureMask | KeyPressMask | KeyReleaseMask |
		 PointerMotionMask | /* PointerMotionHintMask | */
		 ButtonPressMask | ButtonReleaseMask | ButtonMotionMask |
		 EnterWindowMask | LeaveWindowMask | StructureNotifyMask |
		 FocusChangeMask | VisibilityChangeMask;


Cursor
getCursor ( jint jCursor )
{
  Cursor cursor;

  if ( jCursor > 13 ) jCursor = 0;

  if ( !(cursor = X->cursors[jCursor]) ){
	int shape;
	switch ( jCursor ) {
	case  0: shape = XC_top_left_arrow; break;       /*  0: DEFAULT_CURSOR     */
	case  1: shape = XC_crosshair; break;            /*  1: CROSSHAIR_CURSOR   */
	case  2: shape = XC_xterm; break;                /*  2: TEXT_CURSOR        */
	case  3: shape = XC_watch; break;                /*  3: WAIT_CURSOR        */
	case  4: shape = XC_bottom_left_corner; break;   /*  4: SW_RESIZE_CURSOR   */
	case  5: shape = XC_bottom_right_corner; break;  /*  5: SE_RESIZE_CURSOR   */
	case  6: shape = XC_top_left_corner; break;      /*  6: NW_RESIZE_CURSOR   */
	case  7: shape = XC_top_right_corner; break;     /*  7: NE_RESIZE_CURSOR   */
	case  8: shape = XC_top_side; break;             /*  8: N_RESIZE_CURSOR    */
	case  9: shape = XC_bottom_side; break;          /*  9: S_RESIZE_CURSOR    */
	case 10: shape = XC_left_side; break;            /* 10: W_RESIZE_CURSOR    */
	case 11: shape = XC_right_side; break;           /* 11: E_RESIZE_CURSOR    */
	case 12: shape = XC_hand2; break;                /* 12: HAND_CURSOR        */
	case 13: shape = XC_fleur; break;                /* 13: MOVE_CURSOR        */
	default: shape = XC_top_left_arrow;
	}

	cursor = X->cursors[jCursor] = XCreateFontCursor( X->dsp, shape);
  }

  return cursor;
}


void
Java_java_awt_Toolkit_wndSetTitle ( JNIEnv* env, jclass clazz, void* wnd, jstring jTitle )
{
  char *buf;

  if ( jTitle ) {
	buf = java2CString( env, X, jTitle);
	XStoreName( X->dsp, (Window)wnd, buf);
  }
}


void
Java_java_awt_Toolkit_wndSetResizable ( JNIEnv* env, jclass clazz, void* wnd, jboolean isResizable,
										int x, int y, int width, int height )
{
  XSizeHints hints;

  if ( !isResizable ) {
	hints.min_width  = hints.max_width = width;
	hints.min_height = hints.max_height = height;
  }
  else {
	hints.min_width = hints.min_height = 0;
	hints.max_width = hints.max_height = INT_MAX;
  }

  hints.flags = PMinSize | PMaxSize;
  XSetWMNormalHints( X->dsp, (Window)wnd, &hints);
}


Window
createWindow ( JNIEnv* env, jclass clazz, Window parent, void* owner, jstring jTitle,
			   jint x, jint y, jint width, jint height,
			   jint jCursor, jint clrBack, jboolean isResizable )
{
  unsigned long          valueMask;
  XSetWindowAttributes   attributes;
  Window                 wnd;
  Atom                   protocol[2];
  int                    i;

  attributes.event_mask = StdEvents;
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

	x      += X->borderWidth;
	y      += X->titleBarHeight;
	width  -= 2 * X->borderWidth;
	height -= X->titleBarHeight + X->bottomBarHeight;
  }

  if ( width <= 0 )  width = 1;
  if ( height <= 0 ) height = 1;

  wnd = XCreateWindow( X->dsp, parent, x, y, width, height, 0,
					   CopyFromParent, InputOutput, CopyFromParent,
					   valueMask, &attributes);

  if ( wnd ) {
	X->newWindow = wnd;

	i=0;
	protocol[i++] = WM_DELETE_WINDOW;
	protocol[i++] = WM_TAKE_FOCUS;
	XSetWMProtocols( X->dsp, wnd, protocol, i);

	if ( owner )
	  XSetTransientForHint( X->dsp, wnd, (Window)owner );

	if ( !isResizable )
	  Java_java_awt_Toolkit_wndSetResizable( env, clazz,
											 (void*)wnd, isResizable, x, y, width, height);

	if ( jTitle )
	  Java_java_awt_Toolkit_wndSetTitle( env, clazz, (void*)wnd, jTitle);

	return wnd;
  }
  else {
	fprintf( stderr, "XCreateFrame failed\n");
	return 0;
  }
}


void*
Java_java_awt_Toolkit_wndCreateFrame ( JNIEnv* env, jclass clazz, jstring jTitle,
									   jint x, jint y, jint width, jint height,
									   jint jCursor, jint clrBack, jboolean isResizable )
{
  return (void*) createWindow( env, clazz, DefaultRootWindow( X->dsp), 0, jTitle,
							   x, y, width, height,
							   jCursor, clrBack, isResizable);
}


void*
Java_java_awt_Toolkit_wndCreateWindow ( JNIEnv* env, jclass clazz, void* owner,
										jint x, jint y, jint width, jint height,
										jint jCursor, jint clrBack )
{
  return (void*) createWindow( env, clazz, X->root, owner, NULL,
							   x, y, width, height,
							   jCursor, clrBack, JNI_TRUE);
}


void*
Java_java_awt_Toolkit_wndCreateDialog ( JNIEnv* env, jclass clazz, void* owner, jstring jTitle,
										jint x, jint y, jint width, jint height,
										jint jCursor, jint clrBack, jboolean isResizable )
{
  return (void*) createWindow( env, clazz, DefaultRootWindow( X->dsp), owner, jTitle,
							   x, y, width, height,
							   jCursor, clrBack, isResizable);
}


void
Java_java_awt_Toolkit_wndDestroyWindow ( JNIEnv* env, jclass clazz, void* wnd )
{
  /* this is just a last defense against multiple destroy requests causing X errors */
  static Window lastDestroyed;

  if ( (Window)wnd != lastDestroyed ) {
	XSync( X->dsp, False); /* maybe we still have pending requests for wnd */
	XDestroyWindow( X->dsp, (Window)wnd);

	lastDestroyed = (Window)wnd;
  }
}


void
Java_java_awt_Toolkit_wndRequestFocus ( JNIEnv* env, jclass clazz, void* wnd )
{
  XEvent event;

  /*
   * If this is the most recently created window, there is a good chance that
   * it is not mapped yet. In this case, we avoid getting annoying BadMatch errors
   * by retrying later
   */
  if ( (Window)wnd == X->newWindow ){
    event.xclient.type = ClientMessage; 
	event.xclient.message_type = RETRY_FOCUS;
	event.xclient.format = 32;
	event.xclient.data.l[0] = 5;
	event.xclient.window = (Window)wnd;

	XSendEvent( X->dsp, (Window)wnd, False, StdEvents, &event);
	XSync( X->dsp, False);
  }
  else
	XSetInputFocus( X->dsp, (Window)wnd, RevertToNone, CurrentTime);
}


void
Java_java_awt_Toolkit_wndSetWindowBounds ( JNIEnv* env, jclass clazz, void* wnd,
										   jint x, jint y, jint width, jint height )
{
  if ( width < 0 )  width = 0;
  if ( height < 0 ) height = 0;
  XMoveResizeWindow( X->dsp, (Window)wnd, x, y, width, height);
}


void
Java_java_awt_Toolkit_wndSetFrameBounds ( JNIEnv* env, jclass clazz, void* wnd,
										  jint x, jint y, jint width, jint height )
{
  /* apply offsets, Frame.setBounds pretends to include decorations */
  x      += X->borderWidth;
  y      += X->titleBarHeight;
  width  -= 2 * X->borderWidth;
  height -= X->titleBarHeight + X->bottomBarHeight;

  XMoveResizeWindow( X->dsp, (Window)wnd, x, y, width, height);
}


void
Java_java_awt_Toolkit_wndSetFrameOffsets ( JNIEnv* env, jclass clazz,
																					 jint titleBarHeight, jint menuBarHeight,
																					 jint bottomBarHeight, jint borderWidth )
{
  X->titleBarHeight  = titleBarHeight;
  X->menuBarHeight   = menuBarHeight;
  X->bottomBarHeight = bottomBarHeight;
  X->borderWidth     = borderWidth;
}


void
Java_java_awt_Toolkit_wndSetIcon ( JNIEnv* env, jclass clazz, void* wnd, void* img )
{
}


void
Java_java_awt_Toolkit_wndSetVisible ( JNIEnv* env, jclass clazz, void* wnd, jboolean showIt )
{
  Window owner = 0;

  if ( showIt ){
	XMapWindow( X->dsp, (Window)wnd);
	XSync( X->dsp, False);

	/*
	 * This is a fix for some WindowManagers with focus problems of decorated
     * transients (e.g. AfterStep). The decoration of the transient has to be
	 * explicitly focused in order to activate the Dialog.
	 */
	XGetTransientForHint( X->dsp, (Window)wnd, &owner);
	if ( owner ) {
	  Window   rWnd, pWnd, *cWnds = 0;
	  int      nChilds;

	  XQueryTree( X->dsp, (Window)wnd, &rWnd, &pWnd, &cWnds, &nChilds);
	  if ( cWnds) XFree( cWnds); /* we aren't interested in descendants */
	  XSetInputFocus( X->dsp, pWnd, RevertToNone, CurrentTime);
	}
  }
  else
	XUnmapWindow( X->dsp, (Window)wnd);
}


void
Java_java_awt_Toolkit_wndToBack ( JNIEnv* env, jclass clazz, void* wnd )
{
  XLowerWindow( X->dsp, (Window)wnd);
}


void
Java_java_awt_Toolkit_wndToFront ( JNIEnv* env, jclass clazz, void* wnd )
{
  XRaiseWindow( X->dsp, (Window)wnd);
}


void
Java_java_awt_Toolkit_wndSetCursor ( JNIEnv* env, jclass clazz, void* wnd, jint jCursor )
{
  XDefineCursor( X->dsp, (Window)wnd, getCursor( jCursor));
}
