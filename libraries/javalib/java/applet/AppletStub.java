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

public interface AppletStub {
boolean isActive();
URL getDocumentBase();
URL getCodeBase();
String getParameter( String name);
AppletContext getAppletContext();
void appletResize( int width, int height);
}
