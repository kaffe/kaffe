package java.awt;

import java.awt.ImageNativeConsumer;
import java.awt.image.ColorModel;
import java.awt.image.ImageConsumer;
import java.awt.image.ImageProducer;
import java.util.Enumeration;
import java.util.Vector;
import kaffe.util.Ptr;

abstract public class ImageNativeProducer
  implements ImageProducer
{
	private Vector consumers;

public ImageNativeProducer() {
	this.consumers = new Vector();
}

public void addConsumer ( ImageConsumer consumer ) {
	if ( !consumers.contains( consumer) ) {
		consumers.addElement( consumer);
	}
}

// ---------------------------------------------------------------------
// The following methods are called by the native image system.
void imageComplete ( int status ) {
	for (Enumeration e = consumers.elements(); e.hasMoreElements(); ){
		ImageConsumer c = (ImageConsumer)e.nextElement();
		c.imageComplete( status);
	}

	consumers.removeAllElements();
}

public boolean isConsumer ( ImageConsumer consumer ) {
	return consumers.contains( consumer);
}

public void removeConsumer ( ImageConsumer consumer ) {
	consumers.removeElement( consumer);
}

public void requestTopDownLeftRightResend ( ImageConsumer consumer ) {
	/* Does nothing */
}

void setColorModel ( ColorModel cm ) {
	for (Enumeration e = consumers.elements(); e.hasMoreElements(); ){
		ImageConsumer c = (ImageConsumer) e.nextElement();
		c.setColorModel( cm);
	}
}

void setDimensions ( int w, int h ) {
	for (Enumeration e = consumers.elements(); e.hasMoreElements(); ){
		ImageConsumer c = (ImageConsumer) e.nextElement();
		c.setDimensions( w, h);
	}
}

void setHints ( int hints ) {
	for (Enumeration e = consumers.elements(); e.hasMoreElements(); ){
		ImageConsumer c = (ImageConsumer) e.nextElement();
		c.setHints( hints);
	}
}

void setPixels ( int x, int y, int w, int h, ColorModel cm, int[] pixels, int off, int ssize ) {
	for (Enumeration e = consumers.elements(); e.hasMoreElements(); ){
		ImageConsumer c = (ImageConsumer) e.nextElement();
		c.setPixels( x, y, w, h, cm, pixels, off, ssize);
	}
}

void setRawData(Ptr ptr) {
	for (Enumeration e = consumers.elements(); e.hasMoreElements(); ){
		Object o = e.nextElement();
		if (o instanceof ImageNativeConsumer) {
			((ImageNativeConsumer)o).setRawData(ptr);
		}
		else {
			Toolkit.imgProduceImage(this, ptr);
		}
	}
}

abstract public void startProduction (ImageConsumer consumer);
}
