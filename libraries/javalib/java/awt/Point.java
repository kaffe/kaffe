package java.awt;


/**
 * class Point - represent (x,y) locations
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
 */
public class Point
{
	public int x;
	public int y;

public Point () {
}

public Point ( Point pt ) {
	x = pt.x;
	y = pt.y;
}

public Point ( int x, int y ) {
	this.x = x;
	this.y = y;
}

public boolean equals ( Object obj ) {
	if ( obj instanceof Point ) {
		Point pt = (Point) obj;
		return (pt.x == x) && (pt.y == y);
	}
	else
		return false;
}

public Point getLocation () {
	return new Point( x, y);
}

public int hashCode () {
	return x ^ (y*31);
}

/**
 * @deprecated (I presume), use setLocation()
 */
public void move ( int xNew, int yNew ) {
	x = xNew;
	y = yNew;
}

public void setLocation ( Point pt ) {
	x = pt.x;
	y = pt.y;
}

public void setLocation ( int xNew, int yNew ) {
	x = xNew;
	y = yNew;
}

public String toString() {
	return getClass().getName() + " [" + x + ',' + y + ']';
}

public void translate ( int xDelta, int yDelta ) {
	x += xDelta;
	y += yDelta;
}
}
