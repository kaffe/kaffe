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
private static final long serialVersionUID = -2284879212465893870L;

public Canvas() {
	// Canvases usually get their own update events, not being updated
	// sync within their parents
	flags |= IS_ASYNC_UPDATED;
}

/**
 * Sun's version of this class apparently has this method. So we put
 * one here so applets compiled against kaffe's version of this class
 * will call the right method when they do super.addNotify().
 */
public void addNotify() {
	super.addNotify();
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
