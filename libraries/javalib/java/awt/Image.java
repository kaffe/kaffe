package java.awt;

import java.awt.image.ColorModel;
import java.awt.image.DirectColorModel;
import java.awt.image.ImageConsumer;
import java.awt.image.ImageObserver;
import java.awt.image.ImageProducer;
import java.awt.image.IndexColorModel;
import java.awt.image.MemoryImageSource;
import java.io.File;
import java.io.InputStream;
import java.net.URL;
import java.net.URLConnection;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;
import kaffe.awt.JavaColorModel;
import kaffe.io.AccessibleBAOStream;
import kaffe.util.Ptr;
import kaffe.util.VectorSnapshot;

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
	int width = -1;
	int height = -1;
	ImageProducer producer;
	Object observers;
	int flags;
	Hashtable props;
	Image next;
	final public static int SCALE_DEFAULT = 1;
	final public static int SCALE_FAST = 2;
	final public static int SCALE_SMOOTH = 4;
	final public static int SCALE_REPLICATE = 8;
	final public static int SCALE_AREA_AVERAGING = 16;
	final static int MASK = ImageObserver.WIDTH|ImageObserver.HEIGHT|
			ImageObserver.PROPERTIES|ImageObserver.SOMEBITS|
			ImageObserver.FRAMEBITS|ImageObserver.ALLBITS|
			ImageObserver.ERROR|ImageObserver.ABORT;
	final static int PRODUCING = 1 << 8;
	final static int READY = ImageObserver.WIDTH | ImageObserver.HEIGHT | ImageObserver.ALLBITS | PRODUCING;
	final static int SCREEN = 1 << 10;
	final static int BLOCK_FRAMELOADER = 1 << 11;
	final static int IS_ANIMATION = 1 << 12;
	final public static Object UndefinedProperty = ImageLoader.class;
	static Image unknownImage;

Image ( File file ) {
	producer = new ImageNativeProducer( this, file);
}

Image ( Image img, int w, int h ) {
	nativeData = Toolkit.imgCreateScaledImage(img.nativeData, w, h);
	width = w;
	height = h;
	flags = READY;
}

Image ( ImageProducer ip ) {
	producer = ip;

	// there currently is no way to check the size of a MemoryImageSource, 
	// so everything != 0 causes async production
	if ( (Defaults.MemImageSrcThreshold == 0) && (producer instanceof MemoryImageSource) ) {
		flags |= PRODUCING;
		synchronized ( ImageLoader.syncLoader ) {
			ImageLoader.syncLoader.img = this;
			ip.startProduction( ImageLoader.syncLoader);
		}
	}
}

Image ( URL url ) {
	producer = new ImageNativeProducer( url);
}

Image ( byte[] data, int offs, int len) {
	producer = new ImageNativeProducer( this, data, offs, len);
}

Image ( int w, int h ) {
	nativeData = Toolkit.imgCreateScreenImage( w, h );
	width = w;
	height = h;
	flags = READY | SCREEN;
}

synchronized void activateFrameLoader () {
  // activate waiting FrameLoader
	if ( (flags & BLOCK_FRAMELOADER) != 0 ){
		flags &= ~BLOCK_FRAMELOADER;
		notify();
	}
}

// -------------------------------------------------------------------------
void addObserver ( ImageObserver observer ) {
	if ( observer == null ) {
		return;
	}

	if ( observers == null ) {
		observers = observer;
	}	
	else if ( observers instanceof Vector ) {
		Vector v = (Vector) observers;
		if ( !v.contains(observer) )
			v.addElement(observer);
	}
	else {
		if ( observer != observers ) {
			Vector v = new Vector( 3);
			v.addElement( observers);
			v.addElement( observer);
			observers = v;
		}
	}
}

synchronized int checkImage ( int w, int h, ImageObserver obs, boolean load ){
	if ( (flags & (ImageObserver.ALLBITS | ImageObserver.FRAMEBITS)) != 0 ) {
		if ( ((w > 0) || (h > 0)) && (w != width) && (h != height) ){
			scale( w, h);
		}
		
		if ( load && (flags & ImageObserver.FRAMEBITS) != 0 )
			addObserver( obs);
	}
	else if ( load && ((flags & PRODUCING) == 0) ) {
		loadImage( this, w, h, obs);
	}

	return (flags & MASK);
}

protected void finalize () throws Throwable {
	flush();
	super.finalize();
}

public void flush () {
	// This is the bad guy - the spec demands that it reverts us into a "not-yet-loaded"
	// state, "to be recreated or fetched again from its source" when a subsequent
	// rendering takes place. Since "its source" doesn't mean the *original* source
	// (see getSource() - "an object that *produces* the image"), we could stay with the
	// native representation (probably the most efficient storage and prod environment),
	// but apps explicitly using flush most probably use it because the original
	// image source has changed, i.e. they use flush() as a way to in-situ modify
	// otherwise constant images (by means of changed sources). Only heaven knows why
	// they don't replace the image (this is like a Graphics that can be brought back
	// to life after a dispose()). That means we have to keep a ref of byte[] data, and
	// we can't produce on-screen images

	if ( nativeData != null && (flags & ImageObserver.ALLBITS) != 0){
		Toolkit.imgFreeImage( nativeData);
		nativeData = null;
		flags = 0;
		width = -1;
		height = -1;
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
		loadImage( this, -1, -1, observer);
		return (-1);
	}
}

public Object getProperty ( String name, ImageObserver observer ) {
	if ( props == null ) {
		if ( (flags & (ImageObserver.FRAMEBITS | ImageObserver.ALLBITS)) != 0 )
			return UndefinedProperty;
		else if ( (flags & PRODUCING) == 0 )
			loadImage( this, -1, -1, observer);

		return null;
	}
	else {
		Object val = props.get( name);
		if ( val == null )
			return UndefinedProperty;
			
		return val;
	}
}

public Image getScaledInstance (int width, int height, int hints) {

	if ( (flags & (ImageObserver.ERROR|ImageObserver.FRAMEBITS)) == 0 ){
		MediaTracker mt = new MediaTracker( null);
		mt.addImage( this, 0);
		try {
			mt.waitForID( 0);
		}
		catch ( InterruptedException x ){
			return null;
		}
	}

	if ( (flags & (ImageObserver.ERROR|ImageObserver.ABORT)) != 0 ){
		return null;
	}

	if ( (width < 0) && (height < 0) ){
		throw new AWTError( "illegal image scaling request: , w: "
		                        + width + ", h: " + height);
	}

	// a negative value is computed from the other dimension and
	// a constant aspect ratio
	if ( width < 0 ){
		width = (int) (((float)height / this.height) * this.width);
	}
	else if ( height < 0 ){
		height = (int) (((float)width / this.width) * this.height);
	}

	return (new Image(this, width, height));
}

public ImageProducer getSource () {
	if ( producer == null )
		producer = new ImageNativeProducer( this);

	return producer;
}

static Image getUnknownImage (){
	if ( unknownImage == null ){
		int d = Toolkit.screenSize.height / 64;

		unknownImage = new Image( d, d);

		Graphics g = unknownImage.getGraphics();
		if ( g != null ){
			g.setColor( Color.white);
			g.fillRect( 0, 0, d, d);
			g.setColor( Color.red);
			g.draw3DRect( 0, 0, d, d, false);
			
			g.dispose();
		}
	}
	
	return unknownImage;
}

public synchronized int getWidth ( ImageObserver observer ) {
	if ( (flags & ImageObserver.WIDTH) != 0 ) {
		return (width);
	}
	else {
		loadImage( this, -1, -1, observer);
		return (-1);
	}
}

static boolean loadImage ( Image img, int w, int h, ImageObserver obs ) {

	// a last safeguard against careless apps which don't handle
	// failed image loads
	if ( img != null ){	
		synchronized ( img ) {
			// it's (partly?) loaded already
			if ( (img.flags & (ImageObserver.FRAMEBITS|ImageObserver.ALLBITS)) != 0 ){
				if ( ((w > 0) || (h > 0)) && (w != img.width) && (h != img.height) ) {
					// but do we have to scale it? 
					img.scale( w, h);
				}
				if ( (img.flags & ImageObserver.FRAMEBITS) != 0 ) {
					img.addObserver( obs);
				}
				return (true);
			}
			// we ultimately failed
			else if ( (img.flags & ImageObserver.ABORT) != 0 ) {
				return (false);
			}
			// there's work ahead, kick it off
			else if ( (img.flags & PRODUCING) == 0 ) {
				img.flags |= PRODUCING;
				if ( obs != null ) {
					img.addObserver( obs);
				}
				/*
				 * We now load the image data synchronously - it's not clear
				 * that's what you should actually do but it fixes a problem
				 * with the Citrix ICA client. (Tim 1/18/99)
				 */
				ImageLoader.loadSync( img);
				return (true);
			}
		}
	}
	
	return (false);
}

void removeObserver ( ImageObserver observer ) {
	if ( (observers == null) || (observer == null) )
		return;

	if ( observers == observer ){
		observers = null;
	}
	else if ( observers instanceof Vector ) {
		Vector v = (Vector)observers;
		v.removeElement( observer);
		
		if ( v.size() == 1 )
			observers = v.firstElement();
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
	ImageObserver obs;

	this.flags |= flags;

	if (observers == null) {
		return;
	}

	// We get false if they're no longer interested in updates. This is *NOT* what is said in the
	// Addison-Wesley documentation, but is what is says in the JDK javadoc documentation.	
	if ( observers instanceof ImageObserver ){
		obs = (ImageObserver) observers;
		if ( !obs.imageUpdate( this, flags, x, y, w, h)) {
			observers = null;
		}
	}
	else if ( observers instanceof Vector ){
		// we can't use a (index-based) standard Vector enumerator because we have
		// to notify *all* elements regardless of any removals
		for ( Enumeration e=VectorSnapshot.getCached( (Vector)observers); e.hasMoreElements(); ) {
			obs = (ImageObserver) e.nextElement();
			if ( !obs.imageUpdate( this, flags, x, y, w, h))
				removeObserver( obs);
		}
	}
}

public String toString() {
	String s = getClass().getName() + " [" + width + ',' + height + 
	              ((nativeData != null) ? ", native" : ", non-native") +
                ", flags: "  + Integer.toHexString( flags);

	if ( (flags & PRODUCING) != 0 )
		s += " producing";
	if ( (flags & ImageObserver.ALLBITS) != 0 )
		s += " allbits";
	else if ( (flags & ImageObserver.FRAMEBITS) != 0 )
		s += " framebits";
	else if ( (flags & ImageObserver.ERROR) != 0 )
		s += " error";

	s += "]";

	return s;		
}
}

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
	Ptr ptr = img.nativeData;

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
	}
	else if ( status == SINGLEFRAMEDONE ) {
		s = ImageObserver.FRAMEBITS;

		// This is a (indefinite) movie production - move it out of the way (in its own thread)
		// so that we can go on with useful work. Note that if our producer was a ImageNativeProducer,
		// the whole external image has been read in already (no IO required, anymore)
		new ImageFrameLoader( img);
	}
	else {
		if ( (status & IMAGEERROR) != 0 )       s |= ImageObserver.ERROR;
		if ( (status & IMAGEABORTED) != 0 )     s |= ImageObserver.ABORT;
	}

	img.stateChange( s, 0, 0, img.width, img.height);

	// this has to be called *after* a optional ImageFrameLoader went into action, since
	// the producer might decide to stop if it doesn't have another consumer
	img.producer.removeConsumer( this);
	
	img = null; 	// we are done with it, prevent memory leaks
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

/**
 * This shouldn't be a inner class since you can easily grab the source of an
 * image and use it outside of this image (e.g. to create other images - whatever
 * it might be good for)
 */
class ImageNativeProducer
  implements ImageProducer
{
	Object consumer;
	Object src;
	int off;
	int len;

ImageNativeProducer ( Image img ) {
	src = img;
}

ImageNativeProducer ( Image img, File file ) {
	src = file;

	// check if we can produce immediately
	if ( file.exists() ) {
		if  ( file.length() < Defaults.FileImageThreshold ){
			img.flags |= Image.PRODUCING;
			img.producer = this;
			ImageLoader.loadSync(img);
		}
	}
	else {
		img.flags = ImageObserver.ERROR | ImageObserver.ABORT;
	}
}

ImageNativeProducer ( Image img, byte[] data, int off, int len ) {
	src = data;
	this.off = off;
	this.len = len;
	
	if ( len < Defaults.DataImageThreshold ) {
	  img.flags |= Image.PRODUCING;
		synchronized ( ImageLoader.syncLoader ) {
			ImageLoader.syncLoader.img = img;
			img.producer = this;
			startProduction( ImageLoader.syncLoader);
		}
	}
}

ImageNativeProducer ( URL url ) {
	src = url;
}

public void addConsumer ( ImageConsumer ic ) {
	if ( consumer == null ){
		consumer = ic;
	}
	else if ( this.consumer instanceof Vector ) {
		((Vector)consumer).addElement( ic);
	}
	else {
		Vector v = new Vector(3);
		v.addElement( consumer);
		v.addElement( ic);
		consumer = v;
	}
}

void imageComplete ( int flags ){
	if ( consumer instanceof ImageConsumer ){
		((ImageConsumer)consumer).imageComplete( flags);
	}
	else if ( consumer instanceof Vector) {
		Vector v = (Vector) consumer;
		int i, n = v.size();
		for ( i=0; i<n; i++ ){
			((ImageConsumer)v.elementAt( i)).imageComplete( flags);
		}
	}
}

public boolean isConsumer ( ImageConsumer ic ) {
	if ( consumer instanceof ImageConsumer )
		return (consumer == ic);
	else if ( consumer instanceof Vector )
		return ((Vector)consumer).contains( ic);
	else
		return false;
}

void produceFrom ( File file ) {
	Ptr ptr;

	if ( file.exists() &&
	     (ptr = Toolkit.imgCreateFromFile( file.getAbsolutePath())) != null ){
		produceFrom( ptr);
	}
	else {
		imageComplete( ImageConsumer.IMAGEERROR | ImageConsumer.IMAGEABORTED);
	}
}

void produceFrom ( Ptr ptr ) {
	if ( consumer instanceof ImageLoader ) {
		ImageLoader loader = (ImageLoader)consumer;
		Image img = loader.img;
	
		img.nativeData = ptr;
		img.width = Toolkit.imgGetWidth( ptr);
		img.height = Toolkit.imgGetHeight( ptr);

		loader.setDimensions( img.width, img.height);
		loader.imageComplete( Toolkit.imgIsMultiFrame( ptr) ?
		                           ImageConsumer.SINGLEFRAMEDONE : ImageConsumer.STATICIMAGEDONE);
	}
	else {
		Toolkit.imgProduceImage( this, ptr);
		Toolkit.imgFreeImage( ptr);
	}
}

void produceFrom ( URL url ) {
	// since this is most likely used in a browser context (no file
	// system), the only thing we can do (in the absence of a suspendable
	// native image production) is to temporarily store the data in
	// memory. Note that this is done via kaffe.io.AccessibleBAOStream,
	// to prevent the inacceptable memory consumption duplication of
	// "toByteArray()".
	// Ideally, we would have a suspendable image production (that can
	// deal with reading and processing "incomplete" data), but that
	// simply isn't supported by many native image conversion libraries.
	// Some could be done in Java (at the expense of a significant speed
	// degradation - this is the classical native functionality), but
	// things like Jpeg ?

	try {
		URLConnection conn = url.openConnection();
		if (conn != null) {
			int sz = conn.getContentLength();
			if (sz <= 0) {  // it's unknown, let's assume some size
				sz = 4096;
			}
			AccessibleBAOStream out = new AccessibleBAOStream(sz);
			InputStream in = conn.getInputStream();
			if ( in != null ) {
				out.readFrom(in);
				in.close();
				Ptr ptr = Toolkit.imgCreateFromData(out.getBuffer(), 0, out.size());
				if ( ptr != null ){
					produceFrom( ptr);
					return;
				}
			}
		}
	}
	catch ( Exception x ) {}

	imageComplete( ImageConsumer.IMAGEERROR|ImageConsumer.IMAGEABORTED);
}

void produceFrom ( byte[] data, int off, int len ) {
	Ptr ptr;

	if ( (ptr = Toolkit.imgCreateFromData( data, off, len)) != null )
		produceFrom( ptr);
	else
		imageComplete( ImageConsumer.IMAGEERROR | ImageConsumer.IMAGEABORTED);
}

public void removeConsumer ( ImageConsumer ic ) {
	if ( consumer == ic ){
		consumer = null;
	}
	else if ( consumer instanceof Vector ) {
		Vector v = (Vector) consumer;		
		v.removeElement( ic);
		if ( v.size() == 1 )
			consumer = v.elementAt( 0);
	}
}

public void requestTopDownLeftRightResend ( ImageConsumer consumer ) {
	// ignored
}

void setColorModel ( ColorModel model ){
	if ( consumer instanceof ImageConsumer ){
		((ImageConsumer)consumer).setColorModel( model);
	}
	else if ( consumer instanceof Vector) {
		Vector v = (Vector)consumer;
		int i, n = v.size();
		for ( i=0; i<n; i++ ){
			((ImageConsumer)v.elementAt( i)).setColorModel( model);
		}
	}
}

void setDimensions ( int width, int height ){
	if ( consumer instanceof ImageConsumer ){
		((ImageConsumer)consumer).setDimensions( width, height);
	}
	else if ( consumer instanceof Vector) {
		Vector v = (Vector)consumer;
		int i, n = v.size();
		for ( i=0; i<n; i++ ){
			((ImageConsumer)v.elementAt( i)).setDimensions( width, height);
		}
	}
}

void setHints ( int hints ){
	if ( consumer instanceof ImageConsumer ){
		((ImageConsumer)consumer).setHints( hints);
	}
	else if ( consumer instanceof Vector) {
		Vector v = (Vector)consumer;
		int i, n = v.size();
		for ( i=0; i<n; i++ ){
			((ImageConsumer)v.elementAt( i)).setHints( hints);
		}
	}
}

void setPixels ( int x, int y, int w, int h,
                       ColorModel model, int pixels[], int offset, int scansize ) {
	if ( consumer instanceof ImageConsumer ){
		((ImageConsumer)consumer).setPixels( x, y, w, h, model, pixels, offset, scansize);
	}
	else if ( consumer instanceof Vector) {
		Vector v = (Vector)consumer;
		int i, n = v.size();
		for ( i=0; i<n; i++ ){
			((ImageConsumer)v.elementAt( i)).setPixels( x, y, w, h,
			                                            model, pixels, offset, scansize);
		}
	}
}

public void startProduction ( ImageConsumer ic ) {
	addConsumer( ic);
	if ( src instanceof File ) {
		produceFrom( (File)src);
	}
	else if ( src instanceof URL ) {
		produceFrom( (URL)src);
	}
	else if ( src instanceof byte[] ) {
		produceFrom( (byte[])src, off, len);
	}
	else if ( src instanceof Image ) {
		Toolkit.imgProduceImage( this, ((Image)src).nativeData);
	}
	else {
		System.err.println( "unsupported production source: " + src.getClass());
	}
	removeConsumer( ic);
}
}
