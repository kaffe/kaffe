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

package java.awt.image;

import java.util.Hashtable;

public interface ImageConsumer {

int RANDOMPIXELORDER = 1;
int TOPDOWNLEFTRIGHT = 2;
int COMPLETESCANLINES = 4;
int SINGLEPASS = 8;
int SINGLEFRAME = 16;
int IMAGEERROR = 1;
int SINGLEFRAMEDONE = 2;
int STATICIMAGEDONE = 3;
int IMAGEABORTED = 4;

void imageComplete ( int status );
void setColorModel ( ColorModel model );
void setDimensions ( int width, int height );
void setHints ( int hints );
void setPixels ( int x, int y, int w, int h, ColorModel model, byte pixels[], int offset, int scansize );
void setPixels ( int x, int y, int w, int h, ColorModel model, int pixels[], int offset, int scansize );
void setProperties ( Hashtable properties );

}
