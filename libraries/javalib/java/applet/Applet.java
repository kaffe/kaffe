package java.applet;

import java.awt.Dimension;
import java.awt.Image;
import java.awt.Panel;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.Locale;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class Applet
  extends Panel
{
	private static final long serialVersionUID = -5836846270535785031L;
	private AppletStub stub;

public Applet() {
}

public void destroy() {
}

public AppletContext getAppletContext() {
	return stub.getAppletContext();
}

public String getAppletInfo() {
	return (null);
}

public AudioClip getAudioClip( URL url) {
	return (getAppletContext().getAudioClip(url));
}

public AudioClip getAudioClip( URL url, String name) {
	try {
		return getAudioClip( new URL( url, name));
	}
	catch ( MalformedURLException m) {
		return null;
	}
}

public URL getCodeBase() {
	return (stub.getCodeBase());
}

public URL getDocumentBase() {
	return (stub.getDocumentBase());
}

public Image getImage( URL url) {
	return getAppletContext().getImage( url);
}

public Image getImage( URL url, String name) {
	if (url == null) {
		url = getCodeBase();
	}
	try { 
		return (getImage( new URL( url, name)));
	}
	catch ( MalformedURLException m) {
		// Hmmm, this seems to be one of the JDK plaaces where a image
		// factory method indeed returns null. No need to be more consistent
		return (null);
	}
}

public Locale getLocale() {
	return (super.getLocale());
}

public String getParameter( String name) {
	return (stub.getParameter(name));
}

public String[][] getParameterInfo() {
	return null;
}

public void init() {
}

public boolean isActive() {
	return (stub == null ? false : stub.isActive());
}

final public static AudioClip newAudioClip( URL url) {
	return new kaffe.applet.AudioPlayer(url);
}

public void play( URL url){
	final AudioClip ac = getAudioClip( url);
	if ( ac != null) {
		ac.play();	
	}
}

public void play( URL url, String name) {
	try {
		play( new URL( url, name));
	}
	catch ( MalformedURLException m) {
	}
}

public void resize( Dimension d) {
	resize( d.width, d.height);
}

public void resize( int width, int height) {
	super.resize( width, height);
	if ( stub != null) {
		stub.appletResize( width, height);
	}
}

final public void setStub( AppletStub stub) {
	this.stub = stub;
}

public void showStatus( String msg) {
	final AppletContext ac = getAppletContext();
	if ( ac != null) {
		ac.showStatus( msg);
	}
}

public void start() {
}

public void stop() {
}
}
