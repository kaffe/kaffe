/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */


package java.awt.image;


public class AreaAveragingScaleFilter
  extends ReplicateScaleFilter
{
public AreaAveragingScaleFilter ( int width, int height) {
	super( width, height);
}

public void setHints( int hints) {

}

public void setPixels( int x, int y, int w, int h, ColorModel cm, byte[] pels, int off, int scan) {

}

public void setPixels( int x, int y, int w, int h, ColorModel cm, int[] pels, int off, int scan) {

}
}
