/**
 * Copyright (c) 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author J.Mehlitz
 */

package kaffe.applet;

import java.applet.Applet;
import java.applet.AppletContext;
import java.applet.AppletStub;
import java.applet.AudioClip;
import java.awt.Frame;
import java.awt.Dimension;
import java.awt.Image;
import java.awt.Label;
import java.awt.Insets;
import java.awt.Panel;
import java.awt.Menu;
import java.awt.MenuBar;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowListener;
import java.awt.event.WindowEvent;
import java.io.File;
import java.io.IOException;
import java.io.FileReader;
import java.io.StreamTokenizer;
import java.net.URL;
import java.net.MalformedURLException;
import java.util.Enumeration;
import java.util.Observable;
import java.util.Observer;
import java.util.StringTokenizer;
import java.util.Vector;
import java.util.Hashtable;

public class AppletViewer
  extends Frame
  implements AppletStub, AppletContext, ActionListener, WindowListener
{
	Applet app;
	Vector apps = new Vector();
	URL base;
	String codebase;
	String code;
	Hashtable paramDict = new Hashtable();
	Panel client = new Panel();
	Label state = new Label();

public AppletViewer ( File html) {
	
	super( "AppletViewer: " + html);
	setLayout( null);

	setMenus();
	add( state);
	addWindowListener(this);

	Dimension dim = new Dimension( 200, 200);	

	try {
		int ttype;
		FileReader fs = new FileReader( html);
		StreamTokenizer st = new StreamTokenizer( fs);

		st.lowerCaseMode( true);

		while ( (ttype = st.nextToken()) != st.TT_EOF ) {
			if ( ttype == '<' ){
				ttype = st.nextToken();
				if ( ttype == st.TT_WORD ) {
					if ( st.sval.equals( "applet") )
						parseApplet( st, dim);
					else if ( st.sval.equals( "param") )
						parseParam( st);
				}
			}
		}

		fs.close();
// System.out.println( code + " " + dim);

		Class c = Class.forName( code);

		app = (Applet) c.newInstance();
		app.setStub( this);
		apps.addElement( app);
		add( app);

		showStatus( "applet started");
	}
	catch ( Exception e) {
		e.printStackTrace();
		showStatus( "cannot start applet");
		e.printStackTrace();
	}

	getFrameDim( dim);
	setBounds( 0, 0, dim.width, dim.height);

	init();
	setVisible( true);
}

public void doLayout() {
	Insets in = getInsets();
	Dimension d = getSize();
	int sh = getStateHeight();

	if ( app != null )
		app.setBounds( in.left, in.top, d.width - in.left - in.right, d.height - in.top - in.bottom - sh);
	state.setBounds( in.left, d.height - in.bottom - sh, d.width - in.left - in.right, sh);

	super.doLayout();
}

int getStateHeight() {
	return 2 * state.getFontMetrics( state.getFont()).getHeight();
}

void getFrameDim( Dimension appDim) {
	Insets in = getInsets();
	appDim.width += in.left + in.right;
	appDim.height += in.top + in.bottom + getStateHeight();
}

void parseApplet( StreamTokenizer st, Dimension dim) throws IOException {

	while ( st.nextToken() != '>' ) {
		if ( st.sval.equals( "codebase") ) {
			st.lowerCaseMode( false);
			st.nextToken();
			st.nextToken();
			codebase = new String( st.sval);
			st.lowerCaseMode( true);
		}
		else if ( st.sval.equals( "code") ) {
			st.lowerCaseMode( false);
			st.nextToken();
			st.nextToken();
			code = new String( st.sval);
			if ( code.endsWith( ".class") )
				code = code.substring( 0, code.length() - 6);
			st.lowerCaseMode( true);
		}
		else if ( st.sval.equals( "height") ) {
			st.nextToken();
			st.nextToken();
			dim.height = (int)st.nval;
		}
		else if ( st.sval.equals( "width") ) {
			st.nextToken();
			st.nextToken();
			dim.width = (int)st.nval;
		}
	}

}

void parseParam( StreamTokenizer st) throws IOException {
	String key = null;
	String val = null;

	while ( st.nextToken() != '>' ) {
		if ( st.sval.equals( "name") ) {
			st.nextToken();
			st.nextToken();
			key = new String( st.sval);
		}
		else if ( st.sval.equals( "value") ) {
			st.nextToken();
			st.lowerCaseMode( false);
			st.nextToken();
			if ( st.ttype == st.TT_NUMBER)
				val = Double.toString( st.nval);
			else
				val = new String( st.sval);
			st.lowerCaseMode( true);
		}
	}

	if ( ( key != null) && ( val != null) ) {
//System.out.println( "put: " + key + " " + val);
		paramDict.put( key, val);
	}
}

public void appletResize ( int width, int height) {
	Dimension d = new Dimension( width, height);
	getFrameDim( d);
	setSize( d);
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
	return null;
}

public URL getCodeBase () {
	return base;
}

public URL getDocumentBase () {
	return base;
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

void init(){
	try {
		if ( (codebase == null) || (codebase.equals( ".") ) ){
			String curDir = System.getProperty( "user.dir");
			base = new URL( "file", "localhost", curDir + File.separator );
		}
		else {
			base = new URL( codebase);
		}
	}
	catch ( MalformedURLException _ ) {
		showStatus( "invalid codebase: " + codebase);
	}

	try {
		app.init();
		app.start();
	}
	catch( NullPointerException _) {
		showStatus( "invalid code");
	}

}

public boolean isActive () {
	return true;
}

public static void main ( String[] args) {
	if ( args.length == 0)
		System.out.println( "usage: AppletViewer <html file>");
	else {
		File f = new File( args[0] );
		if ( f.exists() )
			new AppletViewer( f);
		else
			System.out.println( "file " + f + " does not exist");
	}
}	


void setMenus () {
	MenuBar mb = new MenuBar();

	Menu m = new Menu( "Applet");
	m.add( "Restart");
	m.add( "Reload");
	m.add( "Stop");
	m.add( "Save...");
	m.add( "Start");
	m.add( "Clone...");
	m.addSeparator();
	m.add( "Tag...");
	m.add( "Info...");
	m.add( "Edit");
	m.add( "Character Encoding");
	m.addSeparator();
	m.add( "Print...");
	m.addSeparator();
	m.add( "Properties...");
	m.addSeparator();
	m.add( "Close");
	m.add( "Quit");

	m.addActionListener( this);
	
	mb.add( m);

	setMenuBar( mb);
}

public void showDocument ( URL url) {
}

public void showDocument ( URL url, String target) {
}

public void showStatus ( String str) {
	state.setText( " " + str + " " );
}

public void actionPerformed ( ActionEvent e ) {
	String cmd = e.getActionCommand();
	if ( "Quit".equals( cmd)) {
		app.stop();
		app.destroy();
		dispose();
	}
	else if ( "Stop".equals( cmd)) {
		app.stop();
		showStatus( "applet stopped");
	}
	else if ( "Start".equals( cmd)) {
		app.start();
		showStatus( "applet started");
	}
}

public void windowActivated ( WindowEvent evt ) {
}

public void windowClosed ( WindowEvent evt ) {
}

public void windowDeactivated ( WindowEvent evt ) {
}

public void windowDeiconified ( WindowEvent evt ) {
}

public void windowIconified ( WindowEvent evt ) {
}

public void windowOpened ( WindowEvent evt ) {
}

public void windowClosing(WindowEvent e) {
	app.stop();
	app.destroy();
	dispose();
}

}
