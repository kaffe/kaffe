package java.awt;


/**
 *
 * Copyright (c) 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 */
public class Panel
  extends Container
{

static private LayoutManager defaultLayout = new FlowLayout();

public Panel() {
	this( defaultLayout );
}

public Panel( LayoutManager layout) {
	setLayout( layout);
}

public void update( Graphics g) {
	g.setColor( getBackground() );
	g.fillRect( 0, 0, width, height);
	g.setColor( getForeground() );
	paint( g);
}

}
