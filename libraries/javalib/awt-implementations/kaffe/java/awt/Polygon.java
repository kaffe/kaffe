package java.awt;


/**
 * class Polygon - represents (closed or open) arrays of points
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
 */
import java.awt.geom.AffineTransform;
import java.awt.geom.PathIterator;
import java.awt.geom.Point2D;
import java.awt.geom.Rectangle2D;
import java.io.Serializable;

public class Polygon
  implements Shape, Serializable
{
	/** @serial The total number of points. This value can be NULL. */
	public int npoints;

	/** @serial The array of x coordinates. */
	public int[] xpoints;

	/** @serial The array of y coordinates. */
	public int[] ypoints;

	/** @serial 
	 * Bounds of the polygon.  This value can be NULL.  See getBounds()
	 */
	protected Rectangle bounds;
	private static final long serialVersionUID = -6460061437900069969L;

public Polygon() {
	npoints = 0;
	xpoints = new int[5];
	ypoints = new int[5];
}

public Polygon ( int x[], int y[], int n ) {
	int l = x.length;
	if ( y.length < l ) l = y.length;

	if ( l < n ) n = l;
	npoints = n;
	
	xpoints = new int[n];
	ypoints = new int[n];
	
	System.arraycopy( x, 0, xpoints, 0, n);
	System.arraycopy( y, 0, ypoints, 0, n);	
}

public void addPoint ( int x, int y ) {
	if (npoints == xpoints.length)
		grow();
	
	xpoints[npoints] = x;
	ypoints[npoints] = y;
	npoints++;

	if (bounds != null) {	
		int d;
		if ( x < bounds.x ) bounds.x = x;
		if ( (d = x - bounds.x) > bounds.width ) bounds.width = d;
		if ( y < bounds.y ) bounds.y = y;
		if ( (d = y - bounds.y) > bounds.height ) bounds.height = d;
	}
}

public boolean contains ( Point p ) {
	return contains( p.x, p.y);
}

public boolean contains ( Point2D p ) {
	return contains( p.getX(), p.getY());
}

public boolean contains ( Rectangle2D r ) {
	return contains( r.getX(), r.getY(), r.getWidth(), r.getHeight());
}

public boolean contains ( int x, int y ) {
	return (inside( x, y ));
}

public boolean contains(double x, double y) {
	return contains ((int) x, (int) y);
}

public boolean contains(double x, double y, double w, double h) {
	return contains (x, y)
		&& contains(x + w, y)
		&& contains(x, y + h)
		&& contains(x + w, y + h);
}

/**
 * @deprecated ,use contains(x,y)
 */
public boolean inside ( int x, int y ) {
	int   i, j, k=0;
	int   x0, x1, y0, y1;
	float m;

	if ( bounds == null ) getBounds();
	if ( (x < bounds.x) || (y < bounds.y) ||
			 (x > (bounds.x + bounds.width))  || (y > (bounds.y + bounds.height)) )
		return false;
		
	for ( j=0, i=1; j<npoints; j++, i++ ) {
		if ( i == npoints ) i = 0;
		
		x0 = xpoints[j]; x1 = xpoints[i];
		y0 = ypoints[j]; y1 = ypoints[i];

		if ( (y0 > y) == (y1 > y) )    // no intersection at all
			continue;
		if ( y0 == y )                 // don't count testline vertices twice
			continue;
		if ( (x0 < x) && (x1 < x) )    // just count intersections to the right
			continue;
		if ( x1 == x0 ) {              // we know where vertical lines intersect
			k++;
			continue;
		}

		m = (y1 - y0) / (x1 - x0);     // test if intersection is to the right
		if ( ((m * x0 - y0) / m) > 0 )
			k++;
	}

	return ((k & 1) != 0);
}

public Rectangle getBounds() {
	return (getBoundingBox());
}

public Rectangle2D getBounds2D() {
	return (getBounds());
}

/** taken from GNU Classpath */
public PathIterator getPathIterator(final AffineTransform transform) {
	return new PathIterator() {
			/** The current vertex of iteration. */
			private int vertex;
			
			public int getWindingRule() {
				return WIND_EVEN_ODD;
			}

			public boolean isDone() {
				return vertex > npoints;
			}

			public void next() {
				vertex++;
			}

			public int currentSegment(float[] coords) {
				if (vertex >= npoints)
					return SEG_CLOSE;
				coords[0] = xpoints[vertex];
				coords[1] = ypoints[vertex];
				if (transform != null)
					transform.transform(coords, 0, coords, 0, 1);
				return vertex == 0 ? SEG_MOVETO : SEG_LINETO;
			}

			public int currentSegment(double[] coords) {
				if (vertex >= npoints)
					return SEG_CLOSE;
				coords[0] = xpoints[vertex];
				coords[1] = ypoints[vertex];
				if (transform != null)
					transform.transform(coords, 0, coords, 0, 1);
				return vertex == 0 ? SEG_MOVETO : SEG_LINETO;
			}
		};
}

/* taken from GNU Classpath */
public PathIterator getPathIterator(AffineTransform transform, double flatness) {
	return getPathIterator(transform);
}

/**
 * @deprecated , use getBounds()
 */
public Rectangle getBoundingBox() {
	if ( bounds != null )
		return bounds;

	int i, x, y;
	int x0 = Integer.MAX_VALUE;
	int y0 = x0;
	int x1 = Integer.MIN_VALUE;
	int y1 = x1;

	for ( i=0; i<npoints; i++ ) {
		x = xpoints[i];
		y = ypoints[i];
		
		if ( x < x0 )  x0 = x;
		if ( x > x1 )  x1 = x;
		
		if ( y < y0 )  y0 = y;
		if ( y > y1 )  y1 = y;
	}
	
	bounds = new Rectangle( x0, y0, (x1 - x0), (y1 - y0));
	
	return bounds;
}

void grow () {
	int   n = xpoints.length * 3 / 2;

	int[] a = new int[n];
	System.arraycopy( xpoints, 0, a, 0, npoints);
	xpoints = a;
		
	a = new int[n];
	System.arraycopy( ypoints, 0, a, 0, npoints);
	ypoints = a;
}

public boolean intersects(double x, double y, double w, double h) {
	return contains(x, y)
		|| contains(x + w, y)
		|| contains(x, y + h)
		|| contains (x + w, y + h);
}

public boolean intersects(Rectangle2D r) {
	return intersects(r.getX(), r.getY(), r.getWidth(), r.getHeight());
}

public void invalidate() {
	bounds = null;
}

public void reset() {
	invalidate();
	npoints = 0;
}

public void translate ( int dx, int dy ) {
	int i;

	for ( i = 0; i < npoints; i++ ) {
		xpoints[i] += dx;
		ypoints[i] += dy;
	}

	if (bounds != null) {
		bounds.x += dx;
		bounds.y += dy;
	}
}
}
