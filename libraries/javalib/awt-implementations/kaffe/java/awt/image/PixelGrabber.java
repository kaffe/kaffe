package java.awt.image;

import java.awt.Image;
import java.awt.Rectangle;
import java.util.Hashtable;

import kaffe.awt.JavaColorModel;

public class PixelGrabber
  implements ImageConsumer
{
	int x;
	int y;
	int width;
	int height;
	ColorModel model;
	ImageProducer producer;
	int[] pels;
	int off;
	int scan;
	int status;
	boolean active;
	Rectangle updRect = new Rectangle();

public PixelGrabber ( Image img, int x, int y, int w, int h, boolean forceRGB) {
	this.x = x;
	this.y = y;
	this.width = w;
	this.height = h;
	this.producer = img.getSource();
	this.model = forceRGB ? JavaColorModel.getSingleton() : null;
}

public PixelGrabber ( Image img, int x, int y, int w, int h, int[] pels, int off, int scan) {
	this( img.getSource(), x, y, w, h, pels, off, scan);
}

public PixelGrabber ( ImageProducer ip, int x, int y, int w, int h, int[] pels, int off, int scan) {
	this.producer = ip;
	this.x = x;
	this.y = y;
	this.width = w;
	this.height = h;
	this.pels = pels;
	this.off = off;
	this.scan = scan;
	this.model = JavaColorModel.getSingleton();
}

public synchronized void abortGrabbing () {
	imageComplete( IMAGEABORTED);
}

public synchronized ColorModel getColorModel () {
	return model;
}

public synchronized int getHeight () {
	return height;
}

public synchronized Object getPixels () {
	return pels;
}

public synchronized int getStatus () {
	return status;
}

int getUpdateRect( int x, int y, int w, int h, int off, int scan ) {
	if ( y < this.y) {
		int dy = this.y - y;
		off += scan * dy;
		y   += dy;
		h   -= dy;
		if ( h <= 0)
			return -1;
	}
	if ( y + h > this.y + this.height) {
		h = this.y + this.height - y;
		if ( h <= 0)
			return -1;
	}
	if ( x < this.x) {
		int dx = this.x - x;
		off += dx;
		x += dx;
		w -= dx;
		if ( w <= 0)
			return -1;
	}
	if ( x + w > this.x + this.width ){
		w = this.x + this.width - x;
		if ( w <= 0)
			return -1;
	}
		
	updRect.setBounds( x, y, w, h);
	return off;
}

public synchronized int getWidth () {
	return width;
}

public boolean grabPixels () throws InterruptedException {
	if ( ! active)
		startGrabbing();
		
	// has to be a loop because this may be notified from outside	
	while( active)
		wait( 0);
	
	return (status & (ImageObserver.FRAMEBITS | ImageObserver.ALLBITS)) != 0;

}

public synchronized boolean grabPixels ( long ms) throws InterruptedException {
	if ( ms <= 0)
		return grabPixels();
		
	if ( ! active)
		startGrabbing();
		
	long stopTime = System.currentTimeMillis() + ms;
	long remainingTime;
	
	// has to be a loop because this may be notified from outside	
	while( active) {
		if ( (remainingTime = stopTime - System.currentTimeMillis()) <= 0)
			break;
		wait( remainingTime);
	}
	
	return (status & (ImageObserver.FRAMEBITS | ImageObserver.ALLBITS)) != 0;
}

public synchronized void imageComplete ( int status) {
	switch( status) {
		case IMAGEERROR:
			this.status |= ImageObserver.ERROR;
			break;
		case SINGLEFRAMEDONE:
			this.status |= ImageObserver.FRAMEBITS;
			break;
		case STATICIMAGEDONE:
			this.status |= ImageObserver.ALLBITS;
			break;
		case IMAGEABORTED:
			this.status |= ImageObserver.ABORT;
	}
	active = false;
	producer.removeConsumer( this);
	notifyAll();
}

public void setColorModel ( ColorModel model) {
}

public void setDimensions ( int width, int height) {
	if ( this.width == -1 )
		this.width  = width - this.x;
	if ( this.height == -1 )
		this.height = height - this.y;	
	if ( pels == null) {
		pels = new int[ this.width * this.height ];
		this.scan = this.width;
		this.off = 0;
	}
	status |= ImageObserver.WIDTH | ImageObserver.HEIGHT;
}

public void setHints ( int hints) {
}

public void setPixels ( int x, int y, int w, int h, ColorModel cm, byte[] pels, int off, int scan ) {
}

public void setPixels ( int x, int y, int w, int h, ColorModel cm, int[] pels, int off, int scan ) {
	off = getUpdateRect( x, y, w, h, off, scan);
	if ( off < 0 )
		return;
		
	int idx = this.off + (updRect.y - this.y) * this.scan + (updRect.x - this.x);
	for ( ;updRect.height > 0; updRect.height--){
		System.arraycopy( pels, off, this.pels, idx, updRect.width);
		off += scan;
		idx += this.scan;
	}
	
	status |= ImageObserver.SOMEBITS;
}

public void setProperties ( Hashtable props) {
	status |= ImageObserver.PROPERTIES;
}

public synchronized void startGrabbing () {
	if ( ! active) {
		active = true;
		status = 0;
		producer.startProduction( this);
	}
}

public int status() {
        return getStatus();
}
}
