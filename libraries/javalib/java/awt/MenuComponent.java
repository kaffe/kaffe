/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */


package java.awt;


abstract public class MenuComponent
{
	Font fnt;
	String name;
	MenuContainer parent;
	FontMetrics fm;
	static Font defFnt = Defaults.MenuFont;

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

protected String paramString() {
	return name;
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
