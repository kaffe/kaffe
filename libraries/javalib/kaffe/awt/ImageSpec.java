package kaffe.awt;

/**
 * Copyright (c) 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author J.Mehlitz
 */

import java.awt.Image;
import java.awt.Toolkit;
import java.awt.image.ImageObserver;
import java.io.File;
import java.net.URL;
import java.net.URLConnection;

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

boolean loadImage( ImageObserver obs) {
	if ( img == null ) {
		Toolkit tlk = Toolkit.getDefaultToolkit();
		File f = new File( fileName);
		if ( f.exists() ) {
			img = tlk.getImage( fileName);
			return true;
		}
		try {
			URL url = loaderCls.getResource( fileName);
			img = tlk.getImage( url);
			if (obs != null)
				img.getWidth( obs);
			return true;
		}
		catch ( Exception x) {
		}
		return false;
	}
	
	return true;
}
}
