package kaffe.applet;

/**
 * Copyright (c) 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author J.Mehlitz, G.Back
 */

import java.applet.Applet;
import java.applet.AppletContext;
import java.applet.AppletStub;
import java.applet.AudioClip;
import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Frame;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.Insets;
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
import java.io.FileReader;
import java.io.InputStreamReader;
import java.io.Reader;
import java.io.IOException;
import java.io.StreamTokenizer;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.FileOutputStream;
import java.io.FileNotFoundException;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLConnection;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Observable;
import java.util.Observer;
import java.util.StringTokenizer;
import java.util.Vector;
import java.util.zip.ZipFile;
import java.util.zip.ZipEntry;
import java.lang.String;

public class AppletViewer
  extends Frame
  implements AppletStub, AppletContext, ActionListener, WindowListener
{
	static boolean debug = false;
	Applet app;
	static Vector apps = new Vector();
	URL codebase = null;
	String archive = "";
	String code;
	Hashtable paramDict;
	Label state = new Label();
	Dimension appletSize;
	URL documentBase;
	ZipFile	ziparchive;
	AppletTag tag;

class AppletClassLoader extends ClassLoader
{
	AppletClassLoader() {

		if (debug) {
			System.out.println("AV: codebase= " + codebase);
			System.out.println("AV: archive= " + archive);
		}

		try {
			// copy jar file over
			if (!archive.equals("")) {
				URL url = new URL(codebase, archive);
				if (debug)
				    System.out.println("AV: opening url: " + url);
				InputStream in = url.openStream();
				byte b[] = new byte[4096];
				int r;
				File lf = new File("/tmp/" + archive);
				FileOutputStream out = new FileOutputStream(lf);
				while ((r = in.read(b)) != -1) {
					out.write(b, 0, r);
				}
				out.close();
				in.close();
				ziparchive = new ZipFile(lf);
				lf.delete();	// Unixy (!?)
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	private InputStream getStreamByName(String cname) throws Exception {
		if (ziparchive == null) {
			URL url = new URL(codebase, cname);
			if (debug) 
				System.out.println("AV: opening URL " + url);
			return (url.openStream());
		} else {
			ZipEntry ze = ziparchive.getEntry(cname);
			if (ze == null) {
				throw new Exception("Didn't find " 
					+ cname + " in " + archive);
			}
			return (ziparchive.getInputStream(ze));
		}		
	}

	public Class findLocalClass(String name) throws ClassNotFoundException {
		InputStream in = null;
		String cname = name.replace('.', '/') + ".class";
		if (debug)
			System.out.println("AV: loading class " + name);

		try {
			in = getStreamByName(cname);
			int  total = 0;
			byte code[] = new byte[8192];	

			for (;;) {
				int r = in.read(code, total, code.length-total);
				if (r == -1) {
					break;
				} else {
					total += r;
					if (total == code.length) {
						byte []ncode = new byte[code.length * 2];
						System.arraycopy(code, 0, ncode, 0, code.length);
						code = ncode;
					}
				}
			}

			if (debug)
			    System.out.println("AV: Defining " + name + " with " + total + " bytes");

			Class c = defineClass(name, code, 0, total);
			return (c);
		} catch (Exception e) {
			System.out.println(e.getMessage());
			throw new ClassNotFoundException(e.getMessage());
		}
	}

	public URL getResource(String name) {
		if (debug)
			System.out.println("AV: getResource called: " + name);
		try {
			return new URL(codebase, name);	// ???
		} catch (Exception e) {
			System.out.println(e);
			return (null);
		}
	}

	public InputStream getResourceAsStream(String name) {
		// hack for old version of custom, remove me when fixed!!!
		while (name.startsWith("/"))
			name = name.substring(1);
		if (debug)
			System.out.println("AV: getResourceAsStream: " + name);
		try {
			return (getStreamByName(name));
		} catch (Exception e) {
			System.out.println(e);
			return (null);
		}
	}
}

public AppletViewer ( URL documentBase, AppletTag tag) throws IOException {
	super( tag.toString());
	
	setLayout( null );
	setMenus();
	state.setFont(new Font("SansSerif", Font.BOLD, 12));
	add( state);
	addWindowListener(this);
	this.paramDict = tag.paramDict;
	this.code = tag.code;
	this.archive = tag.archive;
	this.appletSize = tag.appletSize;
	this.documentBase = documentBase;
	this.tag = tag;

	// no codebase given, default to documentbase
	if (tag.codebase == null || tag.codebase.equals("")) {
		if (documentBase.getFile().endsWith("/")) {
			this.codebase = documentBase;
		} else {
			String s = documentBase.getFile();

			// determine basename for file by stripping everything 
			// past the last slash
			int spos = s.lastIndexOf('/');
			if (spos != -1) {
			    s = s.substring(0, spos+1);
			}

			this.codebase = new URL(
				documentBase.getProtocol(), 
				documentBase.getHost(),
				documentBase.getPort(), s);
		}
	} else {	
		// codebase was given, put it in context to documentBase
		if (!tag.codebase.endsWith("/")) {
			this.codebase = new URL(documentBase, tag.codebase + "/");
		} else {
			this.codebase = new URL(documentBase, tag.codebase);
		}
	}
	if (debug) {
		System.out.println("AV: effective codebase= " + this.codebase);
	}

	try {
		AppletClassLoader loader = new AppletClassLoader();

		if (code == null) {
		    System.out.println("didn't find code tag");
		    System.exit(-1);
		}
		Class c = loader.loadClass(code);
		app = (Applet) c.newInstance();
		app.setStub( this);
		apps.addElement( app);

		add( app);

		addNotify(); // a hack, in case the applet creates Graphics during init (no joke)
		appletResize( appletSize.width, appletSize.height);

		app.init();
		app.validate();
		app.start();

		setVisible( true);		
		showStatus( "Applet started");
	}
	catch ( Exception e) {
		e.printStackTrace();
		showStatus( "Cannot start applet");
	}
}

private final String TagMenu	= "Tag";

public void actionPerformed ( ActionEvent e ) {
	String cmd = e.getActionCommand();
	if ( "Quit".equals( cmd)) {
		if (app != null) {
			app.stop();
			app.destroy();
		}
		dispose();
	}
	else if ( "Stop".equals( cmd)) {
		if (app != null) {
			app.stop();
		}
		showStatus( "Applet stopped");
	}
	else if ( "Start".equals( cmd)) {
		if (app != null)
			app.start();
		showStatus( "Applet started");
	}
	else if ( TagMenu.equals( cmd)) {
		System.out.println("printing tags: ");
		System.out.println(tag);
		System.out.println(paramDict);
	}
}

public void appletResize ( int width, int height ) {
	appletSize.setSize( width, height);
	
	Insets in = getInsets();
	int h = height + in.top + in.bottom;
	int w = width + in.left + in.right;
	
	setSize( w, h);
}

public void doLayout () {
	Insets in = getInsets();
	Dimension s = getSize();
	Dimension d = state.getPreferredSize();
	int cw = s.width - in.left - in.right;
	
	super.doLayout();
	app.setBounds( in.left, in.top, cw, s.height - in.top - in.bottom - d.height); 
	state.setBounds( in.left, s.height - in.bottom - d.height, cw, d.height );
}

public Applet getApplet ( String name) {
	return app;
}

public AppletContext getAppletContext () {
	return this;
}

public Enumeration getApplets () {
	return apps.elements();
}

public AudioClip getAudioClip ( URL url) {
	return new AudioPlayer( url);
}

public URL getCodeBase () {
	return (codebase);
}

public URL getDocumentBase () {
	return (documentBase);
}

public Image getImage ( URL url) {
	return Toolkit.getDefaultToolkit().getImage( url );
}

public String getParameter ( String name) {
	String key = name.toLowerCase();
	String val = (String)paramDict.get( key);
//System.out.println( "get: " + key + " " + val);
	return val;
}

private int getStateHeight() {
	return 2 * state.getFontMetrics( state.getFont()).getHeight();
}

public boolean isActive () {
	return true;
}

public static void main ( String[] args) throws Exception {
	if ( args.length == 0) {
		System.out.println( "Usage: AppletViewer [-debug] <url|file>");
	} else {
		int ac = 0;
		// support -debug switch
		if (args[ac].startsWith("-d")) {
			debug = true;
			ac++;
		}

		String loc = args[ac];
		int ttype;
		URL documentBase;
		Reader fs;
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
					    System.getProperty("user.dir") 
						+ File.separator + loc)
						.getCanonicalPath();
			}
			documentBase = new URL("file", "", fullpath);
		}
		if (debug) {
			System.out.println("AV: reading from URL: " + documentBase);
		}

		// open a connection to determine the real documentBase URL
		URLConnection uc = documentBase.openConnection();
		documentBase = uc.getURL();
		fs = new InputStreamReader(uc.getInputStream());

		StreamTokenizer st = new StreamTokenizer( fs);
		AppletTag tag = null;

		st.lowerCaseMode( true);
		st.ordinaryChar('/');
		st.ordinaryChar('\'');

		int nApplets = 0;
		while ( (ttype = st.nextToken()) != st.TT_EOF ) {
			if ( ttype == '<' ){
				ttype = st.nextToken();
				if ( ttype == st.TT_WORD ) {
					if ( st.sval.equals("applet") ) {
						tag = parseApplet( st);
						if (debug)
						    System.out.println("AV: found tag: " + tag);
					}
					else if ( st.sval.equals( "param") ) {
						if (tag != null) {
							tag.parseParam(st);
						}	// ignore <PARAM> outside of <APPLET>
					}
				} else
				if (ttype == '/') {
					ttype = st.nextToken();
					if (ttype == st.TT_WORD && st.sval.equals("applet")) {
						if (debug)
						    System.out.println("AV: creating new viewer " + tag);
						new AppletViewer(documentBase, tag);
						tag = null;
						nApplets++;
					}
				}
			}
		}
		fs.close();
		if (nApplets == 0) {
			System.err.println("Warning: no applets were found." +
			    " Make sure the input contains an <applet> tag");
		}
	}
}

static AppletTag parseApplet( StreamTokenizer st ) throws IOException {
	AppletTag tag = new AppletTag();

	while ( st.nextToken() != '>' ) {
		if (st.sval == null) {
			continue;
		}
		switch (st.ttype) {
		case StreamTokenizer.TT_WORD:
			if ( st.sval.equals("codebase") ) {
				st.lowerCaseMode( false);
				st.nextToken();
				st.nextToken();
				tag.codebase = new String( st.sval);
				st.lowerCaseMode( true);
			}
			else if ( st.sval.equals("archive") ) {
				st.lowerCaseMode(false);
				st.nextToken();
				st.nextToken();
				tag.archive = new String( st.sval);
				st.lowerCaseMode( true);
			}
			else if ( st.sval.equals( "code") ) {
				st.lowerCaseMode( false);
				st.nextToken();
				st.nextToken();
				tag.code = new String( st.sval);
				if (tag.code.endsWith( ".class")) {
					tag.code = tag.code.substring(0, tag.code.length() - 6);
				}
				st.lowerCaseMode( true);
			}
			else if ( st.sval.equals( "height") ) {
				st.nextToken();
				st.nextToken();
				if (st.ttype == st.TT_NUMBER) {
					tag.appletSize.height = (int)st.nval;
				}
				else {
					tag.appletSize.height = Integer.parseInt(st.sval);
				}
			}
			else if ( st.sval.equals( "width") ) {
				st.nextToken();
				st.nextToken();
				if (st.ttype == st.TT_NUMBER) {
					tag.appletSize.width = (int)st.nval;
				}
				else {
					tag.appletSize.width = Integer.parseInt(st.sval);
				}
			}
			break;

		default:
			break;
		}
	}
	return (tag);
}

static class AppletTag {
	Hashtable paramDict = new Hashtable();
	String codebase = "";
	String archive = "";
	String code;
        Dimension appletSize = new Dimension( 200, 200);

public String toString() {
	return "applet tag: codebase=" +  codebase 
		+ " archive= " + archive
		+ " code= " + code
		+ " appletSize= " + appletSize;
}

void parseParam( StreamTokenizer st) throws IOException {
	String key = null;
	String val = null;

	while ( st.nextToken() != '>' ) {
		if (st.sval == null) {
			continue;
		}
		if ( st.sval.equals( "name") ) {
			st.nextToken();
			st.nextToken();
			key = new String( st.sval);
		}
		else if ( st.sval.equals( "value") ) {
			st.nextToken();
			st.lowerCaseMode( false);
			st.nextToken();
			if ( st.ttype == st.TT_NUMBER) {
				int r = (int)st.nval;
				if (Math.abs(r - st.nval) < 1e-7) {	// hmmm
					val = Integer.toString(r);
				} else {
					val = Double.toString( st.nval);
				}
			}
			else {
				val = new String( st.sval);
			}
			st.lowerCaseMode( true);
		}
	}

	if ( key != null && val != null ) {
		key = key.toLowerCase();
//System.out.println( "put: " + key + " " + val);
		paramDict.put( key, val);
	}
}
}

void setMenus () {
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

public void showDocument ( URL url) {
	System.out.println( url);
}

public void showDocument ( URL url, String target) {
	try {
		showDocument( new URL( url, target));
	}
	catch ( MalformedURLException _x ) {
	}
}

public void showStatus ( String str) {
	state.setText( " " + str + " " );
}

void start() {
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
