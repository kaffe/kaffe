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
	AppletStub stub;

public Applet() {
}

public void destroy() {
}

public AppletContext getAppletContext() {
	return ( stub != null) ? stub.getAppletContext() : null;
}

public String getAppletInfo() {
	return null;
}

public AudioClip getAudioClip( URL url) {
	AppletContext ac = getAppletContext();
	if ( ac != null)
		return ac.getAudioClip( url);
	return null;
}

public AudioClip getAudioClip( URL url, String name) {
	try { return getAudioClip( new URL( url, name)); }
	catch ( MalformedURLException m) { return null; }
}

public URL getCodeBase() {
	return ( stub != null) ? stub.getCodeBase() : null;
}

public URL getDocumentBase() {
	return ( stub != null) ? stub.getDocumentBase() : null;
}

public Image getImage( URL url) {
	AppletContext ac = getAppletContext();
	if ( ac != null)
		return (ac.getImage( url));
	else
		return (null);
}

public Image getImage( URL url, String name) {
	try { 
		return (getImage( new URL( url, name)));
	}
	catch ( MalformedURLException m) {
		return (null);
	}
}

public Locale getLocale() {
	return super.getLocale();
}

public String getParameter( String name) {
	return ( stub != null) ? stub.getParameter( name) : null;
}

public String[][] getParameterInfo() {
	return null;
}

public void init() {
}

public boolean isActive() {
	return ( stub != null) ? stub.isActive() : false;
}

final public static AudioClip newAudioClip( URL url) {
	return null;
}

public void play( URL url){
	AudioClip ac = getAudioClip( url);
	if ( ac != null)
		ac.play();	
}

public void play( URL url, String name) {
	try { play( new URL( url, name)); }
	catch ( MalformedURLException m) {}
}

public void resize( Dimension d) {
	if ( stub != null)
		stub.appletResize( d.width, d.height);
}

public void resize( int width, int height) {
	if ( stub != null)
		stub.appletResize( width, height);
}

final public void setStub( AppletStub stub){
	this.stub = stub;
}

public void showStatus( String msg) {
	AppletContext ac = getAppletContext();
	if ( ac != null)
		ac.showStatus( msg);
}

public void start() {
}

public void stop() {
}
}
