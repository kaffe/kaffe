package java.awt.image;

import java.awt.Image;

/**
 * ImageObserver - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author P. Mehlitz
 */
public interface ImageObserver
{
	final public static int WIDTH = 1;
	final public static int HEIGHT = 2;
	final public static int PROPERTIES = 4;
	final public static int SOMEBITS = 8;
	final public static int FRAMEBITS = 16;
	final public static int ALLBITS = 32;
	final public static int ERROR = 64;
	final public static int ABORT = 128;

public boolean imageUpdate ( Image img, int infoflags, int x, int y, int width, int height );
}
