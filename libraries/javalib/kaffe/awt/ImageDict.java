/**
 * Copyright (c) 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author J.Mehlitz
 */

package kaffe.awt;

import java.awt.Component;
import java.awt.Image;
import java.awt.image.ImageObserver;
import java.util.Hashtable;

public class ImageDict
{
	static ImageDict singleton;
	Hashtable dict;

protected ImageDict () {
	dict = new Hashtable();
}

public static ImageDict getDefaultDict() {
	if ( singleton == null ) {
		String id = System.getProperty( "user.imagedict" );
		if ( id != null ) {
			try {
				singleton = (ImageDict) Class.forName( id).newInstance();
			}
			catch ( Exception e) {}
		}
		if ( singleton == null )
			singleton = new ImageDict();
	}
	return singleton;
}

public Image getImage( String key ) {
	return getImage( key, null, null );
}

public Image getImage( String key, String qual, Component target) {
	ImageSpec is = getSpec( key, qual, target);
	return ( is != null ? is.img : null );
}

public ImageSpec getSpec( String key, String qual, Component target) {
	ImageSpec spec = null;
	
	if ( qual != null )
		spec = (ImageSpec) dict.get( qual + key );
	if ( (spec == null) && (target != null) ) {
		for ( ; target.getParent() != null; target = target.getParent() );
		spec = (ImageSpec) dict.get( target.getClass().getName() + "." + key );
	}
	if ( spec == null )
		spec = (ImageSpec) dict.get( key);
		
	return spec;
}

public boolean isPlainImage( String key, String qual, Component target) {
	ImageSpec is = getSpec( key, qual, target);
	return ( is != null ? is.plain : false );
}

public Image putImage( String key, String fileName, String loaderCls, ImageObserver obs, boolean preload, boolean plain) {
	Class lc = null;
	Image img = null;
	
	if ( key != null )
		img = getImage( key);

	if ( img != null )
		return img;
		
	try {
		lc = Class.forName( loaderCls);
	}
	catch ( Exception x) {
		lc = getClass();
	}	
	
	ImageSpec spec = new ImageSpec( fileName, lc, obs, preload, plain);

	if ( key != null )
		dict.put( key, spec );
	
	return spec.img;
}

public Image putImage( String key, String fileName, boolean plain) {
	return putImage( key, fileName, null, null, true, plain);
}
}
