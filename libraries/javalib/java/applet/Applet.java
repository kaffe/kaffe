package java.applet;

import java.awt.Panel;
import java.awt.Image;
import java.awt.Dimension;
import java.net.URL;
import java.net.MalformedURLException;
import java.util.Locale;


public class Applet
  extends Panel
{
 AppletStub stub;

public Applet() {
}

public final void setStub( AppletStub stub){
	this.stub = stub;
}

public boolean isActive() {
	return ( stub != null) ? stub.isActive() : false;
}

public URL getDocumentBase() {
	return ( stub != null) ? stub.getDocumentBase() : null;
}

public URL getCodeBase() {
	return ( stub != null) ? stub.getCodeBase() : null;
}

public String getParameter( String name) {
	return ( stub != null) ? stub.getParameter( name) : null;
}

public AppletContext getAppletContext() {
	return ( stub != null) ? stub.getAppletContext() : null;
}

public void resize( int width, int height) {
	if ( stub != null)
		stub.appletResize( width, height);
}

public void resize( Dimension d) {
	if ( stub != null)
		stub.appletResize( d.width, d.height);
}

public void showStatus( String msg) {
	AppletContext ac = getAppletContext();
	if ( ac != null)
		ac.showStatus( msg);
}

public Image getImage( URL url) {
	AppletContext ac = getAppletContext();
	if ( ac != null)
		return ac.getImage( url);
	return null;
}

public Image getImage( URL url, String name) {
	try { return getImage( new URL( url, name)); }
	catch ( MalformedURLException m) { return null; }
}

public static final AudioClip newAudioClip( URL url) {
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

public String getAppletInfo() {
	return null;
}

public Locale getLocale() {
	return super.getLocale();
}

public String[][] getParameterInfo() {
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

public void init() {
}

public void start() {
}

public void stop() {
}

public void destroy() {
}
}
