/*
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
 * Copyright (c) 2004, 2005
 * 	The Kaffe.org's developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author P.C.Mehlitz
 */

package java.awt;

import java.awt.image.ImageObserver;
import gnu.classpath.Pointer;

class NativeGraphics
  extends Graphics
{
	Pointer nativeData;
	int xOffset;
	int yOffset;
	Color fgClr;
	Color bgClr;
	Font font;
	int xClip;
	int yClip;
	int wClip;
	int wClipDefault;
	int hClip;
	int hClipDefault;
	Color xClr;
/*
 * this field can be used to link a Graphics object to a non-native
 * Component (e.g. Panel, Canvas), so that it can be notified in case the
 * component changes visibility and/or location/size.
 */
	Component target;
	GraphicsLink link;
	NativeGraphics next;
	static NativeGraphics cache;
	static Object lock = new Object();
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
	
	int xNew = (x > xClip) ? x : xClip;
	int yNew = (y > yClip) ? y : yClip;
	
	int clipXw = xClip + wClip;
	int clipYh = yClip + hClip;
	int xw     = x + width;
	int yh     = y + height;
	
	int wNew   = ((xw > clipXw) ? clipXw : xw) - xNew;
	int hNew   = ((yh > clipYh) ? clipYh : yh) - yNew;
	
	if ( wNew < 0 ) wNew = 0;
	if ( hNew < 0 ) hNew = 0;
	
	xClip = xNew;
	yClip = yNew;
	wClip = wNew;
	hClip = hNew;
	
	Toolkit.graSetClip( nativeData, xClip, yClip, wClip, hClip);
}

public void copyArea ( int x, int y, int width, int height, int dx, int dy ){
	Toolkit.graCopyArea( nativeData, x, y, width, height, dx, dy);
}

public Graphics create () {
	NativeGraphics g = getGraphics( this, nativeData, TGT_TYPE_GRAPHICS,
	                xOffset, yOffset,
									xClip, yClip, wClip, hClip,
									fgClr, bgClr, font, false);
	if ( xClr != null )
		g.setXORMode( xClr);

	if ( target != null ){
		target.linkGraphics( g);
	}

	return g;
}

public Graphics create ( int x, int y, int width, int height ) {
	// pretty redundant with setClip, but since it might be called frequently
	// (Swing, again), it is a good idea to save a native call

	int xw = x + width;
	int yh = y + height;
	int cxw = xClip + wClip;
	int cyh = yClip + hClip;
	
	int clx = (x > xClip) ? x : xClip;
	int cly = (y > yClip) ? y : yClip;
	int clw = ((xw > cxw) ? cxw : xw) - clx;
	int clh = ((yh > cyh) ? cyh : yh) - cly;
	
	NativeGraphics g = getGraphics( this, nativeData, TGT_TYPE_GRAPHICS,
	                x + xOffset, y + yOffset,
									clx - x, cly - y, clw, clh,
									fgClr, bgClr, font, false);
							
	if ( xClr != null )
		g.setXORMode( xClr);

	if ( target != null ){
		target.linkGraphics( g);
	}

	return g;
}

public void dispose () {
	if ( bgClr == null ) {
		// We have to provide some protection against double disposes
		// since it would make the whole cache inconsistent (leads to
		// simultanous use). We shouldn't store the target here because
		// this would lead to a memory leak if Graphics objects are
		// not disposed. Each of the colors should be a good choice
		// (since they can't get null while in use)
		return;
	}

	if ( target != null ) {
		// make sure we don't get a interspersed updateLinkedGraphics()
		synchronized ( target ) {
			xClr = null;
			font = null;
			fgClr = null;
			bgClr = null;

			target.unlinkGraphics( this);
			target = null;
		}
	}
	else {
		xClr = null;
		font = null;
		fgClr = null;
		bgClr = null;
	}

	if ( nativeData != null ) {
		Toolkit.graFreeGraphics( nativeData);
		nativeData = null;
	}

	synchronized ( lock ) {
		next = cache;
		cache = this;
	}
}

public void draw3DRect ( int x, int y, int width, int height, boolean raised ){
	// we pass the rgb color value because it might be stored on the native
	// side as pixel value (requiring an additional color conversion)
	Toolkit.graDraw3DRect( nativeData, x, y, width, height, raised, fgClr.value);
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
	// if the image isn't loaded yet, start production and return false
	if ( !Image.loadImage( img, -1, -1, observer)) {
		return (false);
	}
	else {
		drawImg( img, x, y, 0, 0, img.width, img.height, bgcolor);
		return true;
	}
}

public boolean drawImage ( Image img, int x, int y, ImageObserver observer) {
	if ( !Image.loadImage( img, -1, -1, observer)) {
		return (false);
	}
	else {
		drawImg( img, x, y, 0, 0, img.width, img.height, null);
		return true;
	}
}

public boolean drawImage ( Image img, int x, int y, int width, int height, Color background, ImageObserver observer ) {

	// Load image if it's not loaded - we don't scale because we
	// can do this while drawing.
	if ( !Image.loadImage( img, -1, -1, observer)) {
		return (false);
	}

	// Handle proportional widths and heights
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

	if ( (img.width != width) || (img.height != height) ){
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

	// Load image if it's not loaded - we don't scale because we
	// can do this while drawing.
	if (!Image.loadImage( img, -1, -1, observer)) {
		return (false);
	}

	// If any of the source points are negative then error
	if (sx0 < 0 || sy0 < 0 || sx1 < 0 || sy1 < 0) {
		return (false);
	}

	if ( ((sx1 - sx0) == (dx1 - dx0)) && ((sy1 - sy0) == (dy1 - dy0)) ) {
		// bozo. don't you know about the costs of image scaling?
		drawImg( img, dx0, dy0, sx0, sy0, (sx1 - sx0), (sy1 - sy0), bgColor);
	}
	else {
		// We don't create a scaled Image instance since we can draw scaled
		drawImgScaled( img, dx0, dy0, dx1, dy1, sx0, sy0, sx1, sy1, bgColor);
	}
	
	return true;
}

public boolean drawImage ( Image img,
		int dx1, int dy1, int dx2, int dy2,
		int sx1, int sy1, int sx2, int sy2,
		ImageObserver observer) {
	return drawImage(img, dx1, dy1, dx2, dy2, sx1, sy1, sx2, sy2, null, observer);
}

void drawImg ( Image img,	int x, int y, int sx, int sy,	int width, int height, Color background ){	
	if ( (img.flags & Image.BLOCK_FRAMELOADER) != 0 ){
		img.activateFrameLoader();
	}
	else if ( (img.flags & Image.IS_ANIMATION) != 0 ) {
		Toolkit.imgSetFrame( img.nativeData, 0);
	}
	
	Toolkit.graDrawImage( nativeData, img.nativeData,
	                      sx, sy, x, y, width, height,
		                    (background == null) ? -1 : background.getNativeValue());
}

void drawImgScaled ( Image img,
		int dx0, int dy0, int dx1, int dy1,
		int sx0, int sy0, int sx1, int sy1,
		Color background ){
	if ( img.nativeData != null ) {
		if ( (img.flags & Image.BLOCK_FRAMELOADER) != 0 )
			img.activateFrameLoader();

		Toolkit.graDrawImageScaled( nativeData, img.nativeData,
			dx0, dy0, dx1, dy1,
			sx0, sy0, sx1, sy1,
			(background == null) ? -1 : background.getNativeValue());
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

public void drawRect ( int x, int y, int width, int height ) {
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
	Toolkit.graFill3DRect( nativeData, x, y, width, height, raised, fgClr.value);
}

public void fillArc ( int x, int y, int width, int height, int startAngle, int arcAngle ){
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

protected void finalize () throws Throwable {
	if ( nativeData != null ) {
		Toolkit.graFreeGraphics( nativeData);
		nativeData = null;
	}
	super.finalize();
}

Color getBackColor () {
	return bgClr;
}

public Shape getClip (){
	return (getClipRect());
}

public Rectangle getClipBounds(Rectangle rect) {
	rect.x = xClip;
	rect.y = yClip;
	rect.width = wClip;
	rect.height = hClip;

	return rect;
}

int getClipHeight () {
	return hClip;
}

int getClipWidth () {
	return wClip;
}

int getClipX () {
	return xClip;
}

int getClipY () {
	return yClip;
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

	if ( (c.flags & Component.IS_ADD_NOTIFIED) == 0 ) return null;

	if ( g == null ) {
		fg  = c.fgClr;	bg  = c.bgClr;	fnt = c.font;
	}
	else { // otherwise, the silly compiler complains about missing init
		fg = g.fgClr;   bg = g.bgClr;   fnt = g.font;
	}

	while ( true ) {
		if ( c.parent == null ) {
			if ( (Toolkit.flags & Toolkit.EXTERNAL_DECO) != 0 ) {
				// compensate the artificial Frame decoration offsets (NOT insets)
				// note that this requires the graphics offset to be private
				xOff -= c.deco.x;
				yOff -= c.deco.y;
			}

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
	   	                           fg.getNativeValue(),
	    	                         bg.getNativeValue(),
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
	return fgClr;
}

public Font getFont() {
	return font;
}

public FontMetrics getFontMetrics ( Font fnt ) {
	return FontMetrics.getFontMetrics( fnt);
}

static NativeGraphics getGraphics ( Object target, Pointer tgtData, int tgtType,
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
	g.xClip  = xClip;
	g.yClip  = yClip;
	g.wClip  = wClip;
	g.wClipDefault = wClip;
	g.hClip = hClip;
	g.hClipDefault = hClip;
	g.font     = fnt;
	g.fgClr    = fg;
	g.bgClr    = bg;

if ( fg == null ){ Thread.currentThread().dumpStack();}

	g.nativeData = Toolkit.graInitGraphics( g.nativeData, tgtData, tgtType,
	                                        xOffset, yOffset,
	                                        xClip, yClip, wClip, hClip,
	                                        fnt.nativeData,
	                                        fg.getNativeValue(), bg.getNativeValue(),
	                                        blank);
	return g;
}

void paintChild ( Component c, boolean isUpdate ) {
	// needs to be here because we are the only one knowing about the clip fields
	// (a generic version would have to use the dreadful getClipBounds())

	int xw = c.x + c.width;
	int yh = c.y + c.height;
	int cxw = xClip + wClip;
	int cyh = yClip + hClip;

	if ( (xClip > xw) || (yClip > yh) || (c.x > cxw) || (c.y > cyh) )
		return;

	int clx = (c.x > xClip) ? c.x : xClip;
	int cly = (c.y > yClip) ? c.y : yClip;
	int clw = ((xw > cxw) ? cxw : xw) - clx;
	int clh = ((yh > cyh) ? cyh : yh) - cly;
	
	clx -= c.x;
	cly -= c.y;

// HACK: maybe we have to set all widgets to IS_ASYNC_UPDATED because
// of bad apps implicitly relying on async update solicitation in order to avoid flicker
// (doing lots of redundant repaint requests without even knowing about it). The downside is
// that it would slow down all nice apps

//	if ( (c.flags & Component.IS_ASYNC_UPDATED) != 0 ) {
	if ( (c.flags & Component.IS_NATIVE_LIKE) != 0 ) {
		// This is a really nasty problem with Panels and Canvases: they don't get
		// drawn sync, but usually receive their own native paint (not even update!)
		// events *after* the parent got painted. We have to simulate this because
		// - believe it or not - some apps rely on UPDATE/PAINT vs. 
		// COMPONENT_RESIZED/SHOWN order (this is *bad*, since it heavily depends
		// on unspecified behavior ofthe AWT *and* the underlying native window system).
		// Note that we shouldn't leave the repaint up to the ragman (processPaintEvent)
		// since some apps might even call update/paint explicitly (again, bad!). But
		// we want to support at least those who call super.paint()
		// The PAINT vs. UPDATE problem also shows up in Container.emitRepaints
		Toolkit.eventQueue.postPaintEvent( PaintEvt.PAINT, c, clx, cly, clw, clh);
	}
	else {
		NativeGraphics g = getGraphics( this, nativeData, TGT_TYPE_GRAPHICS,
										                c.x + xOffset, c.y + yOffset,
																		clx, cly, clw, clh,
																		c.fgClr, c.bgClr, c.font, false);
		if ( g != null ) {
			if ( isUpdate )
				c.update( g);
			else
				c.paint( g);

			g.dispose();
		}
	}
	
	c.flags &= ~Component.IS_DIRTY; // no need for subsequent repaints anymore
}

void setBackColor ( Color clr ){
	if ( (clr != null) && (clr != bgClr) ) {
		bgClr = clr;
		
		Toolkit.graSetBackColor( nativeData, bgClr.getNativeValue());
	}
}

public void setClip ( Shape clip ){
	if ( clip != null ) {
		Rectangle r = clip.getBounds();
		setClip( r.x, r.y, r.width, r.height);
	} else if ( target != null) {
		setClip( 0, 0, target.width, target.height );
	} else {
		setClip( 0, 0, wClipDefault, hClipDefault );
	}
}

public void setClip ( int x, int y, int width, int height ) {
	if ( target != null ) {
		// be paranoid, native widgets automatically clip to their boundaries
		if ( x < 0 ) x = 0;
		if ( y < 0 ) y = 0;
		if ( (x + width) > target.width )
			width = target.width - x;
		if ( (y + height) > target.height ) 
			height = target.height - y;
	}

	xClip      = x;
	yClip      = y;
	wClip  = width;
	hClip = height;		

	Toolkit.graSetClip ( nativeData, x, y, width, height);
}

public void setColor ( Color clr ){
	if ( (clr != null) && (clr != fgClr) ) {
		fgClr = clr;
		Toolkit.graSetColor( nativeData, fgClr.getNativeValue());
	}
}

public void setFont ( Font newFnt ) {
	if ( (newFnt != null) && (newFnt != font) ){
		font = newFnt;
		Toolkit.graSetFont( nativeData, font.nativeData);
	}
}

void setGraphics ( Pointer tgtData, int tgtType, int xOffset, int yOffset,
		               int xClip, int yClip, int wClip, int hClip,
		               Color fg, Color bg, Font fnt, boolean blank ) {

	this.xOffset = xOffset;
	this.yOffset = yOffset;
	this.xClip  = xClip;
	this.yClip  = yClip;
	this.wClip  = wClip;
	this.hClip = hClip;
	
	if ( fnt != null ) {
		font = fnt;
	}
	if ( fg != null ) {
		fgClr = fg;
	}
	if ( bg != null ) {
		bgClr = bg;
	}

	nativeData = Toolkit.graInitGraphics( nativeData, tgtData, tgtType,
	                                      xOffset, yOffset,
	                                      xClip, yClip, wClip, hClip,
	                                      font.nativeData,
	                                      fgClr.getNativeValue(),
	                                      bgClr.getNativeValue(),
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
		Toolkit.graSetXORMode( nativeData, xClr.getNativeValue());
	}
}

public String toString() {
	return  getClass().getName() +
	           ' ' + hashCode() + 
	           " [" + xOffset + ',' + yOffset +
	           " clip:" + xClip+','+yClip+' '+wClip+','+hClip + ']';
}

public void translate ( int x, int y ) {
	xOffset += x;
	yOffset += y;

	xClip -= x;
	yClip -= y;

	Toolkit.graSetOffset( nativeData, xOffset, yOffset);
}
}
