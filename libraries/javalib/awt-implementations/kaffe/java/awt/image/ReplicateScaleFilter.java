package java.awt.image;

import java.util.Hashtable;

public class ReplicateScaleFilter
  extends ImageFilter
{
	protected int srcWidth;
	protected int srcHeight;
	protected int destWidth;
	protected int destHeight;
	protected int[] srcrows;
	protected int[] srccols;
	protected Object outpixbuf;

public ReplicateScaleFilter ( int width, int height) {
	destWidth = width;
	destHeight = height;
}

public void setDimensions ( int width, int height) {
	double scaleX = (double)srcWidth / width;
	double scaleY = (double)srcHeight / height;

	srcWidth = width;
	srcHeight = height;
	
	if ( scaleX > 0) {
		destWidth = (int)(scaleX * destWidth);
	}
	else {
		destWidth = srcWidth;
	}

	if ( scaleY > 0) {
		destHeight = (int)(scaleY * destHeight);
	}
	else {
		destHeight = srcHeight;
	}
	
	consumer.setDimensions( destWidth, destHeight);
}

public void setPixels ( int x, int y, int w, int h, ColorModel cm, byte[] pels, int off, int scan) {
}

public void setPixels ( int x, int y, int w, int h, ColorModel cm, int[] pels, int off, int scan) {
}

public void setProperties ( Hashtable props) {
	//add applied scale later
	super.setProperties( props);
}
}
