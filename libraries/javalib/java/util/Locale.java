package java.util;

import java.io.Serializable;
import java.lang.String;
import java.lang.System;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
final public class Locale
  implements Cloneable, Serializable
{
	final private static long serialVersionUID = 9149081749638150636L;
	final public static Locale ENGLISH = new Locale("en", null);
	final public static Locale FRENCH = new Locale("fr", null);
	final public static Locale GERMAN = new Locale("de", null);
	final public static Locale ITALIAN = new Locale("it", null);
	final public static Locale JAPANESE = new Locale("ja", null);
	final public static Locale KOREAN = new Locale("ko", null);
	final public static Locale CHINESE = new Locale("zh", null);
	final public static Locale SIMPLIFIED_CHINESE = new Locale("zh", "CN");
	final public static Locale TRADITIONAL_CHINESE = new Locale("zh", "TW");
	final public static Locale FRANCE = new Locale("fr", "FR");
	final public static Locale GERMANY = new Locale("de", "DE");
	final public static Locale ITALY = new Locale("it", "IT");
	final public static Locale JAPAN = new Locale("ja", "JP");
	final public static Locale KOREA = new Locale("ko", "KR");
	final public static Locale CHINA = new Locale("zh", "CN");
	final public static Locale PRC = new Locale("zh", "CN");
	final public static Locale TAIWAN = new Locale("zh", "TW");
	final public static Locale UK = new Locale("en", "GB");
	final public static Locale US = new Locale("en", "US");
	final public static Locale CANADA = new Locale("en", "CA");
	final public static Locale CANADA_FRENCH = new Locale("fr", "CA");
	private static Locale defaultLocale;
	private String lang;
	private String cntry;
	private String var;
	private int hashcode;

static {
	String dlang = System.getProperty("user.language", "en");
	String dcntry = System.getProperty("user.region", "US");
	defaultLocale = new Locale(dlang, dcntry);
}

public Locale(String language, String country) {
	this(language, country, "");
}

public Locale(String language, String country, String variant) {
/*
	lang = (language != null) ? language : "";
	cntry = (country != null) ? country : "";
	var = (variant != null) ? variant : "";
*/

	if ( (language != null) && (language.length() > 0) ){
		lang = language;
	}
	if ( (country != null) && (country.length() > 0) ){
		cntry = country;
	}
	if ( (variant != null) && (variant.length() > 0) ){
		var = variant;
	}

	hashcode = super.hashCode();
}

public Object clone() {
	return (new Locale(lang, cntry, var));
}

public boolean equals(Object obj) {
	if (obj instanceof Locale) {
		Locale lcl = (Locale)obj;
		if ((lang == lcl.lang || lang.equals(lcl.lang)) &&
		    (cntry == lcl.cntry || cntry.equals(lcl.cntry)) &&
		    (var == lcl.var || var.equals(lcl.var))) {
			return (true);
		}
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
	boolean hasLang = (lang != null) && (lang.length() > 0);
	boolean hasCntry = (cntry != null) && (cntry.length() > 0);
	boolean hasVar = (var != null) && (var.length() > 0);

	if ( hasLang ) {
		buf.append(lang);
	}
	if ( hasCntry ) {
		buf.append( '_');
		buf.append(cntry);
	}
	if ( hasVar) {
		buf.append( '_');
		buf.append(var);
	}

	return (new String(buf));
}
}
