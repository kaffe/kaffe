package java.beans;

import java.awt.Component;
import java.awt.Graphics;
import java.awt.Rectangle;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public interface PropertyEditor
{
abstract public void addPropertyChangeListener(PropertyChangeListener listener);

abstract public String getAsText();

abstract public Component getCustomEditor();

abstract public String getJavaInitializationString();

abstract public String[] getTags();

abstract public Object getValue();

abstract public boolean isPaintable();

abstract public void paintValue(Graphics gfx, Rectangle box);

abstract public void removePropertyChangeListener(PropertyChangeListener listener);

abstract public void setAsText(String text) throws IllegalArgumentException;

abstract public void setValue(Object value);

abstract public boolean supportsCustomEditor();
}
