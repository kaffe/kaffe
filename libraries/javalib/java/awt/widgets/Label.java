/**
 * class Label - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.awt;


public class Label
  extends Component
{
	private static final long serialVersionUID = 3094126758329070636L;

	final public static int LEFT = 0;
	final public static int CENTER = 1;
	final public static int RIGHT = 2;

	private static final String [] ALIGNMENTS = {
		"left",
		"center",
		"right",
	};

	private static int counter;

	int align;
	String label;
	boolean hasBorder;
	FontMetrics fm;

public Label() {
	this( null );
}

public Label( String label) {
	this( label, LEFT);
}

public Label( String label, int align) {
	setForeground(Defaults.LabelClr);
	setFont(Defaults.LabelFont);
	setText(label != null ? label : "");
	setAlignment(align);
	setName("label" + counter++);
}

public int getAlignment() {
	return align;
}

ClassProperties getClassProperties () {
	return ClassAnalyzer.analyzeAll( getClass(), true);
}

public String getText() {
	return label;
}

public boolean isFocusTraversable() {
	return false;
}

public void paint( Graphics g) {
	int sw = fm.stringWidth( label);
	int db = hasBorder ? BORDER_WIDTH : 0;
	int y0 = height - (height-fm.getHeight())/2 - fm.getDescent();
	int x0;

	switch( align) {
	case CENTER:
		x0 = (width - sw)/2;
		break;
	case RIGHT:
		x0 = width - sw - db;
		break;
	case LEFT:
	default:
		x0 = db + 1;
		break;
	}

	g.setColor( getBackground() );
	g.fillRect( 0, 0, width, height);

	if ( hasBorder )
		kaffePaintBorder( g);
			
	if ( Defaults.LabelTxtCarved ) {
		g.setColor( Color.white);
		g.drawString( label, x0+1, y0+1 );
	}
	
	g.setColor( fgClr);
	g.drawString( label, x0, y0);
}

protected String paramString() {
	return ( super.paramString()
		 + ",align=" + ALIGNMENTS[getAlignment()]
		 + ",text=" + getText());
}

/**
 * @deprecated
 */
public Dimension preferredSize() {
	int cx = 40;
	int cy = 20;
	if ( fm != null ){
		cx = Math.max( cx, fm.stringWidth( label));
		cy = Math.max( cy, 3*fm.getHeight()/2 );
	}
	return new Dimension( cx, cy);
}

public void setAlignment( int align) {
	if ( align < LEFT || align > RIGHT ) {
		throw new IllegalArgumentException();
	}
	this.align = align;
	if ( isShowing() ) {
		repaint();
	}
}

public void setFont( Font f) {
	super.setFont( f);
	fm = getFontMetrics( f);
	if ( isShowing() ) {
		repaint();
	}
}

public void setText( String label) {
	if ( label == null ) {
		label = "";
	}
	if ( (this.label == null ) || ! (this.label.equals( label)) ){
		this.label = label;
		hasBorder = label.startsWith(" ") && label.endsWith( " ");
		if ( (flags & IS_SHOWING) == IS_SHOWING ) {
			Graphics g = getGraphics();
			if ( g != null ) {
				paint( g);
				g.dispose();
			}
			//repaint();
		}
	}
}
}
