/**
 * DirectColorModel -
 *
 * Copyright (c) 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P. Mehlitz
 */

package java.awt.image;


public class DirectColorModel
  extends ColorModel
{
	int rmask;
	int gmask;
	int bmask;
	int amask;

public DirectColorModel( int bits, int rmask, int gmask, int bmask) {
	this( bits, rmask, gmask, bmask, 0);
}

public DirectColorModel( int bits, int rmask, int gmask, int bmask, int amask) {
	super( bits);

	this.rmask = rmask;
	this.gmask = gmask;
	this.bmask = bmask;
	this.amask = amask;
}

final public int getAlpha( int pixel) {
	if (amask == 0) {
		return (255);
	}
	return ((pixel & amask) * 255 / amask);
}

final public int getAlphaMask() {
	return amask;
}

final public int getBlue( int pixel) {
	if (bmask == 0) {
		return (255);
	}
	return ((pixel & bmask) * 255 / bmask);
}

final public int getBlueMask() {
	return bmask;
}

final public int getGreen( int pixel) {
	if (gmask == 0) {
		return (255);
	}
	return ((pixel & gmask) * 255 / gmask);
}

final public int getGreenMask() {
	return gmask;
}

final public int getRGB( int pixel) {
	if ( this != defaultCM ) {
		pixel = super.getRGB(pixel);
	}
	return (pixel);
}

final public int getRed( int pixel) {
	if (rmask == 0) {
		return (255);
	}
	return ((pixel & rmask) * 255 / rmask);
}

final public int getRedMask() {
	return rmask;
}

}
