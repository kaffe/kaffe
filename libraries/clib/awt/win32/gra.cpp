/**
* gra.cpp - 
*
* Copyright (c) 1999
*      Transvirtual Technologies, Inc.  All rights reserved.
*
* See the file "license.terms" for information on usage and redistribution 
* of this file. 
*/

#include "math.h"
#include "toolkit.hpp"

#define  WINDOW   0
#define  IMAGE    1
#define  GRAPHICS 2

void _cdecl getArcPoints( int x, int y, int width, int height,
						 int startAngle, int arcAngle, POINT& ps, POINT& pe) {
	
	int maxr, xc, yc;
	double rad, sa, ea;
	
	rad = -3.1415926535 / 180;
	maxr = ( max( width, height) ) /2;
	sa = rad * startAngle;
	ea = rad * (startAngle+arcAngle);
	xc = x + width/2 - 1;
	yc = y + height/2 - 1;
	
	ps.x = (int)( cos(sa) * maxr) + xc;
	ps.y = (int)( sin(sa) * maxr) + yc;
	pe.x = (int)( cos(ea) * maxr) + xc;
	pe.y = (int)( sin(ea) * maxr) + yc;
	
	//		printf( "x:%d y:%d w:%d h:%d x1:%d y1:%d x2:%d y2:%d\n",
	//			x, y, width, height, ps.x, ps.y, pe.x, pe.y);
}

int __cdecl jarray2Points ( POINT** pp, int x0, int y0,
						   jintArray xPoints, jintArray yPoints, int nPoints )
{
	register int i;
	int      n;
	jboolean isCopy;
	jint     *jx = JniEnv->GetIntArrayElements( xPoints, &isCopy);
	jint     *jy = JniEnv->GetIntArrayElements( yPoints, &isCopy);
	
	if ( (n = JniEnv->GetArrayLength( xPoints)) < nPoints ) nPoints = n;
	if ( (n = JniEnv->GetArrayLength( yPoints)) < nPoints ) nPoints = n;
	
	*pp = (POINT*)getBuffer( X, (nPoints+1)*sizeof(POINT));
	
	for ( i=0; i<nPoints; i++){
		(*pp)[i].x = jx[i] + x0;
		(*pp)[i].y = jy[i] + y0;
	}
	
	JniEnv->ReleaseIntArrayElements( xPoints, jx, JNI_ABORT);
	JniEnv->ReleaseIntArrayElements( yPoints, jy, JNI_ABORT);
	
	return nPoints;
}


HBRUSH HB_0 = (HBRUSH)GetStockObject( NULL_BRUSH);
HPEN   HP_0 = (HPEN)GetStockObject( NULL_PEN);

struct Graphics {
	HDC       dc;
	KWnd	  *wnd;
	HBITMAP	  hbm;
	HBRUSH	  brush;
	POINT	  cliOff;
	HPEN	  pen;
	COLORREF  fgClr;
	COLORREF  bgClr;
	char      xor;
	COLORREF  xclr;
	int		  x0;
	int		  y0;
	
	void init ( HDC hdc, KWnd* pWnd, HBITMAP hb, COLORREF cfg, COLORREF cbg, Font* fnt) {
		dc = hdc;
		wnd = pWnd;
		hbm = hb;
		fgClr = cfg;
		bgClr = cbg;
		
		brush = CreateSolidBrush( fgClr);
		pen = CreatePen( PS_SOLID, 0, fgClr);
		
		xor = 0;
		
		DeleteObject( SelectObject( dc, brush));
		DeleteObject( SelectObject( dc, pen ));
		SelectObject( dc, ((Font*)fnt)->fnt);
		
		if ( hbm ) {
			SelectObject( dc, hbm );
		}
#if !defined(UNDER_CE)
		SetTextAlign( dc, TA_BASELINE | TA_LEFT);
#endif
		SetBkMode( dc, TRANSPARENT);
		SetTextColor( dc, fgClr);
		
		if ( wnd) {
			cliOff.x = wnd->insets.left;
			cliOff.y = wnd->insets.top;
		}
		else {
			cliOff.x = 0;
			cliOff.y = 0;
		}
	}
	
	void restoreBrush() {
		SelectObject( dc, brush);
	}
	
	void selectNullBrush() {
		SelectObject ( dc, HB_0);
	}
	
	void restorePen() {
		SelectObject( dc, pen);
	}
	
	void selectNullPen() {
		SelectObject ( dc, HP_0);
	}
	
	void setFont( HFONT fnt) {
		SelectObject( dc, fnt);
	}
	
	void setXORMode( int xorClr) {
		xor = 1;
		xclr = xorClr;
		pen = CreatePen( PS_SOLID, 0, fgClr ^ xorClr);
		DeleteObject( SelectObject( dc, pen));
		SetROP2( dc, R2_XORPEN);
	}
	
	void setPaintMode() {
		xor = 0;
		pen = CreatePen( PS_SOLID, 0, fgClr);
		DeleteObject( SelectObject( dc, pen));
		SetROP2( dc, R2_COPYPEN);
	}
	
	bool setBackground ( int irgb) {
		if ( irgb < 0 )
			return FALSE;
		
		bgClr = (COLORREF)irgb;
		return true;
	}
	
	bool setForeground ( int irgb) {
		if ( irgb < 0 )
			return FALSE;
		
		fgClr = (COLORREF)irgb;
		SetTextColor( dc, fgClr);
		
		brush = CreateSolidBrush( fgClr);
		DeleteObject( SelectObject( dc, brush));
		
		pen = CreatePen( PS_SOLID, 0, fgClr);
		DeleteObject( SelectObject( dc, pen));
		
		return true;
	}
	
	
	boolean fillRect( int x, int y, int w, int h, int clr) {
		HBRUSH hbr = 0;
		HPEN hp;
		
		if ( clr < 0 )
			return false;
		
		if ( (COLORREF)clr != fgClr ) {
			hbr = (HBRUSH)SelectObject( dc, CreateSolidBrush( (COLORREF)clr ));
		}
		
#if defined( UNDER_95 )
		// strange repaint() behaviour under NT ???
		PatBlt( dc, x+x0, y+y0, w, h, PATCOPY );
#else
		hp = (HPEN)SelectObject( dc, HP_0);
		Rectangle( dc, x+x0, y+y0, x+x0+w+1, y+y0+h+1);
		SelectObject( dc, hp);
#endif
		
		if ( hbr != 0) {
			DeleteObject( SelectObject( dc, hbr));
		}
		
		GFLUSH();
		return true;
	}
	
	void drawRect( int x, int y, int w, int h ) {
		HBRUSH bo = (HBRUSH)SelectObject( dc, HB_0);
		Rectangle( dc, x+x0, y+y0, x+x0+w+1, y+y0+h+1);
		SelectObject( dc, bo);
		GFLUSH();
	}
	
	void drawLine ( int x1, int y1, int x2, int y2) {
		POINT pts[2];
		pts[0].x = x1+x0;
		pts[0].y = y1+y0;
		pts[1].x = x2+x0;
		pts[1].y = y2+y0;
		Polyline(dc, pts, 2);
		GFLUSH();
	}
	
	void drawText( int x, int y, TCHAR* txt, int len) {
#if defined(UNDER_CE)
	/* The 'y' value is the base of the character but WinCE things is the top - we deduce 10
	* to adjust it (we should be doing this based on the font size.
		*/
		y -= 10;
#endif
		TextOut( dc, x+x0, y+y0, txt, len);
		GFLUSH();
	}
	
	void translate( int x, int y) {
	/*
	* we use explicit translation, do not change
	* viewport here
		*/
		x0 = x - cliOff.x;
		y0 = y - cliOff.y;
	}
	
	void cleanUp() {
		
		//		kprintf( "releaseDC: %x %x\n", wnd, dc);
		
		if ( brush ) { 
			SelectObject( dc, HB_0 );
			DeleteObject( brush);
			brush = 0;
		}
		if ( pen ) {
			SelectObject( dc, HP_0 );
			DeleteObject( pen);
			pen = 0;
		}
		if ( wnd ) {
			ReleaseDC( wnd->hwnd, dc);
			wnd = 0;
		}
		if ( hbm ) {
			hbm = 0;
		}
		
		
	}
	
	void setClip( int x, int y, int w, int h, boolean set) {
		HRGN rgn;
		
		x += x0;
		y += y0;
		rgn = CreateRectRgn( x, y, x+w, y+h);
		
		if (set) {
			SelectClipRgn(dc, rgn);
		}
		else {
#if !defined(UNDER_CE)
			ExtSelectClipRgn( dc, rgn, RGN_AND);
#else
			//How do we do this in WinCE ???
#endif
		}
		DeleteObject( rgn);
	}
	
};


/*******************************************************************************
*	exported functions
*/


extern "C" {
	void* __cdecl
		Java_java_awt_Toolkit_graInitGraphics ( JNIEnv* env, jclass clazz,
		Graphics* gr, jobject tgt, jint tgtType,
		jint xOff, jint yOff,
		jint xClip, jint yClip, jint wClip, jint hClip,
		jobject fnt, jint fg, jint bg, jboolean blank )
	{
		KWnd *wnd = 0;
		HDC hdc = 0;
		Image *img = 0;
		HBITMAP hbm = 0;
		
		Graphics *gr1;
		
		//		printf( "%d,%d   %d,%d,%d,%d  %x %x %d\n", xOff, yOff, xClip, yClip, wClip, hClip, gr, tgt, tgtType);
		
		// disable batching / flush
		//GdiSetBatchLimit( 1);
		
		if ( tgt ) {
			
			if ( gr ) {
				gr->cleanUp();
			}
			
			switch ( tgtType) {
			case WINDOW:
				wnd = (KWnd*)tgt;
				hdc = GetDC( wnd->hwnd);
				break;
			case IMAGE:
				img = (Image*)tgt;
				hdc = img->dc;
				hbm = img->bmp;
				break;
			case GRAPHICS:
				gr1 = (Graphics*)tgt;
				wnd = gr1->wnd;
				if ( wnd ) {
					hdc = GetDC( wnd->hwnd);
				}
				else {
					hdc = gr1->dc;
				}
				hbm = gr1->hbm;
				break;
			}
		}
		else if ( gr ) {
			hdc = gr->dc;
			wnd = gr->wnd;
			hbm = gr->hbm;
		}
		
		if ( ! gr) {
			gr = new Graphics();
		}
		
		gr->init( hdc, wnd, hbm, fg, bg, (Font*)fnt);
		gr->translate( xOff, yOff);
		gr->setClip( xClip, yClip, wClip, hClip, TRUE);
		
		if ( blank) {
			gr->fillRect( xClip, yClip, wClip, hClip, (int)bg);
		}
		
		return gr;
	}
	
	
	void __cdecl
		Java_java_awt_Toolkit_graFreeGraphics ( JNIEnv* env, jclass clazz, Graphics* gr )
	{
		DBG( awt_gra, ("freeGraphics: %x\n", gr));
		
		gr->cleanUp();
		delete gr;
	}
	
	
	void __cdecl
		Java_java_awt_Toolkit_graCopyArea ( JNIEnv* env, jclass clazz, Graphics* gr, 
		jint x, jint y, jint width, jint height,
		jint xDelta, jint yDelta )
	{
		DBG( awt_gra, ("copyArea: %x, %d,%d-%d,%d  %d,%d\n", gr, x,y,width,height, xDelta,yDelta));
		BitBlt( gr->dc, x+xDelta+gr->x0, y+yDelta+gr->y0, width, height, gr->dc, x+gr->x0, y+gr->y0, SRCCOPY);
		GFLUSH();
	}
	
	void __cdecl
		Java_java_awt_Toolkit_graClearRect ( JNIEnv* env, jclass clazz, Graphics* gr,
		jint x, jint y, jint width, jint height )
	{
		DBG( awt_gra, ("clearRect: %x, %d,%d-%d,%d\n", gr, x,y,width,height));
		gr->fillRect( x, y, width, height, (int)gr->bgClr);	
	}
	
	
	void __cdecl
		Java_java_awt_Toolkit_graDrawBytes ( JNIEnv* env, jclass clazz, Graphics* gr,
		jbyteArray jBytes, jint offset, jint len, jint x, jint y )
	{
		jboolean isCopy;
		int      n;
		jbyte    *jb, *jbo;
		TCHAR	 *cs;
		
		if ( !jBytes ) return;
		
		n   = env->GetArrayLength( jBytes);
		jb = env->GetByteArrayElements( jBytes, &isCopy);
		jbo = jb + offset;
		
		if ( offset+len > n )
			len = n - offset;
		
		cs = jbyte2WinString( X, jbo, len);
		
		DBG( awt_gra, ("drawBytes: %x, %x,%d,%d  \"%s\"  %d,%d\n", gr, jBytes,offset,len, cs, x,y));
		gr->drawText( x, y, cs, len);
		
		env->ReleaseByteArrayElements( jBytes, jb, JNI_ABORT);
	}
	
	
	void __cdecl
		Java_java_awt_Toolkit_graDrawChars ( JNIEnv* env, jclass clazz, Graphics* gr,
		jcharArray jChars, jint offset, jint len, jint x, jint y )
	{
		jboolean isCopy;
		int      n;
		jchar    *jc, *jco;
		TCHAR	 *cs;
		
		if ( !jChars ) return;
		
		n   = env->GetArrayLength( jChars);
		jc = env->GetCharArrayElements( jChars, &isCopy);
		jco = jc + offset;
		
		if ( offset+len > n )
			len = n - offset;
		
		cs = jchar2WinString( X, jco, len);
		
		DBG( awt_gra, ("drawChars: %x, %x,%d,%d  \"%s\"  %d,%d\n", gr, jChars,offset,len, cs, x,y));
		gr->drawText( x, y, cs, len);
		
		env->ReleaseCharArrayElements( jChars, jc, JNI_ABORT);
	}
	
	
	void __cdecl
		Java_java_awt_Toolkit_graDrawString ( JNIEnv* env, jclass clazz,
		Graphics* gr, jstring str, jint x, jint y )
	{
		TCHAR  *jc = java2WinString( env, X, str);
		
		DBG( awt_gra, ("drawString: %x  \"%s\"  %d,%d\n", gr, jc, x,y));
		gr->drawText( x, y, jc, _tcslen( jc));
	}
	
	
	void __cdecl
		Java_java_awt_Toolkit_graDrawLine ( JNIEnv* env, jclass clazz, Graphics* gr,
		jint x1, jint y1, jint x2, jint y2 )
	{
		DBG( awt_gra, ("drawLine: %x, %d,%d - %d,%d\n", gr, x1,y1, x2,y2));
		gr->drawLine( x1, y1, x2, y2);
	}
	
	void __cdecl
		Java_java_awt_Toolkit_graDrawArc ( JNIEnv* env, jclass clazz, Graphics* gr,
		jint x, jint y, jint width, jint height,
		jint startAngle, jint arcAngle )
	{
#if !defined(UNDER_CE)
		POINT p1, p2;
		
		getArcPoints( x+gr->x0, y+gr->y0, width, height, startAngle, arcAngle, p1, p2);
		
		DBG( awt_gra, ("drawArc: %x, %d,%d-%d,%d  %d,%d\n", gr,
			x,y,width,height, startAngle,arcAngle));
		
		Arc( gr->dc, x+gr->x0, y+gr->y0, x+width+gr->x0, y+height+gr->y0, p1.x, p1.y, p2.x, p2.y);
		GFLUSH();
#endif
	}
	
	
	void __cdecl
		Java_java_awt_Toolkit_graFillArc ( JNIEnv* env, jclass clazz, Graphics* gr,
		jint x, jint y, jint width, jint height,
		jint startAngle, jint arcAngle )
	{
#if !defined(UNDER_CE)
		POINT p1, p2;
		
		getArcPoints( x+gr->x0, y+gr->y0, width, height, startAngle, arcAngle, p1, p2);
		
		DBG( awt_gra, ("fillArc: %x, %d,%d-%d,%d  %d,%d\n", gr,
			x,y,width,height, startAngle, arcAngle));
		
		Pie( gr->dc, x+gr->x0, y+gr->y0, x+width+gr->x0, y+height+gr->y0, p1.x, p1.y, p2.x, p2.y);
		GFLUSH();
#endif
	}
	
	
	void __cdecl
		Java_java_awt_Toolkit_graDrawOval ( JNIEnv* env, jclass clazz, Graphics* gr,
		jint x, jint y, jint width, jint height )
	{
		DBG( awt_gra, ("drawOval: %x, %d,%d - %d,%d\n", gr, x,y,width,height));
		SelectObject( gr->dc, HB_0);
		Ellipse( gr->dc, x+gr->x0, y+gr->y0, x+width+gr->x0, y+height+gr->y0);
		GFLUSH();
		SelectObject( gr->dc, gr->brush);
	}
	
	
	void __cdecl
		Java_java_awt_Toolkit_graFillOval ( JNIEnv* env, jclass clazz, Graphics* gr,
		jint x, jint y, jint width, jint height )
	{
		DBG( awt_gra, ("fillOval: %x, %d,%d - %d,%d\n", gr, x,y,width,height));
		SelectObject( gr->dc, HP_0);
		Ellipse( gr->dc, x+gr->x0, y+gr->y0, x+width+gr->x0, y+height+gr->y0);
		GFLUSH();
		SelectObject( gr->dc, gr->pen);
	}
	
	
	void __cdecl
		Java_java_awt_Toolkit_graDrawPolygon ( JNIEnv* env, jclass clazz, Graphics* gr,
		jintArray xPoints, jintArray yPoints, jint nPoints )
	{
		int n;
		POINT *p;
		
		DBG( awt_gra, ("drawPolygon: %x, %x,%x  %d\n", gr, xPoints, yPoints, nPoints));
		
		if ( !xPoints || !yPoints ) return;
		
		nPoints = jarray2Points( &p, gr->x0, gr->y0, xPoints, yPoints, nPoints);
		
		n = nPoints-1;
		if ( (p[0].x != p[n].x) || (p[0].y != p[n].y) ) {
			p[nPoints].x = p[0].x;
			p[nPoints].y = p[0].y;
			nPoints++;
		}
		
		gr->selectNullBrush();
		Polygon( gr->dc, p, nPoints);
		GFLUSH();
		gr->restoreBrush();
	}
	
	
	void __cdecl
		Java_java_awt_Toolkit_graDrawPolyline ( JNIEnv* env, jclass clazz, Graphics* gr,
		jintArray xPoints, jintArray yPoints, jint nPoints )
	{
		POINT *p;
		DBG( awt_gra, ("drawPolyline: %x, %x,%x  %d\n", gr, xPoints, yPoints, nPoints));
		
		if ( !xPoints || !yPoints ) return;
		
		nPoints = jarray2Points( &p, gr->x0, gr->y0, xPoints, yPoints, nPoints);
		
		gr->selectNullBrush();
		Polygon( gr->dc, p, nPoints);
		GFLUSH();
		gr->restoreBrush();
	}
	
	
	void __cdecl
		Java_java_awt_Toolkit_graFillPolygon ( JNIEnv* env, jclass clazz, Graphics* gr,
		jintArray xPoints, jintArray yPoints, jint nPoints )
	{
		int n;
		POINT *p;
		
		DBG( awt_gra, ("fillPolygon: %x, %x,%x  %d\n", gr, xPoints, yPoints, nPoints));
		
		if ( !xPoints || !yPoints ) return;
		
		nPoints = jarray2Points( &p, gr->x0, gr->y0, xPoints, yPoints, nPoints);
		
		n = nPoints-1;
		if ( (p[0].x != p[n].x) || (p[0].y != p[n].y) ) {
			p[nPoints].x = p[0].x;
			p[nPoints].y = p[0].y;
			nPoints++;
		}
		
		Polygon( gr->dc, p, nPoints);
		GFLUSH();
	}
	
	
	void __cdecl
		Java_java_awt_Toolkit_graDrawRect ( JNIEnv* env, jclass clazz, Graphics* gr,
		jint x, jint y, jint width, jint height )
	{
		DBG( awt_gra, ("drawRect: %x, %d,%d - %d,%d\n", gr, x,y,width,height));
		
		if ( (width >= 0) && (height >= 0) ){
			gr->drawRect( x, y, width, height);
		}
	}
	
	
	void __cdecl
		Java_java_awt_Toolkit_graFillRect ( JNIEnv* env, jclass clazz, Graphics* gr,
		jint x, jint y, jint width, jint height )
	{
		DBG( awt_gra, ("fillRect: %x, %d,%d - %d,%d\n", gr, x,y,width,height));
		gr->fillRect( x, y, width, height, (int)gr->fgClr);
	}
	
	
	
	void __cdecl
		Java_java_awt_Toolkit_graDrawRoundRect ( JNIEnv* env, jclass clazz, Graphics* gr,
		jint x, jint y, jint width, jint height, 
		jint wArc, jint hArc )
	{
		
		DBG( awt_gra, ("drawRoundRect: %x, %d,%d - %d,%d  %d,%d\n", gr,
			x,y,width,height, wArc, hArc));
		
		gr->selectNullBrush();
		RoundRect( gr->dc, x+gr->x0, y+gr->y0, x+width+gr->x0, y+height+gr->y0, wArc, hArc);
		GFLUSH();
		gr->restoreBrush();
	}
	
	void __cdecl
		Java_java_awt_Toolkit_graFillRoundRect ( JNIEnv* env, jclass clazz, Graphics* gr,
		jint x, jint y, jint width, jint height, 
		jint wArc, jint hArc )
	{
		
		DBG( awt_gra, ("fillRoundRect: %x, %d,%d - %d,%d  %d,%d\n", gr,
			x,y,width,height, wArc, hArc));
		
		gr->selectNullPen();
		RoundRect( gr->dc, x+gr->x0, y+gr->y0, x+width+gr->x0, y+height+gr->y0, wArc, hArc);
		GFLUSH();
		gr->restorePen();
	}
	
	
	void __cdecl
		Java_java_awt_Toolkit_graDraw3DRect ( JNIEnv* env, jclass clazz, Graphics* gr,
		jint x, jint y, jint width, jint height,
		jboolean raised, jint javargb )
	{
		int     bright, dark;
		HPEN	pb, pd;
		int     xw, yh;
		
		DBG( awt_gra, ("draw3DRect: %x, %d,%d - %d,%d  %d %x\n", gr,
			x,y,width,height, raised, javargb));
		
		/* we should use the same mechanism like ordinary Graphics brighter()/darker() here */
		dark   = (int)ColorJ2N((int)Java_java_awt_Toolkit_clrDark( env, clazz, javargb));
		bright = (int)ColorJ2N((int)Java_java_awt_Toolkit_clrBright( env, clazz, javargb));
		pd = CreatePen( PS_SOLID, 0, dark);
		pb = CreatePen( PS_SOLID, 0, bright);
		
		xw = x + width;
		yh = y + height;
		
		SelectObject( gr->dc, raised ? pb : pd);
		gr->drawLine( x, y, xw-1, y);
		gr->drawLine( x, y, x, yh);
		
		SelectObject( gr->dc, raised ? pd : pb);
		gr->drawLine( x+1, yh, xw, yh);
		gr->drawLine( xw, y, xw, yh);
		
		GFLUSH();
		
		DeleteObject( pb);
		DeleteObject( pd);
		
		SelectObject( gr->dc, gr->pen);
	}
	
	void __cdecl
		Java_java_awt_Toolkit_graFill3DRect ( JNIEnv* env, jclass clazz, Graphics* gr,
		jint x, jint y, jint width, jint height,
		jboolean raised, jint javargb )
	{
		
		DBG( awt_gra, ("fill3DRect: %x, %d,%d - %d,%d  %d %x\n", gr,
			x,y,width,height, raised, javargb));	
		gr->fillRect( x+1, y+1, width-2, height-2, ColorJ2N(javargb));
		Java_java_awt_Toolkit_graDraw3DRect( env, clazz, gr, x, y, width-1, height-1, raised, javargb);
	}
	
	
	void __cdecl
		Java_java_awt_Toolkit_graAddClip ( JNIEnv* env, jclass clazz, Graphics* gr,
		jint xClip, jint yClip, jint wClip, jint hClip )
	{
		DBG( awt_gra, ("addClip: %x, %d,%d - %d,%d\n", gr, xClip, yClip, wClip, hClip));
		gr->setClip( xClip, yClip, wClip, hClip, FALSE);	  
	}
	
	
	void __cdecl
		Java_java_awt_Toolkit_graSetClip ( JNIEnv* env, jclass clazz, Graphics* gr,
		jint xClip, jint yClip, jint wClip, jint hClip )
	{
		DBG( awt_gra, ("setClip: %x, %d,%d - %d,%d\n", gr, xClip, yClip, wClip, hClip));
		gr->setClip( xClip, yClip, wClip, hClip, TRUE);	  
	}
	
	void __cdecl
		Java_java_awt_Toolkit_graSetColor ( JNIEnv* env, jclass clazz, Graphics* gr, jint clr )
	{
		DBG( awt_gra, ("setColor: %x, %x\n", gr, clr));
		
		if ( gr->xor )
			clr ^= gr->xclr;
		
		gr->setForeground( clr);
	}
	
	void __cdecl
		Java_java_awt_Toolkit_graSetBackColor ( JNIEnv* env, jclass clazz, Graphics* gr, jint clr )
	{
		DBG( awt_gra, ("setBackColor: %x, %x\n", gr, clr));
		gr->setBackground( clr);
	}
	
	void __cdecl
		Java_java_awt_Toolkit_graSetFont ( JNIEnv* env, jclass clazz, Graphics* gr, jobject fnt )
	{
		DBG( awt_gra, ("setFont: %x, %x\n", gr, fnt));
		gr->setFont( ((Font*)fnt)->fnt);
	}
	
	void __cdecl
		Java_java_awt_Toolkit_graSetOffset ( JNIEnv* env, jclass clazz, Graphics* gr, jint xOff, jint yOff )
	{
		DBG( awt_gra, ("setOffset: %x, %d,%d\n", gr, xOff, yOff));
		gr->translate( xOff, yOff);
	}
	
	void __cdecl
		Java_java_awt_Toolkit_graSetPaintMode ( JNIEnv* env, jclass clazz, Graphics* gr )
	{
		DBG( awt_gra, ("setPaintMode: %x\n", gr));
		gr->setPaintMode();		
	}
	
	void __cdecl
		Java_java_awt_Toolkit_graSetXORMode ( JNIEnv* env, jclass clazz, Graphics* gr, jint xorClr )
	{
		DBG( awt_gra, ("setXORMode: %x, %x\n", gr, xorClr));
		gr->setXORMode( xorClr);
	}
	
	void __cdecl
		Java_java_awt_Toolkit_graSetVisible ( JNIEnv* env, jclass clazz, Graphics* gr, jint isVisible )
	{
		DBG( awt_gra, ("setVisble: %x  %d\n", gr, isVisible));
		
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
	
	void __cdecl
		drawAlphaImage ( Graphics* gr, Image* img,
		jint srcX, jint srcY, jint dstX, jint dstY,
		jint width, jint height, jint bgval )
	{
	}
	
	
	void __cdecl
		Java_java_awt_Toolkit_graDrawImage ( JNIEnv* env, jclass clazz, Graphics* gr, Image* img,
		jint srcX, jint srcY,
		jint dstX, jint dstY,
		jint width, jint height, jint bgval )
	{
		if ( img ) {
			if ( img->mask) {
				// gr->fillRect( dstX, dstY, width, height, bgval);
#if defined (UNDER_95)
				BitBlt( gr->dc, dstX+gr->x0, dstY+gr->y0, width, height, img->dcMask, srcX, srcY, SRCAND);
				BitBlt( gr->dc, dstX+gr->x0, dstY+gr->y0, width, height, img->dc, srcX, srcY, SRCINVERT);
#else				
				MaskBlt( gr->dc, dstX+gr->x0, dstY+gr->y0, width, height, img->dc, srcX, srcY,
					img->mask, 0, 0, MAKEROP4( SRCPAINT, SRCCOPY));
#endif
			}
			else {
				BitBlt( gr->dc, dstX+gr->x0, dstY+gr->y0, width, height, img->dc, srcX, srcY, SRCCOPY);
			}
		}
		GFLUSH();
	}
	
	
	void __cdecl
		Java_java_awt_Toolkit_graDrawImageScaled ( JNIEnv* env, jclass clazz, Graphics* gr, Image* img,
		jint dx0, jint dy0, jint dx1, jint dy1,
		jint sx0, jint sy0, jint sx1, jint sy1, jint bgval )
	{
		Image* tgt;
		int iw, x0, y0, x1, y1;
		
		iw = img->width;
		
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
		
		tgt = createImage( (x1-x0+1), (y1-y0+1) );
		StretchBlt( tgt->dc, 0, 0, tgt->width, tgt->height,
			img->dc, sx0, sy0, sx1-sx0, sy1-sy0, SRCCOPY);
		
		if ( img->mask) {
			createMaskImage( X, tgt);
			StretchBlt( tgt->dcMask, 0, 0, tgt->width, tgt->height,
				img->dcMask, sx0, sy0, sx1-sx0, sy1-sy0, SRCCOPY);
		}
		
		
		Java_java_awt_Toolkit_graDrawImage ( env, clazz, gr, tgt, 0, 0,
									   x0, y0, tgt->width, tgt->height, bgval);
		freeImage( X, tgt);		
	}
}
