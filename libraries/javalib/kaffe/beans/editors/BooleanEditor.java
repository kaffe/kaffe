package kaffe.beans.editors;

import java.beans.PropertyEditorSupport;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class BooleanEditor
  extends PropertyEditorSupport
{
public String getAsText()
	{
	return (getValue().toString());
}

public String getJavaInitializationString()
	{
	return (getValue().toString());
}

public void setAsText(String text) throws IllegalArgumentException
{
	if (text.equals("true")) {
		setValue(new Boolean(true));
	}
	else if (text.equals("false")) {
		setValue(new Boolean(false));
	}
	else {
		throw new IllegalArgumentException(text);
	}
}
}
