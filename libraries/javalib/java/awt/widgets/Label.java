package java.awt;


/**
 * class Label - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class Label
  extends Component
{
	final public static int CENTER = 0;
	final public static int LEFT = 1;
	final public static int RIGHT = 2;
	int align;
	String label;
	boolean hasBorder;
	FontMetrics fm;

public Label() {
	this( "", LEFT);
}

public Label( String label) {
	this( label, LEFT);
}

public Label( String label, int align) {
	setForeground( Defaults.LabelClr);
	setFont( Defaults.LabelFont);
	setText( label);
	setAlignment( align);
}

public int getAlignment() {
	return align;
}

public Dimension getPreferredSize() {
	int cx = 50;
	int cy = 20;
	if ( fm != null ){
		cx = Math.max( cx, fm.stringWidth( label));
		cy = Math.max( cy, 3*fm.getHeight()/2 );
	}
	return new Dimension( cx, cy);
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
	int x0 = db + 1;

	switch( align) {
		case CENTER: x0 = (width - sw)/2; break;
		case RIGHT:  x0 = width - sw - db; break;
	}

	g.setColor( getBackground() );
	g.fillRect( 0, 0, width, height);

	if ( hasBorder )
		paintBorder( g);
			
	if ( Defaults.LabelsCarved ) {
		g.setColor( Color.white);
		g.drawString( label, x0+1, y0+1 );
	}
	
	g.setColor( fgClr);
	g.drawString( label, x0, y0);
}

protected String paramString() {
	return ( super.paramString() + ",Label: " + label);
}

public void setAlignment( int align) {
	if ( (align < CENTER) || (align > RIGHT) )
		throw new IllegalArgumentException();
	this.align = align;
	if ( isShowing() )
		repaint();
}

public void setFont( Font f) {
	super.setFont( f);
	fm = getFontMetrics( f);
	if ( isShowing() )
		repaint();
}

public void setText( String label) {
	if ( label == null )
		label = "";
	if ( (this.label == null ) || ! (this.label.equals( label)) ){
		this.label = label;
		hasBorder = label.startsWith( " ") && label.endsWith( " ");
		if ( isShowing() )
			repaint();
	}
}
}
