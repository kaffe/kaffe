
/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.text;

import java.io.File;
import java.io.Serializable;
import java.util.Locale;
import java.util.ResourceBundle;

public abstract class Format implements Serializable, Cloneable {
/*
 * XXX implement serial form!
 */

private static final String RESOURCEBASE = "kaffe.text.";

public Format() {
	// Does nothing
}

public Object clone() {
	try {
		return (super.clone());
	}
	catch (CloneNotSupportedException _) {
		return (null);
	}
}

public final String format(Object obj) {
	return (format(obj, new StringBuffer(), new FieldPosition(0)).toString());
}

public abstract StringBuffer format(Object obj, StringBuffer buf, FieldPosition pos);

public abstract Object parseObject(String source, ParsePosition status);

public Object parseObject(String source) throws ParseException {
	ParsePosition pp = new ParsePosition(0);
	Object obj = parseObject(source, pp);
	if (obj == null || pp.getIndex() == 0)
		throw new ParseException("parse failed", pp.getErrorIndex());
	return obj;
}

static ResourceBundle getResources(String name, Locale loc) {
	return (ResourceBundle.getBundle(RESOURCEBASE + name + ".locale", loc));
}
}
