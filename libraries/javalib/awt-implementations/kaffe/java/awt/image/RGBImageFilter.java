/**
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P. Mehlitz
 */

package java.awt.image;

import kaffe.awt.JavaColorModel;

abstract public class RGBImageFilter
  extends ImageFilter
{
	protected ColorModel origmodel;
	protected ColorModel newmodel;
	protected boolean canFilterIndexColorModel;

public RGBImageFilter() {
}

public IndexColorModel filterIndexColorModel( IndexColorModel icm) {
	int trans = icm.getTransparentPixel();
	int ms = icm.getMapSize();
	int newTrans = trans;
	
	int[] newRgbs = new int[ ms];

	for ( int i=0; i<ms; i++) {
		int value = filterRGB( -1, -1, icm.rgbs[i] );
		if ( (newTrans == trans) && (i != trans) ) {
			byte alpha = (byte)(value >> 24);
			if ( alpha != 0xff )
				newTrans = -1;
		}
		newRgbs[i] = value;
	}

	return new IndexColorModel( icm.getPixelSize(), newRgbs, newTrans);
}

abstract public int filterRGB( int x, int y, int rgb );

public void filterRGBPixels( int x, int y, int w, int h, int[] pels, int off, int scan) {
	int xw = x + w;
	int yh = y + h;
	for ( int py = y; py < yh; py++) {
		for ( int px = x; px < xw; px++) {
			int pIdx = px + py * scan + off;
			pels[pIdx] = filterRGB(px, py, pels[pIdx]);
		}
	}
	
	consumer.setPixels( x, y, w, h, JavaColorModel.getSingleton(), pels, off, scan);
}

public void setColorModel( ColorModel model) {
	ColorModel cm = JavaColorModel.getSingleton();
	
	if ( (model instanceof IndexColorModel) && canFilterIndexColorModel) {
		cm = filterIndexColorModel( (IndexColorModel) model);
		substituteColorModel( model, cm);
	}
	consumer.setColorModel( cm);
}

public void setPixels( int x, int y, int w, int h, ColorModel cm, byte[] pels, int off, int scan) {
	if ( cm == origmodel ) {
		consumer.setPixels( x, y, w, h, newmodel, pels, off, scan);
	}
	else {
		int xw = x + w;
		int yh = y + h;
		int[] fPels = new int[pels.length];
		for ( int py = y; py < yh; py++) {
			for ( int px = x; px < xw; px++) {
				int pIdx = px + py * scan + off;
				fPels[pIdx] = cm.getRGB(pels[pIdx]);
			}
		}
		filterRGBPixels( x, y, w, h, fPels, 0, w);
	}
}

public void setPixels( int x, int y, int w, int h, ColorModel cm, int[] pels, int off, int scan) {
	if ( cm == origmodel ) {
		consumer.setPixels( x, y, w, h, newmodel, pels, off, scan);
	}
	else {
		int xw = x + w;
		int yh = y + h;
		int[] fPels = new int[pels.length];
		for ( int py = y; py < yh; py++) {
			for ( int px = x; px < xw; px++) {
				int pIdx = px + py * scan + off;
				fPels[pIdx] = cm.getRGB(pels[pIdx]);
			}
		}
		filterRGBPixels( x, y, w, h, fPels, 0, w);
	}
}

public void substituteColorModel( ColorModel oldcm, ColorModel newcm) {
	this.origmodel = oldcm;
	this.newmodel  = newcm;
}
}
