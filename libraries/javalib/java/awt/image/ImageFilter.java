package java.awt.image;

import java.util.Hashtable;

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
	consumer.imageComplete( status);
}

public void resendTopDownLeftRight( ImageProducer ip) {
	ip.requestTopDownLeftRightResend( this);
}

public void setColorModel( ColorModel model) {
	consumer.setColorModel( model);
}

public void setDimensions( int width, int height) {
	consumer.setDimensions( width, height);
}

public void setHints( int hints) {
	consumer.setHints( hints);
}

public void setPixels( int x, int y, int w, int h, ColorModel model, byte[] pels, int off, int scan) {
	consumer.setPixels( x, y, w, h, model, pels, off, scan);
}

public void setPixels( int x, int y, int w, int h, ColorModel model, int[] pels, int off, int scan) {
	consumer.setPixels( x, y, w, h, model, pels, off, scan);
}

public void setProperties( Hashtable props) {
	consumer.setProperties( props);
}
}
