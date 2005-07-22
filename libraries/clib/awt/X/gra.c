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

/*******************************************************************************
 *
 */


jobject
Java_java_awt_Toolkit_graInitGraphics ( JNIEnv* env, jclass clazz UNUSED,
					jobject ngr, jobject tgt, jint tgtType,
					jint xOff, jint yOff,
					jint xClip, jint yClip, jint wClip, jint hClip,
					jobject fnt, jint fg, jint bg, jboolean blank )
{
  Graphics *gr = NULL;
  Drawable       drw;
  XGCValues      values;
  XRectangle     rect;
#ifdef KAFFE_I18N
  unsigned long  valueMask = GCForeground | GCBackground | GCFunction;
#else  
  unsigned long  valueMask = GCForeground | GCBackground | GCFont | GCFunction;
#endif  

  if (ngr != NULL)
    gr = UNVEIL_GR(ngr);

  DBG( AWT_GRA, printf("initGraphics: %p (%p, %d) %d,%d  %d,%d - %d,%d  %p %x %x\n",
				 gr, tgt,tgtType, xOff,yOff, xClip,yClip,wClip,hClip, fnt,fg,bg));

  if ( tgt ) {
    void *tgtPtr = JCL_GetRawData(env, tgt);
	switch ( tgtType ) {
	case 0:  drw = (Drawable)tgtPtr; break;          /* Window   */
	case 1:  drw = ((Image*)tgtPtr)->pix; break;     /* Image    */
	case 2:  drw = ((Graphics*)tgtPtr)->drw; break;  /* Graphics */
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
#ifndef KAFFE_I18N /* !KAFFE_I18N */ 
  values.font       = UNVEIL_FS(fnt)->fid;
#endif  
  values.function   = GXcopy;
  
  if ( gr ) {
	XChangeGC( X->dsp, gr->gc, valueMask, &values);
  }
  else {
	gr = (Graphics*) AWT_MALLOC( sizeof(Graphics));
	gr->gc = XCreateGC( X->dsp, drw, valueMask, &values);

	ngr = JCL_NewRawDataObject(env, gr);

	DBG( AWT_GRA, printf(" ->gr: %p (%p)\n", gr, gr->gc));
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
#ifdef KAFFE_I18N
  gr->oc   = UNVEIL_XOC(fnt);
#endif  

  if ( blank ) {
	XSetForeground( X->dsp, gr->gc, gr->bg);
	XFillRectangle( X->dsp, gr->drw, gr->gc, xOff+xClip, yOff+yClip, wClip, hClip);
	XSetForeground( X->dsp, gr->gc, gr->fg);	
  }

  return ngr;
}


void
Java_java_awt_Toolkit_graFreeGraphics ( JNIEnv* env UNUSED, jclass clazz UNUSED, jobject ngr )
{
  Graphics *gr = UNVEIL_GR(ngr);
  DBG( AWT_GRA, printf("freeGraphics: %p\n", gr));

  XFreeGC( X->dsp, gr->gc);
  AWT_FREE( gr);
}


void
Java_java_awt_Toolkit_graCopyArea ( JNIEnv* env UNUSED, jclass clazz UNUSED, jobject ngr, 
				    jint x, jint y, jint width, jint height,
				    jint xDelta, jint yDelta )
{
  Graphics *gr = UNVEIL_GR(ngr);
  DBG( AWT_GRA, printf("copyArea: %p, %d,%d-%d,%d  %d,%d\n", gr, x,y,width,height, xDelta,yDelta));

  /* some X servers act alleric on that (careless bitblt, again) */
  if ( (width < 0) || (height < 0) )
	return;

  x += gr->x0; y += gr->y0;
  XCopyArea( X->dsp, gr->drw, gr->drw, gr->gc, x, y, width, height, x + xDelta, y + yDelta);
  XFLUSH( X, False);
}


void
Java_java_awt_Toolkit_graClearRect ( JNIEnv* env UNUSED, jclass clazz UNUSED, jobject ngr,
				     jint x, jint y, jint width, jint height )
{
  Graphics *gr = UNVEIL_GR(ngr);
  DBG( AWT_GRA, printf("clearRect: %p, %d,%d-%d,%d\n", gr, x,y,width,height));

  XSetForeground( X->dsp, gr->gc, gr->bg);
  XFillRectangle( X->dsp, gr->drw, gr->gc, x+gr->x0, y+gr->y0, width, height);
  XSetForeground( X->dsp, gr->gc, gr->fg);
  XFLUSH( X, False);
}


void
Java_java_awt_Toolkit_graDrawBytes ( JNIEnv* env, jclass clazz UNUSED,
				     jobject ngr, jbyteArray jBytes, jint offset, jint len, jint x, jint y )
{
  Graphics *gr = UNVEIL_GR(ngr);
  jboolean isCopy;
  int      n;
  jbyte    *jb, *jbo;
#ifdef KAFFE_I18N
  wchar_t *wch;
#endif  

  if ( !jBytes ) return;

  n = (*env)->GetArrayLength( env, jBytes);
  jb = (*env)->GetByteArrayElements( env, jBytes, &isCopy);
  jbo = jb + offset;

  DBG( AWT_GRA, printf("drawBytes: %p, %p,%d,%d  \"%s\"  %d,%d\n", gr,
				 jBytes,offset,len, jb, x,y));

  if ( offset+len > n )
	len = n - offset;

#ifdef KAFFE_I18N
  wch = jbyte2wchar( jbo, len );
  XwcDrawString( X->dsp, gr->drw, gr->oc, gr->gc, x+gr->x0, y+gr->y0, wch,len);
  free((void*) wch);  
#else  
  XDrawString( X->dsp, gr->drw, gr->gc, x+gr->x0, y+gr->y0, jbo, len);
#endif  

  (*env)->ReleaseByteArrayElements( env, jBytes, jb, JNI_ABORT);
  XFLUSH( X, False);
}


void
Java_java_awt_Toolkit_graDrawChars ( JNIEnv* env, jclass clazz UNUSED,
	jobject ngr, jcharArray jChars, jint offset, jint len, jint x, jint y )
{
  Graphics *gr = UNVEIL_GR(ngr);
  jboolean isCopy;
  int      n;
  jchar    *jc, *jco;
#ifdef KAFFE_I18N
  wchar_t *wch;  
#else  
  XChar2b  *b;
#endif  

  if ( !jChars ) return;

  n   = (*env)->GetArrayLength( env, jChars);
  jc = (*env)->GetCharArrayElements( env, jChars, &isCopy);
  jco = jc + offset;

  DBG( AWT_GRA, printf("drawChars: %p, %p,%d,%d  \"%s\"  %d,%d\n", gr, jChars,offset,len,
				 jchar2CString(X,jco,len), x,y));

  if ( offset+len > n )
	len = n - offset;

#ifdef KAFFE_I18N
  wch = jchar2wchar( jco, len );  
  XwcDrawString( X->dsp, gr->drw, gr->oc, gr->gc, x+gr->x0, y+gr->y0, wch,len);
  free((void*) wch);
#else /* !KAFFE_I18N */
	 
#ifndef WORDS_BIGENDIAN
  n = sizeof(XChar2b)*len;
  b = (XChar2b*) getBuffer( X, n);
  swab( (void*)jco, (void*)b, n);
#else
  b = (XChar2b*) jco;
#endif

  XDrawString16( X->dsp, gr->drw, gr->gc, x+gr->x0, y+gr->y0, b, len);
#endif  

  (*env)->ReleaseCharArrayElements( env, jChars, jc, JNI_ABORT);
  XFLUSH( X, False);
}


void
Java_java_awt_Toolkit_graDrawString ( JNIEnv* env, jclass clazz UNUSED,
				      jobject ngr, jstring str, jint x, jint y )
{
  Graphics *gr = UNVEIL_GR(ngr);
  jboolean     isCopy;
  int          len;

#ifndef KAFFE_I18N
  
#ifndef WORDS_BIGENDIAN
  int n;
#endif

#endif
  
  const jchar  *jc;
#ifdef KAFFE_I18N
  wchar_t *wch;  
#else  
  XChar2b      *b;
#endif  

  DBG( AWT_GRA, printf("drawString: %p  \"%s\"  %d,%d\n", gr, java2CString(env,X,str), x,y));

  if ( !str ) return;

  len = (*env)->GetStringLength( env, str);
  jc = (*env)->GetStringChars( env, str, &isCopy);

#ifdef KAFFE_I18N
  wch = jchar2wchar( jc, len );
  XwcDrawString( X->dsp, gr->drw, gr->oc, gr->gc, x+gr->x0, y+gr->y0, wch,len);
  free( (void *)wch );  
#else /* !KAFFE_I18N */
  
#ifndef WORDS_BIGENDIAN
  n = sizeof(XChar2b)*len;
  b = (XChar2b*) getBuffer( X, n);
  swab( jc, b, n);
#else
  b = (XChar2b*) jc;
#endif

  XDrawString16( X->dsp, gr->drw, gr->gc, x+gr->x0, y+gr->y0, b, len);  
#endif
  
  (*env)->ReleaseStringChars( env, str, jc);
  XFLUSH( X, False);
}


void
Java_java_awt_Toolkit_graDrawLine ( JNIEnv* env UNUSED, jclass clazz UNUSED, jobject ngr,
				    jint x1, jint y1, jint x2, jint y2 )
{
  Graphics *gr = UNVEIL_GR(ngr);

  DBG( AWT_GRA, printf("drawLine: %p, %d,%d - %d,%d\n", gr, x1,y1, x2,y2));

  if ( (x1==x2) && (y1==y2) ) {  /* swing gimmick */
	XDrawPoint( X->dsp, gr->drw, gr->gc, x1 +gr->x0, y1 +gr->y0);
  }
  else {
	XDrawLine( X->dsp, gr->drw, gr->gc, x1 + gr->x0, y1 + gr->y0, x2 + gr->x0, y2 + gr->y0);
  }
  XFLUSH( X, False);
}


void
Java_java_awt_Toolkit_graDrawArc ( JNIEnv* env UNUSED, jclass clazz UNUSED, jobject ngr,
								   jint x, jint y, jint width, jint height,
								   jint startAngle, jint arcAngle )
{
  Graphics *gr = UNVEIL_GR(ngr);
  DBG( AWT_GRA, printf("drawArc: %p, %d,%d-%d,%d  %d,%d\n", gr,
				 x,y,width,height, startAngle,arcAngle));

  XDrawArc( X->dsp, gr->drw, gr->gc, x+gr->x0, y+gr->y0, width, height,
			startAngle<<6, arcAngle<<6);
  XFLUSH( X, False);
}


void
Java_java_awt_Toolkit_graFillArc ( JNIEnv* env UNUSED, jclass clazz UNUSED, jobject ngr,
								   jint x, jint y, jint width, jint height,
								   jint startAngle, jint arcAngle )
{
  Graphics *gr = UNVEIL_GR(ngr);
  DBG( AWT_GRA, printf("fillArc: %p, %d,%d-%d,%d  %d,%d\n", gr,
				 x,y,width,height, startAngle, arcAngle));

  XFillArc( X->dsp, gr->drw, gr->gc, x+gr->x0, y+gr->y0, width, height,
			startAngle<<6, arcAngle<<6);
  XFLUSH( X, False);
}


void
Java_java_awt_Toolkit_graDrawOval ( JNIEnv* env UNUSED, jclass clazz UNUSED, jobject ngr,
									jint x, jint y, jint width, jint height )
{
  Graphics *gr = UNVEIL_GR(ngr);
  DBG( AWT_GRA, printf("drawOval: %p, %d,%d - %d,%d\n", gr, x,y,width,height));

  XDrawArc( X->dsp, gr->drw, gr->gc, x+gr->x0, y+gr->y0, width, height, 0, 23040);
  XFLUSH( X, False);
}


void
Java_java_awt_Toolkit_graFillOval ( JNIEnv* env UNUSED, jclass clazz UNUSED, jobject ngr,
									jint x, jint y, jint width, jint height )
{
  Graphics *gr = UNVEIL_GR(ngr);
  DBG( AWT_GRA, printf("fillOval: %p, %d,%d - %d,%d\n", gr, x,y,width,height));

  XFillArc( X->dsp, gr->drw, gr->gc, x+gr->x0, y+gr->y0, width, height, 0, 23040);
  XFLUSH( X, False);
}


int jarray2Points ( JNIEnv* env, Toolkit* tk, XPoint** pp, int x0, int y0,
			jintArray xPoints, jintArray yPoints, int nPoints )
{
  register int i;
  int      n;
  jboolean isCopy;
  jint     *jx = (*env)->GetIntArrayElements( env, xPoints, &isCopy);
  jint     *jy = (*env)->GetIntArrayElements( env, yPoints, &isCopy);

  if ( (n = (*env)->GetArrayLength( env, xPoints)) < nPoints ) nPoints = n;
  if ( (n = (*env)->GetArrayLength( env, yPoints)) < nPoints ) nPoints = n;

  *pp = getBuffer( tk, (nPoints+1)*sizeof(XPoint));

  for ( i=0; i<nPoints; i++){
	(*pp)[i].x = jx[i] + x0;
    (*pp)[i].y = jy[i] + y0;
  }

  (*env)->ReleaseIntArrayElements( env, xPoints, jx, JNI_ABORT);
  (*env)->ReleaseIntArrayElements( env, yPoints, jy, JNI_ABORT);

  return nPoints;
}


void
Java_java_awt_Toolkit_graDrawPolygon ( JNIEnv* env, jclass clazz UNUSED,
	jobject ngr, jintArray xPoints, jintArray yPoints, jint nPoints )
{
  Graphics *gr = UNVEIL_GR(ngr);
  int n;
  XPoint   *p;

  DBG( AWT_GRA, printf("drawPolygon: %p, %p,%p  %d\n", gr, xPoints, yPoints, nPoints));

  if ( !xPoints || !yPoints ) return;

  nPoints = jarray2Points( env, X, &p, gr->x0, gr->y0, xPoints, yPoints, nPoints);

  n = nPoints-1;
  if ( (p[0].x != p[n].x) || (p[0].y != p[n].y) ) {
	p[nPoints].x = p[0].x;
	p[nPoints].y = p[0].y;
	nPoints++;
  }

  XDrawLines( X->dsp, gr->drw, gr->gc, p, nPoints, CoordModeOrigin);
  XFLUSH( X, False);
}


void
Java_java_awt_Toolkit_graDrawPolyline ( JNIEnv* env, jclass clazz UNUSED, jobject ngr,
					jintArray xPoints, jintArray yPoints, jint nPoints )
{
  Graphics *gr = UNVEIL_GR(ngr);
  XPoint   *p;

  DBG( AWT_GRA, printf("drawPolyline: %p, %p,%p  %d\n", gr, xPoints, yPoints, nPoints));

  if ( !xPoints || !yPoints ) return;

  nPoints = jarray2Points( env, X, &p, gr->x0, gr->y0, xPoints, yPoints, nPoints);
  XDrawLines( X->dsp, gr->drw, gr->gc, p, nPoints, CoordModeOrigin);
  XFLUSH( X, False);
}


void
Java_java_awt_Toolkit_graFillPolygon ( JNIEnv* env, jclass clazz UNUSED,
	jobject ngr, jintArray xPoints, jintArray yPoints, jint nPoints )
{
  Graphics *gr = UNVEIL_GR(ngr);
  XPoint   *p;

  DBG( AWT_GRA, printf("fillPolygon: %p, %p,%p  %d\n", gr, xPoints, yPoints, nPoints));

  if ( !xPoints || !yPoints ) return;

  nPoints = jarray2Points( env, X, &p, gr->x0, gr->y0, xPoints, yPoints, nPoints);
  XFillPolygon( X->dsp, gr->drw, gr->gc, p, nPoints, Nonconvex, CoordModeOrigin);
  XFLUSH( X, False);
}


void
Java_java_awt_Toolkit_graDrawRect ( JNIEnv* env UNUSED, jclass clazz UNUSED, jobject ngr,
				    jint x, jint y, jint width, jint height )
{
  Graphics *gr = UNVEIL_GR(ngr);
  DBG( AWT_GRA, printf("drawRect: %p, %d,%d - %d,%d\n", gr, x,y,width,height));

  if ( (width >= 0) && (height >= 0) )
	XDrawRectangle( X->dsp, gr->drw, gr->gc, x+gr->x0, y+gr->y0, width, height);
  XFLUSH( X, False);
}


void
Java_java_awt_Toolkit_graFillRect ( JNIEnv* env UNUSED, jclass clazz UNUSED, jobject ngr,
				    jint x, jint y, jint width, jint height )
{
  Graphics *gr = UNVEIL_GR(ngr);
  DBG( AWT_GRA, printf("fillRect: %p, %d,%d - %d,%d\n", gr, x,y,width,height));

  if ( (width >= 0) && (height >= 0) ) {
	if ( width == 1 ) {   /* some swing gimmicks */
	  XDrawLine( X->dsp, gr->drw, gr->gc, x+gr->x0, y+gr->y0, x+gr->x0, y +gr->y0 +height -1);
	}
	else if ( height == 1 ){
	  XDrawLine( X->dsp, gr->drw, gr->gc, x+gr->x0, y+gr->y0, x +gr->x0 +width -1, y+gr->y0);
	}
	else {
	  XFillRectangle( X->dsp, gr->drw, gr->gc, x+gr->x0, y+gr->y0, width, height);
	}
  }
  XFLUSH( X, False);
}



/*
 * we don't use the Xmu routines here because it would drag the whole Xt libs in
 */
void
Java_java_awt_Toolkit_graDrawRoundRect ( JNIEnv* env UNUSED, jclass clazz UNUSED, jobject ngr,
					 jint x, jint y, jint width, jint height, 
					 jint wArc, jint hArc )
{
  Graphics *gr = UNVEIL_GR(ngr);
  int x1, x2, y1, y2, a, b;

  DBG( AWT_GRA, printf("drawRoundRect: %p, %d,%d - %d,%d  %d,%d\n", gr,
				 x,y,width,height, wArc, hArc));
  
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

  XFLUSH( X, False);
}

void
Java_java_awt_Toolkit_graFillRoundRect ( JNIEnv* env UNUSED, jclass clazz UNUSED, jobject ngr,
					 jint x, jint y, jint width, jint height, 
					 jint wArc, jint hArc )
{
  Graphics *gr = UNVEIL_GR(ngr);
  int x1, x2, y1, y2, a, b;

  DBG( AWT_GRA, printf("fillRoundRect: %p, %d,%d - %d,%d  %d,%d\n", gr,
				 x,y,width,height, wArc, hArc));
  
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

  XFLUSH( X, False);
}


void
Java_java_awt_Toolkit_graDraw3DRect ( JNIEnv* env, jclass clazz, jobject ngr,
				      jint x, jint y, jint width, jint height,
				      jboolean raised, jint rgb )
{
  Graphics *gr = UNVEIL_GR(ngr);
  int      bright, dark;
  int      xw, yh;

  DBG( AWT_GRA, printf("draw3DRect: %p, %d,%d - %d,%d  %d %x\n", gr,
				 x,y,width,height, raised, rgb));

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

  XFLUSH( X, False);
}

void
Java_java_awt_Toolkit_graFill3DRect ( JNIEnv* env, jclass clazz, jobject ngr,
				      jint x, jint y, jint width, jint height,
				      jboolean raised, jint rgb )
{
  Graphics *gr = UNVEIL_GR(ngr);
  DBG( AWT_GRA, printf("fill3DRect: %p, %d,%d - %d,%d  %d %x\n", gr,
				 x,y,width,height, raised, rgb));

  XFillRectangle( X->dsp, gr->drw, gr->gc, x+gr->x0+1, y+gr->y0+1, width-2, height-2);

  Java_java_awt_Toolkit_graDraw3DRect( env, clazz, ngr, x, y, width-1, height-1, raised, rgb);
}


void
Java_java_awt_Toolkit_graAddClip ( JNIEnv* env UNUSED, jclass clazz UNUSED, jobject gr UNUSED,
				   jint xClip UNUSED, jint yClip UNUSED, jint wClip UNUSED, jint hClip UNUSED )
{
	/* not implemented yet */
}


void
Java_java_awt_Toolkit_graSetClip ( JNIEnv* env, jclass clazz UNUSED, jobject ngr,
				   jint xClip, jint yClip, jint wClip, jint hClip )
{
  Graphics *gr = UNVEIL_GR(ngr);
  XRectangle rect;

  DBG( AWT_GRA, printf("setClip: %p, %d,%d - %d,%d\n", gr, xClip, yClip, wClip, hClip));

  rect.x      = xClip;
  rect.y      = yClip;
  rect.width  = (wClip > 0) ? wClip : 0;
  rect.height = (hClip > 0) ? hClip : 0;
  XSetClipRectangles( X->dsp, gr->gc, gr->x0, gr->y0, &rect, 1, Unsorted);
}

void
Java_java_awt_Toolkit_graSetColor ( JNIEnv* env, jclass clazz UNUSED, jobject ngr, jint clr )
{
  Graphics *gr = UNVEIL_GR(ngr);
  DBG( AWT_GRA, printf("setColor: %p, %x\n", gr, clr));

  gr->fg = clr;

  if ( gr->xor )
	clr ^= gr->xclr;

  XSetForeground( X->dsp, gr->gc, clr);
}

void
Java_java_awt_Toolkit_graSetBackColor ( JNIEnv* env, jclass clazz UNUSED, jobject ngr, jint clr )
{
  Graphics *gr = UNVEIL_GR(ngr);
  DBG( AWT_GRA, printf("setBackColor: %p, %x\n", gr, clr));

  gr->bg = clr;
  XSetBackground( X->dsp, gr->gc, clr);
}

void
Java_java_awt_Toolkit_graSetFont ( JNIEnv* env, jclass clazz UNUSED, jobject ngr, jobject fnt )
{
  Graphics *gr = UNVEIL_GR(ngr);
  DBG( AWT_GRA, printf("setFont: %p, %p\n", gr, fnt));
#ifdef KAFFE_I18N
  gr->oc=UNVEIL_XOC(fnt);  
#else
  XSetFont( X->dsp, gr->gc, UNVEIL_FS(fnt)->fid);
#endif  
}

void
Java_java_awt_Toolkit_graSetOffset ( JNIEnv* env, jclass clazz UNUSED, jobject ngr, jint xOff, jint yOff )
{
  Graphics *gr = UNVEIL_GR(ngr);
  DBG( AWT_GRA, printf("setOffset: %p, %d,%d\n", gr, xOff, yOff));

  gr->x0 = xOff;
  gr->y0 = yOff;
}

void
Java_java_awt_Toolkit_graSetPaintMode ( JNIEnv* env, jclass clazz UNUSED, jobject ngr )
{
  Graphics *gr = UNVEIL_GR(ngr);
  DBG( AWT_GRA, printf("setPaintMode: %p\n", gr));

  gr->xor = 0;
  XSetForeground( X->dsp, gr->gc, gr->fg);
  XSetFunction( X->dsp, gr->gc, GXcopy);
}

void
Java_java_awt_Toolkit_graSetXORMode ( JNIEnv* env, jclass clazz UNUSED, jobject ngr, jint xorClr )
{
  Graphics *gr = UNVEIL_GR(ngr);
  DBG( AWT_GRA, printf("setXORMode: %p, %x\n", gr, xorClr));

  gr->xor = 1;
  gr->xclr = xorClr;
  XSetForeground( X->dsp, gr->gc, gr->fg ^ xorClr);
  XSetFunction( X->dsp, gr->gc, GXxor);
}

void
Java_java_awt_Toolkit_graSetVisible ( JNIEnv* env, jclass clazz UNUSED, jobject ngr, jint isVisible )
{
  Graphics *gr = UNVEIL_GR(ngr);
  DBG( AWT_GRA, printf("setVisble: %p  %d\n", gr, isVisible));

  /*
   * This is rather a hack to "defuse" a Graphics object, but we don't want to
   * add checks on every draw op to test if the target is invisible
   */
  if ( !isVisible ) {
	if ( gr->y0 >= -10000 ) 
	  gr->y0 -= 100000;
  }
  else {
	if ( gr->y0 <= -10000 )
	  gr->y0 += 100000;
  }
}


/************************************************************************************
 * image rendering
 */

static void
drawAlphaImage ( Graphics *gr, Image* img,
		 jint srcX, jint srcY, jint dstX, jint dstY,
		 jint width, jint height, jint bgval )
{
  Image  *dstImg = 0;
  XImage *dstXim = 0;
  int    i, j, si, sj, alpha;
  unsigned long dpix, spix, bgpix = 0;
  int    sr, sg, sb, dr, dg, db;

  DBG( AWT_GRA, printf("drawAlphaImage: %p %p (%p, %p),  %d,%d  %d,%d  %d,%d  %x\n",
				 gr, img, img->xImg, img->alpha, srcX, srcY, dstX, dstY, width, height, bgval));

  if ( !img ) return;

  if ( X->shm == USE_SHM ){
	dstImg = createImage( width, height);
	createXImage( X, dstImg);
	if ( dstImg->shmiImg ){
	  dstXim = dstImg->xImg;
	  XShmGetImage( X->dsp, gr->drw, dstXim, dstX, dstY, 0xffffffff);
	}
	else {  /* Shm failed, backup to normal XImage */
	  imgFreeImage( dstImg);
	  dstImg = 0;
	}
  }

  if ( !dstXim ) {
	dstXim = XGetImage( X->dsp, gr->drw, dstX, dstY, width, height, 0xffffffff, ZPixmap);
  }

  if ( bgval != -1 )
	bgpix = pixelValue( X, bgval);

  if ( dstXim ) {
	for ( j=0, sj=srcY; j<dstXim->height; j++, sj++ ) {
	  for ( i=0, si=srcX; i<dstXim->width; i++, si++ ) {
		dpix = (bgval == -1) ? XGetPixel( dstXim, i, j) : bgpix;
		spix = XGetPixel( img->xImg, si, sj);
		alpha = GetAlpha( img->alpha, si, sj);

		rgbValues( X, dpix, &dr, &dg, &db);

		if ( alpha ) {
		  /*
		   * ints still considered to be substantially faster than floats on most
		   * architectures (avoid div by 255), map all 0..255 -> 1..256
		   */
		  alpha++; sr++; sg++; sb++; dr++; dg++; db++;

		  rgbValues( X, spix, &sr, &sg, &sb);
		  dr = ((alpha * sr + (256 - alpha) * dr) + 128) >> 8;
		  dg = ((alpha * sg + (256 - alpha) * dg) + 128) >> 8;
		  db = ((alpha * sb + (256 - alpha) * db) + 128) >> 8;

		  dr--; dg--; db--;

		  XPutPixel( dstXim, i, j, pixelValue( X, (dr << 16)|(dg << 8)|(db) ));
		}
	  }
	}

	if ( dstImg != 0 ){
	  XShmPutImage( X->dsp, gr->drw, gr->gc, dstXim, 0, 0, dstX, dstY, width, height, False);
	  XSync( X->dsp, False);
	  imgFreeImage( dstImg);
	}
	else {
	  XPutImage( X->dsp, gr->drw, gr->gc, dstXim, 0, 0, dstX, dstY, width, height);
	  XFlush( X->dsp);
	  XDestroyImage( dstXim);
	}

	XFLUSH( X, True);
  }
}

static void
drawImage (Graphics *gr, Image *img, 
	   jint srcX, jint srcY,
	   jint dstX, jint dstY,
	   jint width, jint height, jint bgval )
{
  XGCValues values;

  DBG( AWT_GRA, printf("drawImage: %p %p (%p,%p,%p %d,%d) %d,%d, %d,%d, %d,%d, %x\n",
				 gr, img,  img->xImg,img->xMask,img->alpha,  img->width,img->height,
				 srcX,srcY,  dstX,dstY,	 width,height, bgval));

  if ( !img ) return;

  dstX += gr->x0;
  dstY += gr->y0;

  dstX += img->left;
  dstY += img->top;

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
	  if ( img->shmiMask ){
		XShmPutImage( X->dsp, gr->drw, gr->gc, img->xMask,
					  srcX, srcY, dstX, dstY, width, height, False);
	  }
	  else {
		XPutImage( X->dsp, gr->drw, gr->gc, img->xMask,
				   srcX, srcY, dstX, dstY, width, height);
	  }

	  /* restore gc values except of the function */
	  XChangeGC( X->dsp, gr->gc, GCFunction|GCPlaneMask|GCForeground|GCBackground, &values);
	  XSetFunction( X->dsp, gr->gc, GXor);
	}
	else if ( img->alpha ) {
	  drawAlphaImage( gr, img, srcX, srcY, dstX, dstY, width, height, bgval);
	  return;
	}

	/* this is the real image drawing */
	if ( img->shmiImg ){
	  XShmPutImage( X->dsp, gr->drw, gr->gc, img->xImg,
					srcX, srcY, dstX, dstY, width, height, False);
	}
	else {
	  XPutImage( X->dsp, gr->drw, gr->gc, img->xImg,
				 srcX, srcY, dstX, dstY, width, height);
	}

	/* in case we had a mask, restore the original function */
	if ( img->xMask )
	  XSetFunction( X->dsp, gr->gc, values.function);
  }

  XFLUSH( X, True);
}


void
Java_java_awt_Toolkit_graDrawImage ( JNIEnv* env UNUSED, jclass clazz UNUSED, jobject ngr, jobject nimg,
				     jint srcX, jint srcY,
				     jint dstX, jint dstY,
				     jint width, jint height, jint bgval )
{
  if (nimg == NULL)
    return;

  drawImage(UNVEIL_GR(ngr), UNVEIL_IMG(nimg), srcX, srcY, dstX, dstY, width, height, bgval);
}

void
Java_java_awt_Toolkit_graDrawImageScaled ( JNIEnv* env, jclass clazz, jobject ngr, jobject nimg,
					   jint dx0, jint dy0, jint dx1, jint dy1,
					   jint sx0, jint sy0, jint sx1, jint sy1, jint bgval )
{
  Graphics *gr = UNVEIL_GR(ngr);
  Image *img = UNVEIL_IMG(nimg);
  int        iw, x0, y0, x1, y1;
  Image      *tgt;
  int        tmpXImg = (img->xImg == NULL);

  DBG( AWT_GRA, printf("drawImageScaled: %p %p (%p), %d,%d,%d,%d, %d,%d,%d,%d, %x\n",
				 gr, img, img->xImg, dx0, dy0, dx1, dy1, sx0, sy0, sx1, sy1, bgval));

  if ( !img ) return;

  /* screen images don't have an XImage, we have to get the data first */
  if ( tmpXImg ) {
  	img->xImg = XGetImage( X->dsp, img->pix, 0, 0, img->width, img->height,
						   0xffffffff, ZPixmap);
  }
  iw = img->xImg->width;

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

  tgt = createImage( (x1 - x0 +1), (y1 - y0 +1));
  /* alpha image should always go first, since it would disable subsequent Shm usage */
  if ( img->alpha )
	createAlphaImage( X, tgt);
  if ( img->xMask )
	createXMaskImage( X, tgt);
  createXImage( X, tgt);

  initScaledImage( X, tgt, img, dx0-x0, dy0-y0, dx1-x0, dy1-y0, sx0, sy0, sx1, sy1);
  drawImage ( gr, tgt, 0, 0,
	      x0, y0, tgt->width, tgt->height, bgval);
  if ( tgt->shmiImg ) {
	XSync( X->dsp, False); /* since we're going to destroy tgt, process its drawing first */
  }
  imgFreeImage( tgt );

  if ( tmpXImg ) {
	XDestroyImage( img->xImg);
	img->xImg = NULL;
  }
}
