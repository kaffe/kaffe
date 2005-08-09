/*
 * Copyright (c) 1998
 *    Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *	The Kaffe.org's developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
 */

package java.awt;

import java.awt.image.ImageObserver;
import java.awt.image.ImageProducer;
import java.awt.image.MemoryImageSource;
import java.io.File;
import java.net.URL;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

import gnu.classpath.Pointer;
import kaffe.util.VectorSnapshot;

public class Image
{
	Pointer nativeData;
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

public Image() {
}

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
		Pointer oldNativeData = nativeData;
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


