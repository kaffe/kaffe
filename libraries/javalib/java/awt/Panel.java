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
	private static LayoutManager defaultLayout = new FlowLayout();

public Panel() {
	this( defaultLayout );
}

public Panel( LayoutManager layout) {
	// Panels usually get their own update events, not being updated
	// sync within their parents
	flags |= IS_ASYNC_UPDATED;

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
}
