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

import java.awt.geom.Rectangle2D;
import java.io.Serializable;

public class Rectangle extends Rectangle2D
  implements Shape, Serializable
{
	private static final long serialVersionUID = -4345857070255674764L;

	/** @serial The x coordinate of the Rectangle.  */
	public int x;
	/** @serial The y coordinate of the Rectangle.  */
	public int y;
	/** @serial The width of the Rectangle.  */
	public int width;
	/** @serial The height of the Rectangle.  */
	public int height;

public Rectangle() {
}

public Rectangle ( Dimension d ) {
	setSize(d.width, d.height);
}

public Rectangle ( Point pt ) {
	setLocation(pt.x, pt.y);
}

public Rectangle ( Point pt, Dimension d ) {
	setBounds(pt.x, pt.y, d.width, d.height);
}

public Rectangle ( Rectangle r ) {
	setBounds(r.x, r.y, r.width, r.height);
}

public Rectangle ( int width, int height ) {
	setSize(width, height);
}

public Rectangle ( int x, int y, int width, int height ) {
	setBounds(x, y, width, height);
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

public boolean contains (Rectangle r) {
	return contains (r.x, r.y, r.width, r.height);
}

public boolean contains ( int x, int y ) {
	return (inside(x, y));
}

public boolean contains ( int x, int y, int w, int h ) {
	return (inside(x, y) && inside(x + width, y + height));
}

public Rectangle2D createIntersection(Rectangle2D r) {
	Rectangle2D result = getBounds2D();
	Rectangle2D.intersect(this, r, result);
	return result;
}

public Rectangle2D createUnion(Rectangle2D r) {
	Rectangle2D result = getBounds2D();
	Rectangle2D.union(this, r, result);
	return result;
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
	return new Rectangle(this);
}

public Rectangle2D getBounds2D() {
	return getBounds();
}

public double getHeight() {
	return height;
}

public Point getLocation () {
	return new Point( x, y);
}

public Dimension getSize () {
	return new Dimension( width, height);
}

public double getWidth() {
	return width;
}

public double getX() {
	return x;
}

public double getY() {
	return y;
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

/* taken from GNU Classpath */
public int outcode(double x, double y) {
	int result = 0;
	if (width <= 0)
		result |= OUT_LEFT | OUT_RIGHT;
	else if (x < this.x)
		result |= OUT_LEFT;
	else if (x > this.x + width)
		result |= OUT_RIGHT;
	if (height <= 0)
		result |= OUT_BOTTOM | OUT_TOP;
	else if (y < this.y) // Remember that +y heads top-to-bottom.
		result |= OUT_TOP;
	else if (y > this.y + height)
		result |= OUT_BOTTOM;
	return result;
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

public void setRect(double x, double y, double width, double height) {
	setBounds((int) x, (int) y, (int) width, (int) height);
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
	return getClass().getName() + "[x=" + x + ",y=" + y +
	                              ",width="  + width + ",height=" + height + ']';
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
