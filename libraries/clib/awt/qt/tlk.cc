
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

#ifdef QPE
#  include <qpe/qpeapplication.h>
#else
#  include <qapplication.h>
#endif

#include <qpaintdevicemetrics.h>

#include "toolkit.h"
#include "tlkprops.h"

#ifdef QPE
QPEApplication *qapp;
#else
QApplication *qapp;
#endif

/********************************************************************************
 * exported functions
 */

jint Java_java_awt_Toolkit_tlkProperties(JNIEnv* env, jclass clazz)
{
  fprintf(stderr,"tlkProperties\n");
  jint    props = TLK_EXTERNAL_DECO;

  // take out the flags TLK_IS_BLOCKING & TLK_NEEDS_FLUSH from props

  return props;
}


// tlkInit(System.getProperty( "awt.display")); in Toolkit.java
jboolean Java_java_awt_Toolkit_tlkInit(JNIEnv* env, jclass clazz,
  jstring name)
{
  char * argv[1] = { "Qt AWT backend for Kaffe" };
  int argc = 1;           

#ifdef QPE
  qapp = new QPEApplication( argc, argv);
#else
  qapp = new QApplication( argc, argv, QApplication::GuiServer);
#endif  
  qDebug( "qapp initialization.\n" );

  // allocate X->buf for string opeartion
  getBuffer(X, 128);

  JniEnv = env;
  AWTError = env->FindClass( "java/awt/AWTError");
#if 0
  dspName = ":0.0";

  if ( !(X->dsp = XOpenDisplay( dspName)) ) {
	fprintf(stderr, "XOpenDisplay failed: %s\n", dspName);
	return JNI_FALSE;
  }

  DBG_ACTION(awt, XSynchronize( X->dsp, True));
#endif
  X->nWindows = 47;
  X->windows = (WindowRec*) AWT_CALLOC( X->nWindows, sizeof(WindowRec));

//  X->root   = DefaultRootWindow( X->dsp);
  X->fwdIdx = -1;
#if 0
  WM_PROTOCOLS     = XInternAtom( X->dsp, "WM_PROTOCOLS", False);
  WM_DELETE_WINDOW = XInternAtom( X->dsp, "WM_DELETE_WINDOW", False);

  WM_TAKE_FOCUS    = XInternAtom( X->dsp, "WM_TAKE_FOCUS", False);
  WAKEUP           = XInternAtom( X->dsp, "WAKEUP", False);
  RETRY_FOCUS      = XInternAtom( X->dsp, "RETRY_FOCUS", False);
  FORWARD_FOCUS    = XInternAtom( X->dsp, "FORWARD_FOCUS", False);
#endif
  fprintf(stderr,"tlkInit Done.\n");
  return JNI_TRUE;
}

void Java_java_awt_Toolkit_tlkTerminate(JNIEnv* env, jclass clazz)
{
/*
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
*/

  if ( qapp ) {
    qapp->closeAllWindows();
    delete qapp;
    qapp = NULL;
  }
}


jstring Java_java_awt_Toolkit_tlkVersion(JNIEnv* env, jclass clazz)
{
  fprintf(stderr,"tlkVersion\n");
  return env->NewStringUTF( "QTE-1.0");
}

jint Java_java_awt_Toolkit_tlkGetResolution(JNIEnv* env, jclass clazz)
{
  fprintf(stderr,"tlkGetResolution\n");
  /*
   * DPI of screen. It seems that this information will not be used by Java.
   */
  return QPaintDeviceMetrics(qapp->desktop()).logicalDpiX();
}

jint Java_java_awt_Toolkit_tlkGetScreenHeight(JNIEnv* env, jclass clazz)
{
  fprintf(stderr,"tlkGetScreenHeight\n");
  return qapp->desktop()->height();
}

jint Java_java_awt_Toolkit_tlkGetScreenWidth(JNIEnv* env, jclass clazz)
{
  fprintf(stderr,"tlkGetScreenWidth\n");
  return qapp->desktop()->width();
}

void Java_java_awt_Toolkit_tlkSync(JNIEnv* env, jclass clazz)
{
  fprintf(stderr,"tlkSync\n");
  /*
   * this one flushes the request buffer and waits until all reqs have been processed
   * (a roundtrip for input based on prior requests)
   */
  /* XSync( X->dsp, False); */
}

void Java_java_awt_Toolkit_tlkFlush(JNIEnv* env, jclass clazz)
{
  fprintf(stderr,"tlkFlush\n");
  /* simply flush request buffer (mainly for background threads and blocked AWT) */
  /* XFlush( X->dsp); */
}


void Java_java_awt_Toolkit_tlkBeep(JNIEnv* env, jclass clazz)
{
  fprintf(stderr,"tlkBeep\n");
  qapp->beep();
}

void Java_java_awt_Toolkit_tlkDisplayBanner(JNIEnv* env, jclass clazz,
  jstring banner )
{
  fprintf(stderr,"tlkDisplayBanner\n");
}

