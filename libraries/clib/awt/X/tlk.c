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
#include "tlkprops.h"

#ifdef KAFFE_I18N
#include "locale.h"
#endif

/********************************************************************************
 * auxiliary functions
 */

static int
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

  DBG( AWT, printf("X error:      %s\n", buf));

  sprintf( key, "%d", err->request_code);
  XGetErrorDatabaseText( dsp, "XRequest", key, "", buf, sizeof( buf));

  DBG( AWT, printf("  request:    %s\n", buf));
  DBG( AWT, printf("  resource:   %lx\n", err->resourceid));

  /* DBG( AWT, (*JniEnv)->ThrowNew( JniEnv, AWTError, "X error occured")); */

  return 0;
}


/********************************************************************************
 * exported functions
 */

jint 
Java_java_awt_Toolkit_tlkProperties ( JNIEnv* env UNUSED, jclass clazz UNUSED )
{
  jint    props = TLK_EXTERNAL_DECO;

#if !defined(USE_POLLING_AWT)
  props |= TLK_IS_BLOCKING;
  props |= TLK_NEEDS_FLUSH;
#endif

  return props;
}

jboolean
Java_java_awt_Toolkit_tlkInit ( JNIEnv* env UNUSED, jclass clazz UNUSED, jstring name )
{
  const char    *dspName;

#ifdef KAFFE_I18N
  /* for X Output Method */
  int i;
  Bool direction;
  Bool contextdrawing;
  XOMCharSetList cslist;
  XOMOrientation orientation;
#endif
  
  getBuffer(X, 128);

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
	DBG( AWT, printf("XOpenDisplay failed: %s\n", dspName));
	return JNI_FALSE;
  }

#if !defined (USE_POLLING_AWT)
  XInitThreads();
#endif

  DBG( AWT, printf("synchronize X\n"));
  DBG_ACTION(awt, XSynchronize( X->dsp, True));

  X->nWindows = 47;
  X->windows = AWT_CALLOC( (size_t)X->nWindows, sizeof(WindowRec));

  X->root   = DefaultRootWindow( X->dsp);
  X->fwdIdx = -1;

// #if defined(USE_XSHM_EXTENSION)
#if defined(HAVE_LIBXEXT) && defined(KAFFE_I18N)  
  /*
   * We just can use XShm in case we don't run remote, and we better don't rely on
   * XShmQueryExtension to make this distinction
   */
  if ( (dspName[0] == ':') || (strncmp( "localhost", dspName, 9) == 0) ) {
	if ( XShmQueryExtension( X->dsp) ){
	  X->shm =  USE_SHM;
	  /*
	   * threshold shouldn't be much smaller than page size, since this is
	   * usually the smallest amount of sharable memory, anyway
	   */
	  X->shmThreshold = 4096;
	}
  }
#endif

  WM_PROTOCOLS     = XInternAtom( X->dsp, "WM_PROTOCOLS", False);
  WM_DELETE_WINDOW = XInternAtom( X->dsp, "WM_DELETE_WINDOW", False);

  WM_TAKE_FOCUS    = XInternAtom( X->dsp, "WM_TAKE_FOCUS", False);
  WAKEUP           = XInternAtom( X->dsp, "WAKEUP", False);
  RETRY_FOCUS      = XInternAtom( X->dsp, "RETRY_FOCUS", False);
  FORWARD_FOCUS    = XInternAtom( X->dsp, "FORWARD_FOCUS", False);

#ifdef KAFFE_I18N
  /* Open  X Output Method */
  setlocale(LC_ALL ,"");
  
  X->xom = XOpenOM(X->dsp, NULL, NULL, NULL);
  if (X->xom) {
    DBG( AWT, printf("locale of XOM: %s\n", XLocaleOfOM(X->xom)));
    XGetOMValues(X->xom,
		XNRequiredCharSet, &cslist,
		XNQueryOrientation, &orientation,
	        XNDirectionalDependentDrawing, &direction,
	        XNContextualDrawing, &contextdrawing,
	        NULL);
     /* We could create fontset list from these. */
    for (i = 0; i < cslist.charset_count; i++) {
      DBG( AWT, printf("needed charset[%d]: %s\n", i, cslist.charset_list[i]));
    }
  } else {
    DBG( AWT, fprintf(stderr ,"XOpenOM error\n") );
  }
#endif
    
  return JNI_TRUE;
}

void
Java_java_awt_Toolkit_tlkTerminate ( JNIEnv* env UNUSED, jclass clazz UNUSED )
{
  if ( X->cbdOwner ) {
	XDestroyWindow( X->dsp, X->cbdOwner);
	X->cbdOwner = 0;
  }

  if ( X->wakeUp ) {
	XDestroyWindow( X->dsp, X->wakeUp);
	X->wakeUp = 0;
  }

  if ( X->dsp ){
	XSync( X->dsp, False);
	XCloseDisplay( X->dsp);
	X->dsp = 0;
  }
}


jstring
Java_java_awt_Toolkit_tlkVersion ( JNIEnv* env UNUSED, jclass clazz UNUSED )
{
  return (*env)->NewStringUTF( env, "X-1.0");
}

jint
Java_java_awt_Toolkit_tlkGetResolution ( JNIEnv* env UNUSED, jclass clazz UNUSED )
{
  /*
   * This is just a guess since WidthMMOfScreen most probably isn't exact.
   * We are interested in pixels per inch
   */
  Screen *scr = DefaultScreenOfDisplay( X->dsp);
  return ((WidthOfScreen( scr) * 254) + 5) / (WidthMMOfScreen( scr) *10);
}

jint
Java_java_awt_Toolkit_tlkGetScreenHeight ( JNIEnv* env UNUSED, jclass clazz UNUSED )
{
  return DisplayHeight( X->dsp, DefaultScreen( X->dsp));
}

jint
Java_java_awt_Toolkit_tlkGetScreenWidth ( JNIEnv* env UNUSED, jclass clazz UNUSED )
{
  return DisplayWidth( X->dsp, DefaultScreen( X->dsp));
}

void
Java_java_awt_Toolkit_tlkSync ( JNIEnv* env UNUSED, jclass clazz UNUSED )
{
  /*
   * this one flushes the request buffer and waits until all reqs have been processed
   * (a roundtrip for input based on prior requests)
   */
  XSync( X->dsp, False);
}

void
Java_java_awt_Toolkit_tlkFlush ( JNIEnv* env UNUSED, jclass clazz UNUSED )
{
  /* simply flush request buffer (mainly for background threads and blocked AWT) */
  XFlush( X->dsp);
}


void
Java_java_awt_Toolkit_tlkBeep ( JNIEnv* env UNUSED, jclass clazz UNUSED )
{
  XBell( X->dsp, 100);
}

void
Java_java_awt_Toolkit_tlkDisplayBanner ( JNIEnv* env UNUSED, jclass clazz UNUSED, jstring banner UNUSED )
{
}
