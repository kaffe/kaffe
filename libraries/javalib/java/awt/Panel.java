/**
 *
 * Copyright (c) 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 */

package java.awt;


public class Panel
  extends Container
{
	private static LayoutManager defaultLayout = new FlowLayout();

public Panel() {
	this( defaultLayout );
}

public Panel( LayoutManager layout) {
	setLayout( layout);
}

ClassProperties getClassProperties () {
	return ClassAnalyzer.analyzeAll( getClass(), true);
}

public Graphics getGraphics () {
	Graphics g = super.getGraphics();
	if ( g != null )
		g.setTarget( this);
	
	return g;
}

void paintChild ( Graphics g ) {
	g.setColor( getBackground() );
	g.fillRect( 0, 0, width, height);
	g.setColor( getForeground() );

	paint( g);
	paintChildren( g);
}

public void update( Graphics g) {
	paintChild( g);
}
}
