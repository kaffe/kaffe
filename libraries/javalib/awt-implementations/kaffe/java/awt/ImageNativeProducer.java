package java.awt;

import java.awt.image.ColorModel;
import java.awt.image.ImageConsumer;
import java.awt.image.ImageObserver;
import java.awt.image.ImageProducer;
import java.io.File;
import java.io.InputStream;
import java.net.URL;
import java.net.URLConnection;
import java.util.Vector;

import kaffe.io.AccessibleBAOStream;
import gnu.classpath.Pointer;

/**
 * Copyright (c) 1998
 *    Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
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
	Pointer ptr;

	if ( file.exists() &&
	     (ptr = Toolkit.imgCreateFromFile( file.getAbsolutePath())) != null ){
		produceFrom( ptr);
	}
	else {
		imageComplete( ImageConsumer.IMAGEERROR | ImageConsumer.IMAGEABORTED);
	}
}

void produceFrom ( Pointer ptr ) {
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
				Pointer ptr = Toolkit.imgCreateFromData(out.getBuffer(), 0, out.size());
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
	Pointer ptr;

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
