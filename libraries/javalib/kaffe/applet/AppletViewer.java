
/**
 * Copyright (c) 1998, 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author J.Mehlitz, G.Back
 *
 * These classes have been rewritten in a way that allow applications
 * to easily embed applets in their own containers
 */

package kaffe.applet;

import java.applet.Applet;
import java.applet.AppletContext;
import java.applet.AppletStub;
import java.applet.AudioClip;
import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Frame;
import java.awt.Image;
import java.awt.Label;
import java.awt.Menu;
import java.awt.MenuBar;
import java.awt.Panel;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.io.File;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLClassLoader;
import java.net.URLConnection;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.StringTokenizer;
import java.util.Vector;

public class AppletViewer
  extends Frame
  implements ActionListener, WindowListener
{
    public static boolean debug;
    private static boolean showMenuBar = true;
    private static boolean showStatusBar = true;

    private Label 		statusBar = new Label();

    private Applet 		app;
    private AppletTag 		tag;
    private AppletContext	context;
    private static Vector 	applets = new Vector();

    // Our class loader
    private static class AppletClassLoader extends URLClassLoader {
	AppletClassLoader(URL codebase, String archive) throws IOException {
	    super(new URL[0]);
	    if (archive.equals("")) {
		addURL(codebase);
	    } else {
		for (StringTokenizer t = new StringTokenizer(archive, ", ");
			t.hasMoreTokens(); ) {
		    addURL(new URL(codebase, t.nextToken()));
		}
	    }
	}
    }

public static class DefaultAppletContext implements AppletContext {
    private Vector apps = new Vector();
    private Label	statusBar;

    public DefaultAppletContext(Label statusBar) {
	this(new Vector(), statusBar);
    }

    public DefaultAppletContext(Vector apps, Label statusBar) {
	this.statusBar = statusBar;
	this.apps = apps;
    }

    public void addApplet(Applet app) {
	apps.addElement(app);
    }

    public Applet getApplet (String name) {
	for (int i = 0; i < apps.size(); i++) {
	    Applet app = (Applet)apps.elementAt(i);
	    if (app.getName().equals(name))
		return (app);
	}
	return (null);
    }

    public Enumeration getApplets () {
	return (apps.elements());
    }

    public AudioClip getAudioClip ( URL url) {
	return (new AudioPlayer(url));
    }

    public Image getImage ( URL url) {
	return (Toolkit.getDefaultToolkit().getImage( url ));
    }

    public void showDocument ( URL url) {
	showDocument(url, "_self");
    }

    public void showDocument ( URL url, String target) {
	System.err.println("Not implemented in AppletViewer:\n"
	    + "showDocument("  + url + ", " + target + ")");
    }

    public void showStatus ( String str) {
	statusBar.setText( " " + str + " " );
    }
}

public static class DefaultAppletStub extends Panel implements AppletStub {
    private AppletContext	context;
    private URL			codebase;
    private URL			documentBase;
    private Hashtable		paramDict;
    private Dimension		preferredSize;

    public DefaultAppletStub(URL documentBase, AppletTag tag,
		AppletContext context)
    {
	this.paramDict = tag.getParameters();
    	this.context = context;
    	this.codebase = tag.getCodebaseURL();
    	this.documentBase = documentBase;
	// we are doing our own layout, see doLayout()
	setLayout(null);
    }

    public Dimension getMinimumSize() {
	return (getPreferredSize());
    }

    public Dimension getPreferredSize() {
	return (preferredSize);
    }

    public AppletContext getAppletContext() {
	return (context);
    }

    public URL getCodeBase() {
	return (codebase);
    }

    public URL getDocumentBase() {
	return (documentBase);
    }

    public String getParameter( String name) {
	if (paramDict == null) {
	    return (null);
	}
	String key = name.toLowerCase();
	String val = (String)paramDict.get( key);
	if (debug) {
	    System.out.println( "AV: getP: " + key + " " + val);
	}
	return (val);
    }

    public boolean isActive () {
	return (true);
    }

    /**
     * Resize applet to a given width/height
     *
     * We simply set our own size to the desired size.
     */
    public void appletResize ( int width, int height ) {

	preferredSize = new Dimension(width, height);
	if (debug) {
	    System.out.println("AV: resizing app to " + preferredSize);
	}
	setSize(width, height);
    }

    public void doLayout () {
	super.doLayout();

	// assume applet is our only child, give it all the space we have
	Component c = this.getComponent(0);
	if (c != null) {
	    Dimension cs = preferredSize;
	    if (cs == null) {
	    	if (debug)
		    System.out.println("AV: no pref size, using current size");
		cs = getSize();
	    }
	    if (debug) {
		System.out.println("AV: setting child to " + cs);
		System.out.println("AV: my insets " + getInsets());
	    }
	    c.setBounds(0, 0, cs.width, cs.height);
	}
    }
}

public AppletViewer (URL documentBase, AppletTag tag) throws IOException
{
    super(tag.toString());
    
    setLayout(new BorderLayout());
    setMenus();
    statusBar.setFont(new Font("SansSerif", Font.BOLD, 12));
    if (showStatusBar) {
	add(BorderLayout.SOUTH, statusBar);
    }
    addWindowListener(this);

    this.tag = tag;

    //
    // we're  creating a new applet context for each applet so that each
    // applet gets its own status bar
    // this is different from a browser page, where multiple applets
    // on the same page would share the same context and status bar
    // to account for that, we tell the context a vector to keep track
    // of all applets that logically belong to the same context
    //
    DefaultAppletContext dcontext = new DefaultAppletContext(applets, statusBar);
    context = dcontext;

    DefaultAppletStub stub = new DefaultAppletStub(documentBase, tag, context); 

    app = createApplet(this.tag, stub);
    if (app == null) {
	context.showStatus( "Cannot start applet");
	return;
    } 

    stub.add(app);
    add(BorderLayout.CENTER, stub);

    addNotify(); // a hack, in case the applet creates Graphics during init (no joke)
    Dimension appletSize = tag.getAppletSize();
    stub.appletResize( appletSize.width, appletSize.height);

    app.init();
    app.validate();
    app.start();

    pack();
    setVisible( true);		
    if (debug) {
	System.out.println("my size " + getSize());
	System.out.println("stub size " + stub.getSize());
	System.out.println("label size " + statusBar.getSize());
    }
    context.showStatus( "Applet started");
}

public final static Applet createApplet(AppletTag tag, AppletStub stub) {
    Applet app = null;

    try {
	String code = tag.getCodeTag();
	if (code == null) {
	    System.out.println("didn't find code tag");
	    System.exit(-1);
	}

	AppletClassLoader loader = 
	    new AppletClassLoader(tag.getCodebaseURL(), tag.getArchiveTag());
	/* Set the contect loader of the current thread to the applet class loader,
	 * to avoid problems between casting classes loaded by separate class loaders.
	 */
	Thread.currentThread().setContextClassLoader(loader);

	Class c = loader.loadClass(code);
	app = (Applet) c.newInstance();

	// set applet.name according to <applet name= > tag
	// used in AppletContext.getApplet()
	String appName = tag.getName();
	if (appName != null) {
	    app.setName(appName);
	}

	app.setStub(stub);

	// Convenience:
	// if the applet's stub context is one of our default contexts
	// add it to that context.
	AppletContext ctxt = stub.getAppletContext();
	if (ctxt instanceof DefaultAppletContext) {
	    ((DefaultAppletContext)ctxt).addApplet(app);
	}
    } catch (Exception e) {
	e.printStackTrace();
    }
    return (app);
}

private static final String TagMenu	= "Tag";
private static final String QuitMenu	= "Quit";
private static final String StopMenu	= "Stop";
private static final String StartMenu	= "Start";

public void actionPerformed ( ActionEvent e ) {
	String cmd = e.getActionCommand();
	if ( QuitMenu.equals( cmd)) {
		if (app != null) {
			app.stop();
			app.destroy();
		}
		dispose();
	}
	else if ( StopMenu.equals( cmd)) {
		if (app != null) {
			app.stop();
		}
		context.showStatus( "Applet stopped");
	}
	else if ( StartMenu.equals( cmd)) {
		if (app != null)
			app.start();
		context.showStatus( "Applet started");
	}
	else if ( TagMenu.equals( cmd)) {
		System.out.println("printing tags: ");
		System.out.println(tag);
		System.out.println(tag.getParameters());
	}
}

public static void main(String[] args) throws Exception {
    int width = -1;
    int height = -1;
    String loc = null;
    showMenuBar = true;
    showStatusBar = true;

    for (int i = 0; i < args.length; i++) {
	if (args[i].startsWith("-d")) {
	    debug = true;
	} else
	if (args[i].equals("-nomenu")) {
	    showMenuBar = false;
	} else
	if (args[i].equals("-nostatus")) {
	    showStatusBar = false;
	} else
	if (args[i].startsWith("-w")) {
	    width = Integer.parseInt(args[++i]);
	} else
	if (args[i].startsWith("-h")) {
	    height = Integer.parseInt(args[++i]);
	} else {
	    loc = args[i];
	    break;
	}
    }

    if (loc == null) {
	System.out.println( "Usage: AppletViewer [-debug] <url|file>");
	System.exit(0);
    } 

    URLConnection uc = openAppletURLConnection(loc);
    URL documentBase = uc.getURL();
    AppletTag [] appletTags = AppletTag.parseForApplets(uc.getInputStream());

    if (appletTags.length == 0) {
	System.err.println("Warning: no applets were found." +
	    " Make sure the input contains an <applet> tag");
	System.exit(0);
    }

    for (int i = 0; i < appletTags.length; i++) {
	AppletTag currentTag = appletTags[i];

	currentTag.computeCodeBaseURL(documentBase);
	if (debug) {
	    System.out.println("AV: effective codebase= " 
		+ currentTag.getCodebaseURL());
	}

	// override width/height if user says so
	if (width != -1) {
	    currentTag.setAppletWidth(width);
	}
	if (height != -1) {
	    currentTag.setAppletHeight(height);
	}

	new AppletViewer(documentBase, currentTag);
    }
}

/**
 * open a url connection to a specified location, file or url.
 * Use the obtained URLConnection to get the effective documentbase for
 * the applet (getURL()),  and use getInputStream() to get the html
 * input to parse for applet tags
 */
public static URLConnection openAppletURLConnection(String loc) 
	throws IOException 
{
    URL documentBase;

    try {
	//
	// normally, doing "new URL()" should cause a
	// malformed exception if there's no "protocol:"
	// field.  Currently, Kaffe doesn't, fake one for now
	//
	if (!(loc.startsWith("http:") || loc.startsWith("file:"))) {
	    throw new MalformedURLException();	
	}
	documentBase = new URL(loc);

    } catch (MalformedURLException e) {
	// if it's not a well-formed URL, we assume it's
	// a filename and see where this gets us
		
	String fullpath;

	// note that getCanonicalPath is not fully implemented
	// yet (as of 7/26/99), but once it is, it should do
	// what we want (I hope)
	if (loc.startsWith(File.separator)) {
	    fullpath = new File(loc).getCanonicalPath();
	} else {
	    fullpath = new File(
		System.getProperty("user.dir") + File.separator + loc)
		    .getCanonicalPath();
	}
	documentBase = new URL("file", "", fullpath);
    }

    if (debug) {
	System.out.println("AV: reading from URL: " + documentBase);
    }

    // open a connection to determine the real documentBase URL
    return (documentBase.openConnection());
}

void setMenus () {
	// user said -nomenu
	if (!showMenuBar) {
		return;
	}
	MenuBar mb = new MenuBar();

	Menu m = new Menu( "Applet");
	m.add( "Restart (dummy)");
	m.add( "Reload (dummy)");
	m.add( "Stop");
	m.add( "Save... (dummy)");
	m.add( "Start");
	m.add( "Clone... (dummy)");
	m.addSeparator();
	m.add( TagMenu);
	m.add( "Info... (dummy)");
	m.add( "Edit (dummy)");
	m.add( "Character Encoding (dummy)");
	m.addSeparator();
	m.add( "Print... (dummy)");
	m.addSeparator();
	m.add( "Properties... (dummy)");
	m.addSeparator();
	m.add( "Close (dummy)");
	m.add( "Quit");

	m.addActionListener( this);
	
	mb.add( m);

	setMenuBar( mb);
}

public void windowActivated ( WindowEvent evt ) {
}

public void windowClosed ( WindowEvent evt ) {
}

public void windowClosing(WindowEvent e) {
	if (app != null) {
		app.stop();
		app.destroy();
	}
	dispose();
}

public void windowDeactivated ( WindowEvent evt ) {
}

public void windowDeiconified ( WindowEvent evt ) {
}

public void windowIconified ( WindowEvent evt ) {
}

public void windowOpened ( WindowEvent evt ) {
}
}
