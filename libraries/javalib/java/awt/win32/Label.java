package java.awt;

import java.awt.Dimension;
import java.awt.FontMetrics;
import java.awt.Toolkit;


/**
 * class Label - 
 *
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class Label
  extends NativeComponent
{
	final public static int LEFT = 0;
	final public static int CENTER = 1;
	final public static int RIGHT = 2;
	int align;
	String label;

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

public void addNotify() {
	if ( nativeData == null ) {
		Toolkit.createNative(this);
		super.addNotify();
		Toolkit.lblSetJustify( nativeData, align );
	}
}

void createNative() {
	nativeData = Toolkit.lblCreateLabel( getParentData(), label);
}

public int getAlignment() {
	return align;
}

public Dimension getPreferredSize() {
	int cx = 50;
	int cy = 20;

	FontMetrics fm = getFontMetrics( getFont() );
	cx = Math.max( cx, fm.stringWidth( label));
	cy = Math.max( cy, 3*fm.getHeight()/2 );

	return new Dimension( cx, cy);
}

public String getText() {
	return label;
}

public boolean isFocusTraversable() {
	return false;
}

protected String paramString() {
	return ( super.paramString() + ",Label: " + label);
}

public void setAlignment( int align) {
	if ( (align < LEFT) || (align > RIGHT) )
		throw new IllegalArgumentException();
	this.align = align;

	if ( nativeData != null ) {
		Toolkit.lblSetJustify( nativeData, align);
	}
}

public void setText( String label) {
	if ( label == null )
		label = "";
	if ( (this.label == null ) || ! (this.label.equals( label)) ){
		this.label = label;
		if ( nativeData != null )
			Toolkit.lblSetText( nativeData, this.label);
	}
}
}
