/*
 * Graphics - abstract draw object
 *
 * Note that this had to be changed into an abstract class with *some*
 * generic methods (most notably drawString()), because there are strange subclasses
 * out there using this parent genericity (e.g. SwingGraphics)
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *	The Kaffe.org's developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author P.C.Mehlitz
 */

package java.awt;

import java.awt.image.ImageObserver;

abstract public class Graphics
{
protected Graphics () {
}

abstract public void clearRect ( int x, int y, int width, int height );

abstract public void clipRect ( int x, int y, int width, int height );

abstract public void copyArea ( int x, int y, int width, int height, int dx, int dy );

public Graphics create () {
	try {
		return ((Graphics)clone());
	}
	catch (CloneNotSupportedException _) {
		return (null);
	}
}

public Graphics create ( int x, int y, int width, int height ) {
	// slow, generic version
	Graphics g = create();
		
	// modify cloned state acording to request
	g.translate( x, y);
	g.clipRect( 0, 0, width, height); // spec says this should be the intersection

	return g;
}

abstract public void dispose ();

public void draw3DRect ( int x, int y, int width, int height, boolean raised ) {
	// this is the slow, but generic version
	Color clr, bri, drk;
	int   xw = x + width;
	int   yh = y + height;

	clr = getColor();
	bri = clr.brighter();
	drk = clr.darker();

	setColor( (raised) ? bri : drk);

	drawLine( x, y, xw-1, y); // beware of xor mode (don't overdraw corners)
	drawLine( x, y+1, x, yh);

	setColor( (raised) ? drk : bri);

	drawLine( xw, y, xw, yh);
	drawLine( x+1, yh, xw-1, yh);

	setColor( clr);
}

abstract public void drawArc ( int x, int y, int width, int height,
		      int startAngle, int arcAngle );

public void drawBytes ( byte data[], int offset, int length, int x, int y ) {
	// awefully slow, generic version
	String s = new String( data, 0, offset, length);
	drawString( s, x, y);
}

public void drawChars ( char data[], int offset, int length, int x, int y ){
	// awefully slow, generic version
	String s = new String( data, offset, length);
	drawString( s, x, y);
}

abstract public boolean drawImage (Image img, int x, int y, Color bgcolor,
								   ImageObserver observer);

abstract public boolean drawImage ( Image img, int x, int y, ImageObserver observer);

abstract public boolean drawImage ( Image img, int x, int y, int width, int height, Color background,
									ImageObserver observer );

abstract public boolean drawImage ( Image img, int x, int y, int width, int height,
									ImageObserver observer);

abstract public boolean drawImage ( Image img,
		int dx0, int dy0, int dx1, int dy1,
		int sx0, int sy0, int sx1, int sy1,
		Color bgColor, ImageObserver observer);

abstract public boolean drawImage ( Image img,
		int dx1, int dy1, int dx2, int dy2,
		int sx1, int sy1, int sx2, int sy2,
		ImageObserver observer);

abstract public void drawLine ( int x1, int y1, int x2, int y2 );

abstract public void drawOval ( int x, int y, int width, int height );

public void drawPolygon ( Polygon p ){
	// generic version
	drawPolygon( p.xpoints, p.ypoints, p.npoints);
}

abstract public void drawPolygon ( int xPoints[], int yPoints[], int nPoints );

abstract public void drawPolyline ( int xPoints[], int yPoints[], int nPoints );

public void drawRect ( int x, int y, int width, int height ) {
	// Awefully slow generic version. Be aware of possible xor mode
	// (don't overdraw corners)
	int xw = x + width;
	int yh = y + height;
	
	drawLine( x, y, xw, y);
	drawLine( x, y+1, x, yh);
	drawLine( xw, y+1, xw, yh);
	drawLine( x+1, yh, xw-1, yh);
}

abstract public void drawRoundRect ( int x, int y, int width, int height,
			    int arcWidth, int arcHeight);

abstract public void drawString ( String str, int x, int y );

public void fill3DRect ( int x, int y, int width, int height, boolean raised ){
	// slow, generic version
	fillRect( x+1, y+1, width-2, height-2);
	draw3DRect( x, y, width, height, raised);
}

abstract public void fillArc ( int x, int y, int width, int height,
		      int startAngle, int arcAngle );

abstract public void fillOval ( int x, int y, int width, int height );

public void fillPolygon ( Polygon p ) {
	fillPolygon( p.xpoints, p.ypoints, p.npoints);
}

abstract public void fillPolygon ( int xPoints[], int yPoints[], int nPoints );

abstract public void fillRect ( int x, int y, int width, int height );

abstract public void fillRoundRect ( int x, int y, int width, int height,
									 int arcWidth, int arcHeight );

protected void finalize () throws Throwable {
	super.finalize();
}

Color getBackColor () {
	return null;
}

abstract public Shape getClip ();

public Rectangle getClipBounds() {
	// Another return object which is modified by Swing, causing more garbage
	return (getClipBounds(new Rectangle()));
}

abstract public Rectangle getClipBounds(Rectangle rect);

int getClipHeight () {
	// this is only here to be resolved in concrete subclasses
	return getClipBounds().height;
}

/**
 * @deprecated, use getClipBounds()
 */
public Rectangle getClipRect() {
	// finally, at least one deprecated that refers to the new method
	return getClipBounds();
}

int getClipWidth () {
	// this is only here to be resolved in concrete subclasses
	return getClipBounds().width;
}

int getClipX () {
	// this is only here to be resolved in concrete subclasses
	return getClipBounds().x;
}

int getClipY () {
	// this is only here to be resolved in concrete subclasses
	return getClipBounds().y;
}

abstract public Color getColor();

abstract public Font getFont();

public FontMetrics getFontMetrics () {
	return (getFontMetrics(getFont()));
}

abstract public FontMetrics getFontMetrics ( Font fnt );

void paintChild ( Component c, boolean isUpdate ) {
	Rectangle clip = getClipBounds();

	if ( (clip.x > (c.x + c.width))  ||
	     (clip.y > (c.y + c.height)) ||
	     (c.x > (clip.x + clip.width)) ||
	     (c.y > (clip.y + clip.height)) )
		return;

	// a nasty app problem with Canvases and Panels, see NativeGraphics
	if ( (c.flags & Component.IS_ASYNC_UPDATED) != 0 ){
		c.repaint();
		return;
	}

	Graphics g = create( c.x, c.y, c.width, c.height);
	if ( g != null ) {
		if ( isUpdate )
			c.update( g);
		else
			c.paint( g);

		g.dispose();
	}
	
	c.flags &= ~Component.IS_DIRTY; // no need for subsequent repaints anymore

}

void setBackColor ( Color c ) {
}

abstract public void setClip ( Shape clip );

abstract public void setClip ( int x, int y, int width, int height );

abstract public void setColor ( Color clr );

abstract public void setFont ( Font newFnt );

abstract public void setPaintMode();

void setTarget ( Component tgt ) {
}

abstract public void setXORMode ( Color newXorClr );

Graphics subGraphics () {
	return this;
}

public String toString() {
	return (getClass().getName() + " [" + getFont() + "," + getColor() + ']');
}

abstract public void translate ( int x, int y );
}
