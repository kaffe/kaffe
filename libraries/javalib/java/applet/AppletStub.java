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
public abstract boolean isActive();
public abstract URL getDocumentBase();
public abstract URL getCodeBase();
public abstract String getParameter( String name);
public abstract AppletContext getAppletContext();
public abstract void appletResize( int width, int height);
}
