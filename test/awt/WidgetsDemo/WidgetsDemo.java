import java.awt.*;
import java.awt.datatransfer.*;
import java.awt.event.*;
import java.io.*;
import java.util.Vector;

/**
 * bundle test for awt-widgets
 *
 * Copyright (c) 1998
 *   Transvirtual Technologies Inc., BISS GmbH.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 * @author J.H.Mehlitz
 */
public class WidgetsDemo
  extends Frame
  implements ActionListener, ItemListener, MouseListener, LayoutManager
{
	Button CAdd = new java.awt.Button(" Add ");
	Checkbox CC1 = new java.awt.Checkbox("Male");
	Checkbox CC2 = new java.awt.Checkbox("Female");
	Choice CCat = new java.awt.Choice();
	Button CClose = new java.awt.Button(" Close ");
	Button CMod = new java.awt.Button(" Modify ");
	List CNames = new java.awt.List();
	Button CRem = new java.awt.Button(" Remove ");
	Label LCat = new java.awt.Label("category:");
	Label LCom = new java.awt.Label("comments:");
	Label LFirst = new java.awt.Label("first name:");
	Label LLast = new java.awt.Label("last name:");
	Label LNames = new java.awt.Label("names:");
	Label LSName = new java.awt.Label(" ");
	Label LSState = new java.awt.Label(" ");
	Label LSex = new java.awt.Label("sex:");
	TextArea TCom = new java.awt.TextArea("");
	TextField TFirst = new java.awt.TextField("");
	Vector persons = new Vector();
	Image Img = getToolkit().getImage( "kaffe_powered.png");
	TextField TLast = new java.awt.TextField("");
	int[] CCords = { 0, 0, 1000, 0, 12, 33, -280, 171,
		1000, 0, 1000, 0, -268, 33, -192, 58,
		1000, 0, 1000, 0, -187, 33, -11, 58,
		1000, 0, 1000, 0, -268, 60, -192, 85,
		1000, 0, 1000, 0, -187, 60, -11, 85,
		1000, 0, 1000, 0, -268, 87, -192, 112,
		1000, 0, 1000, 0, -187, 87, -10, 112,
		1000, 0, 1000, 0, -268, 114, -192, 139,
		1000, 0, 1000, 0, -187, 114, -105, 138,
		1000, 0, 1000, 0, -97, 114, -16, 138,
		0, 0, 0, 0, 12, 174, 88, 199,
		0, 0, 1000, 1000, 12, 201, -11, -133,
		1000, 0, 1000, 0, -269, 141, -209, 169,
		1000, 0, 1000, 0, -206, 141, -146, 169,
		1000, 0, 1000, 0, -143, 141, -83, 169,
		1000, 0, 1000, 0, -80, 141, -11, 169,
		0, 1000, 455, 1000, 2, -25, 0, -2,
		455, 1000, 1000, 1000, 0, -25, -2, -2,
		0, 0, 0, 0, 12, 6, 88, 31 };

class Person
{
	String firstName;
	String lastName;
	boolean female;
	String category;
	String comment;

Person( String firstName, String lastName, boolean female, String category, String comment) {
	this.firstName = firstName;
	this.lastName = lastName;
	this.female = female;
	this.category = category;
	this.comment = comment;
}

String mangle() {
	return (lastName + ", " + firstName);
}
}

public WidgetsDemo() {
  super( "WidgetsDemo");
  setLayout( this);
  open();
}

public void actionPerformed( ActionEvent e) {
	Object src = e.getSource();
	if ( src == CAdd ) {
		Person p = new Person( TFirst.getText(), TLast.getText(), CC2.getState(), CCat.getSelectedItem(), TCom.getText());
		persons.addElement( p);
		CNames.addItem( p.mangle() );
		LSState.setText( " person added ");
	}
	else if ( src == CMod ) {
		int sIdx = CNames.getSelectedIndex();
		Person p = new Person( TFirst.getText(), TLast.getText(), CC2.getState(), CCat.getSelectedItem(), TCom.getText());
		persons.setElementAt( p, sIdx );
		CNames.replaceItem( p.mangle(), sIdx );
		LSState.setText( " person modified ");
	}
	else if ( src == CRem ) {
		int sIdx = CNames.getSelectedIndex();
		persons.removeElementAt( sIdx);
		CNames.remove( sIdx );
		LSState.setText( " person removed ");
	}
	else if ( src == CClose ) {
		dispose();
	}
	else {
		String cmd = e.getActionCommand();
		if ( cmd.equals( "Open" )) {
			LSState.setText( " i'm afraid there is nothing to open ");
		}
		else if ( cmd.equals( "Close" )) {
			dispose();
		}
		else if ( cmd.equals( "Fonts" )) {
			LSState.setText( " would like to change some fonts, eh ? ");
		}
		else if ( cmd.equals( "Colors" )) {
			LSState.setText( " just another dummy ");
		}
		else if ( cmd.equals( "Help" )) {
			LSState.setText( " a little widgets demo ");
		}
		else if ( cmd.equals( "Load" )) {
			FileDialog fd = new FileDialog( this, "Load File" );
			fd.setVisible( true);
			loadFile( fd.getDirectory() + fd.getFile() );
		}
		else if ( cmd.equals( "Cut" )) {
			cut();
		}
		else if ( cmd.equals( "Copy" )) {
			copy();
		}
		else if ( cmd.equals( "Paste" )) {
			paste();
		}
		else if ( cmd.equals( "Select All" )) {
			TCom.selectAll();
		}
	}
}

public void addLayoutComponent( String name, Component child) {
}

void copy() {
	String s = TCom.getSelectedText();
	Clipboard cb = Toolkit.getDefaultToolkit().getSystemClipboard();
	if ( ( s != null) && ( s.length() > 0 ) ) {
		StringSelection ss = new StringSelection( s);
		cb.setContents( ss, ss);
	}
}

void cut() {
	int ss = TCom.getSelectionStart();
	int se = TCom.getSelectionEnd();
	if ( se > ss) {
		copy();
		TCom.replaceRange( null, ss, se);
	}
	
}

public void init() {
	initPersons();

	CheckboxGroup cb1 = new CheckboxGroup();
	CC1.setCheckboxGroup( cb1);
	CC2.setCheckboxGroup( cb1);

	CC1.setState( true);
	CC1.addItemListener( this);
	CC2.addItemListener( this);
	
	CCat.add( "actor");
	CCat.add( "doctor");
	CCat.add( "cowboy");
	CCat.add( "cop");
	CCat.add( "investigator");
	CCat.add( "witch");
	CCat.add( "other");

	CCat.addItemListener( this);
	
	
	MenuBar mb = new MenuBar();

	Menu m1 = new Menu( "File");
	m1.addActionListener( this);
	m1.add( "Load");

	Menu m2 = new Menu( "Options");
	m2.addActionListener( this);
	m2.add( "Colors");
	m2.add( "Fonts");

	Menu m3 = new Menu ( "Help");
	m3.addActionListener( this);
	m3.setShortcut( new MenuShortcut( 'H', true));

	mb.add( m1);
	mb.add( m2);
	mb.add( m3);
	setMenuBar( mb);

	PopupMenu mp = new PopupMenu();
	Menu m4 = new Menu( "File");
	m4.add( "Load");
	Menu m5 = new Menu( "Edit");
	m5.add( "Select All");
	m5.add( "Cut");
	m5.add( "Copy");
	m5.add( "Paste");
	mp.add( m4);
	mp.add( m5);
	mp.addActionListener( this);
	TCom.add( mp);
	
	CNames.addItemListener( this);
	CAdd.addActionListener( this);
	CAdd.addMouseListener( this);
	CMod.addActionListener( this);
	CMod.addMouseListener( this);
	CRem.addActionListener( this);
	CRem.addMouseListener( this);
	CClose.addActionListener( this);
	CClose.addMouseListener( this);

	CNames.select( 0);

	LSState.setAlignment( Label.CENTER);
	LSState.setForeground( Color.blue);
}

void initPersons() {
	persons.addElement( new Person( "Sam", "Spade", false, "investigator", "private investigations") );
	persons.addElement( new Person( "Gundel", "Gaukley", true, "witch", "Donald's nightmare") );
	persons.addElement( new Person( "Don", "Johnson", false, "actor", "no comment") );
	persons.addElement( new Person( "Marie Jeanne", "Dubarray", true, "other", "a french lady ...") );
	persons.addElement( new Person( "Humphrey", "Bogart", false, "actor", "looking at you") );

	for ( int i=0; i<persons.size(); i++) {
		CNames.addItem( ((Person)persons.elementAt( i)).mangle() );
	}
}

public void itemStateChanged( ItemEvent e) {
	Component c = (Component) e.getSource();
	if ( c == CNames) {
		if ( e.getStateChange() == e.SELECTED ) {
			String sel = CNames.getSelectedItem();
			for ( int i=0; i<persons.size(); i++) {
				Person p = (Person)persons.elementAt( i);
				if ( p.mangle().equals( sel) ) {
					TFirst.setText( p.firstName );
					TLast.setText( p.lastName );
					TCom.setText( p.comment );
					CCat.select( p.category );
					if ( p.female )
						CC2.setState( true);
					else
						CC1.setState( true);

					LSName.setText( " " + sel + " ");
					LSState.setText( " new selection ");
					break;
				}
			}
		}
	}
}

public void layoutContainer( Container parent) {
	Rectangle b = getBounds();
	Insets in = getInsets();
	int cc = getComponentCount();
	int ci = 0;
	int cx, cy, cw, ch, clw, clh;

	clw = b.width-in.left-in.right;
	clh = b.height-in.top-in.bottom;

	for ( int i=0; i<cc; i++) {
		Component c = getComponent( i);
		cx = CCords[ci++]*clw/1000+CCords[ci+3] + in.left;
		cy = CCords[ci++]*clh/1000+CCords[ci+3] + in.top;
		cw = CCords[ci++]*clw/1000+CCords[ci+3] - cx + in.left;
		ch = CCords[ci++]*clh/1000+CCords[ci+3] - cy + in.top;
		c.setBounds( cx, cy, cw, ch);
		ci += 4;
	}

}

void loadFile( String pathName) {
	File f = new File( pathName);
	if ( f.exists() && ! f.isDirectory() ) {
		try {
			FileReader fr = new FileReader( f);
			char[] fb = new char[ (int)f.length() ];
			fr.read( fb, 0, fb.length );
			fr.close();
			TCom.setText( new String( fb));
		}
		catch( Exception e) {}
	}
}

public static void main( String[] args) {
  new WidgetsDemo();
}

public Dimension minimumLayoutSize( Container parent) {
	return new Dimension( 100, 100);
}

public void mouseClicked( MouseEvent e) {
}

public void mouseEntered( MouseEvent e) {
	Object s = e.getSource();
	if ( s == CAdd )
		LSState.setText( " add entry ");
	else if ( s == CMod )
		LSState.setText( " modify entry ");
	else if ( s == CRem )
		LSState.setText( " remove entry ");
	else if ( s == CClose )
		LSState.setText( " close dialog ");
}

public void mouseExited( MouseEvent e) {
	LSState.setText( " " );
}

public void mousePressed( MouseEvent e) {
}

public void mouseReleased( MouseEvent e) {
}

protected void open() {
	add( CNames);
	add( LLast);
	add( TLast);
	add( LFirst);
	add( TFirst);
	add( LCat);
	add( CCat);
	add( LSex);
	add( CC1);
	add( CC2);
	add( LCom);
	add( TCom);
	add( CAdd);
	add( CMod);
	add( CRem);
	add( CClose);
	add( LSName);
	add( LSState);
	add( LNames);

	init();

	setBounds( 10, 10, 430, 450);
	setVisible( true);
}

public void paint( Graphics g) {
	super.paint( g);

	Dimension d = getSize();
	int h = Img.getHeight( this);
	int w = Img.getWidth( this);

	int x1 = 70;
	int y1 = d.height-125;
	int x2 = d.width-70;
	int y2 = d.height-38;
	g.drawImage( Img, x1, y1, x2, y2, 0, 0, w, h, Color.lightGray, this);

	g.setColor( getBackground() );
	w = x2-x1+1;
	h = y2-y1+1;
	g.draw3DRect( x1, y1, w, h, true);
	x1++; y1++; w -=2; h -= 2;
	g.draw3DRect( x1, y1, w, h, false);
}

void paste() {
	Clipboard cb = Toolkit.getDefaultToolkit().getSystemClipboard();
	Transferable tf = cb.getContents( TCom);
	if ( tf != null ) {
		try {
			String s = (String) tf.getTransferData( DataFlavor.stringFlavor);
			TCom.replaceRange( s, TCom.getSelectionStart(), TCom.getSelectionEnd() );
		}
		catch ( Exception x) {}
	}
}

public Dimension preferredLayoutSize( Container parent) {
	return new Dimension( 300, 300);
}

public void removeLayoutComponent( Component child) {
}
}
