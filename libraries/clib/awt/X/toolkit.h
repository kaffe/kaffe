/**
 * toolkit.h - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __toolkit_h
#define __toolkit_h

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <jni.h>
#if defined(HAVE_STRING_H)
#include <string.h>
#endif

/*******************************************************************************
 *
 */

typedef struct _Rgb2True {
  unsigned int   redMask;
  unsigned int   greenMask;
  unsigned int   blueMask;
  char           blueShift;
  char           redShift;
  char           greenShift;
} Rgb2True;


typedef struct _RgbColor {
  unsigned char r;
  unsigned char g;
  unsigned char b;
} RgbColor;


typedef struct _Rgb2Pseudo {
  RgbColor       rgb[256];
  unsigned char  pix[8][8][8];
} Rgb2Pseudo;


typedef struct _AlphaImage {
  unsigned char *buf;
  int           width, height;
} AlphaImage;

typedef struct _Image {
  Pixmap         pix;
  XImage         *xImg;
  XImage         *xMask;
  AlphaImage     *alpha;         /* alpha channel (for alpha != 0x00 or 0xff) */
  int            trans;          /* transparent index */
  int            width, height;  /* we need this in case we are a pixmap */
} Image;


typedef struct _Toolkit {
  Display        *dsp;
  Window         root;

  char           *buf;
  unsigned int   nBuf;

  int            visualClass;
  Rgb2True       *tclr;
  Rgb2Pseudo     *pclr;

  Cursor         cursors[14];

  int            titleBarHeight;
  int            menuBarHeight;
  int            bottomBarHeight;
  int            borderWidth;

  XEvent         event;
  char           preFetched;
  char           peek;
  int            pending;
  int            evtId;

  Window         lastWindow;
  int            srcIdx;
  Window         *windows;
  int            nWindows;

  Window         cbdOwner;

  Window         newWindow;
} Toolkit;


/*******************************************************************************
 *
 */
#ifdef MAIN

Toolkit   XTk;
Toolkit   *X = &XTk;

Atom WM_PROTOCOLS;
Atom WM_DELETE_WINDOW;
Atom WM_TAKE_FOCUS;
Atom WAKEUP;
Atom RETRY_FOCUS;
Atom SELECTION_DATA;
Atom JAVA_OBJECT;

/*******************************************************************************
 *
 */
#else

extern Toolkit* X;

extern Atom WM_PROTOCOLS;
extern Atom WM_DELETE_WINDOW;
extern Atom WM_TAKE_FOCUS;
extern Atom WAKEUP;
extern Atom RETRY_FOCUS;
extern Atom SELECTION_DATA;
extern Atom JAVA_OBJECT;

#endif

extern long StdEvents;

/*******************************************************************************
 *
 */


static __inline__ char* java2CString ( JNIEnv *env, Toolkit* X, jstring jstr ) {
  jboolean isCopy;
  register int i;
  int      n = (*env)->GetStringLength( env, jstr);
  const jchar    *jc = (*env)->GetStringChars( env, jstr, &isCopy);

  if ( n >= X->nBuf ) {
	if ( X->buf )
	  free( X->buf);
	X->buf = malloc( n+1);
	X->nBuf = n+1;
  }

  for ( i=0; i<n; i++ ) X->buf[i] = (char) jc[i];
  X->buf[i] = 0;
  (*env)->ReleaseStringChars( env, jstr, jc);

  return X->buf;
}

static __inline__ char* jchar2CString ( Toolkit* X, jchar* jc, int len ) {
  register int i;
  int      n = len+1;
  
  if ( n > X->nBuf ) {
	if ( X->buf )
	  free( X->buf);
	X->buf  = malloc( n);
	X->nBuf = n;
  }

  for ( i=0; i<len; i++ ) X->buf[i] = (char) jc[i];
  X->buf[i] = 0;

  return X->buf;
}

static __inline__ void* getBuffer ( Toolkit* X, unsigned int nBytes ) {
  if ( nBytes > X->nBuf ) {
	if ( X->buf )
	  free( X->buf);
	X->buf  = malloc( nBytes);
	X->nBuf = nBytes;
  }
  return X->buf;
}


/*****************************************************************************************
 * color functions
 */

void initColorMapping ( JNIEnv* env, Toolkit* X);
jlong Java_java_awt_Toolkit_clrBright ( JNIEnv* env, jclass clazz, jint rgb );
jlong Java_java_awt_Toolkit_clrDark ( JNIEnv* env, jclass clazz, jint rgb );

#define JRGB(_r,_g,_b)  (_r<<16 | _g<<8 | _b)
#define JRED(_rgb)      ((_rgb & 0xff0000) >> 16)
#define JGREEN(_rgb)    ((_rgb & 0x00ff00) >> 8)
#define JBLUE(_rgb)     (_rgb & 0x0000ff)

#define D8   36.43  /* 255 / 7 */
#define D16  18.21

#define JI8(_v) (int)((_v + D16) / D8)
#define XI8(_v) (int)(((_v>>8) + D16) / D8)

static __inline__ jint
pixelValue ( Toolkit* X, jint rgb )
{
  switch ( X->visualClass ) {
  case TrueColor:
	return (((rgb & X->tclr->blueMask)  >> X->tclr->blueShift) +
            ((rgb & X->tclr->greenMask) >> X->tclr->greenShift) +
            ((rgb & X->tclr->redMask)   >> X->tclr->redShift));

  case DirectColor:
	return (rgb & 0xffffff);

  case PseudoColor:
	return X->pclr->pix [JI8(JRED(rgb))] [JI8(JGREEN(rgb))] [JI8(JBLUE(rgb))];

  case StaticGray:
    return (JRED(rgb)+ JGREEN(rgb)+ JBLUE(rgb) < 196);

  default:
    return (0);
  }
}

static __inline__ void
rgbValues ( Toolkit* X, unsigned long pixel, int* r, int* g, int* b )
{
  Visual         *v;

  switch ( X->visualClass ) {
  case TrueColor:
	v = DefaultVisual( X->dsp, DefaultScreen( X->dsp));
	*r = ((pixel & v->red_mask)   << X->tclr->redShift)   >> 16;
	*g = ((pixel & v->green_mask) << X->tclr->greenShift) >> 8;
	*b = ((pixel & v->blue_mask)  << X->tclr->blueShift);
	break;

  case DirectColor:
	*r = JRED( pixel);
	*g = JGREEN( pixel);
	*b = JBLUE( pixel);
	break;

  case PseudoColor:
	*r = X->pclr->rgb[(unsigned char)pixel].r;
	*g = X->pclr->rgb[(unsigned char)pixel].g;
	*b = X->pclr->rgb[(unsigned char)pixel].b;
	break;

  case StaticGray:
    *r = *g = *b = (JRED(pixel)+JGREEN(pixel)+JBLUE(pixel)/3);
    break;

  default:
    *r = *g = *b = 0;
    break;
  }
}


/*****************************************************************************************
 * image functions
 */

XImage* createXMaskImage ( Toolkit* X, int width, int height );
XImage* createXImage ( Toolkit* X, int width, int height );
AlphaImage* createAlphaImage ( Toolkit* X, int width, int height );
void initScaledImage ( Toolkit* X, Image *tgt, Image *src,
					   int dx0, int dy0, int dx1, int dy1,
					   int sx0, int sy0, int sx1, int sy1 );


static __inline__ void
PutAlpha ( AlphaImage* img, int col, int row, unsigned char alpha )
{
  img->buf[ row*img->width + col ] = alpha;
}

static __inline__ int
GetAlpha ( AlphaImage* img, int col, int row )
{
  return img->buf[ row*img->width + col];
}

/*****************************************************************************************
 * clipboard functions
 */

jobject selectionClear ( JNIEnv* env, Toolkit* X );
jobject selectionRequest ( JNIEnv* env, Toolkit* X );


#endif
