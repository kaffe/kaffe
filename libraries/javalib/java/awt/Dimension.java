package java.awt;
import java.awt.geom.Dimension2D;


/**
 * class Dimension - represents (width x height) extensions
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
 */
public class Dimension extends Dimension2D implements java.io.Serializable
{
	private static final long serialVersionUID = 4723952579491349524L;
	/** @serial The width dimension. Negative values can be used. */
	public int width;

	/** @serial The height dimension. Negative values can be used. */
	public int height;

public Dimension() {
}

public Dimension ( Dimension d ) {
	width = d.width;
	height = d.height;
}

public Dimension ( int w, int h ) {
	width  = w;
	height = h;
}

public boolean equals ( Object obj ) {
	if ( obj instanceof Dimension ) {
		Dimension d = (Dimension) obj;
		return (d.width == width) && (d.height == height);
	}
	else
		return false;
}

public double getWidth() {
	return width;
}

public double getHeight() {
	return height;
}

public Dimension getSize () {
	return new Dimension( width, height);
}

public void setSize ( Dimension d ) {
	width  = d.width;
	height = d.height;
}

public void setSize ( Dimension2D d ) {
	width  = (int)d.getWidth();
	height = (int)d.getHeight();
}

public void setSize ( int w, int h ) {
	width  = w;
	height = h;
}

public void setSize ( double w, double h ) {
	width  = (int)w;
	height = (int)h;
}

public String toString() {
	return getClass().getName() + "[width=" + width + ",height=" + height + ']';
}
}
