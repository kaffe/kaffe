package java.awt;


/**
 * Canvas - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author J. Mehlitz
 */
public class Canvas
  extends Component
{
public Canvas() {
	// Canvases usually get their own update events, not being updated
	// sync within their parents
	flags |= IS_ASYNC_UPDATED;
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

public void paint( Graphics g) {
	// Canvas is a nativeLike Component, i.e. its background would
	// normally be blanked by the native window system
	g.clearRect( 0, 0, width, height);
}
}
