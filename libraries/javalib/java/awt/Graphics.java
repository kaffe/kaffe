/**
 * Graphics - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author P.C.Mehlitz
 */

package java.awt;

import java.awt.image.ImageObserver;
import kaffe.util.Ptr;

public class Graphics
{
	Ptr nativeData;
	int xOffset;
	int yOffset;
	Font fnt;
	Color fg;
	Color bg;
	Color xClr;
	Rectangle clip = new Rectangle();
	Graphics next;
	static Graphics cache;
	static Object lock = new Object();

protected Graphics () {
}

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
	if ( hNew < 0 ) yNew = 0;
	
	clip.x = xNew;
	clip.y = yNew;
	clip.width = wNew;
	clip.height = hNew;
	
	Toolkit.graSetClip( nativeData, clip.x, clip.y, clip.width, clip.height);
}

public void copyArea ( int x, int y, int width, int height, int dx, int dy ){
	Toolkit.graCopyArea( nativeData, x, y, width, height, dx, dy);
}

public Graphics create () {
	Graphics g = getGraphics( this, xOffset, yOffset,
	                          clip.x, clip.y, clip.width, clip.height,
	                          fg, bg, fnt, false);
	if ( xClr != null )
		g.setXORMode( xClr);
		
	return g;
}

public Graphics create ( int x, int y, int width, int height ) {
	Graphics g = getGraphics( this, x + xOffset, y + yOffset,
	                          0, 0, width, height,
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
	return drawImage( img, x, y, img.width, img.height, bgcolor, observer);
}

public boolean drawImage ( Image img, int x, int y, ImageObserver observer) {
	return drawImage( img, x, y, img.width, img.height, null, observer);
}

public boolean drawImage ( Image img, int x, int y, int width, int height, Color background, ImageObserver observer ) {
	if ((Image.checkImage( img, -1, -1, observer, true) & ImageObserver.ALLBITS) != 0) {
		if(img.width != width || img.height != height) {
			drawImgScaled(img, x,y , x+width, y+height, 0, 0,img.width,img.height, background);
		}
		else {
			drawImg(img, x, y, 0, 0, width, height, background);
		}
		return true;
	}
	return false;
}

public boolean drawImage ( Image img, int x, int y, int width, int height, ImageObserver observer){
	return drawImage( img, x, y, width, height, null, observer);
}

public boolean drawImage ( Image img, int dx0, int dy0, int dx1, int dy1,
				            int sx0, int sy0, int sx1, int sy1,
				            Color bgColor, ImageObserver observer) {
	if ( (Image.checkImage( img, -1, -1, observer, true) & ImageObserver.ALLBITS) != 0 ) {
		// we don't create a scaled Image instance since we can draw scaled
		drawImgScaled( img, dx0, dy0, dx1, dy1, sx0, sy0, sx1, sy1, bgColor);
		return true;
	}
	return false;
}

public boolean drawImage ( Image img, int dx1, int dy1, int dx2, int dy2,
				            int sx1, int sy1, int sx2, int sy2,
				            ImageObserver observer) {
	return drawImage( img, dx1, dy1, dx2, dy2, sx1, sy1, sx2, sy2, null, observer);
}

void drawImg ( Image img, int x, int y, int sx, int sy,
	             int width, int height, Color background ){
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

public void drawRect ( int x, int y, int width, int height ){
	Toolkit.graDrawRect( nativeData, x, y, width, height);
}

public void drawRoundRect ( int x, int y, int width, int height,
			    int arcWidth, int arcHeight){
	Toolkit.graDrawRoundRect( nativeData, x, y, width, height, arcWidth, arcHeight);
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
	Toolkit.graFillRect( nativeData, x, y, width, height);
}

public void fillRoundRect ( int x, int y, int width, int height, int arcWidth, int arcHeight ){
	Toolkit.graFillRoundRect( nativeData, x, y, width, height, arcWidth, arcHeight);
}

protected void finalize (){
	if ( nativeData != null ) {
		Toolkit.graFreeGraphics( nativeData);
		nativeData = null;
	}
}

public Shape getClip (){
	return clip;
}

public Rectangle getClipBounds() {
	return clip;
}

/**
 * @deprecated, use getClipBounds()
 */
public Rectangle getClipRect() {
	return (getClipBounds());
}

static Graphics getClippedGraphics ( Component c, int xClip, int yClip, int wClip, int hClip, boolean blank ) {
	Container   p;
	int         xOff = 0, yOff = 0;
	Color       fg  = c.fgClr;
	Color       bg  = c.bgClr;
	Font        fnt = c.font;

	if ( (c.width == 0) || (c.height == 0) ) return null;

	while ( c.isVisible ) {		
		if ( c.parent == null ) {
			return getGraphics( c, xOff, yOff, xClip, yClip, wClip, hClip,
			                             fg, bg, fnt, blank);
		}
		else {
			xOff += c.x;
			yOff += c.y;
			p = c.parent;

			// clip to parent (inset aware)
			int  px  = 0, py = 0;
			int  pxw = p.width, pyh = p.height;
			int  xwClip, yhClip, cxw, cyh;
			
			if ( p.insets != Insets.noInsets ) {
				Insets in  = p.insets;
				px  += in.left;
				py  += in.top;
				pxw -= in.right;
				pyh -= in.bottom;
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
			
			if ( (cxw = xOff + xwClip) > pxw )
				wClip -= cxw - pxw;
			if ( (cyh = yOff + yhClip) > pyh )
				hClip -= cyh - pyh;


			c = p;
			if ( fg == null )  fg  = c.fgClr;
			if ( bg == null )  bg  = c.bgClr;
			if ( fnt == null ) fnt = c.font;
		}
	}

	return null;
}

public Color getColor() {
	return fg;
}

public Font getFont() {
	return fnt;
}

public FontMetrics getFontMetrics () {
	return FontMetrics.getFontMetrics( fnt);
}

public FontMetrics getFontMetrics ( Font fnt ) {
	return FontMetrics.getFontMetrics( fnt);
}

static Graphics getGraphics ( Object target, int xOffset, int yOffset,
		                   int xClip, int yClip, int wClip, int hClip,
		                   Color fg, Color bg, Font fnt, boolean blank ) {
	Graphics g;
	Ptr tgtData = null;
	int tgtType = 0;

	if ( target instanceof Window ){
		tgtData = ((Window)target).nativeData;
		tgtType = 0;
	}
	else if ( target instanceof Image ) {
		tgtData = ((Image)target).nativeData;
		tgtType = 1;
	}
	else if ( target instanceof Graphics ) {

		tgtData = ((Graphics)target).nativeData;
		tgtType = 2;
	}

	if ( tgtData == null ) {
		return null;
	}

	synchronized ( lock ) {
		if ( cache == null ) {
			g = new Graphics();
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
	
	if ( fnt != null )	this.fnt     = fnt;
	if ( fg != null )   this.fg      = fg;
	if ( bg != null )   this.bg      = bg;

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
