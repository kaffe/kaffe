/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */


package java.applet; 
import java.net.URL;
import java.awt.Image;
import java.util.Enumeration;

public interface AppletContext {
public abstract AudioClip getAudioClip( URL url);
public abstract Image getImage( URL url);
public abstract Applet getApplet( String name);
public abstract Enumeration getApplets();
public abstract void showDocument( URL url);
public abstract void showDocument( URL url, String target);
public abstract void showStatus( String status);
}
