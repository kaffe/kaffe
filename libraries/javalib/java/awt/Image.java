package java.awt;

import java.lang.String;
import java.awt.ImageDataProducer;
import java.awt.ImageFileProducer;
import java.awt.ImageURLProducer;
import java.awt.image.ColorModel;
import java.awt.image.DirectColorModel;
import java.awt.image.ImageObserver;
import java.awt.image.ImageProducer;
import java.awt.image.IndexColorModel;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;
import kaffe.io.AccessibleBAOStream;
import kaffe.util.Ptr;

/**
 * Copyright (c) 1998
 *    Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
 */
public class Image
{
	Ptr nativeData;
	int width = 0;
	int height = 0;
	ImageProducer producer;
	Vector observers;
	int flags;
	Image next;
	final public static int SCALE_DEFAULT = 1;
	final public static int SCALE_FAST = 2;
	final public static int SCALE_SMOOTH = 4;
	final public static int SCALE_REPLICATE = 8;
	final public static int SCALE_AREA_AVERAGING = 16;
	final static int READY = ImageObserver.WIDTH|ImageObserver.HEIGHT|
			 ImageObserver.ALLBITS;
	final static int MASK = ImageObserver.WIDTH|ImageObserver.HEIGHT|
			ImageObserver.PROPERTIES|ImageObserver.SOMEBITS|
			ImageObserver.FRAMEBITS|ImageObserver.ALLBITS|
			ImageObserver.ERROR|ImageObserver.ABORT;
	final static int PRODUCING = 1 << 8;
	final static int SCREEN = 1 << 10;

Image ( File file ) {
	producer = new ImageFileProducer(file);
}

Image ( Image img, int w, int h ) {
	nativeData = Toolkit.imgCreateScaledImage(img.nativeData, w, h);
	width = w;
	height = h;
	flags = READY;
}

Image ( ImageProducer ip ) {
	producer = ip;
}

Image ( URL url ) {
	producer = new ImageURLProducer(url);
}

Image ( byte[] data, int offs, int len) {
	producer = new ImageDataProducer(data, offs, len);
}

Image ( int w, int h ) {
	nativeData = Toolkit.imgCreateScreenImage( w, h );
	width = w;
	height = h;
	flags = READY | SCREEN;
}

// -------------------------------------------------------------------------
void addObserver ( ImageObserver observer ) {
	if ( observer == null ) {
		return;
	}
	if (observers == null) {
		observers = new Vector(1);
		observers.addElement(observer);
	} else if (!observers.contains(observer)) {
		observers.addElement(observer);
	}
}

int checkImage(int width, int height, ImageObserver obs) {
	return (checkImage(width, height, obs, false) & MASK);
}

synchronized int checkImage (int width, int height, ImageObserver obs, boolean load ){
	if ( (flags & ImageObserver.ALLBITS) != 0 ) {
		if (width > 0 && height > 0) {
			scale( width, height);
		}
	}
	else if (load) {
		loadImageAsync(width, height, obs);
	}	
	return (flags);
}

protected void finalize () throws Throwable {
	flush();
	super.finalize();
}

public void flush () {
	if ( nativeData != null && (flags & ImageObserver.ALLBITS) != 0){
		Toolkit.imgFreeImage( nativeData);
		nativeData = null;
		flags &= ~(ImageObserver.ALLBITS|PRODUCING);
	}
}

public Graphics getGraphics () {
	if ((flags & SCREEN) == 0 || nativeData == null) {
		return null;
	}
	else {
		return NativeGraphics.getGraphics( this,
		                   nativeData, NativeGraphics.TGT_TYPE_IMAGE,
		                   0, 0, 0, 0, width, height,
										   Color.black, Color.white, Defaults.WndFont, false);
	}
}

public synchronized int getHeight ( ImageObserver observer ) {
	if ( (flags & ImageObserver.HEIGHT) != 0 ) {
		return height;
	}
	else {
		loadImage(-1, -1, observer);
		return (-1);
	}
}

public Object getProperty ( String name, ImageObserver observer ) {
	return (null);
}

public Image getScaledInstance (int width, int height, int hints) {
	if (nativeData == null || width <= 0 || height <= 0 || (flags & SCREEN) != 0 || (flags & ImageObserver.ALLBITS) == 0) {
		return (null);
	}
	return (new Image(this, width, height));
}

public ImageProducer getSource () {
	return (producer);
}

public synchronized int getWidth ( ImageObserver observer ) {
	if ( (flags & ImageObserver.WIDTH) != 0 ) {
		return (width);
	}
	else {
		loadImage(-1, -1, observer);
		return (-1);
	}
}

synchronized boolean loadImage(int width, int height, ImageObserver obs) {
	for (;;) {
		if ((flags & ImageObserver.ALLBITS) != 0) {
			if (width > 0 && height > 0) {
				scale(width, height);
			}
			return (true);
		}
		else if ((flags & (ImageObserver.ERROR|ImageObserver.ABORT)) != 0) {
			return (false);
		}
		else if ((flags & PRODUCING) == 0) {
			flags |= PRODUCING;
			addObserver(obs);
			producer.startProduction(new ImageNativeConsumer(this));
		}
		else {
			return (false);
		}
	}
}

synchronized boolean loadImageAsync(int width, int height, ImageObserver obs) {
	if ((flags & ImageObserver.ALLBITS) != 0) {
		if (width > 0 && height > 0) {
			scale(width, height);
		}
		return (true);
	}
	else if ((flags & (ImageObserver.ERROR|ImageObserver.ABORT)) != 0) {
		/* Do nothing */
	}
	else if ((flags & PRODUCING) == 0) {
		flags |= PRODUCING;
		addObserver(obs);
		ImageNativeThread.startAsyncProduction(this);
	}
	return (false);
}

void removeObserver ( ImageObserver observer ) {
	if (observers != null) {
		observers.removeElement(observer);
		if (observers.size() == 0) {
			observers = null;
		}
	}
}

private boolean scale (int w, int h) {
	if ((flags & SCREEN) != 0 || (flags & ImageObserver.ALLBITS) == 0 || nativeData == null) {
		return (false);
	}
	if (w != width || h != height) {
		Ptr oldNativeData = nativeData;
		nativeData = Toolkit.imgCreateScaledImage( nativeData, w, h );
		Toolkit.imgFreeImage( oldNativeData);
		width = w;
		height = h;
	}
	return (true);
}

synchronized void stateChange(int flags, int x, int y, int w, int h) {

	// System.out.println("stateChange: " + this + ", f="+flags + ", x="+x + ", y="+y + ", w="+w + ", h="+h);

	this.flags |= flags;

	if (observers == null) {
		return;
	}

	for ( Enumeration e = observers.elements(); e.hasMoreElements(); ) {
		ImageObserver obs = (ImageObserver)e.nextElement();
		/* We get false if they're no longer interested in updates. 
		 * This is *NOT* what is says in the Addison-Wesley
		 * documentation, but is what is says in the JDK javadoc
		 * documentation.
		 */
		if (obs.imageUpdate(this, flags, x, y, w, h) == false) {
			observers.removeElement(obs);
		}
	}
}

public String toString() {
	return getClass().getName() + " [" + width + ',' + height + ", flags: "  + flags + ']';
}
}
