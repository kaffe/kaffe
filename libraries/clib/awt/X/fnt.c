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
#include "toolkit.h"

/*******************************************************************************
 * Font support
 */

/*
 * this is our ultima ratio in case everything else fails, at least this
 * one should be on every X fontlist
 */
char* backupFont = "fixed";

/*
 * these are (hopefully) the most usual XFLD font weight names, ordered
 * in our preferred lookup sequence (from PLAIN to BOLD)
 */
char* weight[] = { "medium", "normal", "regular", "thin", "light",
                   "black", "demibold", "heavy", "extrabold", "extrablack", "bold" };
#define NWEIGHT 11

/*
 * these are (more confident) the XFLD slant names, ordered in our
 * preferred lookup sequence (from PLAIN to ITALIC)
 */
char* slant[] = { "r", "i", "o" };
#define NSLANT 3

/*
 * these are finally the size offsets we want to try
 */
int dsize[] = { 0, 1, -1, 2, -2, 3, -3, 4, -4 };
#define NDSIZE 9

/*
 * Our font lookup strategy is like this: every mapping that can be known
 * a priori goes into java.awt.Defaults, everything that might be specified
 * at runtime (sizes, slant, weight) has to be handled here. In detail:
 * 
 * (1) foundry, family, encoding and everything EXCEPT of weight,slant,size
 *     are specified in java.awt.Defaults FontSpec strings, which are used
 *     as simple C format specifications
 *     (e.g."-adobe-helvetica-%s-%s-*-*-%d-*-*-*-*-*-*-*")
 * (2) since both BOLD and ITALIC map to more than one XFLD attribute value
 *     (e.g. ITALIC-> "o", "i"), and many fonts are not available in all
 *     sizes, we have to make up a sequence of alternate specs to search
 * (3) the search first tries to vary sizes, then weights, then slants
 * (4) in case all of this fails, it directly tries the font spec
 * (5) if this fails, too, it backs up to what is supposed to be a safe X
 *     standard font (fix size)
 *
 * Once again - we don't try to deduce XFLD family names from the Java names,
 * that's in java.awt.Defaults, and Defaults is meant to be the thing which
 * adapts the AWT to your X installation/preferences. Don't lament, modify it!
 */

void*
Java_java_awt_Toolkit_fntInitFont ( JNIEnv* env, jclass clazz, jstring jSpec, jint style, jint size )
{
  int  i, j, k, i0, i1, j0, j1, di, dj;
  char buf[160];
  char *spec = java2CString( env, X, jSpec);
  XFontStruct* fs = 0;

  if ( style & 0x1 ) { /* we have a Font.BOLD request */
    i0 = NWEIGHT - 1; i1 = -1; di = -1;
  }
  else {
    i0 = 0; i1 = NWEIGHT; di = 1;
  }

  if ( style & 0x2 ) { /* we have a Font.ITALIC request */
    j0 = NSLANT - 1; j1 = -1; dj = -1;
  }
  else {
    j0 = 0; j1 = NSLANT; dj = 1;
  }

  for ( j=j0; !fs && (j != j1); j += dj ) {
    for ( i=i0; !fs && (i != i1); i += di ) {
      for ( k=0;  !fs && (k < NDSIZE); k++ ) {
        sprintf( buf, spec, weight[i], slant[j], size + dsize[k]);
        DBG( AWT_FNT, printf("look up font: %s\n", buf));
        fs = XLoadQueryFont( X->dsp, buf);
      }
    }
  }

  if ( ! fs ){
    /* now we are getting desperate, try the spec directly (without vars) */
    fs = XLoadQueryFont( X->dsp, spec);
  }

  if ( ! fs ){
	DBG( AWT, printf("cannot load font: %s (backup to %s)\n", buf, backupFont));
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
  return fs->ascent + fs->descent +1;
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
  register  int i, j;

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
  int      w, n;
  XChar2b  *b;

#ifndef WORDS_BIGENDIAN
  n = sizeof(XChar2b)*len;
  b = (XChar2b*) getBuffer( X, n);
  swab( jc, b, n);
#else
  b = (XChar2b*) jc;
#endif

  w = XTextWidth16( fs, b, len);

  (*env)->ReleaseStringChars( env, jStr, jc);
  return w;
}
