/**
 * Copyright (c) 1998
 *    Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 */

package java.awt;

import java.awt.peer.MenuComponentPeer;

abstract public class MenuComponent
{
	Font fnt;
	String name;
	MenuContainer parent;
	FontMetrics fm;
	MenuComponentPeer peer;
	static Font defFnt = Defaults.MenuFont;
	BarMenu bMenu;
	protected boolean oldEvents;

public MenuComponent () {
	setFont( defFnt);
}

final public void dispatchEvent( AWTEvent e) {
}

public Font getFont() {
	return fnt;
}

public String getName() {
	return name;
}

public MenuContainer getParent() {
	return parent;
}

/**
 * @deprecated
 */
public MenuComponentPeer getPeer() {
	return (null);
}

protected String paramString() {
	return name;
}

/**
 * @deprecated
 */
public boolean postEvent ( Event evt ) {
	boolean ret = false;

	if ( evt != null ) {
		if (parent != null) {
			ret = parent.postEvent( evt);
		}
		else if ( bMenu != null ) {
			ret = bMenu.postEvent( evt);
    }

		evt.recycle();
	}
	
	return ret;
}

protected void propagateOldEvents ( boolean isOldEventClient ) {
	oldEvents = isOldEventClient;
	
	// we're not set yet - this might be a subclassed MenuComponent with a
	// resolved postEvent (call it a zombie)
	if ( !oldEvents ) {
		ClassProperties props = ClassAnalyzer.analyzePostEvent( getClass());
		if ( props.useOldEvents )
			oldEvents = true;
	}
}

public void removeNotify() {
}

public void setFont( Font f) {
	fnt = f;
	fm = Toolkit.getDefaultToolkit().getFontMetrics( fnt);
}

public void setName( String name) {
	this.name = name;
}

public String toString() {
	return paramString();
}
}
