package java.awt;

import java.awt.FlowLayout;
import java.awt.LayoutManager;
import java.awt.Toolkit;

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
	setLayout( layout);
}

public void addNotify() {
	if ( nativeData == null ) {
		Toolkit.createNative(this);

		//native widgets are created visible
		if ( (flags & IS_VISIBLE) == 0 ) {
			Toolkit.cmnSetVisible( nativeData, false);
		}
		super.addNotify();
	}
}

void createNative() {
	nativeData = Toolkit.widgetCreateWidget( getParentData() );
}

public boolean isFocusTraversable () {
	// for some obscure reason, Panels are not focusTraversable by default
	return false;
}
}
