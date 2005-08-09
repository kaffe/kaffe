package java.awt;

import gnu.classpath.Pointer;
import java.awt.image.ImageObserver;

/**
 * Copyright (c) 1998
 *    Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
 */
class ImageFrameLoader
  extends Thread
{
	Image img;

ImageFrameLoader ( Image img ) {
	super("ImageFrameLoader");
	this.img = img;

	img.flags |= Image.BLOCK_FRAMELOADER;

	setPriority( NORM_PRIORITY - 3);
	setDaemon( true);
	start();
}

public void run () {

	// Note that we get started *after* the first SINGLEFRAMEDONE, i.e. our image observers
	// already got the preceeding dimension notification

	if ( img.producer instanceof ImageNativeProducer ) {
		runINPLoop();
	}
	else {
		throw new Error("Unhandled production: " + img.producer);
	}
}

public void runINPLoop () {
	int w, h, latency, dt;
	long t;
	Pointer ptr = img.nativeData;

	img.flags |= Image.IS_ANIMATION;

	// we already have the whole thing physically read in, so just start to notify round-robin. We also
	// got the first frame propperly reported, i.e. we start with flipping to the next one
	do {
		latency = Toolkit.imgGetLatency( img.nativeData);
		t = System.currentTimeMillis();

	  // wait until we get requested the next time (to prevent being spinned around by a MediaTracker)
		synchronized ( img ) {
			try {
				while ( (img.flags & Image.BLOCK_FRAMELOADER) != 0 ){
					img.wait();
				}
			} catch ( InterruptedException _ ) {}
		}
		
		dt = (int) (System.currentTimeMillis() - t);
		if ( dt < latency ){
				try { Thread.sleep( latency - dt); } catch ( Exception _ ) {}
		}
		if ( (latency == 0) || (dt < 2*latency) ){
			if ( (ptr = Toolkit.imgGetNextFrame( img.nativeData)) == null )
				break;
		}
		else {
			// Most probably we weren't visible for a while, reset because this might be a
			// animation with delta frames (and the first one sets the background)
			ptr = Toolkit.imgSetFrame( img.nativeData, 0);
		}

		img.nativeData = ptr;
/*
		w = Toolkit.imgGetWidth( img.nativeData);
		h = Toolkit.imgGetHeight( img.nativeData);
		if ( (img.width != w) || (img.height != h) ){
			img.width = w;
			img.height = h;
			img.stateChange( ImageObserver.WIDTH|ImageObserver.HEIGHT, 0, 0, img.width, img.height);
		}
*/

		img.flags |= Image.BLOCK_FRAMELOADER;
		img.stateChange( ImageObserver.FRAMEBITS, 0, 0, img.width, img.height);

	} while ( img.observers != null );
}
}
