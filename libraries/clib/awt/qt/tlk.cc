/**
 * tlk.c - 
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

/**
 * exported functions
 */

jint Java_java_awt_Toolkit_tlkProperties(JNIEnv* env, jclass clazz)
{
  DBG(AWT, qqDebug("tlkProperties\n"));
  jint props = TLK_EXTERNAL_DECO;

  // take out the flags TLK_IS_BLOCKING & TLK_NEEDS_FLUSH from props

  return props;
}


void qtawtMessageOutput(QtMsgType type, const char *msg)
{
  switch ( type ) {
    case QtDebugMsg:
      fprintf(stderr, "QtAWT: %s\n", msg);
      break;
    case QtWarningMsg:
      fprintf(stderr, "QtAWT - Warning: %s\n", msg);
      break;
    case QtFatalMsg:
      fprintf(stderr, "QtAWT - Fatal: %s\n", msg);
      abort();		// dump core on purpose
  }
}

// tlkInit(System.getProperty( "awt.display")); in Toolkit.java
jboolean Java_java_awt_Toolkit_tlkInit(JNIEnv* env, jclass clazz,
  jstring name)
{
  char * argv[1] = { "Qt AWT backend for Kaffe" };
  int argc = 1;           

  qInstallMsgHandler(qtawtMessageOutput);

#ifdef QPE
  qapp = new QPEApplication(argc, argv);
#else
  qapp = new QApplication(argc, argv, QApplication::GuiServer);
#endif  
  DBG(AWT, qqDebug("qapp initialization.\n"));

  // allocate X->buf for string opeartion
  getBuffer(X, 128);

  JniEnv = env;
  AWTError = env->FindClass( "java/awt/AWTError");
  X->nWindows = 47;
  X->windows = (WindowRec*) AWT_CALLOC( X->nWindows, sizeof(WindowRec));

  X->fwdIdx = -1;
  DBG(AWT, qqDebug("tlkInit Done.\n"));
  return JNI_TRUE;
}

void Java_java_awt_Toolkit_tlkTerminate(JNIEnv* env, jclass clazz)
{
  if ( qapp ) {
    qapp->closeAllWindows();
    delete qapp;
    qapp = NULL;
  }
}


jstring Java_java_awt_Toolkit_tlkVersion(JNIEnv* env, jclass clazz)
{
  DBG(AWT, qqDebug("tlkVersion\n"));
  return env->NewStringUTF( "QtAWT-1.0");
}

jint Java_java_awt_Toolkit_tlkGetResolution(JNIEnv* env, jclass clazz)
{
  DBG(AWT, qqDebug("tlkGetResolution\n"));
  /*
   * DPI of screen. It seems that this information will not be used by Java.
   */
  return QPaintDeviceMetrics(qapp->desktop()).logicalDpiX();
}

jint Java_java_awt_Toolkit_tlkGetScreenHeight(JNIEnv* env, jclass clazz)
{
  DBG(AWT, qqDebug("tlkGetScreenHeight\n"));
  return qapp->desktop()->height();
}

jint Java_java_awt_Toolkit_tlkGetScreenWidth(JNIEnv* env, jclass clazz)
{
  DBG(AWT, qqDebug("tlkGetScreenWidth\n"));
  return qapp->desktop()->width();
}

void Java_java_awt_Toolkit_tlkSync(JNIEnv* env, jclass clazz)
{
  DBG(AWT, qqDebug("tlkSync\n"));
  /*
   * We simply let Qt handles synchronization,
   */
}

void Java_java_awt_Toolkit_tlkFlush(JNIEnv* env, jclass clazz)
{
  DBG(AWT, qqDebug("tlkFlush\n"));
}


void Java_java_awt_Toolkit_tlkBeep(JNIEnv* env, jclass clazz)
{
  DBG(AWT, qqDebug("tlkBeep\n"));
  qapp->beep();
}

void Java_java_awt_Toolkit_tlkDisplayBanner(JNIEnv* env, jclass clazz,
  jstring banner )
{
  DBG(AWT, qqDebug("tlkDisplayBanner\n"));
}

