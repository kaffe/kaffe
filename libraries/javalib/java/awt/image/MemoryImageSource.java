/**
 * MemoryImageSource - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author J. Mehlitz
 */

package java.awt.image;

import java.util.Hashtable;
import java.util.Vector;

import kaffe.awt.JavaColorModel;

public class MemoryImageSource
  implements ImageProducer
{
	Vector consumers = new Vector(3);
	Hashtable props;
	int width;
	int height;
	int scan;
	int offset;
	int[] ipels;
	byte[] bpels;
	ColorModel model;
	boolean animated;
	boolean fullbuffers;

public MemoryImageSource( int w, int h, ColorModel cm, byte[] pix, int off, int scan) {
	this( w, h, cm, pix, off, scan, null);
}

public MemoryImageSource( int w, int h, ColorModel cm, byte[] pix, int off, int scan, Hashtable props) {
	this.width  = w;
	this.height = h;
	this.bpels  = pix;
	this.model  = cm;
	this.offset = off;
	this.scan   = scan;
	this.props  = props;
}

public MemoryImageSource( int w, int h, ColorModel cm, int[] pix, int off, int scan) {
	this( w, h, cm, pix, off, scan, null);
}

public MemoryImageSource( int w, int h, ColorModel cm, int[] pix, int off, int scan, Hashtable props) {
	this.width  = w;
	this.height = h;
	this.ipels  = pix;
	this.model  = cm;
	this.offset = off;
	this.scan   = scan;
	this.props  = props;
}

public MemoryImageSource( int w, int h, int[] pix, int off, int scan) {
	this( w, h, JavaColorModel.getSingleton(), pix, off, scan, null);
}

public MemoryImageSource( int w, int h, int[] pix, int off, int scan, Hashtable props) {
	this( w, h, JavaColorModel.getSingleton(), pix, off, scan, props);
}

public void addConsumer( ImageConsumer ic) {
	if ( ! consumers.contains( ic) ) {
		consumers.addElement( ic);
	}
}

private void initializeConsumer( ImageConsumer ic) {
	ic.setDimensions( width, height);	
	ic.setColorModel( model);
	ic.setProperties( props);
	ic.setHints( ic.TOPDOWNLEFTRIGHT | ic.SINGLEPASS | ic.SINGLEFRAME | ic.COMPLETESCANLINES);
}

public boolean isConsumer( ImageConsumer ic) {
	return consumers.contains( ic);
}

public void newPixels() {
	newPixels( 0, 0, width, height, true);
}

public void newPixels( byte[] newpix, ColorModel newmodel, int offset, int scansize) {
	this.ipels  = null;
	this.bpels  = newpix;
	this.model  = newmodel;
	this.offset = offset;
	this.scan   = scansize;
	newPixels();
}

public void newPixels( int x, int y, int w, int h) {
	newPixels( x, y, w, h, true);
}

public void newPixels( int x, int y, int w, int h, boolean framenotify) {
	if ( ! animated ) {
		return;
	}
	if ( fullbuffers ) {
		x = 0;
		y = 0;
		w = width;
		h = height;
	}
	int sz = consumers.size();
	for ( int i = 0; i < sz; i++) {
		ImageConsumer ic = (ImageConsumer) consumers.elementAt( i);
		transferPels( ic, x, y, w, h);
		if ( framenotify && isConsumer( ic) ) {
			ic.setHints( ic.SINGLEFRAMEDONE);
		}
	}
}

public void newPixels( int[] newpix, ColorModel newmodel, int offset, int scansize) {
	this.bpels  = null;
	this.ipels  = newpix;
	this.model  = newmodel;
	this.offset = offset;
	this.scan   = scansize;
	newPixels();
}

public void removeConsumer( ImageConsumer ic) {
	consumers.removeElement( ic);
}

public void requestTopDownLeftRightResend( ImageConsumer ic) {
}

public void setAnimated( boolean animated) {
	this.animated = animated;
	if ( ! animated ) {
		for ( int i=0; i<consumers.size(); i++) {
			ImageConsumer ic = (ImageConsumer) consumers.elementAt( i);
			ic.imageComplete( ic.STATICIMAGEDONE);
		}
		consumers.removeAllElements();
	}
}

public void setFullBufferUpdates( boolean fullbuffers) {
	this.fullbuffers = fullbuffers;
}

public void startProduction( ImageConsumer ic) {
	addConsumer(ic);
	initializeConsumer(ic);
	transferPels(ic, 0, 0, width, height);
	terminateConsumer(ic);
}

private void terminateConsumer(ImageConsumer ic) {
	ic.imageComplete(ic.STATICIMAGEDONE);
}

private void transferPels( ImageConsumer ic, int x, int y, int w, int h) {
	if ( bpels != null ) {
		ic.setPixels( x, y, w, h, model, bpels, offset, scan );
	}
	else if ( ipels != null ) {
		ic.setPixels( x, y, w, h, model, ipels, offset, scan );
	}
}
}
