package java.awt;

import kaffe.util.Ptr;

/**
 *
 * Copyright (c) 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 */
public class Panel
  extends NativeContainer
{
	final private static long serialVersionUID = -2728009084054400034L;
	private static LayoutManager defaultLayout = new FlowLayout();

public Panel() {
	this( defaultLayout );
}

public Panel( LayoutManager layout) {
	// Panels usually get their own update events, not being updated
	// sync within their parents
	flags |= IS_ASYNC_UPDATED | IS_BG_COLORED;

	bgClr = Defaults.WndBackground;
	setLayout( layout);
}

public void addNotify() {
	if ( nativeData == null ) {

		if ( Toolkit.switchToCreateThread( this, WMEvent.WM_CREATE) )
			return;

		nativeData = Toolkit.widgetCreateWidget( getParentData() );
		//native widgets are created visible
		if ( (flags & IS_VISIBLE) == 0 ) {
			Toolkit.cmnSetVisible( nativeData, false);
		}
		super.addNotify();
	}
}

public Graphics getGraphics () {
	Graphics g = super.getGraphics();
	if ( g != null )
		g.setTarget( this);
	return g;
}

public boolean isFocusTraversable () {
	// for some obscure reason, Panels are not focusTraversable by default
	return false;
}
}
