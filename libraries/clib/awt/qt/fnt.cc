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

#include "toolkit.h"

/**
 * Font support
 */

/*
 * this is our ultima ratio in case everything else fails, at least this
 * one should be.
 */
#ifdef QPE
char* backupFont = "unifont";
#else
char* backupFont = "fixed";
#endif

/*
 * these are (hopefully) the most usual font weight names, ordered
 * in our preferred lookup sequence (from PLAIN to BOLD)
 */
char* weight[] = { "medium", "normal", "regular", "thin", "light",
                   "black", "demibold", "heavy", "extrabold", "extrablack", "bold" };
#define NWEIGHT 11

/*
 * these are finally the size offsets we want to try
 */
int dsize[] = { 0, 10, -10, 20, -20, 30, -30, 40, -40 };
#define NDSIZE 9

/*
 * Our font lookup strategy is like this: every mapping that can be known
 * a priori goes into java.awt.Defaults, everything that might be specified
 * at runtime (sizes, slant, weight) has to be handled here. In detail:
 * 
 * (1) foundry, family, encoding and everything EXCEPT of weight,slant,size
 *     are specified in java.awt.Defaults FontSpec strings, which are used
 *     as simple C format specifications
 * (2) since both BOLD and ITALIC map to more than one attribute value
 *     (e.g. ITALIC-> "o", "i"), and many fonts are not available in all
 *     sizes, we have to make up a sequence of alternate specs to search
 * (3) the search first tries to vary sizes, then weights, then slants
 * (4) in case all of this fails, it directly tries the font spec
 * (5) if this fails, too, it backs up to what is supposed to be a safe
 *     standard font (fix size)
 *
 * Once again - we don't try to deduce Qt font family names from the Java 
 * names, that's in java.awt.Defaults, and Defaults is meant to be the 
 * thing which adapts the AWT to your Qt installation/preferences. Don't 
 * lament, modify it!
 */

void*
Java_java_awt_Toolkit_fntInitFont ( JNIEnv* env, jclass clazz, jstring jSpec,
                                    jint style, jint size )
{
  int  i, j, k, i0, i1, j0, j1, di, dj;
  char buf[160];
  char *spec = java2CString( env, X, jSpec);
  QFont *qf;
  int weight=QFont::Normal;
  bool italic=FALSE;

  DBG(AWT, qDebug("fntInitFont...%s\n",spec));

  if ( style & 0x1 ) { /* we have a Font.BOLD request */
    //i0 = NWEIGHT - 1; i1 = -1; di = -1;
    weight = QFont::Bold;
  }
  else {
    //i0 = 0; i1 = NWEIGHT; di = 1;
  }

  if ( style & 0x2 ) { /* we have a Font.ITALIC request */
    //j0 = NSLANT - 1; j1 = -1; dj = -1;
    italic = TRUE;
  }
  else {
    //j0 = 0; j1 = NSLANT; dj = 1;
  }
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
  DBG(AWT, qDebug("fntGetFixedWidth %d\n",w));
  return w;
}

jint
Java_java_awt_Toolkit_fntGetHeight ( JNIEnv* env, jclass clazz, QFont* fs )
{
  QFontMetrics fm((QFont)(*fs));
  return fm.ascent() + fm.descent() +1;
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
  DBG(AWT, qDebug("fntIsWideFont\n"));
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
  DBG(AWT, qDebug("fntGetWidths %d\n",fm.maxWidth()));
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
  if ( off+len > n ) len = n - off;
  QByteArray a;
  a.setRawData(jb+off,len);
  w = fm.width(QString(a),len);
  env->ReleaseByteArrayElements( jBytes, jb, JNI_ABORT);
  DBG(AWT, qDebug("fntBytesWidth %s %d\n",jb+off, w));
  return w;
}

jint
Java_java_awt_Toolkit_fntCharWidth ( JNIEnv* env, jclass clazz, QFont* fs, jchar jChar )
{
#ifndef WORDS_BIGENDIAN
  jChar = (jChar << 8) | (jChar >> 8);
#endif
  QFontMetrics fm((QFont)(*fs));
  DBG(AWT, qDebug("fntCharWidth...\n"));

  return fm.width(QChar((ushort)jChar));
}

jint
Java_java_awt_Toolkit_fntCharsWidth ( JNIEnv* env, jclass clazz,
	QFont* fs, jcharArray jChars, jint off, jint len )
{
  jboolean  isCopy;
  jchar    *jc = env->GetCharArrayElements( jChars, &isCopy);
  int      n = env->GetArrayLength( jChars);
  ushort *b;
  QChar  *unicode;
  int      w;
  QFontMetrics fm((QFont)(*fs));
  DBG(AWT, qDebug("fntCharsWidth...\n"));

  if ( off+len > n ) len = n - off;

#ifndef WORDS_BIGENDIAN
  n = sizeof(ushort)*len;
  b = (ushort*) getBuffer( X,n);
  swab( (jc+off), b, n);
#else
  b = (ushort *) (jc + off);
#endif
  int i;
  unicode = new QChar[len];
  for(i=0;i<len;i++)
    unicode[i] = QChar((ushort)*(b+i));

  w = fm.width(QString(jchar2CString(X,jc+off,len)));
  env->ReleaseCharArrayElements( jChars, jc, JNI_ABORT);
  DBG(AWT, qDebug("fntCharsWidth %s %d\n",jchar2CString(X,jc+off,len),w));
  return w;
}

jint
Java_java_awt_Toolkit_fntStringWidth ( JNIEnv* env, jclass clazz, QFont* fs, jstring jStr )
{
  jboolean isCopy;
  const jchar    *jc = env->GetStringChars( jStr, &isCopy);
  int      len = env->GetStringLength( jStr);
  int      w, n;
  ushort *b;
  QChar  *unicode;
  QFontMetrics fm((QFont)(*fs));


#ifndef WORDS_BIGENDIAN
  n = sizeof(ushort)*len;
  b = (ushort*) getBuffer( X, n);
  swab( jc, b, n);
#else
  b= (ushort*) jc;
#endif
  int i;
  unicode = new QChar[len];
  for(i=0;i<len;i++)
    unicode[i] = QChar((ushort)*(b+i));

  w = fm.width(QString(jchar2CString(X,jc,len)));

  DBG(AWT, qDebug("fntStringWidth %s %d\n",jchar2CString(X,jc,len),w));
  env->ReleaseStringChars( jStr, jc);
  return w;
}

