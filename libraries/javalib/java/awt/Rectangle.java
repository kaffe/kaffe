package java.awt;


/**
 * class Rectangle - represents a (point, extension) tuple
 *
 * Copyright (c) 1998
 *   Transvirtual Technologies Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 * @author P.C.Mehlitz
 */
public class Rectangle
  implements Shape
{
	public int x;
	public int y;
	public int width;
	public int height;

public Rectangle() {
}

public Rectangle ( Dimension d ) {
	x = 0;
	y = 0;
	width  = d.width;
	height = d.height;
}

public Rectangle ( Point pt ) {
	x = pt.x;
	y = pt.y;

	width = 0;
	height = 0;
}

public Rectangle ( Point pt, Dimension d ) {
	x = pt.x;
	y = pt.y;
	
	width  = d.width;
	height = d.height;
}

public Rectangle ( Rectangle r ) {
	x      = r.x;
	y      = r.y;
	width  = r.width;
	height = r.height;
}

public Rectangle ( int width, int height ) {
	x = 0;
	y = 0;
	
	this.width  = width;
	this.height = height;
}

public Rectangle ( int x, int y, int width, int height ) {
	this.x      = x;
	this.y      = y;
	this.width  = width;
	this.height = height;
}

public void add ( Point pt ) {
	add( pt.x, pt.y);
}

public void add ( Rectangle r ) {
	int xw, yh, rxw, ryh;
	
	xw  = x + width;
	yh  = y + height;
	rxw = r.x + r.width;
	ryh = r.y + r.height;
	
	if ( r.x < x )  x = r.x;
	if ( r.y < y )  y = r.y;
	
	width  = ((xw > rxw) ? xw : rxw) - x;
	height = ((yh > ryh) ? yh : ryh) - y;
}

public void add ( int xExt, int yExt ) {
	int xw, yh;

	xw = x + width;
	yh = y + height;
	
	if ( xExt < x )  x = xExt;
	if ( yExt < y )  y = yExt;

	width  = ((xExt > xw) ? xExt : xw) - x;
	height = ((yExt > yh) ? yExt : yh) - y;
}

public boolean contains ( Point p ) {
	return contains( p.x, p.y);
}

public boolean contains ( int x, int y ) {
	return (inside(x, y));
}

public boolean equals ( Object obj ) {
	if ( obj instanceof Rectangle ) {
		Rectangle r = (Rectangle) obj;
		return (x == r.x) && (y == r.y) && (width == r.width) && (height == r.height);
	}
	else
		return false;
}

public Rectangle getBounds () {
	return new Rectangle( x, y, width, height);
}

public Point getLocation () {
	return new Point( x, y);
}

public Dimension getSize () {
	return new Dimension( width, height);
}

public void grow ( int xDelta, int yDelta ) {
	x      -= xDelta;
	y      -= yDelta;

	width  += 2 * xDelta;
	height += 2 * yDelta;
}

public int hashCode () {
	return  (x ^ (y*37) ^ (width*43) ^ (height*47));
}

/**
 * @deprecated (use contains() )
 */
public boolean inside ( int x, int y ) {
	return (this.x <= x) && ((this.x+width) >= x) && (this.y <= y) && ((this.y+height) >= y);
}

public Rectangle intersection ( Rectangle r ) {
	int xw  = x + width;
	int yh  = y + height;
	int rxw = r.x + r.width;
	int ryh = r.y + r.height;

	if ( (x > rxw) || (y > ryh) || (xw < r.x) || (yh < r.y) ) { // no intersection
		return new Rectangle();
	}
	else {
		int u = (x > r.x) ? x : r.x;
		int v = (y > r.y) ? y : r.y;
	
		int w = ((xw < rxw) ? xw : rxw) - u;
		int h = ((yh < ryh) ? yh : ryh) - v;

		return new Rectangle( u, v, w, h);
	}
}

public boolean intersects ( Rectangle r ) {
	if ( (x > (r.x + r.width))  ||
	     (y > (r.y + r.height)) ||
	     (r.x > (x + width))    ||
	     (r.y > (y + height)) )
		return false;
	else
		return true;
}

public boolean isEmpty () {
	return (width <= 0) || (height <= 0);
}

/**
 * @deprecated ( use setLocation() )
 */
public void move ( int xNew, int yNew ) {
	x = xNew;
	y = yNew;
}

/**
 * @deprecated (use setBounds() )
 */
public void reshape ( int x, int y, int width, int height ) {
	this.x      = x;
	this.y      = y;

	this.width  = width;
	this.height = height;
}

/**
 * @deprecated (use setSize() )
 */
public void resize ( int width, int height ) {
	this.width  = width;
	this.height = height;
}

public void setBounds ( Rectangle r ) {
	setBounds(r.x, r.y, r.width, r.height);
}

public void setBounds ( int x, int y, int width, int height ) {
	reshape(x, y, width, height);
}

public void setLocation ( Point pt ) {
	x = pt.x;
	y = pt.y;
}

public void setLocation ( int xNew, int yNew ) {
	move (xNew, yNew);
}

public void setSize ( Dimension d ) {
	setSize(d.width, d.height);
}

public void setSize ( int width, int height ) {
	resize(width, height);
}

public String toString() {
	return getClass().getName() + " [" + x + ',' + y +
	                              " "  + width + ',' + height + ']';
}

public void translate ( int xDelta, int yDelta ) {
	x += xDelta;
	y += yDelta;
}

public Rectangle union ( Rectangle r ) {
	int xw, yh, rxw, ryh, u, v, w, h;

	xw  = x + width;
	yh  = y + height;
	rxw = r.x + r.width;
	ryh = r.y + r.height;

	u = (x < r.x) ? x : r.x;
	v = (y < r.y) ? y : r.y;
	w = ((xw > rxw) ? xw : rxw) - u;
	h = ((yh > ryh) ? yh : ryh) - v;

	return new Rectangle( u, v, w, h);
}
}
