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
