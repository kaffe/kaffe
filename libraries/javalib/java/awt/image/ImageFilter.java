package java.awt.image;

import java.util.Hashtable;

/**
 * Copyright (c) 2000
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 */
public class ImageFilter
  implements ImageConsumer, Cloneable
{
	protected ImageConsumer consumer;

public ImageFilter() {
}

public Object clone () {
	try { return super.clone(); }
	catch ( CloneNotSupportedException e) { return null;}
}

public ImageFilter getFilterInstance( ImageConsumer ic) {
	ImageFilter filter = (ImageFilter) clone();
	filter.consumer = ic;
	return filter;
}

public void imageComplete( int status) {
	if ( consumer != null )
		consumer.imageComplete( status);
}

public void resendTopDownLeftRight( ImageProducer ip) {
	ip.requestTopDownLeftRightResend( this);
}

public void setColorModel( ColorModel model) {
	if ( consumer != null )
		consumer.setColorModel( model);
}

public void setDimensions( int width, int height) {
	if ( consumer != null )
		consumer.setDimensions( width, height);
}

public void setHints( int hints) {
	if ( consumer != null )
		consumer.setHints( hints);
}

public void setPixels( int x, int y, int w, int h, ColorModel model, byte[] pels, int off, int scan) {
	if ( consumer != null )
		consumer.setPixels( x, y, w, h, model, pels, off, scan);
}

public void setPixels( int x, int y, int w, int h, ColorModel model, int[] pels, int off, int scan) {
	if ( consumer != null )
		consumer.setPixels( x, y, w, h, model, pels, off, scan);
}

public void setProperties( Hashtable props) {
	if ( consumer != null )
		consumer.setProperties( props);
}
}
