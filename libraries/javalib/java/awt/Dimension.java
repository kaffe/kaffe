package java.awt;


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

import java.lang.String;

public class Dimension
{
	public int width;
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

public Dimension getSize () {
	return new Dimension( width, height);
}

public void setSize ( Dimension d ) {
	width  = d.width;
	height = d.height;
}

public void setSize ( int w, int h ) {
	width  = w;
	height = h;
}

public String toString() {
	return getClass().getName() + " [" + width + ',' + height + ']';
}
}
