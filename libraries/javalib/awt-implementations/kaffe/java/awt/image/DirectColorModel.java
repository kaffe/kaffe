/**
 * DirectColorModel - the ColorModel that corresponds to the X TrueColor visual
 * (native pixel values have RGB bitfields) 
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

import java.awt.Transparency;
import java.awt.color.ColorSpace;

public class DirectColorModel
  extends ColorModel
{
	int rmask;
	int gmask;
	int bmask;
	int amask;
	int rshift;
	int bshift;
	int gshift;
	int ashift;
	int rmax;
	int gmax;
	int bmax;
	int amax;

public DirectColorModel( int bits, int rmask, int gmask, int bmask) {
	this( bits, rmask, gmask, bmask, 0);
}

public DirectColorModel( int bits, int rmask, int gmask, int bmask, int amask) {
	super( bits);

	this.rmask = rmask;
	this.gmask = gmask;
	this.bmask = bmask;
	this.amask = amask;
	
	if ( rmask != 0 ) {
		rshift = getShift( rmask);
		rmax = (rshift > 0) ? (0xffffffff & rmask) >> rshift :
		                      (0xffffffff & rmask) << -rshift;
	}
	if ( gmask != 0 ) {
		gshift = getShift( gmask);
		gmax = (gshift > 0) ? (0xffffffff & gmask) >> gshift :
		                      (0xffffffff & gmask) << -gshift;
	}
	if ( bmask != 0 ) {		
		bshift = getShift( bmask);
		bmax = (bshift > 0) ? (0xffffffff & bmask) >> bshift :
		                      (0xffffffff & bmask) << -bshift;
	}
	if ( amask != 0 ){
		ashift = getShift( amask);
		amax =  (ashift > 0) ? (0xffffffff & amask) >> ashift :
		                       (0xffffffff & amask) << -ashift;
	}
}


/** taken from GNU Classpath */
public DirectColorModel(ColorSpace cspace, int pixelBits,
			int rmask, int gmask, int bmask, int amask,
			boolean isAlphaPremultiplied,
			int transferType) {
	super(pixelBits,
	      ((amask == 0) ? new int[] { rmask, gmask, bmask } : new int [] { rmask, gmask, bmask, amask }),
	      cspace,
	      amask == 0,
	      isAlphaPremultiplied,
	      ((amask == 0) ? Transparency.OPAQUE : Transparency.TRANSLUCENT),
	      transferType);
}

final public int getAlpha( int pixel) {
	if ( amask == 0 )
		return 0xff;

	return ((ashift > 0) ? ((pixel&amask)>>ashift) : ((pixel&amask)<< -ashift)) * 255 / amax;
}

final public int getAlphaMask() {
	return amask;
}

final public int getBlue( int pixel) {
	if ( bmask == 0 )
		return 0;

	return ((bshift > 0) ? ((pixel&bmask)>>bshift) : ((pixel&bmask)<< -bshift)) * 255 / bmax;
}

final public int getBlueMask() {
	return bmask;
}

final public int getGreen( int pixel) {
	if ( gmask == 0 )
		return 0;

	return ((gshift > 0) ? ((pixel&gmask)>>gshift) : ((pixel&gmask)<< -gshift)) * 255 / gmax;
}

final public int getGreenMask() {
	return gmask;
}

final public int getRGB( int pixel) {
	return ((getAlpha(pixel) << 24) |
	        (getRed(pixel)   << 16) |
	        (getGreen(pixel) <<  8) |
	        getBlue(pixel)); 
}

final public int getRed( int pixel) {
	if ( rmask == 0 )
		return 0;
	
	return ((rshift > 0) ? ((pixel&rmask)>>rshift) : ((pixel&rmask)<< -rshift)) * 255 / rmax;
}

final public int getRedMask() {
	return rmask;
}

int getShift ( int mask ) {
	int i, j;
	
	for ( i=0; ((mask & 1) == 0); i++ )
		mask >>>= 1;

	for ( j=0; ((mask & 1) != 0); j++ )
		mask >>>= 1;

	return (i + (j - 8));
}

public String toString() {
	return "DirectColorModel: rmask=" + Integer.toHexString(getRedMask())
		+ " gmask=" + Integer.toHexString(getGreenMask())
		+ " bmask=" + Integer.toHexString(getBlueMask())
		+ " amask=" + Integer.toHexString(getAlphaMask());
}
}
