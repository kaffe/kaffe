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

import java.awt.Image;
import java.net.URL;
import java.util.Enumeration;

public interface AppletContext {
AudioClip getAudioClip( URL url);
Image getImage( URL url);
Applet getApplet( String name);
Enumeration getApplets();
void showDocument( URL url);
void showDocument( URL url, String target);
void showStatus( String status);
}
