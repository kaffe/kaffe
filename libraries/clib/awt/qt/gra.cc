/**
 * gra.c - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include <qimage.h>
#include <qpaintdevice.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qwidget.h>

#include "toolkit.h"

#define  WINDOW   0
#define  IMAGE    1
#define  GRAPHICS 2

#define QCOLOR(c) QColor(JRED(c), JGREEN(c), JBLUE(c))

/*******************************************************************************
 *
 */


void* Java_java_awt_Toolkit_graInitGraphics(JNIEnv* env, jclass clazz,
  Graphics* gr, jobject tgt, jint tgtType, jint xOff, jint yOff,
  jint xClip, jint yClip, jint wClip, jint hClip,
  jobject fnt, jint fg, jint bg, jboolean blank)
{
//  Drawable       drw;
  QPaintDevice*       drw;

  AWT_DBG(printf("initGraphics: gr %p (tgt %p, type=%d) xo %d,yo %d  %d,%d - %d,%d  %p %x %x\n",
    gr, tgt,tgtType, xOff,yOff, xClip,yClip,wClip,hClip, fnt,fg,bg));

  if ( tgt ) {
    switch ( tgtType ) {
      case 0:
        drw = (QWidget*)tgt;
        break;          /* Window   */
      case 1:
//        drw = ((Image*)tgt)->pix;
        drw = (QPixmap*)(((Image*)tgt)->qpm);
        AWT_DBG(printf("tgt = IMAGE drw=%p\n",drw));
        break;     /* Image    */
      case 2:
        drw = ((Graphics*)tgt)->drw;
        AWT_DBG(printf("tgt = GRAPHICS\n"));
        break;  /* Graphics */
      default:
        fprintf( stderr, "illegal Graphics target: %d\n", tgtType);
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

  if(gr == 0) {
    gr = (Graphics*) AWT_MALLOC( sizeof(Graphics));
    memset((void*)gr, 0, sizeof(Graphics));
  }

  //if (gr->painter == 0) {
    gr->painter = new QPainter(drw);
    AWT_DBG(printf("painter=%x\n", gr->painter));
  //}

  gr->painter->setPen(QCOLOR(fg));
  gr->painter->setBrush(QCOLOR(fg));
  gr->painter->setBackgroundColor(QCOLOR(bg));
  gr->painter->setFont(*((QFont*)fnt));
  gr->painter->setRasterOp(Qt::CopyROP);

  if (wClip < 0) 
    wClip = 0;
  if (hClip < 0) 
    hClip = 0;

  gr->painter->setClipRect(xOff+xClip, yOff+yClip, wClip, hClip);

  gr->fg        = fg;
  gr->bg        = bg;
  gr->drw       = drw;
  gr->x0        = xOff;
  gr->y0        = yOff;
  gr->xor_mode  = 0;

  if ( blank ) {
    gr->painter->fillRect(xOff+xClip, yOff+yClip, wClip, hClip, QCOLOR(bg));
  }

  return gr;
}


void Java_java_awt_Toolkit_graFreeGraphics(JNIEnv* env, jclass clazz,
  Graphics* gr)
{
  AWT_DBG(printf("freeGraphics: %p\n", gr));

  if(gr->painter)
    delete gr->painter;
  AWT_FREE( gr);
}


void Java_java_awt_Toolkit_graCopyArea(JNIEnv* env, jclass clazz,
  Graphics* gr, jint x, jint y, jint width, jint height,
  jint xDelta, jint yDelta )
{
  AWT_DBG(printf("copyArea: %p, %d,%d-%d,%d  %d,%d\n", gr, x,y,width,height, xDelta,yDelta));

  /* some X servers act alleric on that (careless bitblt, again) */
  if ( (width < 0) || (height < 0) )
	return;

  x += gr->x0; y += gr->y0;
//  XCopyArea( X->dsp, gr->drw, gr->drw, gr->gc, x, y, width, height, x + xDelta, y + yDelta);
  XFLUSH( X, False);
}


void Java_java_awt_Toolkit_graClearRect(JNIEnv* env, jclass clazz,
  Graphics* gr, jint x, jint y, jint width, jint height)
{
  AWT_DBG(printf("clearRect: %p, %d,%d-%d,%d\n", gr, x,y,width,height));

  QPainter *painter = (QPainter*)gr->painter;
  painter->fillRect(x + gr->x0, y + gr->y0, width, height, QCOLOR(gr->bg));
}


void
Java_java_awt_Toolkit_graDrawBytes ( JNIEnv* env, jclass clazz,
	Graphics* gr, jbyteArray jBytes, jint offset, jint len, jint x, jint y )
{
  jboolean isCopy;
  int      n;
  jbyte    *jb, *jbo;
  QPainter *painter;
  painter = (QPainter *)gr->painter;
  if ( !jBytes ) return;

  n = env->GetArrayLength( jBytes);
  jb = env->GetByteArrayElements( jBytes, &isCopy);
  jbo = jb + offset;

  AWT_DBG(printf("drawBytes: %p, %p,%d,%d  \"%s\"  %d,%d\n", gr,
				 jBytes,offset,len, jb, x,y));

  if ( offset+len > n )
	len = n - offset;

//  XDrawString( X->dsp, gr->drw, gr->gc, x+gr->x0, y+gr->y0, jbo, len);
  QByteArray a;
  a.setRawData((char *)jbo,len);
  painter->drawText( x+gr->x0, y+gr->y0, QString(a), len);
  env->ReleaseByteArrayElements( jBytes, jb, JNI_ABORT);
  XFLUSH( X, False);
}


void
Java_java_awt_Toolkit_graDrawChars ( JNIEnv* env, jclass clazz,
	Graphics* gr, jcharArray jChars, jint offset, jint len, jint x, jint y )
{
  jboolean isCopy;
  int      n;
  jchar    *jc, *jco;
  //XChar2b  *b;
  ushort  *b;
  QChar   *unicode;
  QPainter *painter;
  painter = (QPainter *)gr->painter;

  if ( !jChars ) return;

  n   = env->GetArrayLength( jChars);
  jc = env->GetCharArrayElements( jChars, &isCopy);
  jco = jc + offset;

  AWT_DBG(printf("drawChars: %p, %p,%d,%d  \"%s\"  %d,%d\n", gr, jChars,offset,len,
				 jchar2CString(X,jco,len), x,y));

  if ( offset+len > n )
	len = n - offset;

#ifndef WORDS_BIGENDIAN
  //n = sizeof(XChar2b)*len;
  //b = (XChar2b*) getBuffer( X, n);
  n = sizeof(ushort)*len;
  b = (ushort *) getBuffer( X, n);
  swab( (void*)jco, (void*)b, n);
#else
  //b = (XChar2b*) jco;
  b = (ushort *) jco;
#endif
  int i;
  unicode = new QChar[len];
  for(i=0;i<len;i++)
    unicode[i] = QChar((ushort)*(b+i));

//  XDrawString16( X->dsp, gr->drw, gr->gc, x+gr->x0, y+gr->y0, b, len);  
  painter->drawText( x+gr->x0, y+gr->y0, QString(jchar2CString(X,jco,len)),len);
  env->ReleaseCharArrayElements( jChars, jc, JNI_ABORT);
  XFLUSH( X, False);
}


void Java_java_awt_Toolkit_graDrawString(JNIEnv* env, jclass clazz,
  Graphics* gr, jstring str, jint x, jint y)
{
  char *cstr;
  QPainter *painter;

  if ( !str ) return;

  cstr = java2CString(env, X, str);
  AWT_DBG(printf("drawString: %p  \"%s\"  %d,%d\n", gr, cstr, x,y));
  painter = (QPainter*)gr->painter;

  QString qstr(cstr);
  painter->drawText(x+gr->x0, y+gr->y0, qstr);
}


void Java_java_awt_Toolkit_graDrawLine(JNIEnv* env, jclass clazz,
  Graphics* gr, jint x1, jint y1, jint x2, jint y2)
{
  DBG( AWT_GRA, printf("drawLine: %p, %d,%d - %d,%d\n", gr, x1,y1, x2,y2));

  QPainter *painter = (QPainter*)gr->painter;
  if( (x1==x2) && (y1==y2) ) { //swing gimmicks
    painter->drawPoint( x1 + gr->x0, y1 + gr->y0);
  }
  else {
    painter->drawLine( x1 + gr->x0, y1 + gr->y0, x2 + gr->x0, y2 + gr->y0);
  }
}


void Java_java_awt_Toolkit_graDrawArc(JNIEnv* env, jclass clazz,
  Graphics* gr, jint x, jint y, jint width, jint height,
  jint startAngle, jint arcAngle)
{
  DBG( AWT_GRA, printf("drawArc: %p, %d,%d-%d,%d  %d,%d\n", gr,
				 x,y,width,height, startAngle,arcAngle));

  QPainter *painter = (QPainter*)gr->painter;

  painter->drawArc(x+gr->x0, y+gr->y0, width, height,
    startAngle << 4, arcAngle << 4);
}


void Java_java_awt_Toolkit_graFillArc(JNIEnv* env, jclass clazz,
  Graphics* gr, jint x, jint y, jint width, jint height,
  jint startAngle, jint arcAngle)
{
  DBG( AWT_GRA, printf("fillArc: %p, %d,%d-%d,%d  %d,%d\n", gr,
				 x,y,width,height, startAngle, arcAngle));

  QPainter *painter = (QPainter*)gr->painter;

  painter->drawPie(x+gr->x0, y+gr->y0, width, height,
    startAngle << 4, arcAngle << 4);
}


void Java_java_awt_Toolkit_graDrawOval(JNIEnv* env, jclass clazz,
  Graphics* gr, int x, jint y, jint width, jint height)
{
  DBG( AWT_GRA, printf("drawOval: %p, %d,%d - %d,%d\n", gr, x,y,width,height));

  QPainter *painter = (QPainter*)gr->painter;

  painter->drawArc(x+gr->x0, y+gr->y0, width, height,
    0, 360 << 4);
}


void Java_java_awt_Toolkit_graFillOval(JNIEnv* env, jclass clazz,
  Graphics* gr, jint x, jint y, jint width, jint height)
{
  DBG( AWT_GRA, printf("fillOval: %p, %d,%d - %d,%d\n", gr, x,y,width,height));

  QPainter *painter = (QPainter*)gr->painter;

  painter->drawPie(x+gr->x0, y+gr->y0, width, height,
    0, 360 << 4);
}


QPointArray* jarray2Points(JNIEnv* env, Toolkit* X, int x0, int y0,
  jintArray xPoints, jintArray yPoints, int nPoints)
{
  register int i;
  int      n;
  jboolean isCopy;
  jint     *jx = env->GetIntArrayElements( xPoints, &isCopy);
  jint     *jy = env->GetIntArrayElements( yPoints, &isCopy);

  if ( (n = env->GetArrayLength( xPoints)) < nPoints ) nPoints = n;
  if ( (n = env->GetArrayLength( yPoints)) < nPoints ) nPoints = n;

  QPointArray *points = new QPointArray(nPoints);

  for ( i=0; i<nPoints; i++){
    points->setPoint(i, jx[i] + x0, jy[i] + y0);
  }

  env->ReleaseIntArrayElements( xPoints, jx, JNI_ABORT);
  env->ReleaseIntArrayElements( yPoints, jy, JNI_ABORT);

  return points;
}


void Java_java_awt_Toolkit_graDrawPolygon(JNIEnv* env, jclass clazz,
  Graphics* gr, jintArray xPoints, jintArray yPoints, jint nPoints )
{
  int n;

  DBG( AWT_GRA, printf("drawPolygon: %p, %p,%p  %d\n", gr, xPoints, yPoints, nPoints));

  if ( !xPoints || !yPoints ) return;

  QPainter *painter = (QPainter*)gr->painter;
  QPointArray *points = jarray2Points(env, X, gr->x0, gr->y0,
    xPoints, yPoints, nPoints);
  uint size = points->size();
  if(points->point(0) != points->point(size-1)) {
    points->resize(size+1);
    points->setPoint(size, points->point(0));
  }
  painter->drawPolyline(*points);
}


void Java_java_awt_Toolkit_graDrawPolyline(JNIEnv* env, jclass clazz,
  Graphics* gr, jintArray xPoints, jintArray yPoints, jint nPoints)
{
  //XPoint   *p;

  DBG( AWT_GRA, printf("drawPolyline: %p, %p,%p  %d\n", gr, xPoints, yPoints, nPoints));

  if ( !xPoints || !yPoints ) return;

  QPainter *painter = (QPainter*)gr->painter;
  QPointArray *points = jarray2Points(env, X, gr->x0, gr->y0,
    xPoints, yPoints, nPoints);
  painter->drawPolyline(*points);
}


void Java_java_awt_Toolkit_graFillPolygon ( JNIEnv* env, jclass clazz,
  Graphics* gr, jintArray xPoints, jintArray yPoints, jint nPoints)
{
  //XPoint   *p;

  DBG( AWT_GRA, printf("fillPolygon: %p, %p,%p  %d\n", gr, xPoints, yPoints, nPoints));

  if ( !xPoints || !yPoints ) return;

  QPainter *painter = (QPainter*)gr->painter;
  QPointArray *points = jarray2Points(env, X, gr->x0, gr->y0,
    xPoints, yPoints, nPoints);
  painter->drawPolygon(*points);
}


void Java_java_awt_Toolkit_graDrawRect(JNIEnv* env, jclass clazz,
  Graphics* gr, jint x, jint y, jint width, jint height)
{
  DBG( AWT_GRA, printf("drawRect: %p, %d,%d - %d,%d\n", gr, x,y,width,height));

  QPainter *painter = (QPainter*)gr->painter;

  if( (width >= 0) && (height >= 0) )
    painter->drawRect(x+gr->x0, y+gr->y0, width, height);
}


void Java_java_awt_Toolkit_graFillRect(JNIEnv* env, jclass clazz,
  Graphics* gr, jint x, jint y, jint width, jint height)
{
  DBG( AWT_GRA, printf("fillRect: %p, %d,%d - %d,%d\n", gr, x,y,width,height));

  QPainter *painter = (QPainter*)gr->painter;

  if( (width >= 0) && (height >= 0) ) {
    if(width == 1) {  //some swing gimmicks
      painter->drawLine( x+gr->x0, y+gr->y0, x+gr->x0, y+gr->y0 + height -1);
    }
    else if ( height == 1) {
      painter->drawLine( x+gr->x0, y+gr->y0, x+gr->x0 + width -1, y+gr->y0);
    }
    else {
      painter->fillRect(x+gr->x0, y+gr->y0, width, height, QCOLOR(gr->fg));
    }
  }
}

void Java_java_awt_Toolkit_graDrawRoundRect(JNIEnv* env, jclass clazz,
  Graphics* gr, jint x, jint y, jint width, jint height, 
  jint wArc, jint hArc )
{
  int x1, x2, y1, y2, a, b;

  DBG( AWT_GRA, printf("drawRoundRect: %p, %d,%d - %d,%d  %d,%d\n", gr,
				 x,y,width,height, wArc, hArc));

  QPainter *painter = (QPainter*)gr->painter;
  painter->drawRoundRect(x+gr->x0, y+gr->y0, width, height, wArc, hArc);
#if 0  
  x += gr->x0;  y += gr->y0;
  a = wArc / 2; b = hArc / 2;

  x1 = x + a;   x2 = x + width - a;  y2 = y + height;
//  XDrawLine( X->dsp, gr->drw, gr->gc, x1, y, x2, y);
//  XDrawLine( X->dsp, gr->drw, gr->gc, x1, y2, x2, y2);

  y1 = y + b;   y2 = y + height - b; x2 = x + width;
//  XDrawLine( X->dsp, gr->drw, gr->gc, x, y1, x, y2);
//  XDrawLine( X->dsp, gr->drw, gr->gc, x2, y1, x2, y2);

//  XDrawArc( X->dsp, gr->drw, gr->gc, x, y, wArc, hArc, 90*64, 90*64);

  x2 = x + width - wArc;
//  XDrawArc( X->dsp, gr->drw, gr->gc, x2, y, wArc, hArc, 0, 90*64);

  y2 = y + height - hArc;
//  XDrawArc( X->dsp, gr->drw, gr->gc, x2, y2, wArc, hArc, 0, -90*64);

//  XDrawArc( X->dsp, gr->drw, gr->gc, x, y2, wArc, hArc, 180*64, 90*64);
#endif
}

void
Java_java_awt_Toolkit_graFillRoundRect ( JNIEnv* env, jclass clazz, Graphics* gr,
										 jint x, jint y, jint width, jint height, 
										 jint wArc, jint hArc )
{
  int x1, x2, y1, y2, a, b;

  DBG( AWT_GRA, printf("fillRoundRect: %p, %d,%d - %d,%d  %d,%d\n", gr,
				 x,y,width,height, wArc, hArc));

  QPainter *painter = (QPainter*)gr->painter;

#if 0  
  x += gr->x0;  y += gr->y0;
  a = wArc / 2; b = hArc / 2;

  y1 = y + b;  y2 = y + height - b;
//  if ( y2 > y1 )
//	XFillRectangle( X->dsp, gr->drw, gr->gc, x, y1, width, y2-y1);
  
  x1 = x + a;  x2 = x + width - a;
//  if ( x2 > x1 ) {
//	XFillRectangle( X->dsp, gr->drw, gr->gc, x1, y, (x2-x1), b);
//	XFillRectangle( X->dsp, gr->drw, gr->gc, x1, y2, (x2-x1), b);
//  }

//  XFillArc( X->dsp, gr->drw, gr->gc, x, y, wArc, hArc, 90*64, 90*64);

  x2 = x + width - wArc;
//  XFillArc( X->dsp, gr->drw, gr->gc, x2, y, wArc, hArc, 0, 90*64);

  y2 = y + height - hArc;
//  XFillArc( X->dsp, gr->drw, gr->gc, x2, y2, wArc, hArc, 0, -90*64);

//  XFillArc( X->dsp, gr->drw, gr->gc, x, y2, wArc, hArc, 180*64, 90*64);
#endif
}

void Java_java_awt_Toolkit_graDraw3DRect(JNIEnv* env, jclass clazz,
  Graphics* gr, jint x, jint y, jint width, jint height,
  jboolean raised, jint rgb)
{
  int      bright, dark;
  int      xw, yh;

  DBG( AWT_GRA, printf("draw3DRect: %p, %d,%d - %d,%d  %d %x\n", gr,
				 x,y,width,height, raised, rgb));

  QPainter *painter = (QPainter*)gr->painter;

  /* we should use the same mechanism like ordinary Graphics brighter()/darker() here */
  dark   = (int) (Java_java_awt_Toolkit_clrDark( env, clazz, rgb) >> 32);
  bright = (int) (Java_java_awt_Toolkit_clrBright( env, clazz, rgb) >> 32);

  x += gr->x0; y += gr->y0;
  xw = x + width;
  yh = y + height;

  painter->setPen(QCOLOR(raised ? bright : dark));
  painter->drawLine(x, y, xw-1, y);
  painter->drawLine(x, y, x, yh);  

  painter->setPen(QCOLOR(raised ? dark : bright));
  painter->drawLine(x+1, yh, xw, yh);
  painter->drawLine(xw, y, xw, yh);

  painter->setPen(QCOLOR(gr->fg));
}

void Java_java_awt_Toolkit_graFill3DRect(JNIEnv* env, jclass clazz,
  Graphics* gr, jint x, jint y, jint width, jint height,
  jboolean raised, jint rgb)
{
  DBG( AWT_GRA, printf("fill3DRect: %p, %d,%d - %d,%d  %d %x\n", gr,
				 x,y,width,height, raised, rgb));

  QPainter *painter = (QPainter*)gr->painter;

  painter->fillRect(x+gr->x0+1, y+gr->y0+1, width-2, height-2,
    QCOLOR(gr->fg));

  Java_java_awt_Toolkit_graDraw3DRect(env, clazz, gr, x, y,
    width-1, height-1, raised, rgb);
}


void Java_java_awt_Toolkit_graAddClip(JNIEnv* env, jclass clazz,
  Graphics* gr, jint xClip, jint yClip, jint wClip, jint hClip)
{
	/* not implemented yet */
}


void Java_java_awt_Toolkit_graSetClip(JNIEnv* env, jclass clazz,
  Graphics* gr, jint xClip, jint yClip, jint wClip, jint hClip)
{
  //XRectangle rect;

  DBG( AWT_GRA, printf("setClip: %p, %d,%d - %d,%d\n", gr, xClip, yClip, wClip, hClip));

  if(wClip <= 0)
    wClip = 0;

  if(yClip <= 0)
    yClip = 0;

  QPainter *painter = (QPainter*)gr->painter;

  painter->setClipRect(gr->x0 + xClip, gr->y0 + yClip, wClip, hClip);
}

void Java_java_awt_Toolkit_graSetColor(JNIEnv* env, jclass clazz,
  Graphics* gr, jint clr)
{
  AWT_DBG(printf("setColor: gr=%p, clr=%x, painter=%x\n", gr, clr,
    gr->painter));

  gr->fg = clr;

  if ( gr->xor_mode )
	clr ^= gr->xclr;

  QPainter *painter = (QPainter*)gr->painter;

  painter->setPen(QCOLOR(clr));
  painter->setBrush(QCOLOR(clr));
}

void Java_java_awt_Toolkit_graSetBackColor(JNIEnv* env, jclass clazz,
  Graphics* gr, jint clr)
{
  DBG( AWT_GRA, printf("setBackColor: %p, %x\n", gr, clr));

  gr->bg = clr;

  QPainter *painter = (QPainter*)gr->painter;

  painter->setBackgroundColor(QCOLOR(clr));
}

void Java_java_awt_Toolkit_graSetFont(JNIEnv* env, jclass clazz,
  Graphics* gr, jobject fnt)
{
  DBG( AWT_GRA, printf("setFont: %p, %p\n", gr, fnt));

  QPainter *painter = (QPainter*)gr->painter;
  gr->painter->setFont(*((QFont*)fnt));
//  XSetFont( X->dsp, gr->gc, ((XFontStruct*)fnt)->fid);
}

void Java_java_awt_Toolkit_graSetOffset(JNIEnv* env, jclass clazz,
  Graphics* gr, jint xOff, jint yOff)
{
  DBG( AWT_GRA, printf("setOffset: %p, %d,%d\n", gr, xOff, yOff));

  gr->x0 = xOff;
  gr->y0 = yOff;
}

void Java_java_awt_Toolkit_graSetPaintMode(JNIEnv* env, jclass clazz,
  Graphics* gr)
{
  DBG( AWT_GRA, printf("setPaintMode: %p\n", gr));

  gr->xor_mode = 0;

  QPainter *painter = (QPainter*)gr->painter;

  painter->setPen(QCOLOR(gr->fg));
  painter->setBrush(QCOLOR(gr->fg));
  painter->setRasterOp(Qt::CopyROP);
}

void Java_java_awt_Toolkit_graSetXORMode(JNIEnv* env, jclass clazz,
  Graphics* gr, jint xorClr)
{
  DBG( AWT_GRA, printf("setXORMode: %p, %x\n", gr, xorClr));

  gr->xor_mode = 1;
  gr->xclr = xorClr;

  QPainter *painter = (QPainter*)gr->painter;

  painter->setPen(QCOLOR(gr->fg ^ xorClr));
  painter->setBrush(QCOLOR(gr->fg ^ xorClr));
  painter->setRasterOp(Qt::XorROP);
}

void Java_java_awt_Toolkit_graSetVisible(JNIEnv* env, jclass clazz,
  Graphics* gr, jint isVisible)
{
  DBG( AWT_GRA, printf("setVisble: %p  %d\n", gr, isVisible));

  /*
   * This is rather a hack to "defuse" a Graphics object, but we don't want to
   * add checks on every draw op to test if the target is invisible
   */
  if(!isVisible) {
    if(gr->y0 >= -10000) 
      gr->y0 -= 100000;
  }
  else {
    if(gr->y0 <= -10000)
      gr->y0 += 100000;
  }
}


/************************************************************************************
 * image rendering
 */

static void
drawAlphaImage ( Graphics* gr, Image* img,
				 jint srcX, jint srcY, jint dstX, jint dstY,
				 jint width, jint height, jint bgval )
{
  Image  *dstImg = 0;
//  XImage *dstXim = 0;
  QImage *dstQim = 0;
  int    i, j, si, sj, alpha;
  unsigned long dpix, spix, bgpix = 0;
  int    sr, sg, sb, dr, dg, db;

  DBG( AWT_GRA, printf("drawAlphaImage: %p %p (%p, %p),  %d,%d  %d,%d  %d,%d  %x\n",
				 gr, img, img->qImg, img->alpha, srcX, srcY, dstX, dstY, width, height, bgval));

  if ( !img ) return;
#if 0
  if ( X->shm == USE_SHM ){
	dstImg = createImage( width, height);
	createXImage( X, dstImg);
	if ( dstImg->shmiImg ){
	  dstXim = dstImg->xImg;
	  XShmGetImage( X->dsp, gr->drw, dstXim, dstX, dstY, 0xffffffff);
	}
	else {  /* Shm failed, backup to normal XImage */
	  Java_java_awt_Toolkit_imgFreeImage( 0, 0, dstImg);
	  dstImg = 0;
	}
  }
#endif

  for(i=0;i<X->nWindows;i++)
	if (X->windows[i].flags= WND_FRAME) break;
  if(i == X->nWindows) i =0;
  QWidget *toplevel = (QWidget *)(X->windows[i].w);
  if ( !dstQim ) {
	QPixmap tmp;
	tmp = QPixmap::grabWidget(toplevel,(int)dstX,(int)dstY,(int)width,(int)height);
  	*dstQim = tmp.convertToImage();
  }
#if 0
  if ( !dstXim ) {
//	dstXim = XGetImage( X->dsp, gr->drw, dstX, dstY, width, height, 0xffffffff, ZPixmap);
  }

//  if ( bgval != -1 )
//	bgpix = pixelValue( X, bgval);
#endif
  if ( dstQim ) {
	for ( j=0, sj=srcY; j<dstQim->height(); j++, sj++ ) {
	  for ( i=0, si=srcX; i<dstQim->width(); i++, si++ ) {
		dpix = (bgval == -1) ? dstQim->pixel(i,j):bgpix; //XGetPixel( dstXim, i, j) : bgpix;
		spix = img->qImg->pixel(si,sj);//XGetPixel( img->xImg, si, sj);
		alpha = GetAlpha( img->alpha, si, sj);

//		rgbValues( X, dpix, &dr, &dg, &db);

		if ( alpha ) {
		  /*
		   * ints still considered to be substantially faster than floats on most
		   * architectures (avoid div by 255), map all 0..255 -> 1..256
		   */
		  alpha++; sr++; sg++; sb++; dr++; dg++; db++;

//		  rgbValues( X, spix, &sr, &sg, &sb);
		  dr = ((alpha * sr + (256 - alpha) * dr) + 128) >> 8;
		  dg = ((alpha * sg + (256 - alpha) * dg) + 128) >> 8;
		  db = ((alpha * sb + (256 - alpha) * db) + 128) >> 8;

		  dr--; dg--; db--;
		  dstQim->setPixel(i,j, qRgb(dr<<16,dg <<8, db));
//		  XPutPixel( dstXim, i, j, pixelValue( X, (dr << 16)|(dg << 8)|(db) ));
		}
	  }
	}

	if ( dstImg != 0 ){
//	  XShmPutImage( X->dsp, gr->drw, gr->gc, dstXim, 0, 0, dstX, dstY, width, height, False);
//	  XSync( X->dsp, False);
	  Java_java_awt_Toolkit_imgFreeImage( 0, 0, dstImg);
	}
	else {
	  QPainter *painter = (QPainter *)gr->painter;
	  QPixmap tmp;
	  
          if(tmp.convertFromImage(*dstQim,0))
	  	painter->drawPixmap((int)dstX, (int)dstY,tmp,0,0,(int)width,(int)height);
//	  XPutImage( X->dsp, gr->drw, gr->gc, dstXim, 0, 0, dstX, dstY, width, height);
//	  XFlush( X->dsp);
//	  XDestroyImage( dstXim);
	}

	XFLUSH( X, True);
  }
}


/*
  This functions scales the pixmap in the member variable "pm" to fit the
  widget size and  puts the resulting pixmap in the member variable
"pmScaled".
*/

void scale(Image *img, int w, int h)
{
    if ( !img->qpm) return;

    if ( w == img->qpm->width() && h == img->qpm->height())
    {						// no need to scale if widget
	    img->qpmScaled = img->qpm;				// size equals pixmap size
    } else {
	    img->qpmScaled = new QPixmap();
            img->qpmScaled->convertFromImage(img->qImg->smoothScale(w, h),0);
    }
}

void
Java_java_awt_Toolkit_graDrawImage ( JNIEnv* env, jclass clazz, Graphics* gr, Image* img,
									 jint srcX, jint srcY,
									 jint dstX, jint dstY,
									 jint width, jint height, jint bgval )
{
//  XGCValues values;

  DBG( AWT_GRA, printf("drawImage: %p %p (%p,%p,%p %d,%d) %d,%d, %d,%d, %d,%d, %x\n",
				 gr, img,  img->qImg,img->qImg_AlphaMask,img->alpha,  img->width,img->height,
				 srcX,srcY,  dstX,dstY,	 width,height, bgval));

  if ( !img ) return;

  dstX += gr->x0;
  dstY += gr->y0;

  dstX += img->left;
  dstY += img->top;
  
  scale(img,width,height);
  
  if ( img->alpha ) {
        drawAlphaImage( gr, img, srcX, srcY, dstX, dstY, width, height,bgval);
  }
  if ( img->qpm ) {
	//if ( img->qImg->isNull() ) return;
	QPainter *painter = (QPainter *)gr->painter;
	painter->drawPixmap((int)dstX, (int)dstY, *(img->qpmScaled));

  }
  
#if 0
  if ( img->pix ) {
//	XCopyArea( X->dsp, img->pix, gr->drw, gr->gc, srcX, srcY, width, height, dstX, dstY);
  }
  else if ( img->xImg ) {
	if ( img->xMask ) {
	  /* save current gc values, we have to change them temporarily */
//	  XGetGCValues( X->dsp, gr->gc, GCFunction|GCPlaneMask|GCForeground|GCBackground,
//					&values);

	  /* in case we have a specified background color, fill area */
	  if ( bgval != -1 ) {
//		XSetForeground( X->dsp, gr->gc, bgval);
//		XFillRectangle( X->dsp, gr->drw, gr->gc, dstX, dstY, width, height);
	  }

	  /* set attributes so that a subsequent image draw leaves the mask pels unmodified */
//	  XSetFunction( X->dsp, gr->gc, GXandInverted);
//	  XSetForeground( X->dsp, gr->gc, 0xffffffff);
//	  XSetBackground( X->dsp, gr->gc, 0);

	  /* draw the mask bitmap */
	  if ( img->shmiMask ){
//		XShmPutImage( X->dsp, gr->drw, gr->gc, img->xMask,
//					  srcX, srcY, dstX, dstY, width, height, False);
	  }
	  else {
//		XPutImage( X->dsp, gr->drw, gr->gc, img->xMask,
//				   srcX, srcY, dstX, dstY, width, height);
	  }

	  /* restore gc values except of the function */
//	  XChangeGC( X->dsp, gr->gc, GCFunction|GCPlaneMask|GCForeground|GCBackground, &values);
//	  XSetFunction( X->dsp, gr->gc, GXor);
	}
	else if ( img->alpha ) {
//	  drawAlphaImage( gr, img, srcX, srcY, dstX, dstY, width, height, bgval);
	  return;
	}

	/* this is the real image drawing */
	if ( img->shmiImg ){
//	  XShmPutImage( X->dsp, gr->drw, gr->gc, img->xImg,
//					srcX, srcY, dstX, dstY, width, height, False);
	}
	else {
//	  XPutImage( X->dsp, gr->drw, gr->gc, img->xImg,
//				 srcX, srcY, dstX, dstY, width, height);
	}

	/* in case we had a mask, restore the original function */
//	if ( img->xMask )
//	  XSetFunction( X->dsp, gr->gc, values.function);
  }
#endif
  XFLUSH( X, True);
}


void
Java_java_awt_Toolkit_graDrawImageScaled ( JNIEnv* env, jclass clazz, Graphics* gr, Image* img,
										   jint dx0, jint dy0, jint dx1, jint dy1,
										   jint sx0, jint sy0, jint sx1, jint sy1, jint bgval )
{
  int        iw, x0, y0, x1, y1;
  Image      *tgt;
  int        tmpXImg = (img->qImg == NULL);

  DBG( AWT_GRA, printf("drawImageScaled: %p %p (%p), %d,%d,%d,%d, %d,%d,%d,%d, %x\n",
				 gr, img, img->qImg, dx0, dy0, dx1, dy1, sx0, sy0, sx1, sy1, bgval));

  if ( !img ) return;

  scale(img,dx1-dx0,dy1-dy0);
  if ( img->qpm ) {
        //if ( img->qImg->isNull() ) return;
        QPainter *painter = (QPainter *)gr->painter;
	painter->drawPixmap((int)dx0, (int)dy0, *(img->qpmScaled));
   
  }

#if 0

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
  Java_java_awt_Toolkit_graDrawImage ( env, clazz, gr, tgt, 0, 0,
									   x0, y0, tgt->width, tgt->height, bgval);
  if ( tgt->shmiImg ) {
	XSync( X->dsp, False); /* since we're going to destroy tgt, process its drawing first */
  }
  Java_java_awt_Toolkit_imgFreeImage( env, clazz, tgt);

  if ( tmpXImg ) {
	XDestroyImage( img->xImg);
	img->xImg = NULL;
  }
#endif
}
