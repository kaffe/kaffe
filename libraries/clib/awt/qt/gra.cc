/**
 * gra.c - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2002, 2003, 2004, 2005
 *	Kaffe.org contributors, see ChangeLog for details.  All rights reserved.
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

void* Java_java_awt_Toolkit_graInitGraphics(JNIEnv* env, jclass clazz,
  Graphics* gr, jobject tgt, jint tgtType, jint xOff, jint yOff,
  jint xClip, jint yClip, jint wClip, jint hClip,
  jobject fnt, jint fg, jint bg, jboolean blank)
{
  QPaintDevice*       drw;

  DBG(AWT_GRA, qqDebug("initGraphics: gr %p (tgt %p, type=%d) xo %d,yo %d  %d,%d - %d,%d  %p %x %x\n",
    gr, tgt,tgtType, xOff,yOff, xClip,yClip,wClip,hClip, fnt,fg,bg));

  if ( tgt ) {
    switch ( tgtType ) {
      case 0:
        drw = (QWidget*)tgt;
        break;          /* Window   */
      case 1:
        drw = (QPixmap*)(((Image*)tgt)->qpm);
        DBG(AWT_GRA, qqDebug("tgt = IMAGE drw=%p\n",drw));
        break;     /* Image    */
      case 2:
        drw = ((Graphics*)tgt)->drw;
        DBG(AWT_GRA, qqDebug("tgt = GRAPHICS\n"));
        break;  /* Graphics */
      default:
        qFatal("illegal Graphics target: %d\n", tgtType);
        drw = 0;
    }
  }
  else {
    if ( gr )
      drw = gr->drw;
    else {
      qFatal("attempt to set an uninitialized graphics");
      drw = 0;
    }
  }

  if(gr == 0) {
    gr = (Graphics*) AWT_MALLOC( sizeof(Graphics));
    memset((void*)gr, 0, sizeof(Graphics));
  }
  else {
    /* Release QPainter objects of reused NativeGraphics Java objects */
    if (gr->painter != NULL) {
      delete gr->painter;
    }
  } 

  gr->painter = new QPainter(drw);
  DBG(AWT_GRA, qqDebug("painter=%x\n", gr->painter));

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
  DBG(AWT_GRA, qqDebug("freeGraphics: %p\n", gr));

  if(gr->painter)
    delete gr->painter;
  AWT_FREE( gr);
}


void Java_java_awt_Toolkit_graCopyArea(JNIEnv* env, jclass clazz,
  Graphics* gr, jint x, jint y, jint width, jint height,
  jint xDelta, jint yDelta )
{
  DBG(AWT_GRA, qqDebug("copyArea: %p, %d,%d-%d,%d  %d,%d\n", gr, x,y,width,height, xDelta,yDelta));

  /* some X servers act alleric on that (careless bitblt, again) */
  if ( (width < 0) || (height < 0) )
	return;

  x += gr->x0; y += gr->y0;
}


void Java_java_awt_Toolkit_graClearRect(JNIEnv* env, jclass clazz,
  Graphics* gr, jint x, jint y, jint width, jint height)
{
  DBG(AWT_GRA, qqDebug("clearRect: %p, %d,%d-%d,%d\n", gr, x,y,width,height));

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

  DBG(AWT_GRA, qqDebug("drawBytes: %p, %p,%d,%d  \"%s\"  %d,%d\n", gr,
				 jBytes,offset,len, jb, x,y));

  if ( offset+len > n )
	len = n - offset;

  QByteArray a;
  a.setRawData((char *)jbo,len);
  painter->drawText( x+gr->x0, y+gr->y0, QString(a), len);
  env->ReleaseByteArrayElements( jBytes, jb, JNI_ABORT);
}


void
Java_java_awt_Toolkit_graDrawChars ( JNIEnv* env, jclass clazz,
	Graphics* gr, jcharArray jChars, jint offset, jint len, jint x, jint y )
{
  jboolean isCopy;
  int      n;
  jchar    *jc, *jco;
  ushort  *b;
  QChar   *unicode;
  QPainter *painter;
  painter = (QPainter *)gr->painter;

  if ( !jChars ) return;

  n   = env->GetArrayLength( jChars);
  jc = env->GetCharArrayElements( jChars, &isCopy);
  jco = jc + offset;

  DBG(AWT_GRA, qqDebug("drawChars: %p, %p,%d,%d  \"%s\"  %d,%d\n", gr, jChars,offset,len,
				 jchar2CString(X,jco,len), x,y));

  if ( offset+len > n )
	len = n - offset;

#ifndef WORDS_BIGENDIAN
  n = sizeof(ushort)*len;
  b = (ushort *) getBuffer( X, n);
  swab( (void*)jco, (void*)b, n);
#else
  b = (ushort *) jco;
#endif
  int i;
  unicode = new QChar[len];
  for(i=0;i<len;i++)
    unicode[i] = QChar((ushort)*(b+i));

  painter->drawText( x+gr->x0, y+gr->y0, QString(jchar2CString(X,jco,len)),len);
  env->ReleaseCharArrayElements( jChars, jc, JNI_ABORT);
}


void Java_java_awt_Toolkit_graDrawString(JNIEnv* env, jclass clazz,
  Graphics* gr, jstring str, jint x, jint y)
{
  char *cstr;
  QPainter *painter;

  if ( !str ) return;

  cstr = java2CString(env, X, str);
  DBG(AWT_GRA, qqDebug("drawString: %p  \"%s\"  %d,%d\n", gr, cstr, x,y));
  painter = (QPainter*)gr->painter;

  QString qstr(cstr);
  painter->drawText(x+gr->x0, y+gr->y0, qstr);
}


void Java_java_awt_Toolkit_graDrawLine(JNIEnv* env, jclass clazz,
  Graphics* gr, jint x1, jint y1, jint x2, jint y2)
{
  DBG( AWT_GRA, qqDebug("drawLine: %p, %d,%d - %d,%d\n", gr, x1,y1, x2,y2));

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
  DBG( AWT_GRA, qqDebug("drawArc: %p, %d,%d-%d,%d  %d,%d\n", gr,
				 x,y,width,height, startAngle,arcAngle));

  QPainter *painter = (QPainter*)gr->painter;

  painter->drawArc(x+gr->x0, y+gr->y0, width, height,
    startAngle << 4, arcAngle << 4);
}


void Java_java_awt_Toolkit_graFillArc(JNIEnv* env, jclass clazz,
  Graphics* gr, jint x, jint y, jint width, jint height,
  jint startAngle, jint arcAngle)
{
  DBG( AWT_GRA, qqDebug("fillArc: %p, %d,%d-%d,%d  %d,%d\n", gr,
				 x,y,width,height, startAngle, arcAngle));

  QPainter *painter = (QPainter*)gr->painter;

  painter->drawPie(x+gr->x0, y+gr->y0, width, height,
    startAngle << 4, arcAngle << 4);
}


void Java_java_awt_Toolkit_graDrawOval(JNIEnv* env, jclass clazz,
  Graphics* gr, int x, jint y, jint width, jint height)
{
  DBG( AWT_GRA, qqDebug("drawOval: %p, %d,%d - %d,%d\n", gr, x,y,width,height));

  QPainter *painter = (QPainter*)gr->painter;

  painter->drawArc(x+gr->x0, y+gr->y0, width, height,
    0, 360 << 4);
}


void Java_java_awt_Toolkit_graFillOval(JNIEnv* env, jclass clazz,
  Graphics* gr, jint x, jint y, jint width, jint height)
{
  DBG( AWT_GRA, qqDebug("fillOval: %p, %d,%d - %d,%d\n", gr, x,y,width,height));

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

  DBG( AWT_GRA, qqDebug("drawPolygon: %p, %p,%p  %d\n", gr, xPoints, yPoints, nPoints));

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
  DBG( AWT_GRA, qqDebug("drawPolyline: %p, %p,%p  %d\n", gr, xPoints, yPoints, nPoints));

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

  DBG( AWT_GRA, qqDebug("fillPolygon: %p, %p,%p  %d\n", gr, xPoints, yPoints, nPoints));

  if ( !xPoints || !yPoints ) return;

  QPainter *painter = (QPainter*)gr->painter;
  QPointArray *points = jarray2Points(env, X, gr->x0, gr->y0,
    xPoints, yPoints, nPoints);
  painter->drawPolygon(*points);
}


void Java_java_awt_Toolkit_graDrawRect(JNIEnv* env, jclass clazz,
  Graphics* gr, jint x, jint y, jint width, jint height)
{
  DBG( AWT_GRA, qqDebug("drawRect: %p, %d,%d - %d,%d\n", gr, x,y,width,height));

  QPainter *painter = (QPainter*)gr->painter;

  if( (width >= 0) && (height >= 0) )
    painter->drawRect(x+gr->x0, y+gr->y0, width, height);
}


void Java_java_awt_Toolkit_graFillRect(JNIEnv* env, jclass clazz,
  Graphics* gr, jint x, jint y, jint width, jint height)
{
  DBG( AWT_GRA, qqDebug("fillRect: %p, %d,%d - %d,%d\n", gr, x,y,width,height));

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

  DBG( AWT_GRA, qqDebug("drawRoundRect: %p, %d,%d - %d,%d  %d,%d\n", gr,
				 x,y,width,height, wArc, hArc));

  QPainter *painter = (QPainter*)gr->painter;
  painter->drawRoundRect(x+gr->x0, y+gr->y0, width, height, wArc, hArc);
}

void
Java_java_awt_Toolkit_graFillRoundRect ( JNIEnv* env, jclass clazz, Graphics* gr,
										 jint x, jint y, jint width, jint height, 
										 jint wArc, jint hArc )
{
  int x1, x2, y1, y2, a, b;

  DBG( AWT_GRA, qqDebug("fillRoundRect: %p, %d,%d - %d,%d  %d,%d\n", gr,
				 x,y,width,height, wArc, hArc));

  QPainter *painter = (QPainter*)gr->painter;
}

void Java_java_awt_Toolkit_graDraw3DRect(JNIEnv* env, jclass clazz,
  Graphics* gr, jint x, jint y, jint width, jint height,
  jboolean raised, jint rgb)
{
  int      bright, dark;
  int      xw, yh;

  DBG( AWT_GRA, qqDebug("draw3DRect: %p, %d,%d - %d,%d  %d %x\n", gr,
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
  DBG( AWT_GRA, qqDebug("fill3DRect: %p, %d,%d - %d,%d  %d %x\n", gr,
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
  /* XXX: not implemented yet */
}


void Java_java_awt_Toolkit_graSetClip(JNIEnv* env, jclass clazz,
  Graphics* gr, jint xClip, jint yClip, jint wClip, jint hClip)
{
  DBG( AWT_GRA, qqDebug("setClip: %p, %d,%d - %d,%d\n", gr, xClip, yClip, wClip, hClip));

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
  DBG(AWT_GRA, qqDebug("setColor: gr=%p, clr=%x, painter=%x\n", gr, clr,
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
  DBG( AWT_GRA, qqDebug("setBackColor: %p, %x\n", gr, clr));

  gr->bg = clr;

  QPainter *painter = (QPainter*)gr->painter;

  painter->setBackgroundColor(QCOLOR(clr));
}

void Java_java_awt_Toolkit_graSetFont(JNIEnv* env, jclass clazz,
  Graphics* gr, jobject fnt)
{
  DBG( AWT_GRA, qqDebug("setFont: %p, %p\n", gr, fnt));

  QPainter *painter = (QPainter*)gr->painter;
  gr->painter->setFont(*((QFont*)fnt));
}

void Java_java_awt_Toolkit_graSetOffset(JNIEnv* env, jclass clazz,
  Graphics* gr, jint xOff, jint yOff)
{
  DBG( AWT_GRA, qqDebug("setOffset: %p, %d,%d\n", gr, xOff, yOff));

  gr->x0 = xOff;
  gr->y0 = yOff;
}

void Java_java_awt_Toolkit_graSetPaintMode(JNIEnv* env, jclass clazz,
  Graphics* gr)
{
  DBG( AWT_GRA, qqDebug("setPaintMode: %p\n", gr));

  gr->xor_mode = 0;

  QPainter *painter = (QPainter*)gr->painter;

  painter->setPen(QCOLOR(gr->fg));
  painter->setBrush(QCOLOR(gr->fg));
  painter->setRasterOp(Qt::CopyROP);
}

void Java_java_awt_Toolkit_graSetXORMode(JNIEnv* env, jclass clazz,
  Graphics* gr, jint xorClr)
{
  DBG( AWT_GRA, qqDebug("setXORMode: %p, %x\n", gr, xorClr));

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
  DBG( AWT_GRA, qqDebug("setVisble: %p  %d\n", gr, isVisible));

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


/**
 * image rendering
 */

static void
drawAlphaImage ( Graphics* gr, Image* img,
				 jint srcX, jint srcY, jint dstX, jint dstY,
				 jint width, jint height, jint bgval )
{
  Image  *dstImg = 0;
  QImage *dstQim = 0;
  int    i, j, si, sj, alpha;
  unsigned long dpix, spix, bgpix = 0;
  int    sr, sg, sb, dr, dg, db;

  DBG( AWT_GRA, qqDebug("drawAlphaImage: %p %p (%p, %p),  %d,%d  %d,%d  %d,%d  %x\n",
				 gr, img, img->qImg, img->alpha, srcX, srcY, dstX, dstY, width, height, bgval));

  if ( !img ) return;

  for(i=0;i<X->nWindows;i++)
	if (X->windows[i].flags= WND_FRAME) break;
  if(i == X->nWindows) i =0;
  QWidget *toplevel = (QWidget *)(X->windows[i].w);
  if ( !dstQim ) {
	QPixmap tmp;
	tmp = QPixmap::grabWidget(toplevel,(int)dstX,(int)dstY,(int)width,(int)height);
  	*dstQim = tmp.convertToImage();
  }
  
  if ( dstQim ) {
	for ( j=0, sj=srcY; j<dstQim->height(); j++, sj++ ) {
	  for ( i=0, si=srcX; i<dstQim->width(); i++, si++ ) {
		dpix = (bgval == -1) ? dstQim->pixel(i,j):bgpix; 
		spix = img->qImg->pixel(si,sj);
		alpha = GetAlpha( img->alpha, si, sj);

		if ( alpha ) {
		  /*
		   * ints still considered to be substantially faster than floats on most
		   * architectures (avoid div by 255), map all 0..255 -> 1..256
		   */
		  alpha++; sr++; sg++; sb++; dr++; dg++; db++;

		  dr = ((alpha * sr + (256 - alpha) * dr) + 128) >> 8;
		  dg = ((alpha * sg + (256 - alpha) * dg) + 128) >> 8;
		  db = ((alpha * sb + (256 - alpha) * db) + 128) >> 8;

		  dr--; dg--; db--;
		  dstQim->setPixel(i,j, qRgb(dr<<16,dg <<8, db));
		}
	  }
	}

	if ( dstImg != 0 ){
	  Java_java_awt_Toolkit_imgFreeImage( 0, 0, dstImg);
	}
	else {
	  QPainter *painter = (QPainter *)gr->painter;
	  QPixmap tmp;
	  
          if(tmp.convertFromImage(*dstQim,0))
	  	painter->drawPixmap((int)dstX, (int)dstY,tmp,0,0,(int)width,(int)height);
	}

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
  DBG( AWT_GRA, qqDebug("drawImage: %p %p (%p,%p,%p %d,%d) %d,%d, %d,%d, %d,%d, %x\n",
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
}


void
Java_java_awt_Toolkit_graDrawImageScaled ( JNIEnv* env, jclass clazz, Graphics* gr, Image* img,
										   jint dx0, jint dy0, jint dx1, jint dy1,
										   jint sx0, jint sy0, jint sx1, jint sy1, jint bgval )
{
  int        iw, x0, y0, x1, y1;
  Image      *tgt;
  int        tmpXImg = (img->qImg == NULL);

  DBG( AWT_GRA, qqDebug("drawImageScaled: %p %p (%p), %d,%d,%d,%d, %d,%d,%d,%d, %x\n",
				 gr, img, img->qImg, dx0, dy0, dx1, dy1, sx0, sy0, sx1, sy1, bgval));

  if ( !img ) return;

  scale(img,dx1-dx0,dy1-dy0);
  if ( img->qpm ) {
        QPainter *painter = (QPainter *)gr->painter;
	painter->drawPixmap((int)dx0, (int)dy0, *(img->qpmScaled));
   
  }
}

