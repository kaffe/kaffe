/**
 * tlk.c - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#define MAIN

#include "toolkit.h"


/********************************************************************************
 * auxiliary functions
 */

int
xErrorHandler ( Display *dsp, XErrorEvent *err )
{
  char buf[128];
  char key[64];

  /*
   * If there are clueless apps using resident Graphics objects in
   * asnyc drawing ops (without checking for window visibility first),
   * we might get BadDrawable errors. Since this is clearly a bug
   * of the app (which has to clean up resident Graphics objects
   * before closing windows), we don't want to add costly checks
   * on every Graphics op (which couldn't be safe, anyway).
   * The only way to handle this safely would be to link Graphics
   * objects to their targets, and this would add the danger of
   * severe memory leaks in case the Graphics objects aren't disposed
   * (for efficiency reasons, we cach Graphics instances).
   * Ignoring BadDrawables is not very nice, but reasonably safe.
   */
  /*
  if ( err->error_code == BadDrawable )
	return 0;
  */

  sprintf( key, "%d", err->error_code);
  XGetErrorDatabaseText( dsp, "XProtoError", key, "", buf, sizeof( buf));

  DBG( awt,("X error:      %s\n", buf));

  sprintf( key, "%d", err->request_code);
  XGetErrorDatabaseText( dsp, "XRequest", key, "", buf, sizeof( buf));

  DBG( awt, ("  request:    %s\n", buf));
  DBG( awt, ("  resource:   %X\n", err->resourceid));

#if 0
  DBG_ACTION( awt, (*JniEnv)->ThrowNew( JniEnv, AWTError, "X error occured"));
#endif

  return 0;
}


/********************************************************************************
 *
 */

jstring
Java_java_awt_Toolkit_tlkVersion ( JNIEnv* env, jclass clazz )
{
  return (*env)->NewStringUTF( env, "X-1.0");
}


/*
 * we do this native because of two reasons: (1) this should appear as fast as
 * possible (without being deferred by potential lengthy Java inits), (2) in
 * native window systems we can't draw to the "root-window" from within Java
 */
Window
displayBanner ( JNIEnv* env, jclass clazz, jstring banner )
{
  return 0;
}


void
Java_java_awt_Toolkit_tlkInit ( JNIEnv* env, jclass clazz, jstring name, jstring banner )
{
  char    *dspName;

  X->nBuf = 128;
  X->buf = KMALLOC( X->nBuf);

  JniEnv = env;
  AWTError = (*env)->FindClass( env, "java/awt/AWTError");

  XSetErrorHandler( xErrorHandler);

  if ( name ) {
	dspName = java2CString( env, X, name);
  }
  else {
	if ( !(dspName = getenv( "DISPLAY")) )
	  dspName = ":0.0";
  }

  if ( !(X->dsp = XOpenDisplay( dspName)) ) {
	fprintf( stderr, "XOpenDisplay failed: %s\n", dspName);
	return;
  }

DBG( awt, ("synchronize X\n"));
DBG_ACTION(awt, XSynchronize( X->dsp, True));

  X->root   = DefaultRootWindow( X->dsp);
  X->banner = displayBanner( env, clazz, banner);

  initColorMapping( env, X);

  WM_PROTOCOLS     = XInternAtom( X->dsp, "WM_PROTOCOLS", False);
  WM_DELETE_WINDOW = XInternAtom( X->dsp, "WM_DELETE_WINDOW", False);

  WM_TAKE_FOCUS    = XInternAtom( X->dsp, "WM_TAKE_FOCUS", False);
  WAKEUP           = XInternAtom( X->dsp, "WAKEUP", False);
  RETRY_FOCUS      = XInternAtom( X->dsp, "RETRY_FOCUS", False);
}

void
Java_java_awt_Toolkit_tlkTerminate ( JNIEnv* env, jclass clazz )
{
  if ( X->cbdOwner ) {
	XDestroyWindow( X->dsp, X->cbdOwner);
	X->cbdOwner = 0;
  }

  if ( X->dsp ){
	XSync( X->dsp, False);
	XCloseDisplay( X->dsp);
	X->dsp = 0;
  }
}


jint
Java_java_awt_Toolkit_tlkGetResolution ( JNIEnv* env, jclass clazz )
{
  /*
   * This is just a guess since WidthMMOfScreen most probably isn't exact.
   * We are interested in pixels per inch
   */
  Screen *scr = DefaultScreenOfDisplay( X->dsp);
  return ((WidthOfScreen( scr) * 254) + 5) / (WidthMMOfScreen( scr) *10);
}


jint
Java_java_awt_Toolkit_tlkGetScreenHeight ( JNIEnv* env, jclass clazz )
{
  return DisplayHeight( X->dsp, DefaultScreen( X->dsp));
}


jint
Java_java_awt_Toolkit_tlkGetScreenWidth ( JNIEnv* env, jclass clazz )
{
  return DisplayWidth( X->dsp, DefaultScreen( X->dsp));
}


jboolean
Java_java_awt_Toolkit_tlkIsBlocking ( JNIEnv* env, jclass clazz )
{
  /*
   * 'isBlocking' means that we suspend the dispatcher thread in
   * evtGetNextEvent() until the next Java event becomes available
   */
#if defined(USE_THREADED_AWT)
  return JNI_TRUE;
#else
  return JNI_FALSE;
#endif
}


jboolean
Java_java_awt_Toolkit_tlkIsDispatchExclusive ( JNIEnv* env, jclass clazz )
{
  /*
   * 'isDispatchExclusive' means that we have a native window system requiring
   * us to create, event-dispatch and destroy windows exclusivly from one thread
   * (the 'dispatcher' thread). The OS/2 presentation manager is an example of such
   * a beast
   */
  return JNI_FALSE;
}

jboolean
Java_java_awt_Toolkit_tlkNeedsFlush ( JNIEnv* env, jclass clazz )
{
  /*
   * 'needsFlush' means we don't execute graphics requests directly, they are
   * queued by the graphics lib in a way that requires frequent
   * queue flushing (e.g. for 'isBlocking' and Xlib). Without this, we have
   * to make sure that a blocked AWT IO doesn't defer auto-flush (e.g. by XNextEvent)
   * until doomsday. The XFLUSH macro (to be called in graphics ops) is a hook for an
   * alternative mechanism (e.g. by "elapsed time since last flush")
   */
#if defined(USE_THREADED_AWT)
  return JNI_TRUE;
#else
  return JNI_FALSE;
#endif
}

void
Java_java_awt_Toolkit_tlkSync ( JNIEnv* env, jclass clazz )
{
  /*
   * this one flushes the request buffer and waits until all reqs have been processed
   * (a roundtrip for input based on prior requests)
   */
  XSync( X->dsp, False);
}

void
Java_java_awt_Toolkit_tlkFlush ( JNIEnv* env, jclass clazz )
{
  /* simply flush request buffer (mainly for background threads and blocked AWT) */
  XFlush( X->dsp);
}


void
Java_java_awt_Toolkit_tlkBeep ( JNIEnv* env, jclass clazz )
{
  XBell( X->dsp, 100);
}
