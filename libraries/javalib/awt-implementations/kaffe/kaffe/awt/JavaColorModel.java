package kaffe.awt;

import java.awt.image.ColorModel;

/**
 * JavaColorModel - This is a ColorModel for Javas own color 8888 ARGB scheme.
 * It doesn't make sense to do all the bit shuffling if we just return what
 * we get in. Unfortunately, we can't derive that from DirectColorModel
 * (which is actually is) because of all the final methods (spec, again)
 * in there
 *
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P. Mehlitz
 */
public class JavaColorModel
  extends ColorModel
{
	static JavaColorModel singleton;

public JavaColorModel () {
	super( 32);
}

public int getAlpha ( int pixel ){
	return (pixel & 0xff000000);
}

public int getBlue ( int pixel ){
	return (pixel & 0xff);
}

public int getGreen ( int pixel ){
	return (pixel & 0xff00);
}

public int getRGB ( int pixel ) {
	return pixel;
}

public int getRed ( int pixel ){
	return (pixel & 0xff0000);
}

public static JavaColorModel getSingleton() {
	if (singleton == null)
		singleton = new JavaColorModel();

	return singleton;
}
}
