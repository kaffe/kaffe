package java.awt;

import java.awt.peer.MenuComponentPeer;

/**
 * Copyright (c) 1998
 *    Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 */
abstract public class MenuComponent implements java.io.Serializable
{
	/* XXX implement serial form! */
	Font fnt;
	String name;
	MenuContainer parent;
	FontMetrics fm;
	static Font defFnt = Defaults.MenuFont;
	final static int IS_ADD_NOTIFIED = Component.IS_ADD_NOTIFIED;
	final static int IS_OLD_EVENT = Component.IS_OLD_EVENT;
	int flags;
	Component owner;
	private static final long serialVersionUID = -4536902356223894379L;

public MenuComponent () {
	setFont( defFnt);
}

final public void dispatchEvent( AWTEvent e) {
}

ClassProperties getClassProperties () {
	return ClassAnalyzer.analyzeProcessEvent( getClass(), false);
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
		else if ( owner != null ) {
			ret = owner.postEvent( evt);
		}

		evt.recycle();
	}
	
	return ret;
}

protected void processEvent(AWTEvent evt) {
        return;
}

protected void propagateOldEvents ( boolean isOldEventClient ) {
	if ( isOldEventClient )
		flags |= IS_OLD_EVENT;
	else {
		flags &= ~IS_OLD_EVENT;
	
		// we're not set yet - this might be a subclassed MenuComponent with a
		// resolved postEvent (call it a zombie)
		ClassProperties props = ClassAnalyzer.analyzePostEvent( getClass());
		if ( props.useOldEvents )
			flags |= IS_OLD_EVENT;
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
	return getClass().getName() + '[' + paramString() + ']';
}
}
