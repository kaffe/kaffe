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
  DBG(AWT, qDebug("tlkProperties\n"));
  jint props = TLK_EXTERNAL_DECO;

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
  qapp = new QPEApplication(argc, argv);
#else
  qapp = new QApplication(argc, argv, QApplication::GuiServer);
#endif  
  DBG(AWT, qDebug("qapp initialization.\n"));

  // allocate X->buf for string opeartion
  getBuffer(X, 128);

  JniEnv = env;
  AWTError = env->FindClass( "java/awt/AWTError");
  X->nWindows = 47;
  X->windows = (WindowRec*) AWT_CALLOC( X->nWindows, sizeof(WindowRec));

  X->fwdIdx = -1;
  DBG(AWT, qDebug("tlkInit Done.\n"));
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
  DBG(AWT, qDebug("tlkVersion\n"));
  return env->NewStringUTF( "QtAWT-1.0");
}

jint Java_java_awt_Toolkit_tlkGetResolution(JNIEnv* env, jclass clazz)
{
  DBG(AWT, qDebug("tlkGetResolution\n"));
  /*
   * DPI of screen. It seems that this information will not be used by Java.
   */
  return QPaintDeviceMetrics(qapp->desktop()).logicalDpiX();
}

jint Java_java_awt_Toolkit_tlkGetScreenHeight(JNIEnv* env, jclass clazz)
{
  DBG(AWT, qDebug("tlkGetScreenHeight\n"));
  return qapp->desktop()->height();
}

jint Java_java_awt_Toolkit_tlkGetScreenWidth(JNIEnv* env, jclass clazz)
{
  DBG(AWT, qDebug("tlkGetScreenWidth\n"));
  return qapp->desktop()->width();
}

void Java_java_awt_Toolkit_tlkSync(JNIEnv* env, jclass clazz)
{
  DBG(AWT, qDebug("tlkSync\n"));
  /*
   * We simply let Qt handles synchronization,
   */
}

void Java_java_awt_Toolkit_tlkFlush(JNIEnv* env, jclass clazz)
{
  DBG(AWT, qDebug("tlkFlush\n"));
}


void Java_java_awt_Toolkit_tlkBeep(JNIEnv* env, jclass clazz)
{
  DBG(AWT, qDebug("tlkBeep\n"));
  qapp->beep();
}

void Java_java_awt_Toolkit_tlkDisplayBanner(JNIEnv* env, jclass clazz,
  jstring banner )
{
  DBG(AWT, qDebug("tlkDisplayBanner\n"));
}

