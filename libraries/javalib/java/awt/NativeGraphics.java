package java.awt;

import java.lang.String;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import java.awt.image.ImageObserver;
import kaffe.util.Ptr;

/**
 * NativeGraphics - concrete, hidden implementation of abstract Graphics
 *
 * The approach of using an abstract Graphics with a concrete NativeGraphics
 * (for both factory purposes and screen drawing) is used in favor of a
 * branched, concrete Graphics (with a "upward-overloaded" subGraphics()
 * method) because of two reasons:
 *
 * (1) the subGraphics() can be assumed to be a non-public quirk in one
 * possible subclass (SwingGraphics), it might be changed anytime (or become
 * invisible because of changed attributes / attribute behavior)
 *
 * (2) the subGraphics-related branch (depending on a method call) has to be
 * used in some potentially high-frequently called methods (e.g. the
 * out-of-update relevant drawChars(), which is heavily used in editors). It
 * is assumed to be more efficient to rely completely on overloading of methods
 * (accepting a rather small speed degradation in Container.paintChildren, where
 * we can't directly use Graphics fields (e.g. clip rect) anymore).
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author P.C.Mehlitz
 */
class NativeGraphics
  extends Graphics
  implements ComponentListener
{
	Ptr nativeData;
	Font fnt;
	Color fg;
	Color bg;
	Color xClr;
	Rectangle clip = new Rectangle();
/*
 * this field can be used to link a Graphics object to a non-native
 * Component (e.g. Panel, Canvas), so that it can be notified in case the
 * component changes visibility and/or location/size.
 */
	Component target;
	NativeGraphics next;
	static NativeGraphics cache;
	static Object lock = new Object();
	int xTarget;
	int yTarget;
	final static int TGT_TYPE_WINDOW = 0;
	final static int TGT_TYPE_IMAGE = 1;
	final static int TGT_TYPE_GRAPHICS = 2;

public void clearRect ( int x, int y, int width, int height ) {
	Toolkit.graClearRect( nativeData, x, y, width, height);
}

public void clipRect ( int x, int y, int width, int height ) {
	// according to the specs, this only shrinks the clip region to the
	// intersection of the current region and the specified rect, i.e. it
	// cannot be used to implement multi-rectangular clipping regions
	
	int xNew = (x > clip.x) ? x : clip.x;
	int yNew = (y > clip.y) ? y : clip.y;
	
	int clipXw = clip.x + clip.width;
	int clipYh = clip.y + clip.height;
	int xw     = x + width;
	int yh     = y + height;
	
	int wNew   = ((xw > clipXw) ? clipXw : xw) - xNew;
	int hNew   = ((yh > clipYh) ? clipYh : yh) - yNew;
	
	if ( wNew < 0 ) wNew = 0;
	if ( hNew < 0 ) hNew = 0;
	
	clip.x = xNew;
	clip.y = yNew;
	clip.width = wNew;
	clip.height = hNew;
	
	Toolkit.graSetClip( nativeData, clip.x, clip.y, clip.width, clip.height);
}

public void componentHidden ( ComponentEvent evt ){
	Toolkit.graSetVisible( nativeData, false);
}

public void componentMoved ( ComponentEvent evt ){
	getClippedGraphics( this, target,
	                    xOffset - xTarget, yOffset - yTarget,
	                    clip.x, clip.y, clip.width, clip.height, false);
}

public void componentResized ( ComponentEvent evt ){
	getClippedGraphics( this, target,
	                    xOffset - xTarget, yOffset - yTarget,
	                    clip.x, clip.y, clip.width, clip.height, false);
}

public void componentShown ( ComponentEvent evt ){
	Toolkit.graSetVisible( nativeData, true);
}

public void copyArea ( int x, int y, int width, int height, int dx, int dy ){
	Toolkit.graCopyArea( nativeData, x, y, width, height, dx, dy);
}

public Graphics create () {
	NativeGraphics g = getGraphics( this, nativeData, TGT_TYPE_GRAPHICS,
	                xOffset, yOffset,
									clip.x, clip.y, clip.width, clip.height,
									fg, bg, fnt, false);
	if ( xClr != null )
		g.setXORMode( xClr);
		
	return g;
}

public Graphics create ( int x, int y, int width, int height ) {
	// pretty redundant with setClip, but since it might be called frequently
	// (Swing, again), it is a good idea to save a native call

	int xw = x + width;
	int yh = y + height;
	int cxw = clip.x + clip.width;
	int cyh = clip.y + clip.height;
	
	int clx = (x > clip.x) ? x : clip.x;
	int cly = (y > clip.y) ? y : clip.y;
	int clw = ((xw > cxw) ? cxw : xw) - clx;
	int clh = ((yh > cyh) ? cyh : yh) - cly;
	
	NativeGraphics g = getGraphics( this, nativeData, TGT_TYPE_GRAPHICS,
	                x + xOffset, y + yOffset,
									clx - x, cly - y, clw, clh,
									fg, bg, fnt, false);
							
	if ( xClr != null )
		g.setXORMode( xClr);
		
	return g;
}

public void dispose () {
	if ( bg == null ) {
		// We have to provide some protection against double disposes
		// since it would make the whole cache inconsistent (leads to
		// simultanous use). We shouldn't store the target here because
		// this would lead to a memory leak if Graphics objects are
		// not disposed. Each of the colors should be a good choice
		// (since they can't get null while in use)
		return;
	}

	synchronized ( lock ) {
		xClr = null;
		fnt = null;
		fg = null;
		bg = null;

		if ( target != null ){
			target.removeComponentListener( this);
			target = null;
		}

		next = cache;
		cache = this;
	}
}

public void draw3DRect ( int x, int y, int width, int height, boolean raised ){
	// we pass the rgb color value because it might be stored on the native
	// side as pixel value (requiring an additional color conversion)
	Toolkit.graDraw3DRect( nativeData, x, y, width, height, raised, fg.rgbValue);
}

public void drawArc ( int x, int y, int width, int height,
		      int startAngle, int arcAngle ){
	Toolkit.graDrawArc( nativeData, x, y, width, height, startAngle, arcAngle);
}

public void drawBytes ( byte data[], int offset, int length, int x, int y ){
	Toolkit.graDrawBytes( nativeData, data, offset, length, x, y);
}

public void drawChars ( char data[], int offset, int length, int x, int y ){
	Toolkit.graDrawChars( nativeData, data, offset, length, x, y);
}

public boolean drawImage (Image img, int x, int y, Color bgcolor, ImageObserver observer) {
	/* We must check the image is loaded before we access it's height
	 * and width.
	 */
	if (img.loadImage(-1, -1, observer) == false) {
		return (false);
	}
	else {
		return (drawImage(img, x, y, img.width, img.height, bgcolor, observer));
	}
}

public boolean drawImage ( Image img, int x, int y, ImageObserver observer) {
	return drawImage(img, x, y, null, observer);
}

public boolean drawImage ( Image img, int x, int y, int width, int height, Color background, ImageObserver observer ) {

	/* Load image if it's not loaded - we don't scale because we
	 * can do this while drawing.
	 */
	if (img.loadImage(-1, -1, observer) == false) {
		return (false);
	}

	/* Handle proportional widths and heights */
	if (width < 0) {
		width = img.width;
		if (height < 0) {
			height = img.height;
		}
		else {
			width = (width * height) / img.height;
		}
	}
	else if (height < 0) {
		height = (img.height * width) / img.width;
	}

	if (img.width != width || img.height != height) {
		drawImgScaled( img, x, y, x+width, y+height, 0, 0, img.width, img.height, background);
	}
	else {
		drawImg( img, x, y, 0, 0, width, height, background);
	}
	return true;
}

public boolean drawImage ( Image img, int x, int y, int width, int height, ImageObserver observer){
	return drawImage( img, x, y, width, height, null, observer);
}

public boolean drawImage ( Image img,
		int dx0, int dy0, int dx1, int dy1,
		int sx0, int sy0, int sx1, int sy1,
		Color bgColor, ImageObserver observer) {

	/* Load image if it's not loaded - we don't scale because we
	 * can do this while drawing.
	 */
	if (img.loadImage(-1, -1, observer) == false) {
		return (false);
	}

	/* If any of the source points are negative then error */
	if (sx0 < 0 || sy0 < 0 || sx1 < 0 || sy1 < 0) {
		return (false);
	}

	// We don't create a scaled Image instance since we can draw scaled
	drawImgScaled( img, dx0, dy0, dx1, dy1, sx0, sy0, sx1, sy1, bgColor);
	return true;
}

public boolean drawImage ( Image img,
		int dx1, int dy1, int dx2, int dy2,
		int sx1, int sy1, int sx2, int sy2,
		ImageObserver observer) {
	return drawImage(img, dx1, dy1, dx2, dy2, sx1, sy1, sx2, sy2, null, observer);
}

void drawImg ( Image img,
		int x, int y, int sx, int sy,
		int width, int height,
		Color background ){
	if ( img.nativeData != null ) {
		Toolkit.graDrawImage( nativeData, img.nativeData,
		                      sx, sy, x, y, width, height,
			                    (background == null) ? -1 : background.nativeValue);
	}
}

void drawImgScaled ( Image img,
		int dx0, int dy0, int dx1, int dy1,
		int sx0, int sy0, int sx1, int sy1,
		Color background ){
	if ( img.nativeData != null ) {
		Toolkit.graDrawImageScaled( nativeData, img.nativeData,
			dx0, dy0, dx1, dy1,
			sx0, sy0, sx1, sy1,
			(background == null) ? -1 : background.nativeValue);
	}
}

public void drawLine ( int x1, int y1, int x2, int y2 ){
	Toolkit.graDrawLine( nativeData, x1, y1, x2, y2);
}

public void drawOval ( int x, int y, int width, int height ){
	Toolkit.graDrawOval( nativeData, x, y, width, height);
}

public void drawPolygon ( Polygon p ){
	Toolkit.graDrawPolygon( nativeData, p.xpoints, p.ypoints, p.npoints);
}

public void drawPolygon ( int xPoints[], int yPoints[], int nPoints ){
	Toolkit.graDrawPolygon( nativeData, xPoints, yPoints, nPoints);
}

public void drawPolyline ( int xPoints[], int yPoints[], int nPoints ){
	Toolkit.graDrawPolyline( nativeData, xPoints, yPoints, nPoints);
}

public void drawRoundRect ( int x, int y, int width, int height,
			    int arcWidth, int arcHeight){
	if (arcWidth <= 0 || arcHeight <= 0) {
		Toolkit.graDrawRect( nativeData, x, y, width, height);
	}
	else {
		Toolkit.graDrawRoundRect( nativeData, x, y, width, height, arcWidth, arcHeight);
	}
}

public void drawString ( String str, int x, int y ){
	Toolkit.graDrawString( nativeData, str, x, y);
}

public void fill3DRect ( int x, int y, int width, int height, boolean raised ){
	Toolkit.graFill3DRect( nativeData, x, y, width, height, raised, fg.rgbValue);
}

public void fillArc ( int x, int y, int width, int height,
		      int startAngle, int arcAngle ){
	Toolkit.graFillArc( nativeData, x, y, width, height, startAngle, arcAngle);
}

public void fillOval ( int x, int y, int width, int height ){
	Toolkit.graFillOval( nativeData, x, y, width, height);
}

public void fillPolygon ( Polygon p ){
	Toolkit.graFillPolygon( nativeData, p.xpoints, p.ypoints, p.npoints);
}

public void fillPolygon ( int xPoints[], int yPoints[], int nPoints ){
	Toolkit.graFillPolygon( nativeData, xPoints, yPoints, nPoints);
}

public void fillRect ( int x, int y, int width, int height ){
	fillRoundRect( x, y, width, height, 0, 0);
}

public void fillRoundRect ( int x, int y, int width, int height, int arcWidth, int arcHeight ){
	if (arcWidth <= 0 || arcHeight <= 0) {
		Toolkit.graFillRect( nativeData, x, y, width, height);
	}
	else {
		Toolkit.graFillRoundRect( nativeData, x, y, width, height, arcWidth, arcHeight);
	}
}

protected void finalize () throws Throwable {
	if ( nativeData != null ) {
		Toolkit.graFreeGraphics( nativeData);
		nativeData = null;
	}
	super.finalize();
}

Color getBackColor () {
	return bg;
}

public Shape getClip (){
	return (getClipRect());
}

public Rectangle getClipBounds() {
	// Another return object which is modified by Swing, causing more garbage <sigh>
	return (new Rectangle(clip));
}

static NativeGraphics getClippedGraphics ( NativeGraphics g, Component c,
                                    int xOff, int yOff,
                                    int xClip, int yClip, int wClip, int hClip,
                                    boolean blank ) {
	// A bit misnomed, this method returns a NativeGraphics object that is
	// propperly clipped to its (insetted) parents. The object can either be
	// a fresh one (with fonts and colors from its target), or a already used,
	// resident one (in this case this is mainly a native initialization)

	Container   p;
	Color       fg, bg;
	Font        fnt;

	if ( c.peer == null ) return null;

	if ( g == null ) {
		fg  = (c.fgClr != null) ? c.fgClr : c.getForeground();
		bg  = (c.bgClr != null) ? c.bgClr : c.getBackground();
		fnt = (c.font != null)  ? c.font  : c.getFont();
	}
	else { // otherwise, the silly compiler complains about missing init
		fg = g.fg; bg = g.bg; fnt = g.fnt;
	}

	while ( true ) {
		if ( c.parent == null ) {
			// compensate the artificial Frame decoration offsets (NOT insets)
			// note that this requires the graphics offset to be private
			xOff -= c.deco.x;
			yOff -= c.deco.y;

			if ( g == null ){
				return getGraphics( c, ((Window)c).nativeData, TGT_TYPE_WINDOW,
				                    xOff, yOff,
				                    xClip, yClip, wClip, hClip,
				                    fg, bg, fnt, blank);
			}
			else {
				Toolkit.graInitGraphics( g.nativeData, null, 0,
		                             xOff, yOff,
	 	                             xClip, yClip, wClip, hClip,
	  	                           fnt.nativeData,
	   	                           fg.nativeValue,
	    	                         bg.nativeValue,
	     	                         blank);
				return g;
			}
		}
		else {
			int  px, py, pxw, pyh;
			int  xwClip, yhClip, cxw, cyh;

			xOff += c.x;
			yOff += c.y;
			p = c.parent;

			// clip to parent (inset aware)			
			if ( p.insets != Insets.noInsets ) {
				Insets in  = p.insets;
				px  = in.left;
				py  = in.top;
				pxw = p.width - in.right;
				pyh = p.height - in.bottom;
			}
			else {
				px = 0; py = 0; pxw = p.width; pyh = p.height;
			}

			if ( xOff + xClip < px ){
				int xc = px - xOff;
				wClip -= (xc - xClip);
				xClip = xc;
			}
			if ( yOff + yClip < py ){
				int yc = py - yOff;
				hClip -= (yc - yClip);
				yClip = yc;
			}
			
			xwClip = xClip + wClip;
			yhClip = yClip + hClip;
			
			cxw = xOff + xwClip;
			cyh = yOff + yhClip;

			if ( cxw > pxw ) {
				wClip -= cxw - pxw;
			}
			if ( cyh  > pyh ) {
				hClip -= cyh - pyh;
			}

			c = p;
		}
	}
}

public Color getColor() {
	return fg;
}

public Font getFont() {
	return fnt;
}

public FontMetrics getFontMetrics ( Font fnt ) {
	return FontMetrics.getFontMetrics( fnt);
}

static NativeGraphics getGraphics ( Object target, Ptr tgtData, int tgtType,
                  int xOffset, int yOffset,
									int xClip, int yClip, int wClip, int hClip,
									Color fg, Color bg, Font fnt, boolean blank ) {
	NativeGraphics g;

	if ( tgtData == null ) {
		return null;
	}

	synchronized ( lock ) {
		if ( cache == null ) {
			g = new NativeGraphics();
		}
		else {
			g = cache;
			cache = g.next;
			g.next = null;
		}
	}

	g.xOffset = xOffset;
	g.yOffset = yOffset;
	g.clip.x  = xClip;
	g.clip.y  = yClip;
	g.clip.width  = wClip;
	g.clip.height = hClip;
	g.fnt     = fnt;
	g.fg      = fg;
	g.bg      = bg;

	g.nativeData = Toolkit.graInitGraphics( g.nativeData, tgtData, tgtType,
	                                        xOffset, yOffset,
	                                        xClip, yClip, wClip, hClip,
	                                        fnt.nativeData,
	                                        fg.nativeValue, bg.nativeValue,
	                                        blank);
	return g;
}

void setBackColor ( Color clr ){
	if ( (clr != null) && (clr != bg) ) {
		bg = clr;
		
		Toolkit.graSetBackColor( nativeData, bg.nativeValue);
	}
}

public void setClip ( Shape clip ){
	Rectangle r = clip.getBounds();
	setClip( r.x, r.y, r.width, r.height);
}

public void setClip ( int x, int y, int width, int height ) {
	clip.x      = x;
	clip.y      = y;
	clip.width  = width;
	clip.height = height;		

	Toolkit.graSetClip ( nativeData, x, y, width, height);
}

public void setColor ( Color clr ){
	if ( (clr != null) && (clr != fg) ) {
		fg = clr;

		Toolkit.graSetColor( nativeData, fg.nativeValue);
	}
}

public void setFont ( Font newFnt ) {
	if ( newFnt != null && newFnt != fnt ){
		fnt = newFnt;
		Toolkit.graSetFont( nativeData, fnt.nativeData);
	}
}

void setGraphics ( Ptr tgtData, int tgtType, int xOffset, int yOffset,
		               int xClip, int yClip, int wClip, int hClip,
		               Color fg, Color bg, Font fnt, boolean blank ) {

	this.xOffset = xOffset;
	this.yOffset = yOffset;
	this.clip.x  = xClip;
	this.clip.y  = yClip;
	this.clip.width  = wClip;
	this.clip.height = hClip;
	
	if ( fnt != null ) {
		this.fnt = fnt;
	}
	if ( fg != null ) {
		this.fg = fg;
	}
	if ( bg != null ) {
		this.bg = bg;
	}

	nativeData = Toolkit.graInitGraphics( nativeData, tgtData, tgtType,
	                                      xOffset, yOffset,
	                                      xClip, yClip, wClip, hClip,
	                                      this.fnt.nativeData,
	                                      this.fg.nativeValue,
	                                      this.bg.nativeValue,
	                                      blank);
}

public void setPaintMode() {
	if ( xClr != null ) {
		xClr = null;
		Toolkit.graSetPaintMode( nativeData);
	}
}

void setTarget ( Component tgt ) {
	target = tgt;
	target.addComponentListener( this);
	
	// not safe in case this isn't directly following a Component.getGraphics
	// (then again, it is just called by us)
	xTarget = xOffset;
	yTarget = yOffset;
	
	if ( !target.isVisible )
		Toolkit.graSetVisible( nativeData, false);
}

public void setXORMode ( Color newXorClr ) {
	if ( newXorClr != xClr ) {
		xClr = newXorClr;
		Toolkit.graSetXORMode( nativeData, xClr.nativeValue);
	}
}

public String toString() {
	return getClass().getName() + " [" + xOffset + ',' + yOffset +
	                     ", " + clip + ']';
}

public void translate ( int x, int y ) {
	xOffset += x;
	yOffset += y;
	
	Toolkit.graSetOffset( nativeData, xOffset, yOffset);
}

void wrapChildPaint ( Component c, int dx, int dy,
		             int xClip, int yClip, int wClip, int hClip,
		             Color fg, Color bg, Font fnt, boolean blank ) {
	// save current state
	Color fgOld  = this.fg;
	Color bgOld  = this.bg;
	Font  fntOld = this.fnt;
	int   cxOld  = clip.x;
	int   cyOld  = clip.y;
	int   cwOld  = clip.width;
	int   chOld  = clip.height;

	// set "child state"
	xOffset += dx;
	yOffset += dy;
	clip.x  = xClip;
	clip.y  = yClip;
	clip.width  = wClip;
	clip.height = hClip;
	
	// these parameters are optional, beware of "null" values
	if ( fnt != null )	this.fnt     = fnt;
	if ( fg != null )   this.fg      = fg;
	if ( bg != null )   this.bg      = bg;

	Toolkit.graInitGraphics( nativeData, null, 0,
	                         xOffset, yOffset,
	                         clip.x, clip.y, clip.width, clip.height,
	                         this.fnt.nativeData,
	                         this.fg.nativeValue,
	                         this.bg.nativeValue,
	                         blank);

	// paint child
	c.paint( this);
	
	// restore previous state
	xOffset -= dx;
	yOffset -= dy;
	clip.x = cxOld;
	clip.y = cyOld;
	clip.width = cwOld;
	clip.height = chOld;
	this.fnt = fntOld;
	this.fg = fgOld;
	this.bg = bgOld;
	
	Toolkit.graInitGraphics( nativeData, null, 0,
	                         xOffset, yOffset,
	                         clip.x, clip.y, clip.width, clip.height,
	                         this.fnt.nativeData,
	                         this.fg.nativeValue,
	                         this.bg.nativeValue,
	                         false);
}
}
