package java.awt;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.io.File;
import java.io.FilenameFilter;
import java.util.Vector;

/**
 * FileDialog - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author J. Mehlitz
 */
public class FileDialog
  extends Dialog
  implements ActionListener, ItemListener, FilenameFilter
{
	private static final long serialVersionUID = 5035145889651310422L;
	final public static int LOAD = 0;
	final public static int SAVE = 1;
	final static String upDir = "..";
	int mode;
	FilenameFilter filter;
	Label lFilt = new Label( "Filter:");
	Label lFile = new Label( "File:");
	TextField tFilt = new TextField();
	TextField tFile = new TextField();
	Button bOk = new Button( "~Ok");
	Button bCan = new Button( "~Cancel");
	String filtExt;
	File dir;
	File tmpFile;
	List lDirs = new List();
	List lFiles = new List();

public FileDialog( Frame parent) {
	this( parent, null, LOAD);
}

public FileDialog( Frame parent, String title) {
	this( parent, title, LOAD);
}

public FileDialog( Frame parent, String title, int mode) {
	super( parent, title, true);
	this.mode = mode;
	
	initLayout();

	tFilt.addActionListener( this);
	tFile.addActionListener( this);
	bOk.addActionListener( this);
	bCan.addActionListener( this);
	lDirs.addItemListener( this);
	lFiles.addItemListener( this);
	lDirs.addActionListener( this);
	lFiles.addActionListener( this);

	filtExt = ".*";
	tFilt.setText( filtExt );
	
	setFilenameFilter( this);

}

public boolean accept ( File dir, String name) {
	if ( filtExt.endsWith( "*") )
		return true;
	return name.endsWith( filtExt);
}

public void actionPerformed( ActionEvent e) {
	Object s = e.getSource();

	if ( s == tFilt ) {
		filtExt = tFilt.getText();
		loadFiles( dir);
	}
	else if ( s == tFile ) {
		setFile( e.getActionCommand() );
		dispose();
	}
	else if ( s == lDirs ) {
		String d = lDirs.getSelectedItem();
		if ( d == upDir )
			loadSubs( new File( dir.getParent()) );
		else
			loadSubs( new File( d) );
	}
	else if ( s == lFiles ) {
		dispose();
	}
	else if ( s == bOk ) {
		dispose();
	}
	else if ( s == bCan ) {
		tFile.setText("");
		dispose();
	}
}

public void addNotify() {
	if ( (width == 0) || (height == 0) )
		setBounds( (Defaults.ScreenWidth - 400) / 2,
			 (Defaults.ScreenHeight - 300) / 2,
			  400, 300 );
	if (dir == null)
		setUserDir();

	super.addNotify();
}

static String getAbsolutePath( String path) {
	String userDir = System.getProperty( "user.dir");

	if ( (path == null) || (path.equals( ".") ) )
		return userDir;

	if ( path.startsWith( File.separator) || ((path.length() > 1) && (path.charAt(1) == ':')) )
		return path;

	char[] cc = path.toCharArray();
	int si;
	int dc=0;
	int ups=0;

	for ( si=0; si<cc.length; si++) {
		if ( cc[si] == '.' )
			dc++;
		else if ( cc[si] == File.separatorChar ) {
			if ( dc == 2)
				ups++;
			dc = 0;
		}
		else
			break;
	}

	if ( dc == 2 )
		ups++;

	if ( ups == 0 )
		return (userDir + File.separator + path );

	int lIdx = userDir.length();
	for ( ; ups > 0; ups-- )
		lIdx = userDir.lastIndexOf( File.separatorChar, lIdx - 1);
		
	if ( lIdx < 0 )
		lIdx = 0;

	return ( userDir.substring( 0, lIdx) + File.separator + path.substring( si) );

}

public String getDirectory() {
	String fn = tFile.getText();
	String ret;
	
	if ( fn.length() == 0 )
		ret = ".";
	else {
		File f = new File( fn );
		if ( f.isDirectory() )
			ret = f.getPath();
		else
			ret = f.getParent();
	}
	
	return (ret + File.separator);
}

public String getFile() {
	String fn = tFile.getText();

	if ( fn.length() == 0 )
		return null;

	File f = new File( fn );
	return f.getName();
}

public FilenameFilter getFilenameFilter() {
	return filter;
}

public int getMode() {
	return mode;
}

void initLayout() {
	int db = 5;
	GridBagConstraints gc = new GridBagConstraints();
	
	setLayout ( new GridBagLayout() );
	gc.ipadx = 1;
	gc.ipady = 1;
	gc.fill = gc.BOTH;
	gc.weightx = 1;
	gc.gridwidth = gc.REMAINDER;

	gc.insets.left = db;
	gc.insets.right = db;
	add( lFilt, gc);
	add( tFilt, gc);

	gc.weighty = 1;
	gc.gridwidth = 1;
	gc.insets.right = 0;
	add( lDirs, gc);
	
	gc.gridwidth = gc.REMAINDER;
	gc.insets.left = 0;
	gc.insets.right = db;
	add( lFiles, gc);
	
	gc.weighty = 0;
	gc.ipadx = 0;
	gc.ipady = 0;
	gc.insets.left = db;
	add( lFile, gc);
	add( tFile, gc);
	
	gc.gridwidth = 1;
	gc.fill = gc.NONE;
	gc.insets.left = db;
	gc.insets.right = db;
	gc.insets.bottom = 10;
	add( bOk, gc);
	
	gc.gridwidth = gc.REMAINDER;
	add( bCan, gc);
}

boolean isFiltered( File dir, String name) {
	if ( filter == null )
		return false;
	return ( ! filter.accept( dir, name) );
}

public void itemStateChanged( ItemEvent e) {

	if ( e.getStateChange() != e.SELECTED )
		return;

	Object s = e.getSource();
	String is;

	if ( s == lDirs ) {
		is = lDirs.getSelectedItem();
		if ( is != upDir ) {
			dir = new File( is );
			tFile.setText( dir.getPath() );
			loadFiles( dir);
		}
	}
	else if ( s == lFiles ) {
		is = lFiles.getSelectedItem();
		if ( dir.getPath().equals( File.separator) )
			tFile.setText( File.separator + is );
		else
			tFile.setText( dir + File.separator + is  );
	}
}

void loadFiles( File dir) {
	String[] all = dir.list( filter);
	lFiles.removeAll();

	// fake visibility while filling in the new contents
	lFiles.flags &= ~IS_VISIBLE;

	for ( int i=0; i<all.length; i++) {
		File f = new File ( dir + File.separator + all[i] );
		if ( ! f.isDirectory() )
			lFiles.addItem( all[i], sortIdx( lFiles.ip.rows, all[i] ) );
	}
	
	// now dislay it again
	lFiles.setVisible( true);
}

void loadSubs( File dir) {
	String dn = dir.getPath();
	boolean up = (dn.length() == 0);
	String[] all;
	
	if ( up )
		dir = new File( File.separator);

	all = dir.list();
		
	lFiles.removeAll();
	lDirs.removeAll();
	
	// fake visibility to disable redraw while filling in new contents
	lFiles.flags &= ~IS_VISIBLE;
	lDirs.flags &= ~IS_VISIBLE;
	
	this.dir = dir;
	tFile.setText( dir.getPath() );

	if ( ! up)	
		lDirs.add( upDir);
		
	for ( int i=0; i<all.length; i++) {
		File f = up ? new File( dir + all[i]) : new File( dir + File.separator + all[i]);
		if ( f.isDirectory() ) {
			String pn = f.getPath();
			lDirs.addItem( pn, sortIdx( lDirs.ip.rows, pn) );
		}
		else if ( ! isFiltered( dir, all[i] ) )
			lFiles.addItem( all[i], sortIdx( lFiles.ip.rows, all[i]) );
	}
		
	// display it again
	lFiles.setVisible( true);
	lDirs.setVisible( true);
}

public static void main( String[] args) {

	Frame f = new Frame();
	f.setBounds( 100, 100, 100, 100);
	f.setVisible( true);
	FileDialog fd = new FileDialog( f, "FileDialog");

//	fd.setBounds( 100, 100, 400, 300);

//	fd.loadSubs( new File( "/home/jhm/tvt/kaffe/libraries/javalib/java") );
//	fd.setFile( "/home/jhm/tvt/kaffe/libraries/javalib/java/awt/FileDialog.java");
//	fd.setFile( "FileDialog.java");
	fd.setFile( null);
	fd.setVisible( true);
	
	System.out.println( fd.getDirectory() );
	System.out.println( fd.getFile() );

}

protected String paramString() {
	return super.paramString();
}

void selectFile( String name) {
	String[] fa = lFiles.getItems();

	for ( int i=0; i<fa.length; i++) {
		if ( fa[i].equals( name)) {
			lFiles.select( i);
			return;
		}
	}

}

public void setDirectory( String dir) {
	File f = new File( getAbsolutePath( dir) );
	if ( f.exists() && f.isDirectory() )
		loadSubs( f);
		
	//file name specified before dir ?
	if ( tmpFile != null ) {
		selectFile( tmpFile.getName() );
		tmpFile = null;
	}
}

public void setFile( String file) {
	File f;

	if ( file == null ) {
		if ( dir == null )
			setUserDir();
		return;
	}
	
	f = new File( file);
	
	if ( f.isDirectory() ) {
		loadSubs( f);
		return;
	}

	if ( f.getParent() == null ) {	//not absolute
		if ( dir != null ) {
			selectFile( f.getName() );
			return;
		}
		tmpFile = f;
		f = new File( f.getAbsolutePath() );
	}
		
	if ( ! f.exists() )
		return;
		
	File dNew = new File( f.getParent() );
	loadSubs( dNew);

	selectFile ( f.getName() );		
}

public void setFilenameFilter( FilenameFilter filter) {
	this.filter = filter;
}

public void setMode( int mode) throws IllegalArgumentException {
	if ( ( mode != LOAD) && ( mode != SAVE) )
		throw new IllegalArgumentException( String.valueOf( mode));
	this.mode = mode;
}

void setUserDir() {
	setDirectory( System.getProperty( "user.dir") );
}

static int sortIdx( Vector v, String pathName) {
	int vs = v.size();
	if ( vs == 0 )
		return 0;

	for ( int i=vs-1; i>= 0; i-- ) {
		String cn = (String)v.elementAt( i);
		if ( pathName.compareTo( cn) >= 0 )
			return i+1;
	}

	return 0;	
}
}
