/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.util;

import java.io.Serializable;
import java.lang.String;
import java.lang.System;

final public class Locale
  implements Cloneable, Serializable
{
	final public static Locale ENGLISH = new Locale("EN", null);
	final public static Locale FRENCH = new Locale("FR", null);
	final public static Locale GERMAN = new Locale("DE", null);
	final public static Locale ITALIAN = new Locale("IT", null);
	final public static Locale JAPANESE = new Locale("JA", null);
	final public static Locale KOREAN = new Locale("KO", null);
	final public static Locale CHINESE = new Locale("ZH", null);
	final public static Locale SIMPLIFIED_CHINESE = new Locale("ZH", null);
	final public static Locale TRADITIONAL_CHINESE = new Locale("ZH", null);
	final public static Locale FRANCE = new Locale("FR", "FR");
	final public static Locale GERMANY = new Locale("DE", "DE");
	final public static Locale ITALY = new Locale("IT", "IT");
	final public static Locale JAPAN = new Locale("JA", "JP");
	final public static Locale KOREA = new Locale("KO", "KR");
	final public static Locale CHINA = new Locale("ZH", "CN");
	final public static Locale PRC = new Locale("??", "??");
	final public static Locale TAIWAN = new Locale("ZH", "TW");
	final public static Locale UK = new Locale("EN", "GB");
	final public static Locale US = new Locale("EN", "US");
	final public static Locale CANADA = new Locale("EN", "CA");
	final public static Locale CANADA_FRENCH = new Locale("FR", "CA");
	private static Locale defaultLocale;
	private String lang;
	private String cntry;
	private String var;
	private int hashcode;

static {
	String dlang = System.getProperty("user.language", "EN");
	String dcntry = System.getProperty("user.region", "US");
	defaultLocale = new Locale(dlang, dcntry);
}

public Locale(String language, String country) {
	this(language, country, null);
}

public Locale(String language, String country, String variant) {
	lang = language;
	cntry = country;
	var = variant;
	hashcode = super.hashCode();
}

public Object clone() {
	return (new Locale(lang, cntry, var));
}

public boolean equals(Object obj) {
	try {
		Locale lcl = (Locale)obj;
		if ((lang == lcl.lang || lang.equals(lcl.lang)) &&
		    (cntry == lcl.cntry || cntry.equals(lcl.cntry)) &&
		    (var == lcl.var || var.equals(lcl.var))) {
			return (true);
		}
	}
	catch (ClassCastException _) {
	}
	return (false);
}

public String getCountry() {
	return (cntry);
}

public static synchronized Locale getDefault() {
	return (defaultLocale);
}

final public String getDisplayCountry() {
	// We don't handle dislayable names yet
	return (cntry);
}

public String getDisplayCountry(Locale inLocale) {
	// We don't handle dislayable names yet
	return (cntry);
}

final public String getDisplayLanguage() {
	// We don't handle dislayable names yet
	return (lang);
}

public String getDisplayLanguage(Locale inLocale) {
	// We don't handle dislayable names yet
	return (lang);
}

final public String getDisplayName() {
	// We don't handle dislayable names yet
	return (toString());
}

public String getDisplayName(Locale inLocale) {
	// We don't handle dislayable names yet
	return (toString());
}

final public String getDisplayVariant() {
	// We don't handle dislayable names yet
	return (var);
}

public String getDisplayVariant(Locale inLocale) {
	// We don't handle dislayable names yet
	return (var);
}

public String getISO3Country() throws MissingResourceException {
	throw new MissingResourceException("ISO3 not supported", "Locale", "");
}

public String getISO3Language() throws MissingResourceException {
	throw new MissingResourceException("ISO3 not supported", "Locale", "");
}

public String getLanguage() {
	return (lang);
}

public String getVariant() {
	return (var);
}

public synchronized int hashCode() {
	return (hashcode);
}

public static synchronized void setDefault(Locale newLocale) {
	defaultLocale = newLocale;
}

final public String toString() {
	StringBuffer buf = new StringBuffer();

	if (lang != null) {
		buf.append(lang);
		if (cntry != null) {
			buf.append("_");
		}
	}
	if (cntry != null) {
		buf.append(cntry);
		if (var != null) {
			buf.append("_");
		}
	}
	if (var != null) {
		buf.append(var);
	}

	return (new String(buf));
}

}
