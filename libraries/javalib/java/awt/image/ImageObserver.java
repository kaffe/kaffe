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

package java.awt.image;

import java.awt.Image;

public interface ImageObserver {

int WIDTH = 1;
int HEIGHT = 2;
int PROPERTIES = 4;
int SOMEBITS = 8;
int FRAMEBITS = 16;
int ALLBITS = 32;
int ERROR = 64;
int ABORT = 128;

boolean imageUpdate ( Image img, int infoflags, int x, int y, int width, int height );

}
