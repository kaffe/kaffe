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
void addPropertyChangeListener(PropertyChangeListener listener);

String getAsText();

Component getCustomEditor();

String getJavaInitializationString();

String[] getTags();

Object getValue();

boolean isPaintable();

void paintValue(Graphics gfx, Rectangle box);

void removePropertyChangeListener(PropertyChangeListener listener);

void setAsText(String text) throws IllegalArgumentException;

void setValue(Object value);

boolean supportsCustomEditor();
}
