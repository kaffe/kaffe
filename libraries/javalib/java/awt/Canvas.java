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
	final private static long serialVersionUID = -2284879212465893870L;
	private static int counter;
 
public Canvas() {
	// Canvases usually get their own update events, not being updated
	// sync within their parents
	flags |= IS_ASYNC_UPDATED;

	setName("canvas" + counter++);
}

ClassProperties getClassProperties () {
	return ClassAnalyzer.analyzeAll( getClass(), true);
}

public Graphics getGraphics () {
	if ( (flags & IS_ADD_NOTIFIED) != 0 ){
		NativeGraphics g = NativeGraphics.getClippedGraphics( null, this,
		                                                      0, 0, 0, 0,
		                                                      width, height, false);
		if ( g != null )
			linkGraphics( g);

		return g;
	}
	else {
		return null;
	}
}

public boolean isFocusTraversable () {
	// for some obscure reason, Canvases are not focusTraversable by default
	return false;
}

//public void paint( Graphics g) {
//	// Canvas is a nativeLike Component, i.e. its background would
//	// normally be blanked by the native window system
//	g.clearRect( 0, 0, width, height);
//}
void processPaintEvent ( int id, int ux, int uy, int uw, int uh ) {
	NativeGraphics g = NativeGraphics.getClippedGraphics( null, this, 0,0,
	                                                      ux, uy, uw, uh,
	                                                      false);
	if ( g != null ){	
		if ( id == PaintEvt.UPDATE ) {
			update( g);
		}
		else {
			// Another anoying anomaly: on some window systems, the
			// background of native Components is automatically blanked (by the
			// native window system or native peer), which is simulated here.
			// Note that we don't do that for UPDATE events, since the update() spec
			// explicitly states "you can assume that the background is not cleared..",
			// and we take that literally! This also shows up in Panel
			g.clearRect( 0, 0, width, height);

			paint( g);
		}
		g.dispose();
	}
}
}
