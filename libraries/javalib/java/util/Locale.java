package java.util;

import java.io.Serializable;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
/* TODO:
 * * implement getAvailableLocales.
 * * implement security checks in setDefault.
 * * add more country and language names translations (chinese, korean, japanese).
 */
final public class Locale
  implements Cloneable, Serializable
{
	final private static long serialVersionUID = 9149081749638150636L;
	final public static Locale CANADA = new Locale("en", "CA");
	final public static Locale CANADA_FRENCH = new Locale("fr", "CA");
	final public static Locale CHINA = new Locale("zh", "CN");
	final public static Locale CHINESE = new Locale("zh", null);
	final public static Locale ENGLISH = new Locale("en", null);
	final public static Locale FRANCE = new Locale("fr", "FR");
	final public static Locale FRENCH = new Locale("fr", null);
	final public static Locale GERMAN = new Locale("de", null);
	final public static Locale GERMANY = new Locale("de", "DE");
	final public static Locale ITALY = new Locale("it", "IT");
	final public static Locale ITALIAN = new Locale("it", null);
	final public static Locale JAPAN = new Locale("ja", "JP");
	final public static Locale JAPANESE = new Locale("ja", null);
	final public static Locale KOREA = new Locale("ko", "KR");
	final public static Locale KOREAN = new Locale("ko", null);
        final public static Locale PRC = CHINA;
	final public static Locale SIMPLIFIED_CHINESE = CHINA;
	final public static Locale TAIWAN = new Locale("zh", "TW");
	final public static Locale TRADITIONAL_CHINESE = TAIWAN;
	final public static Locale UK = new Locale("en", "GB");
	final public static Locale US = new Locale("en", "US");

	/* locations of human-readable locale naming data */
	private static final String RESOURCEBASE = "kaffe.util.locale.";
	private static final String COUNTRY = RESOURCEBASE + "Country";
	private static final String COUNTRY_CODES = RESOURCEBASE + "CountryCodes";
	private static final String LANGUAGE = RESOURCEBASE + "Language";
	private static final String LANGUAGE_CODES = RESOURCEBASE + "LanguageCodes";

	private static Locale defaultLocale;
	private String lang;
	private String cntry;
	private String var;
	private int hashcode;

static {
	String dlang = System.getProperty("user.language", "en");
	String dcntry = System.getProperty("user.region", "US");
	String dvar = "";
	int pos = dcntry.indexOf('_');
	if (pos != -1) {
		dvar = dcntry.substring(pos + 1);
		dcntry = dcntry.substring(pos);
	}
	defaultLocale = new Locale(dlang, dcntry, dvar);
}

// Avoid recursion with String.toLowerCase() that use Locale
private static String toLowerCase(String str) {
	char buf[] = str.toCharArray();
	for (int pos=0; pos < buf.length; pos++)
		buf[pos] = Character.toLowerCase(buf[pos]);
	return new String(buf);
}

// Avoid recursion with String.toUpperCase() that use Locale
private static String toUpperCase(String str) {
	char buf[] = str.toCharArray();
	for (int pos=0; pos < buf.length; pos++)
		buf[pos] = Character.toUpperCase(buf[pos]);
	return new String(buf);
}

/** since 1.4 */
public Locale(String language) {
	this(language, "");
}

public Locale(String language, String country) {
	this(language, country, "");
}

public Locale(String language, String country, String variant) {
	lang = (language != null) ? toLowerCase(language) : "";
	cntry = (country != null) ? toUpperCase(country) : "";
	var = (variant != null) ? toUpperCase(variant) : "";

	hashcode = lang.hashCode() ^ cntry.hashCode() ^ var.hashCode();
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
	return getDisplayCountry(getDefault());
}

public String getDisplayCountry(Locale inLocale) {
	return getDisplayResource(inLocale, COUNTRY, getCountry());
}

final public String getDisplayLanguage() {
	return getDisplayLanguage(getDefault());
}

public String getDisplayLanguage(Locale inLocale) {
	return getDisplayResource(inLocale, LANGUAGE, getLanguage());
}

final public String getDisplayName() {
	return getDisplayName(getDefault());
}

public String getDisplayName(Locale inLocale) {
	String country = getDisplayCountry(inLocale);

	if (country.equals("")) {
		return getDisplayLanguage(inLocale);
	}
	else {
		return getDisplayLanguage(inLocale) + " (" + country + ')';
	}
}

/* looks up a value in a given resource for the given locale */
private String getDisplayResource(Locale inLocale, String resource, String value) {
	if (value.equals("")) {
		return "";
	}

	/* Try to get the value for the given locale */
	ResourceBundle rb = ResourceBundle.getBundle(resource, inLocale);

	String name =  rb.getString(value);

	/* If that fails, return the value for this locale */
	if (name.equals("")) {
		rb =  ResourceBundle.getBundle(resource, this);

		name =  rb.getString(value);
	}

	return name;
}

final public String getDisplayVariant() {
	return getDisplayVariant(getDefault());
}

public String getDisplayVariant(Locale inLocale) {
	return (var);
}

public String getISO3Country() throws MissingResourceException {
	return getDefault().getDisplayResource(this,
					       COUNTRY_CODES,
					       getCountry());
}

/** since 1.2 */
public static String[] getISOCountries() {
	return getKeys(ResourceBundle.getBundle(COUNTRY_CODES));
}

/** since 1.2 */
public String getISO3Language() throws MissingResourceException {
	return getDefault().getDisplayResource(this,
					       LANGUAGE_CODES,
					       getLanguage());
}

/** since 1.2 */
public static String[] getISOLanguages() {
	return getKeys(ResourceBundle.getBundle(LANGUAGE_CODES));
}

/* used to extract the keys of a resource bundle as a String [] */
private static String [] getKeys(ResourceBundle rb) {
	return (String []) Collections.list(rb.getKeys()).toArray(new String[0]);
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
	if (newLocale == null) {
		throw new NullPointerException();
	}

	defaultLocale = newLocale;
}

final public String toString() {
	StringBuffer buf = new StringBuffer();
	boolean hasLang = lang.length() > 0;
	boolean hasCntry = cntry.length() > 0;
	boolean hasVar = var.length() > 0;

	if (hasLang) {
		buf.append(lang);
	}
	if (hasCntry) {
		buf.append('_');
		buf.append(cntry);
	}
	else if (hasLang && hasVar) {
		buf.append('_');
	}
	if (hasVar && (hasLang || hasCntry)) {
		buf.append('_');
		buf.append(var);
	}

	return (new String(buf));
}
}
