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
	private static final String COUNTRY = "Country";
	private static final String LANGUAGE = "Language";

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
	return getDisplayResource(inLocale, COUNTRY);
}

final public String getDisplayLanguage() {
	return getDisplayLanguage(getDefault());
}

public String getDisplayLanguage(Locale inLocale) {
	return getDisplayResource(inLocale, LANGUAGE);
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

private String getDisplayResource(Locale inLocale, String resource) {
	ResourceBundle rb = ResourceBundle.getBundle(RESOURCEBASE + resource, inLocale);

	String name =  rb.getString(toString());

	if (name.equals("")) {
		rb =  ResourceBundle.getBundle(RESOURCEBASE + resource, this);

		name =  rb.getString(toString());
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
	String name = ISO3Support.getISO3Country(this);

	if (name == null) {
		name = getDefault().getISO3Country();
	}

	return name;
}

public String getISO3Language() throws MissingResourceException {
	String name = ISO3Support.getISO3Language(this);

	if (name == null) {
		name = getDefault().getISO3Language();
	}

	return name;
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

	/* A class for ISO3 language and country code support */
	private static class ISO3Support {
		/* a mapping of locales to 3-letter iso language codes */
		private final static Map ISO3_LANGUAGES = new HashMap();
		/* a mapping of locales to 3-letter iso country codes */
		private final static Map ISO3_COUNTRIES = new HashMap();

		static {
			initISO3Countries();
			initISO3Languages();
		}

		public static String getISO3Country(Locale loc) {
			return (String) ISO3_COUNTRIES.get(loc);
		}

		public static String getISO3Language(Locale loc) {
			return (String) ISO3_LANGUAGES.get(loc);
		}

		private static void initISO3Countries() {
			ISO3_COUNTRIES.put(CANADA, "CAN");
			ISO3_COUNTRIES.put(CANADA_FRENCH, "CAN");
			ISO3_COUNTRIES.put(CHINA, "CHN");
			ISO3_COUNTRIES.put(CHINESE, "");
			ISO3_COUNTRIES.put(ENGLISH, "");
			ISO3_COUNTRIES.put(FRANCE, "FRA");
			ISO3_COUNTRIES.put(FRENCH, "");
			ISO3_COUNTRIES.put(GERMAN, "");
			ISO3_COUNTRIES.put(GERMANY, "DEU");
			ISO3_COUNTRIES.put(ITALIAN, "");
			ISO3_COUNTRIES.put(ITALY, "ITA");
			ISO3_COUNTRIES.put(JAPAN, "JPN");
			ISO3_COUNTRIES.put(JAPANESE, "");
			ISO3_COUNTRIES.put(KOREA, "KOR");
			ISO3_COUNTRIES.put(KOREAN, "");
			ISO3_COUNTRIES.put(PRC, "CHN");
			ISO3_COUNTRIES.put(SIMPLIFIED_CHINESE, "CHN");
			ISO3_COUNTRIES.put(TAIWAN, "TWN");
			ISO3_COUNTRIES.put(TRADITIONAL_CHINESE, "TWN");
			ISO3_COUNTRIES.put(UK, "GBR");
			ISO3_COUNTRIES.put(US, "USA");
		}

		private static void initISO3Languages() {
			ISO3_LANGUAGES.put(CANADA, "eng");
			ISO3_LANGUAGES.put(CANADA_FRENCH, "fra");
			ISO3_LANGUAGES.put(CHINA, "zho");
			ISO3_LANGUAGES.put(CHINESE, "zho");
			ISO3_LANGUAGES.put(ENGLISH, "eng");
			ISO3_LANGUAGES.put(FRANCE, "fra");
			ISO3_LANGUAGES.put(FRENCH, "fra");
			ISO3_LANGUAGES.put(GERMAN, "deu");
			ISO3_LANGUAGES.put(GERMANY, "deu");
			ISO3_LANGUAGES.put(ITALIAN, "ita");
			ISO3_LANGUAGES.put(ITALY, "ita");
			ISO3_LANGUAGES.put(JAPAN, "jpn");
			ISO3_LANGUAGES.put(JAPANESE, "jpn");
			ISO3_LANGUAGES.put(KOREA, "kor");
			ISO3_LANGUAGES.put(KOREAN, "kor");
			ISO3_LANGUAGES.put(PRC, "zho");
			ISO3_LANGUAGES.put(SIMPLIFIED_CHINESE, "zho");
			ISO3_LANGUAGES.put(TAIWAN, "zho");
			ISO3_LANGUAGES.put(TRADITIONAL_CHINESE, "zho");
			ISO3_LANGUAGES.put(UK, "eng");
			ISO3_LANGUAGES.put(US, "eng");
		}
	}
}
