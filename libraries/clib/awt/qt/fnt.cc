/*
 * fnt.c - 
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

#include <qfont.h>
#include <qfontmetrics.h>

#include "config.h"
#include "config-int.h"
#include "toolkit.h"

/**
 * Font support
 */

void*
Java_java_awt_Toolkit_fntInitFont ( JNIEnv* env, jclass clazz, jstring jSpec,
                                    jint style, jint size )
{
  int  i, j, k, i0, i1, j0, j1, di, dj;
  char buf[160];
  char *spec = (char *) env->GetStringChars(jSpec,NULL);
  QFont *qf;
  int weight = QFont::Normal;
  bool italic = FALSE;

  DBG(AWT_FNT, qDebug("fntInitFont...%s\n",spec));

  /*
   * Use values from the java.awt.Font class
   */
  static int boldValue = -1;
  static int italicValue = -1;

  /* 
   * Symbolic lookups are expensive, cache the values
   */
  if (boldValue < 0 || italicValue < 0) {
    jfieldID fieldID;
    jclass theClass;

    theClass = env->FindClass("java.awt.Font");
    fieldID = env->GetStaticFieldID(theClass, "BOLD", "I");
    boldValue = env->GetStaticIntField(theClass, fieldID);

    fieldID = env->GetStaticFieldID(theClass, "ITALIC", "I");
    italicValue = env->GetStaticIntField(theClass, fieldID);
  }

  if (style & boldValue)
    weight = QFont::Bold;
  if (style & italicValue)
    italic = TRUE;

  qf = new QFont(spec, size, weight, italic);
  return (void*) qf;
}

void
Java_java_awt_Toolkit_fntFreeFont ( JNIEnv* env, jclass clazz, QFont* fs )
{
}


/**
 * FontMetrics support
 */

void*
Java_java_awt_Toolkit_fntInitFontMetrics ( JNIEnv* env, jclass clazz, void* fs )
{
  return fs;
}

void
Java_java_awt_Toolkit_fntFreeFontMetrics ( JNIEnv* env, jclass clazz, QFont* fs )
{
}


jint
Java_java_awt_Toolkit_fntGetAscent ( JNIEnv* env, jclass clazz, QFont* fs )
{
  QFontMetrics fm((QFont)(*fs));
  return fm.ascent();
}

jint
Java_java_awt_Toolkit_fntGetDescent ( JNIEnv* env, jclass clazz, QFont* fs )
{
  QFontMetrics fm((QFont)(*fs));
  return fm.descent();
}

jint
Java_java_awt_Toolkit_fntGetFixedWidth ( JNIEnv* env, jclass clazz, QFont* fs )
{

  QFontMetrics fm((QFont)(*fs));
  int w =  ((fm.minRightBearing() - fm.minLeftBearing()) == fm.maxWidth()) ? fm.maxWidth() : 0;
  DBG(AWT_FNT, qqDebug("fntGetFixedWidth %d\n", w));
  return w;
}

jint
Java_java_awt_Toolkit_fntGetHeight ( JNIEnv* env, jclass clazz, QFont* fs )
{
  QFontMetrics fm((QFont)(*fs));
  return fm.ascent() + fm.descent() + 1;
}

jint
Java_java_awt_Toolkit_fntGetLeading ( JNIEnv* env, jclass clazz, QFont* fs )
{
  QFontMetrics fm((QFont)(*fs));
  return fm.leading();
}

jint
Java_java_awt_Toolkit_fntGetMaxAdvance ( JNIEnv* env, jclass clazz, QFont* fs )
{
  QFontMetrics fm((QFont)(*fs));
  return fm.maxWidth();
}

jint
Java_java_awt_Toolkit_fntGetMaxAscent ( JNIEnv* env, jclass clazz, QFont* fs )
{
  QFontMetrics fm((QFont)(*fs));
  return fm.ascent();
}

jint
Java_java_awt_Toolkit_fntGetMaxDescent ( JNIEnv* env, jclass clazz, QFont* fs )
{
  QFontMetrics fm((QFont)(*fs));
  return fm.descent();
}

jboolean
Java_java_awt_Toolkit_fntIsWideFont ( JNIEnv* env, jclass clazz, QFont* fs )
{
  DBG(AWT_FNT, qqDebug("fntIsWideFont\n"));
  return 0;
  // XXX: like: (fs->min_byte1 | fs->max_byte1);
}

jobject
Java_java_awt_Toolkit_fntGetWidths ( JNIEnv* env, jclass clazz, QFont* fs )
{
  int       n = 256;
  jintArray widths;
  jint      *jw;
  jboolean isCopy;
  register  int i, j;
  QFontMetrics fm((QFont)(*fs));
  
  widths = env->NewIntArray( 256);
  jw = env->GetIntArrayElements( widths, &isCopy);

  // XXX: fix
  //no similar QT function to find out indivisual char width
  for( i=0;i<n;i++)
  jw[i] = fm.width(QChar((char)i));
  env->ReleaseIntArrayElements( widths, jw, 0);
  DBG(AWT_FNT, qqDebug("fntGetWidths %d\n",fm.maxWidth()));
  return widths;
}


jint
Java_java_awt_Toolkit_fntBytesWidth ( JNIEnv* env, jclass clazz,
	QFont* fs, jbyteArray jBytes, jint off, jint len )
{
  jboolean  isCopy;
  jbyte    *jb = env->GetByteArrayElements( jBytes, &isCopy);
  int       n = env->GetArrayLength( jBytes);
  int       w;
  QFontMetrics fm((QFont)(*fs));
  if ( off  + len > n ) 
    len = n - off;
  QByteArray a;
  a.setRawData((const char*)((uintptr_t)jb + off), len);
  w = fm.width(QString(a),len);
  env->ReleaseByteArrayElements( jBytes, jb, JNI_ABORT);
  DBG(AWT_FNT, qqDebug("fntBytesWidth %s %d\n",jb + off, w));
  return w;
}

jint
Java_java_awt_Toolkit_fntCharWidth ( JNIEnv* env, jclass clazz, QFont* fs, jchar jChar )
{
#ifndef WORDS_BIGENDIAN
  jChar = (jChar << 8) | (jChar >> 8);
#endif
  QFontMetrics fm((QFont)(*fs));
  DBG(AWT_FNT, qqDebug("fntCharWidth...\n"));

  return fm.width(QChar((ushort)jChar));
}

jint
Java_java_awt_Toolkit_fntCharsWidth ( JNIEnv* env, jclass clazz,
	QFont* fs, jcharArray jChars, jint off, jint len )
{
  jboolean  isCopy;
  jchar    *jc = env->GetCharArrayElements( jChars, &isCopy);
  int      n = env->GetArrayLength( jChars);
  int      w;
  QFontMetrics *fm = new QFontMetrics((QFont) (*fs));
  DBG(AWT_FNT, qDebug("fntCharsWidth...\n"));

  if ( off+len > n ) 
    len = n - off;

/* XXX: Unicode
  ushort *b = (ushort *) (jc + off);
  QChar *unicode = new QChar[len];
  for (int i = 0; i < len; i++)
    unicode[i] = QChar((ushort) *(b + i));
 */

  w = fm->width(QString((new QByteArray())->assign((char*) jc + off, len)));
  env->ReleaseCharArrayElements( jChars, jc, JNI_ABORT);
  delete fm;
  return w;
}

// XXX: implement
jint
Java_java_awt_Toolkit_fntStringWidth ( JNIEnv* env, jclass clazz, QFont* fs, jstring jStr )
{
  jboolean isCopy;
  const jchar *jc = env->GetStringChars(jStr, &isCopy);
  int len = env->GetStringLength(jStr);
  if (len <= 0)
    return 0;

  int w;
/* XXX: Unicode
  ushort *b = (ushort*) jc;
  QChar  *unicode = new QChar[len];
 */
  QFontMetrics *fm = new QFontMetrics((QFont) (*fs));
  w = fm->boundingRect(QString(jchar2CString(X,jc,len))).width();

  DBG(AWT_FNT, qDebug("fntStringWidth %s %d\n",jchar2CString(X,jc,len),w));
  env->ReleaseStringChars( jStr, jc);
  delete fm;
  return w;
}

