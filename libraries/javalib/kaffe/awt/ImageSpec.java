package kaffe.awt;

import java.awt.Image;
import java.awt.Toolkit;
import java.awt.image.ImageObserver;
import java.io.File;
import java.net.URL;

/**
 * Copyright (c) 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author J.Mehlitz
 */
public class ImageSpec
{
	Image img;
	boolean plain;
	String fileName;
	Class loaderCls;

public ImageSpec ( String pathName, Class loader, ImageObserver obs, boolean preload, boolean plain) {
	this.loaderCls = loader;
	this.fileName = pathName;
	this.plain = plain;

	if ( preload )
		loadImage( obs);
}

public Image getImage() {
	if ( img == null )
		loadImage( null);
	return img;
}

public boolean isPlain() {
	return plain;
}

boolean loadFromClass( ImageObserver obs) {
	try {
		Class cc = Class.forName( fileName);
		ImageClass ic = (ImageClass)cc.newInstance();
		img = Toolkit.getDefaultToolkit().createImage( ic.source);
		
		if ( obs != null )
			img.getHeight( obs);

		return true;
	}
	catch ( Exception x) {
		return false;
	}
}

boolean loadFromFile() {
	File f = new File( fileName);
	if ( f.exists() ) {
		img = Toolkit.getDefaultToolkit().getImage( fileName);
		return true;
	}

	return false;
}

boolean loadFromURL( ImageObserver obs) {
	try {
		URL url = loaderCls.getResource( fileName);
		img = Toolkit.getDefaultToolkit().getImage( url);
		
		if (obs != null)
			img.getWidth( obs);

		return true;
	}
	catch ( Exception x) {
		return false;
	}
}

boolean loadImage( ImageObserver obs) {

	if ( img != null )
		return true;

	if ( loadFromFile() ) {
		return true;
	}
		
	if ( loadFromClass( obs) ) {
		return true;
	}
		
	if ( loadFromURL( obs) ) {
		return true;
	}
		
	return false;
}
}
