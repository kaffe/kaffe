/**
 * clr.c - color management
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "qapplication.h"

#include "toolkit.h"
#include <limits.h>
#include <math.h>

// Murphy
extern QApplication *qapp;

/********************************************************************************
 * auxiliary functions
 */


/********************************************************************************
 * common funcs
 */

void initColorMapping(JNIEnv* env, jclass clazz,  Toolkit* X)
{
//  qapp->setColorSpec(QApplication::NormalColor);
  X->colorMode = QApplication::ManyColor;
}


/********************************************************************************
 * exported functions
 */

jint Java_java_awt_Toolkit_clrGetPixelValue(JNIEnv* env, jclass clazz,
  jint rgb)
{
  /*
   * We do this deferred to avoid class init problems with Defaults<->Color
   * (the notorious DefaultsRGB workaround)
   */
  if ( !X->colorMode )
	initColorMapping( env, clazz, X);

//  uint pixel = QColor(JRED(rgb), JGREEN(rgb), JBLUE(rgb)).pixel();
//  AWT_DBG(printf("clrGetPixelValue: %8x -> %x (%d)\n", rgb, pixel, pixel));

//  return pixel;
  return rgb;
}

void Java_java_awt_Toolkit_clrSetSystemColors(JNIEnv* env, jclass clazz,
  jintArray sysClrs)
{
}

/*
 * Check brighter, darker and Color(long pixRgb) in Color.java
 */
jlong Java_java_awt_Toolkit_clrBright(JNIEnv* env, jclass clazz, jint rgb)
{
  unsigned int r, g, b;
  jint     modRgb;

  r = JRED( rgb);
  g = JGREEN( rgb);
  b = JBLUE( rgb);

  r = (r * 4) / 3;
  g = (g * 4) / 3;
  b = (b * 4) / 3;
	
  if ( r > 0xff ) r = 0xff;
  if ( g > 0xff ) g = 0xff;
  if ( b > 0xff ) b = 0xff;

  modRgb = JRGB( r, g, b);

  return (jlong(modRgb));
}

jlong Java_java_awt_Toolkit_clrDark(JNIEnv* env, jclass clazz, jint rgb)
{
  unsigned int r, g, b;
  jint modRgb;

  r = JRED( rgb);
  g = JGREEN( rgb);
  b = JBLUE( rgb);

  r = (r * 2) / 3;
  g = (g * 2) / 3;
  b = (b * 2) / 3;
	
  modRgb = JRGB( r, g, b);

  return ((jlong)modRgb);
}

jobject Java_java_awt_Toolkit_clrGetColorModel(JNIEnv* env, jclass clazz)
{
  jobject    cm = 0;
  jclass     cmClazz;
  jmethodID  cmCtorId;
//  QColor color;


  if ( !X->colorMode )
	initColorMapping( env, clazz, X);

//  if(color.numBitPlanes()>8) {
    cmClazz = env->FindClass( "java/awt/image/DirectColorModel");
    cmCtorId = env->GetMethodID( cmClazz, "<init>", "(IIIII)V");
    cm = env->NewObject( cmClazz, cmCtorId, 24,
         0xff0000, 0x00ff00, 0x0000ff, 0);
//  } else {
//    cmClazz = env->FindClass( "java/awt/IndexColorModel");
//    cmCtorId = env->GetMethodID( cmClazz, "<init>", "(I[II)V");
//    //rgbs = env->NewIntArray( 256, 0);
//    //cm = env->NewObject( cmClazz, cmCtorId, 8, rgbs, 0);
//  }

  return cm;
}
