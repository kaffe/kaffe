/**
 * gra.c - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "toolkit.h"

typedef struct {
  GC        gc;
  Drawable  drw;
  int       fg;
  int       bg;
  char      xor;
  int       xclr;
  int       x0;
  int       y0;
} Graphics;

#define  WINDOW   0
#define  IMAGE    1
#define  GRAPHICS 2

/*******************************************************************************
 *
 */


void*
Java_java_awt_Toolkit_graInitGraphics ( JNIEnv* env, jclass clazz,
										Graphics* gr, jobject tgt, jint tgtType,
										jint xOff, jint yOff,
										jint xClip, jint yClip, jint wClip, jint hClip,
										jobject fnt, jint fg, jint bg, jboolean blank )
{
  Drawable       drw;
  XGCValues      values;
  XRectangle     rect;
  unsigned long  valueMask = GCForeground | GCBackground | GCFont | GCFunction;

  if ( tgt ) {
	switch ( tgtType ) {
	case 0:  drw = (Drawable)tgt; break;          /* Window   */
	case 1:  drw = ((Image*)tgt)->pix; break;     /* Image    */
	case 2:  drw = ((Graphics*)tgt)->drw; break;  /* Graphics */
	default: fprintf( stderr, "illegal Graphics target: %d\n", tgtType);
		 drw = 0;
	}
  }
  else {
	if ( gr )
	  drw = gr->drw;
	else {
	  fprintf( stderr, "attempt to set an uninitialized graphics");
	  drw = 0;
	}
  }

  values.foreground = fg;
  values.background = bg;
  values.font       = ((XFontStruct*)fnt)->fid;
  values.function   = GXcopy;
  
  if ( gr ) {
	XChangeGC( X->dsp, gr->gc, valueMask, &values);
  }
  else {
	gr = (Graphics*) malloc( sizeof(Graphics));
	gr->gc = XCreateGC( X->dsp, drw, valueMask, &values);
  }

  rect.x      = xClip; 
  rect.y      = yClip;
  rect.width  = (wClip > 0) ? wClip : 0; 
  rect.height = (hClip > 0) ? hClip : 0;
  XSetClipRectangles( X->dsp, gr->gc, xOff, yOff, &rect, 1, Unsorted);

  gr->fg   = fg;
  gr->bg   = bg;
  gr->drw  = drw;
  gr->x0   = xOff;
  gr->y0   = yOff;
  gr->xor  = 0;

  if ( blank ) {
	XSetForeground( X->dsp, gr->gc, gr->bg);
	XFillRectangle( X->dsp, gr->drw, gr->gc, xOff+xClip, yOff+yClip, wClip, hClip);
	XSetForeground( X->dsp, gr->gc, gr->fg);	
  }

  return gr;
}


void
Java_java_awt_Toolkit_graFreeGraphics ( JNIEnv* env, jclass clazz, Graphics* gr )
{
  XFreeGC( X->dsp, gr->gc);
  free( gr);
}


void
Java_java_awt_Toolkit_graCopyArea ( JNIEnv* env, jclass clazz, Graphics* gr, 
									jint x, jint y, jint width, jint height,
									jint xDelta, jint yDelta )
{
  x += gr->x0; y += gr->y0;
  XCopyArea( X->dsp, gr->drw, gr->drw, gr->gc, x, y, width, height, x + xDelta, y + yDelta);
}


void
Java_java_awt_Toolkit_graClearRect ( JNIEnv* env, jclass clazz, Graphics* gr,
									 jint x, jint y, jint width, jint height )
{
  XSetForeground( X->dsp, gr->gc, gr->bg);
  XFillRectangle( X->dsp, gr->drw, gr->gc, x+gr->x0, y+gr->y0, width, height);
  XSetForeground( X->dsp, gr->gc, gr->fg);
}


void
Java_java_awt_Toolkit_graDrawBytes ( JNIEnv* env, jclass clazz, Graphics* gr,
									 jarray jBytes, jint offset, jint len, jint x, jint y )
{
  jboolean isCopy;
  int      n;
  jbyte    *jb;

  if ( !jBytes ) return;

  n = (*env)->GetArrayLength( env, jBytes);
  jb = (*env)->GetByteArrayElements( env, jBytes, &isCopy);
  
  if ( offset+len > n ) len = n - offset;

  XDrawString( X->dsp, gr->drw, gr->gc, x+gr->x0, y+gr->y0, (jb+offset), len);

  (*env)->ReleaseByteArrayElements( env, jBytes, jb, JNI_ABORT);
}


void
Java_java_awt_Toolkit_graDrawChars ( JNIEnv* env, jclass clazz, Graphics* gr,
									 jarray jChars, jint offset, jint len, jint x, jint y )
{
  jboolean isCopy;
  int      n;
  jchar    *jc;
  XChar2b  *b;

  if ( !jChars ) return;

  n   = (*env)->GetArrayLength( env, jChars);
  jc = (*env)->GetCharArrayElements( env, jChars, &isCopy);

  if ( offset+len > n ) len = n - offset;

#ifndef WORDS_BIGENDIAN
  n = sizeof(XChar2b)*len;
  b = (XChar2b*) getBuffer( X, n);
  swab( (jc+offset), b, n);
#else
  b = (XChar2b*) (jc + offset);
#endif

  XDrawString16( X->dsp, gr->drw, gr->gc, x+gr->x0, y+gr->y0, b, len);  

  (*env)->ReleaseCharArrayElements( env, jChars, jc, JNI_ABORT);
}


void
Java_java_awt_Toolkit_graDrawString ( JNIEnv* env, jclass clazz,
									  Graphics* gr, jstring str, jint x, jint y )
{
  jboolean     isCopy;
  int          len;
  const jchar  *jc;
  XChar2b      *b;

  if ( !str ) return;

  len = (*env)->GetStringLength( env, str);
  jc = (*env)->GetStringChars( env, str, &isCopy);

#ifndef WORDS_BIGENDIAN
  {
    int n = sizeof(XChar2b)*len;
    b = (XChar2b*) getBuffer( X, n);
    swab( jc, b, n);
  }
#else
  b = (XChar2b*) jc;
#endif

  XDrawString16( X->dsp, gr->drw, gr->gc, x+gr->x0, y+gr->y0, b, len);  

  (*env)->ReleaseStringChars( env, str, jc);
}


void
Java_java_awt_Toolkit_graDrawLine ( JNIEnv* env, jclass clazz, Graphics* gr,
									jint x1, jint y1, jint x2, jint y2 )
{
  XDrawLine( X->dsp, gr->drw, gr->gc, x1 + gr->x0, y1 + gr->y0, x2 + gr->x0, y2 + gr->y0);
}


void
Java_java_awt_Toolkit_graDrawArc ( JNIEnv* env, jclass clazz, Graphics* gr,
								   jint x, jint y, jint width, jint height,
								   jint angle1, jint angle2 )
{
  XDrawArc( X->dsp, gr->drw, gr->gc, x+gr->x0, y+gr->y0, width, height, angle1<<6, angle2<<6);
}


void
Java_java_awt_Toolkit_graFillArc ( JNIEnv* env, jclass clazz, Graphics* gr,
								   jint x, jint y, jint width, jint height,
								   jint angle1, jint angle2 )
{
  XFillArc( X->dsp, gr->drw, gr->gc, x+gr->x0, y+gr->y0, width, height, angle1<<6, angle2<<6);
}


void
Java_java_awt_Toolkit_graDrawOval ( JNIEnv* env, jclass clazz, Graphics* gr,
									jint x, jint y, jint width, jint height )
{
  XDrawArc( X->dsp, gr->drw, gr->gc, x+gr->x0, y+gr->y0, width, height, 0, 23040);
}


void
Java_java_awt_Toolkit_graFillOval ( JNIEnv* env, jclass clazz, Graphics* gr,
									jint x, jint y, jint width, jint height )
{
  XFillArc( X->dsp, gr->drw, gr->gc, x+gr->x0, y+gr->y0, width, height, 0, 23040);
}


int jarray2Points ( JNIEnv* env, Toolkit* X, XPoint** pp, int x0, int y0,
					jarray xPoints, jarray yPoints, int nPoints )
{
  register int i;
  int      n;
  jboolean isCopy;
  jint     *jx = (*env)->GetIntArrayElements( env, xPoints, &isCopy);
  jint     *jy = (*env)->GetIntArrayElements( env, yPoints, &isCopy);

  if ( (n = (*env)->GetArrayLength( env, xPoints)) < nPoints ) nPoints = n;
  if ( (n = (*env)->GetArrayLength( env, yPoints)) < nPoints ) nPoints = n;

  *pp = getBuffer( X, (nPoints+1)*sizeof(XPoint));

  for ( i=0; i<nPoints; i++){
	(*pp)[i].x = jx[i] + x0;
    (*pp)[i].y = jy[i] + y0;
  }

  (*env)->ReleaseIntArrayElements( env, xPoints, jx, JNI_ABORT);
  (*env)->ReleaseIntArrayElements( env, yPoints, jy, JNI_ABORT);

  return nPoints;
}


void
Java_java_awt_Toolkit_graDrawPolygon ( JNIEnv* env, jclass clazz, Graphics* gr,
									   jarray xPoints, jarray yPoints, jint nPoints )
{
  int n;
  XPoint   *p;

  if ( !xPoints || !yPoints ) return;

  nPoints = jarray2Points( env, X, &p, gr->x0, gr->y0, xPoints, yPoints, nPoints);

  n = nPoints-1;
  if ( (p[0].x != p[n].x) || (p[0].y != p[n].y) ) {
	p[nPoints].x = p[0].x;
	p[nPoints].y = p[0].y;
	nPoints++;
  }

  XDrawLines( X->dsp, gr->drw, gr->gc, p, nPoints, CoordModeOrigin);
}


void
Java_java_awt_Toolkit_graDrawPolyline ( JNIEnv* env, jclass clazz, Graphics* gr,
										jarray xPoints, jarray yPoints, jint nPoints )
{
  XPoint   *p;

  if ( !xPoints || !yPoints ) return;

  nPoints = jarray2Points( env, X, &p, gr->x0, gr->y0, xPoints, yPoints, nPoints);
  XDrawLines( X->dsp, gr->drw, gr->gc, p, nPoints, CoordModeOrigin);
}


void
Java_java_awt_Toolkit_graFillPolygon ( JNIEnv* env, jclass clazz, Graphics* gr,
									   jarray xPoints, jarray yPoints, jint nPoints )
{
  XPoint   *p;

  if ( !xPoints || !yPoints ) return;

  nPoints = jarray2Points( env, X, &p, gr->x0, gr->y0, xPoints, yPoints, nPoints);
  XFillPolygon( X->dsp, gr->drw, gr->gc, p, nPoints, Nonconvex, CoordModeOrigin);
}


void
Java_java_awt_Toolkit_graDrawRect ( JNIEnv* env, jclass clazz, Graphics* gr,
									jint x, jint y, jint width, jint height )
{
  XDrawRectangle( X->dsp, gr->drw, gr->gc, x+gr->x0, y+gr->y0, width, height);
}


void
Java_java_awt_Toolkit_graFillRect ( JNIEnv* env, jclass clazz, Graphics* gr,
									jint x, jint y, jint width, jint height )
{
  XFillRectangle( X->dsp, gr->drw, gr->gc, x+gr->x0, y+gr->y0, width, height);
}



/*
 * we don't use the Xmu routines here because it would drag the whole Xt libs in
 */
void
Java_java_awt_Toolkit_graDrawRoundRect ( JNIEnv* env, jclass clazz, Graphics* gr,
										 jint x, jint y, jint width, jint height, 
										 jint wArc, jint hArc )
{
  int x1, x2, y1, y2, a, b;
  
  x += gr->x0;  y += gr->y0;
  a = wArc / 2; b = hArc / 2;

  x1 = x + a;   x2 = x + width - a;  y2 = y + height;
  XDrawLine( X->dsp, gr->drw, gr->gc, x1, y, x2, y);
  XDrawLine( X->dsp, gr->drw, gr->gc, x1, y2, x2, y2);

  y1 = y + b;   y2 = y + height - b; x2 = x + width;
  XDrawLine( X->dsp, gr->drw, gr->gc, x, y1, x, y2);
  XDrawLine( X->dsp, gr->drw, gr->gc, x2, y1, x2, y2);

  XDrawArc( X->dsp, gr->drw, gr->gc, x, y, wArc, hArc, 90*64, 90*64);

  x2 = x + width - wArc;
  XDrawArc( X->dsp, gr->drw, gr->gc, x2, y, wArc, hArc, 0, 90*64);

  y2 = y + height - hArc;
  XDrawArc( X->dsp, gr->drw, gr->gc, x2, y2, wArc, hArc, 0, -90*64);

  XDrawArc( X->dsp, gr->drw, gr->gc, x, y2, wArc, hArc, 180*64, 90*64);
}

void
Java_java_awt_Toolkit_graFillRoundRect ( JNIEnv* env, jclass clazz, Graphics* gr,
										 jint x, jint y, jint width, jint height, 
										 jint wArc, jint hArc )
{
  int x1, x2, y1, y2, a, b;
  
  x += gr->x0;  y += gr->y0;
  a = wArc / 2; b = hArc / 2;

  y1 = y + b;  y2 = y + height - b;
  if ( y2 > y1 )
	XFillRectangle( X->dsp, gr->drw, gr->gc, x, y1, width, y2-y1);
  
  x1 = x + a;  x2 = x + width - a;
  if ( x2 > x1 ) {
	XFillRectangle( X->dsp, gr->drw, gr->gc, x1, y, (x2-x1), b);
	XFillRectangle( X->dsp, gr->drw, gr->gc, x1, y2, (x2-x1), b);
  }

  XFillArc( X->dsp, gr->drw, gr->gc, x, y, wArc, hArc, 90*64, 90*64);

  x2 = x + width - wArc;
  XFillArc( X->dsp, gr->drw, gr->gc, x2, y, wArc, hArc, 0, 90*64);

  y2 = y + height - hArc;
  XFillArc( X->dsp, gr->drw, gr->gc, x2, y2, wArc, hArc, 0, -90*64);

  XFillArc( X->dsp, gr->drw, gr->gc, x, y2, wArc, hArc, 180*64, 90*64);
}


void
Java_java_awt_Toolkit_graDraw3DRect ( JNIEnv* env, jclass clazz, Graphics* gr,
									  jint x, jint y, jint width, jint height,
									  jboolean raised, jint rgb )
{
  int      bright, dark;
  int      xw, yh;

  /* we should use the same mechanism like ordinary Graphics brighter()/darker() here */
  dark   = (int) (Java_java_awt_Toolkit_clrDark( env, clazz, rgb) >> 32);
  bright = (int) (Java_java_awt_Toolkit_clrBright( env, clazz, rgb) >> 32);

  x += gr->x0; y += gr->y0;
  xw = x + width;
  yh = y + height;

  XSetForeground( X->dsp, gr->gc, (raised ? bright : dark));
  XDrawLine( X->dsp, gr->drw, gr->gc, x, y, xw-1, y);
  XDrawLine( X->dsp, gr->drw, gr->gc, x, y, x, yh);

  XSetForeground( X->dsp, gr->gc, (raised ? dark : bright));
  XDrawLine( X->dsp, gr->drw, gr->gc, x+1, yh, xw, yh);
  XDrawLine( X->dsp, gr->drw, gr->gc, xw, y, xw, yh);

  XSetForeground( X->dsp, gr->gc, gr->fg);
}

void
Java_java_awt_Toolkit_graFill3DRect ( JNIEnv* env, jclass clazz, Graphics* gr,
									  jint x, jint y, jint width, jint height,
									  jboolean raised, jint rgb )
{
  XFillRectangle( X->dsp, gr->drw, gr->gc, x+gr->x0+1, y+gr->y0+1, width-2, height-2);

  Java_java_awt_Toolkit_graDraw3DRect( env, clazz, gr, x, y, width-1, height-1, raised, rgb);
}


void
Java_java_awt_Toolkit_graAddClip ( JNIEnv* env, jclass clazz, Graphics* gr,
																	jint xClip, jint yClip, jint wClip, jint hClip )
{
	/* not implemented yet */
}


void
Java_java_awt_Toolkit_graSetClip ( JNIEnv* env, jclass clazz, Graphics* gr,
								   jint xClip, jint yClip, jint wClip, jint hClip )
{
  XRectangle rect;

  rect.x      = xClip;
  rect.y      = yClip;
  rect.width  = (wClip > 0) ? wClip : 0; 
  rect.height = (hClip > 0) ? hClip : 0;
  XSetClipRectangles( X->dsp, gr->gc, gr->x0, gr->y0, &rect, 1, Unsorted);
}

void
Java_java_awt_Toolkit_graSetColor ( JNIEnv* env, jclass clazz, Graphics* gr, jint clr )
{
  gr->fg = clr;

  if ( gr->xor )
	clr ^= gr->xclr;

  XSetForeground( X->dsp, gr->gc, clr);
}

void
Java_java_awt_Toolkit_graSetBackColor ( JNIEnv* env, jclass clazz, Graphics* gr, jint clr )
{
  gr->bg = clr;
  XSetBackground( X->dsp, gr->gc, clr);
}

void
Java_java_awt_Toolkit_graSetFont ( JNIEnv* env, jclass clazz, Graphics* gr, jobject fnt )
{
  XSetFont( X->dsp, gr->gc, ((XFontStruct*)fnt)->fid);
}

void
Java_java_awt_Toolkit_graSetOffset ( JNIEnv* env, jclass clazz, Graphics* gr, jint xOff, jint yOff )
{
  gr->x0 = xOff;
  gr->y0 = yOff;
}

void
Java_java_awt_Toolkit_graSetPaintMode ( JNIEnv* env, jclass clazz, Graphics* gr )
{
  gr->xor = 0;
  XSetForeground( X->dsp, gr->gc, gr->fg);
  XSetFunction( X->dsp, gr->gc, GXcopy);
}

void
Java_java_awt_Toolkit_graSetXORMode ( JNIEnv* env, jclass clazz, Graphics* gr, jint xorClr )
{
  gr->xor = 1;
  gr->xclr = xorClr;
  XSetForeground( X->dsp, gr->gc, gr->fg ^ xorClr);
  XSetFunction( X->dsp, gr->gc, GXxor);
}


/************************************************************************************
 * image rendering
 */

void
drawAlphaImage ( Graphics* gr, Image* img,
				 jint srcX, jint srcY, jint dstX, jint dstY,
				 jint width, jint height, jint bgval )
{
  XImage *dstImg;
  int    i, j, si, sj, alpha;
  unsigned long dpix, spix, bgpix = 0;
  int    sr, sg, sb, dr, dg, db;

  dstImg = XGetImage( X->dsp, gr->drw, dstX, dstY, width, height, 0xffffffff, ZPixmap);
  if ( bgval != -1 )
	bgpix = pixelValue( X, bgval);

  if ( dstImg ) {
	for ( j=0, sj=srcY; j<dstImg->height; j++, sj++ ) {
	  for ( i=0, si=srcX; i<dstImg->width; i++, si++ ) {
		dpix = (bgval == -1) ? XGetPixel( dstImg, i, j) : bgpix;
		spix = XGetPixel( img->xImg, si, sj);
		alpha = GetAlpha( img->alpha, si, sj);

		rgbValues( X, dpix, &dr, &dg, &db);
		rgbValues( X, spix, &sr, &sg, &sb);

		dr = ((alpha * sr + (255 - alpha) * dr) + 128) >> 8;
		dg = ((alpha * sg + (255 - alpha) * dg) + 128) >> 8;
		db = ((alpha * sb + (255 - alpha) * db) + 128) >> 8;

		XPutPixel( dstImg, i, j, pixelValue( X, (dr << 16)|(dg << 8)|(db) ));
	  }
	}

	XPutImage( X->dsp, gr->drw, gr->gc, dstImg, 0, 0, dstX, dstY, width, height);
	XDestroyImage( dstImg);
  }
}


void
Java_java_awt_Toolkit_graDrawImage ( JNIEnv* env, jclass clazz, Graphics* gr, Image* img,
									 jint srcX, jint srcY,
									 jint dstX, jint dstY,
									 jint width, jint height, jint bgval )
{
  XGCValues values;

  dstX += gr->x0;
  dstY += gr->y0;

  if ( img->pix ) {
	XCopyArea( X->dsp, img->pix, gr->drw, gr->gc, srcX, srcY, width, height, dstX, dstY);
  }
  else if ( img->xImg ) {
	if ( img->xMask ) {
	  /* save current gc values, we have to change them temporarily */
	  XGetGCValues( X->dsp, gr->gc, GCFunction|GCPlaneMask|GCForeground|GCBackground,
					&values);

	  /* in case we have a specified background color, fill area */
	  if ( bgval != -1 ) {
		XSetForeground( X->dsp, gr->gc, bgval);
		XFillRectangle( X->dsp, gr->drw, gr->gc, dstX, dstY, width, height);
	  }

	  /* set attributes so that a subsequent image draw leaves the mask pels unmodified */
	  XSetFunction( X->dsp, gr->gc, GXandInverted);
	  XSetForeground( X->dsp, gr->gc, 0xffffffff);
	  XSetBackground( X->dsp, gr->gc, 0);

	  /* draw the mask bitmap */
	  XPutImage( X->dsp, gr->drw, gr->gc, img->xMask, srcX, srcY, dstX, dstY, width, height);

	  /* restore gc values except of the function */
	  XChangeGC( X->dsp, gr->gc, GCFunction|GCPlaneMask|GCForeground|GCBackground, &values);
	  XSetFunction( X->dsp, gr->gc, GXor);
	}
	else if ( img->alpha ) {
	  drawAlphaImage( gr, img, srcX, srcY, dstX, dstY, width, height, bgval);
	  return;
	}

	/* this is the real image drawing */
	XPutImage( X->dsp, gr->drw, gr->gc, img->xImg, srcX, srcY, dstX, dstY, width, height);

	/* in case we had a mask, restore the original function */
	if ( img->xMask )
	  XSetFunction( X->dsp, gr->gc, values.function);
  }
}


void
Java_java_awt_Toolkit_graDrawImageScaled ( JNIEnv* env, jclass clazz, Graphics* gr, Image* img,
										   jint dx0, jint dy0, jint dx1, jint dy1,
										   jint sx0, jint sy0, jint sx1, jint sy1, jint bgval )
{
  int        x0, y0, x1, y1;
  int        iw = img->xImg->width;
  Image      tgt;

  dx0 += gr->x0;
  dx1 += gr->x0;
  dy0 += gr->y0;
  dy1 += gr->y0;

  if ( dx1 > dx0 ) {
	x0 = dx0; x1 = dx1;
  }
  else {
	x0 = dx1; x1 = dx0;
  }
  if ( dy1 > dy0 ) {
	y0 = dy0; y1 = dy1;
  }
  else {
	y0 = dy1; y1 = dy0;
  }

  if ( sx0 < 0 ) sx0 = 0;
  if ( sx1 < 0 ) sx1 = 0;
  if ( sx0 >= iw ) sx0 = iw-1;
  if ( sx1 >= iw ) sx1 = iw-1;

  tgt.pix = 0;
  tgt.width = (x1 - x0 +1);
  tgt.height = (y1 - y0 +1);
  tgt.xImg = createXImage( X, tgt.width, tgt.height);
  tgt.xMask = (img->xMask) ? createXMaskImage( X, tgt.width, tgt.height) : 0;
  tgt.alpha = (img->alpha) ? createAlphaImage( X, tgt.width, tgt.height) : 0;

  initScaledImage( X, &tgt, img, dx0-x0, dy0-y0, dx1-x0, dy1-y0, sx0, sy0, sx1, sy1);

  Java_java_awt_Toolkit_graDrawImage ( env, clazz, gr, &tgt, 0, 0,
									   x0, y0, tgt.width, tgt.height, bgval);

  /* since malloc / free might be redirected, we better clean up manually */
  free( tgt.xImg->data);
  tgt.xImg->data = 0;
  XDestroyImage( tgt.xImg);

  if ( tgt.xMask ) {
	free( tgt.xMask->data);
	tgt.xMask->data = 0;
    XDestroyImage( tgt.xMask);
  }
}
