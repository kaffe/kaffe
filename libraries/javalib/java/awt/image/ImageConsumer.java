package java.awt.image;

import java.util.Hashtable;

/**
 * ImageConsumer - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author P. Mehlitz
 */
public interface ImageConsumer
{
	final public static int RANDOMPIXELORDER = 1;
	final public static int TOPDOWNLEFTRIGHT = 2;
	final public static int COMPLETESCANLINES = 4;
	final public static int SINGLEPASS = 8;
	final public static int SINGLEFRAME = 16;
	final public static int IMAGEERROR = 1;
	final public static int SINGLEFRAMEDONE = 2;
	final public static int STATICIMAGEDONE = 3;
	final public static int IMAGEABORTED = 4;

public void imageComplete ( int status );

public void setColorModel ( ColorModel model );

public void setDimensions ( int width, int height );

public void setHints ( int hints );

public void setPixels ( int x, int y, int w, int h,
		             ColorModel model, byte pixels[], int offset, int scansize );

public void setPixels ( int x, int y, int w, int h,
		             ColorModel model, int pixels[], int offset, int scansize );

public void setProperties ( Hashtable properties );
}
