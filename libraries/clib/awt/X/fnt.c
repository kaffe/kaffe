/**
 * fnt.c - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */


#include <X11/Xlib.h>
#include <string.h>
#include "config.h"
#include "toolkit.h"

/*******************************************************************************
 * Font support
 */

char* backupFont = "fixed";

void*
Java_java_awt_Toolkit_fntInitFont ( JNIEnv* env, jclass clazz, jstring jSpec, jint style, jint size )
{
  char buf[128];
  char *spec = java2CString( env, X, jSpec);
  char *weight = "medium";
  char *slant  = "r";
  XFontStruct* fs;

  if ( style & 0x1 ) weight = "bold";
  if ( style & 0x2 ) slant  = "i";

  sprintf( buf, spec, weight, slant, size);
  fs = XLoadQueryFont( X->dsp, buf);

  if ( ! fs ){
	fprintf( stderr, "cannot load font: %s (backup to %s)\n", buf, backupFont);
	if ( !(fs = XLoadQueryFont( X->dsp, backupFont)) ) {
	  fprintf( stderr, "font panic, no default font!\n");
	}
  }

  return (void*) fs;
}

void
Java_java_awt_Toolkit_fntFreeFont ( JNIEnv* env, jclass clazz, XFontStruct* fs )
{
  XFreeFont( X->dsp, fs);
}


/*******************************************************************************
 * FontMetrics support
 */

void*
Java_java_awt_Toolkit_fntInitFontMetrics ( JNIEnv* env, jclass clazz, void* fs )
{
  return fs;
}

void
Java_java_awt_Toolkit_fntFreeFontMetrics ( JNIEnv* env, jclass clazz, XFontStruct* fs )
{
}


jint
Java_java_awt_Toolkit_fntGetAscent ( JNIEnv* env, jclass clazz, XFontStruct* fs )
{
  return fs->ascent;
}

jint
Java_java_awt_Toolkit_fntGetDescent ( JNIEnv* env, jclass clazz, XFontStruct* fs )
{
  return fs->descent;
}

jint
Java_java_awt_Toolkit_fntGetFixedWidth ( JNIEnv* env, jclass clazz, XFontStruct* fs )
{
  return (fs->min_bounds.width == fs->max_bounds.width) ? fs->max_bounds.width : 0;
}

jint
Java_java_awt_Toolkit_fntGetHeight ( JNIEnv* env, jclass clazz, XFontStruct* fs )
{
  return fs->ascent + fs->descent;
}

jint
Java_java_awt_Toolkit_fntGetLeading ( JNIEnv* env, jclass clazz, XFontStruct* fs )
{
  return 0;  /* no leading (interline spacing) for X fonts */
}

jint
Java_java_awt_Toolkit_fntGetMaxAdvance ( JNIEnv* env, jclass clazz, XFontStruct* fs )
{
  return fs->max_bounds.width;
}

jint
Java_java_awt_Toolkit_fntGetMaxAscent ( JNIEnv* env, jclass clazz, XFontStruct* fs )
{
  return fs->max_bounds.ascent;
}

jint
Java_java_awt_Toolkit_fntGetMaxDescent ( JNIEnv* env, jclass clazz, XFontStruct* fs )
{
  return fs->max_bounds.descent;
}

jboolean
Java_java_awt_Toolkit_fntIsWideFont ( JNIEnv* env, jclass clazz, XFontStruct* fs )
{
  return (fs->min_byte1 | fs->max_byte1);
}

jobject
Java_java_awt_Toolkit_fntGetWidths ( JNIEnv* env, jclass clazz, XFontStruct* fs )
{
  int       n = 256;
  jarray    widths;
  jint      *jw;
  jboolean isCopy;
  register int i, j;

  widths = (*env)->NewIntArray( env, 256);
  jw = (*env)->GetIntArrayElements( env, widths, &isCopy);

  if ( fs->max_char_or_byte2 < n ) n = fs->max_char_or_byte2;

  if ( fs->min_bounds.width == fs->max_bounds.width ) {
	for ( i=fs->min_char_or_byte2, j=0; i < n; i++, j++ )
	  jw[i] = fs->max_bounds.width;
  }
  else {
	for ( i=fs->min_char_or_byte2, j=0; i < n; i++, j++ )
	  jw[i] = fs->per_char[j].width;
  }

  (*env)->ReleaseIntArrayElements( env, widths, jw, 0);

  return widths;
}


jint
Java_java_awt_Toolkit_fntBytesWidth ( JNIEnv* env, jclass clazz, XFontStruct* fs,
									  jarray jBytes, jint off, jint len )
{
  jboolean  isCopy;
  jbyte    *jb = (*env)->GetByteArrayElements( env, jBytes, &isCopy);
  int       n = (*env)->GetArrayLength( env, jBytes);
  int       w;

  if ( off+len > n ) len = n - off;

  w = XTextWidth( fs, jb+off, len);

  (*env)->ReleaseByteArrayElements( env, jBytes, jb, JNI_ABORT);
  return w;
}

jint
Java_java_awt_Toolkit_fntCharWidth ( JNIEnv* env, jclass clazz, XFontStruct* fs, jchar jChar )
{
#ifndef WORDS_BIGENDIAN
  jChar = (jChar << 8) | (jChar >> 8);
#endif

  return XTextWidth16( fs, (XChar2b*)&jChar, 1);
}

jint
Java_java_awt_Toolkit_fntCharsWidth ( JNIEnv* env, jclass clazz, XFontStruct* fs,
									  jarray jChars, jint off, jint len )
{
  jboolean  isCopy;
  jchar    *jc = (*env)->GetCharArrayElements( env, jChars, &isCopy);
  int      n = (*env)->GetArrayLength( env, jChars);
  XChar2b  *b;
  int      w;

  if ( off+len > n ) len = n - off;

#ifndef WORDS_BIGENDIAN
  n = sizeof(XChar2b)*len;
  b = (XChar2b*) getBuffer( X, n);
  swab( (jc+off), b, n);
#else
  b = (XChar2b*) (jc + off);
#endif

  w = XTextWidth16( fs, b, len);

  (*env)->ReleaseCharArrayElements( env, jChars, jc, JNI_ABORT);
  return w;
}

jint
Java_java_awt_Toolkit_fntStringWidth ( JNIEnv* env, jclass clazz, XFontStruct* fs, jstring jStr )
{
  jboolean isCopy;
  const jchar    *jc = (*env)->GetStringChars( env, jStr, &isCopy);
  int      len = (*env)->GetStringLength( env, jStr);
  int      w;
  XChar2b  *b;

#ifndef WORDS_BIGENDIAN
  {
    int n = sizeof(XChar2b)*len;
    b = (XChar2b*) getBuffer( X, n);
    swab( jc, b, n);
  }
#else
  b = (XChar2b*) jc;
#endif

  w = XTextWidth16( fs, b, len);

  (*env)->ReleaseStringChars( env, jStr, jc);
  return w;
}
