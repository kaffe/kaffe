package java.awt.image;


/**
 * ColorModel - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author P. Mehlitz
 */
abstract public class ColorModel
{
	protected int pixel_bits;
	static ColorModel defaultCM;

public ColorModel ( int bitsPerPixel ) {
	pixel_bits = bitsPerPixel;
}

abstract public int getAlpha ( int pixel );

abstract public int getBlue ( int pixel );

abstract public int getGreen ( int pixel );

public int getPixelSize () {
	return pixel_bits;
}

public int getRGB ( int pixel ) {
	return (getAlpha(pixel) << 24)
		     | (getRed(pixel) << 16) | (getGreen(pixel) << 8) | getBlue(pixel);
}

public static ColorModel getRGBdefault() {
	if (defaultCM == null)
		defaultCM = new DirectColorModel( 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

	return defaultCM;
}

abstract public int getRed ( int pixel );

boolean isJavaModel () {
	return false;
}
}
