package java.awt.event;

import java.lang.String;
import java.awt.AWTEvent;

/**
 * class TextEvent - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
 */
public class TextEvent
  extends AWTEvent
{
	final public static int TEXT_FIRST = 900;
	final public static int TEXT_LAST = TEXT_FIRST;
	final public static int TEXT_VALUE_CHANGED = TEXT_FIRST;

public TextEvent ( Object src, int evtId ) {
	super( src, evtId);
}

protected void dispatch () {
}

public String paramString() {
	return (id == TEXT_VALUE_CHANGED) ? "TEXT_VALUE_CHANGED" : "TEXT: " + id;
}
}
