package java.awt;

import java.awt.image.ColorModel;
import java.awt.image.DirectColorModel;
import java.awt.image.ImageConsumer;
import java.awt.image.ImageObserver;
import java.awt.image.IndexColorModel;
import java.util.Hashtable;

import kaffe.awt.JavaColorModel;

/**
 * Copyright (c) 1998
 *    Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
 */
class ImageLoader
  implements ImageConsumer, Runnable
{
	Image queueHead;
	Image queueTail;
	Image img;
	static ImageLoader asyncLoader;
	static ImageLoader syncLoader = new ImageLoader();

public synchronized void imageComplete ( int status ) {
	int s = 0;

	if ( status == STATICIMAGEDONE ){
		s = ImageObserver.ALLBITS;
		// give native layer a chance to do alpha channel reduction
		if ( !(img.producer instanceof ImageNativeProducer) )
			Toolkit.imgComplete( img.nativeData, status);
		img.stateChange( s, 0, 0, img.width, img.height);
		img.producer.removeConsumer( this);	
		img = null; 	// we are done with it, prevent memory leaks
	}
	else if ( status == SINGLEFRAMEDONE ) {
		s = ImageObserver.FRAMEBITS;

		// This is a (indefinite) movie production - move it out of the way (in its own thread)
		// so that we can go on with useful work. Note that if our producer was a ImageNativeProducer,
		// the whole external image has been read in already (no IO required, anymore)

		if ( img.producer instanceof ImageNativeProducer ) {
		  new ImageFrameLoader( img);
		  img.stateChange( s, 0, 0, img.width, img.height);
		  img.producer.removeConsumer( this);	
		  img = null; 	// we are done with it, prevent memory leaks
		}
		else {
		  img.stateChange( s, 0, 0, img.width, img.height);
		}
	}
	else {
		if ( (status & IMAGEERROR) != 0 )       s |= ImageObserver.ERROR;
		if ( (status & IMAGEABORTED) != 0 )     s |= ImageObserver.ABORT;
		img.stateChange( s, 0, 0, img.width, img.height);
		img.producer.removeConsumer( this);	
		img = null; 	// we are done with it, prevent memory leaks
	}


	if ( this == asyncLoader )
		notify();     // in case we had an async producer
}

static synchronized void load ( Image img ) {
	if ( asyncLoader == null ){
		asyncLoader = new ImageLoader();
		asyncLoader.queueHead = asyncLoader.queueTail = img;

		Thread t = new Thread( asyncLoader, "asyncLoader");
		t.setPriority( Thread.NORM_PRIORITY - 1);
		t.start();
	}
	else {
		if ( asyncLoader.queueTail == null ) {
			asyncLoader.queueHead = asyncLoader.queueTail = img;
		}
		else {
			asyncLoader.queueTail.next = img;
			asyncLoader.queueTail = img;
		}
		
		ImageLoader.class.notify();
	}
}

static void loadSync( Image img ) {
	synchronized ( syncLoader ) {
		syncLoader.img = img;
		img.producer.startProduction(syncLoader);
	}
}

public void run () {
	for (;;) {
		synchronized ( ImageLoader.class ) {
			if ( queueHead != null ) {
				img = queueHead;
				queueHead = img.next;
				img.next = null;
				if ( img == queueTail )
					queueTail = null;
			}
			else {
				try {
					ImageLoader.class.wait( 20000);
					if ( queueHead == null ) {
						// Ok, we waited for too long, lets do suicide
						asyncLoader = null;
						return;
					}
					else {
						continue;
					}
				}
				catch ( InterruptedException xx ) { xx.printStackTrace(); }
			}
		}
		
		try {
			// this is hopefully sync, but who knows what kinds of producers are out there
			img.producer.startProduction( this);

			if ( img != null ) {
				synchronized ( this ){
					wait();
				}
			}
		}
		catch ( Throwable x ) {
			x.printStackTrace();
			imageComplete( IMAGEERROR | IMAGEABORTED);
		}
	}
}

public void setColorModel ( ColorModel model ) {
	// No way to pass this to ImageObservers. Since we also get it in setPixels, we
	// just ignore it
}

public void setDimensions ( int width, int height ) {
	img.width = width;
	img.height = height;

	// If we were notified by a ImageNativeProducer, the nativeData field is already
	// set. In case this is just an arbitrary producer, create it so that we have a
	// target for subsequent setPixel() calls
	if ( img.nativeData == null ){
		img.nativeData = Toolkit.imgCreateImage( width, height);
	}

	img.stateChange( ImageObserver.WIDTH|ImageObserver.HEIGHT, 0, 0, width, height);
}

public void setHints ( int hints ) {
	// we don't honor them
}

public void setPixels ( int x, int y, int w, int h,
                        ColorModel model, byte pixels[], int offset, int scansize ) {
	if ( img.nativeData == null ) {
		// producer trouble, we haven't got dimensions yet
		return;
	}

	if ( model instanceof IndexColorModel ) {
		IndexColorModel icm = (IndexColorModel) model;
		Toolkit.imgSetIdxPels( img.nativeData, x, y, w, h, icm.rgbs,
		                       pixels, icm.trans, offset, scansize);
		img.stateChange( ImageObserver.SOMEBITS, x, y, w, h);
	}
	else {
		System.err.println("Unhandled colorModel: " + model.getClass());
	}
}

public void setPixels ( int x, int y, int w, int h,
                        ColorModel model, int pixels[], int offset, int scansize ) {
	if ( img.nativeData == null ) {
		// producer trouble, we haven't got dimensions yet
		return;
	}

	if ( model instanceof JavaColorModel ) {
		// Ok, nothing to convert here
	}
	else if ( model instanceof DirectColorModel ) {
		// in case our pels aren't default RGB yet, convert them using the ColorModel
		int xw = x + w;
		int yh = y + h;
		int i, j, idx;
		int i0 = y * scansize + offset;
		for ( j = y; j < yh; j++, i0 += scansize ) {
			for ( i = x, idx = i+i0; i < xw; i++, idx++) {
				// are we allowed to change the array in-situ?
				pixels[idx] = model.getRGB( pixels[idx]);
			}
		}
	}
	else {
		System.out.println("Unhandled colorModel: " + model.getClass());
	}

	Toolkit.imgSetRGBPels( img.nativeData, x, y, w, h, pixels, offset, scansize);
	img.stateChange( ImageObserver.SOMEBITS, x, y, w, h);
}

public void setProperties ( Hashtable properties ) {
	img.props = properties;
	img.stateChange( img.flags | ImageObserver.PROPERTIES, 0, 0, img.width, img.height);
}
}


