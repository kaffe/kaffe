package java.beans;

import java.awt.Component;
import java.awt.Graphics;
import java.awt.Rectangle;
import java.util.Enumeration;
import java.util.Vector;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class PropertyEditorSupport
  implements PropertyEditor
{
	private Vector listen;
	private Object source;
	private Object value;

protected PropertyEditorSupport()
	{
	listen = new Vector();
	value = null;
	source = this;
}

protected PropertyEditorSupport(Object src)
	{
	listen = new Vector();
	value = null;
	source = src;
}

public synchronized void addPropertyChangeListener(PropertyChangeListener listener)
	{
	listen.addElement(listener);
}

public void firePropertyChange()
	{
	PropertyChangeEvent evt = new PropertyChangeEvent(source, null, null, null);

	Enumeration e = ((Vector)listen.clone()).elements();
	while (e.hasMoreElements()) {
		((PropertyChangeListener)e.nextElement()).propertyChange(evt);
	}
}

public String getAsText()
	{
	try {
		return ((String)value);
	}
	catch (ClassCastException _) {
		return (null);
	}
}

public Component getCustomEditor()
	{
	return (null);
}

public String getJavaInitializationString()
	{
	return (null);
}

public String[] getTags()
	{
	return (null);
}

public Object getValue()
	{
	return (value);
}

public boolean isPaintable()
	{
	return (false);
}

public void paintValue(Graphics gfx, Rectangle box)
	{
	// By default we're not paintable so we do nothing here.
}

public synchronized void removePropertyChangeListener(PropertyChangeListener listener)
	{
	listen.removeElement(listener);
}

public void setAsText(String text) throws IllegalArgumentException
{
	if (value == null || value instanceof String) {
		setValue(text);
	}
	else {
		throw new IllegalArgumentException();
	}
}

public void setValue(Object value)
	{
	if (getValue() != value) {
		this.value = value;
		firePropertyChange();
	}
}

public boolean supportsCustomEditor()
	{
	return (false);
}
}
