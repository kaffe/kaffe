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
public class StringEditor
  extends PropertyEditorSupport
{
public String getAsText()
	{
	return ((String)getValue());
}

public String getJavaInitializationString()
	{
	return ((String)getValue());
}

public void setAsText(String text)
	{
	setValue(text);
}
}
