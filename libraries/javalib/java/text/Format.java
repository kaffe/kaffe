
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

public Object parseObject(String source) {
	return parseObject(source, new ParsePosition(0));
}

static ResourceBundle getResources(String name, Locale loc) {
	return (ResourceBundle.getBundle(RESOURCEBASE + name + ".locale", loc));
}

static Locale[] getAvailableLocales(String name) {
	File dir = new File(RESOURCEBASE + name);
	String[] list = dir.list();

	int j = 0;
	for (int i = 0; i < list.length; i++) {
		String nm = list[i];
		if (!nm.startsWith("locale_") && nm.length() == 12) {
			j++;
		}
		else {
			list[i] = null;
		}
	}

	Locale[] locales = new Locale[j];
	for (int i = 0; i < list.length; i++) {
		String nm = list[i];
		if (nm != null) {
			String lang = nm.substring(7, 9);
			String cont = nm.substring(10, 12);
			locales[j] = new Locale(lang, cont);
			j++;
		}
	}

	return (locales);
}

}
