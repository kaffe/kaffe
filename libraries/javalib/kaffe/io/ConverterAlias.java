/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.io;
import java.util.Hashtable;

/**
 * Some encodings can be accessed under different names.
 * This class allows us to find an alternate encoding which we support.
 *
 * @author Godmar Back <gback@cs.utah.edu>
 */
public class ConverterAlias {
    private static final Hashtable alias = new Hashtable();
    private static final Hashtable useIconv = new Hashtable();

    // All aliases should be upper case
    static {
	alias.put("DEFAULT",		"Default");

	alias.put("ISO_8859-1:1987",	"8859_1");
	alias.put("ISO-8859-1", 	"8859_1");
	alias.put("ISO_8859_1",		"8859_1");
	alias.put("ISO 8859-1",		"8859_1");
	alias.put("ISO 8859_1",		"8859_1");
	alias.put("ISO_8859-1",		"8859_1");
	alias.put("ISO8859_1",		"8859_1");
	alias.put("ISO-IR-100",		"8859_1");
	alias.put("LATIN1",	 	"8859_1");
	alias.put("L1",			"8859_1");
	alias.put("IBM819",	 	"8859_1");
	alias.put("CP819",	 	"8859_1");
	alias.put("CSISOLATIN1",	"8859_1");
	useIconv.put("8859_1", "ISO-8859-1");

	alias.put("ISO_8859-2:1987",	"8859_2");
	alias.put("ISO-IR-101",		"8859_2");
	alias.put("ISO_8859-2",		"8859_2");
	alias.put("ISO-8859-2",		"8859_2");
	alias.put("LATIN2",		"8859_2");
	alias.put("L2",                 "8859_2");
	alias.put("CSISOLATIN2",	"8859_2");
	useIconv.put("8859_2", "ISO-8859-2");

	alias.put("ISO_8859-3:1988",	"8859_3");
	alias.put("ISO-IR-109",		"8859_3");
	alias.put("ISO_8859-3",		"8859_3");
	alias.put("ISO-8859-3",		"8859_3");
	alias.put("LATIN3",		"8859_3");
	alias.put("L3",                 "8859_3");
	alias.put("CSISOLATIN3",	"8859_3");
	useIconv.put("8859_3", "ISO-8859-3");

	alias.put("ISO_8859-4:1988",	"8859_4");
	alias.put("ISO-IR-110",		"8859_4");
	alias.put("ISO_8859-4",		"8859_4");
	alias.put("ISO-8859-4",		"8859_4");
	alias.put("LATIN4",		"8859_4");
	alias.put("L4",		        "8859_4");
	alias.put("CSISOLATIN4",	"8859_4");
	useIconv.put("8859_4", "ISO-8859-4");

	alias.put("ISO_8859-5:1988",	"8859_5");
	alias.put("ISO-IR-144",		"8859_5");
	alias.put("ISO_8859-5",		"8859_5");
	alias.put("ISO-8859-5",		"8859_5");
	alias.put("CYRILLIC",		"8859_5");
	alias.put("CSISOLATINCYRILLIC",	"8859_5");
	useIconv.put("8859_5", "ISO-8859-5");

	alias.put("ISO_8859-6:1987",	"8859_6");
	alias.put("ISO-IR-127",		"8859_6");
	alias.put("ISO_8859-6",		"8859_6");
	alias.put("ISO-8859-6",		"8859_6");
	alias.put("ECMA-114",		"8859_6");
	alias.put("ASMO-708",		"8859_6");
	alias.put("ARABIC",		"8859_6");
	alias.put("CSISOLATINARABIC",	"8859_6");
	useIconv.put("8859_6", "ISO-8859-6");

	alias.put("ISO_8859-7:1987",	"8859_7");
	alias.put("ISO-IR-126",		"8859_7");
	alias.put("ISO_8859-7",		"8859_7");
	alias.put("ISO-8859-7",		"8859_7");
	alias.put("ELOT_928",		"8859_7");
	alias.put("ECMA-118",		"8859_7");
	alias.put("GREEK",		"8859_7");
	alias.put("GREEK8",		"8859_7");
	alias.put("CSISOLATINGREEK",	"8859_7");
	useIconv.put("8859_7", "ISO-8859-7");

	alias.put("ISO_8859-8:1988",	"8859_8");
	alias.put("ISO-IR-138",		"8859_8");
	alias.put("ISO_8859-8",		"8859_8");
	alias.put("ISO-8859-8",		"8859_8");
	alias.put("HEBREW",		"8859_8");
	alias.put("CSISOLATINHEBREW",	"8859_8");
	useIconv.put("8859_8", "ISO-8859-8");

	alias.put("ISO_8859-9:1989",	"8859_9");
	alias.put("ISO-IR-148",		"8859_9");
	alias.put("ISO_8859-9",		"8859_9");
	alias.put("ISO-8859-9",		"8859_9");
	alias.put("LATIN5",		"8859_9");
	alias.put("L5",		        "8859_9");
	alias.put("CSISOLATIN5",	"8859_9");
	useIconv.put("8859_9", "ISO-8859-9");

	alias.put("EBCDIC",		"CP1046");
	useIconv.put("CP1046", "Y");

	alias.put("UTF-8",		"UTF8");
	useIconv.put("UTF8", "UTF-8");

	alias.put("KOI8-R",		"KOI8_R");
	alias.put("CSKOI8R",		"KOI8_R");
	useIconv.put("KOI8_R", "KOI8-R");

	alias.put("EUCJP",              "EUC-JP");
	alias.put("EUC_JP",             "EUC-JP");
	alias.put("EUCJIS",             "EUC-JP");
	alias.put("CSEUCPKDFMTJAPANESE","EUC-JP");
	useIconv.put("EUC-JP", "EUC-JP");

	alias.put("US-ASCII",		"ASCII");
	alias.put("ANSI_X3.4-1968",	"ASCII");
	alias.put("ISO-IR-6",	        "ASCII");
	alias.put("ANSI_X3.4-1986",	"ASCII");
	alias.put("ISO_646.IRV:1991",	"ASCII");
	alias.put("ISO646-US",	        "ASCII");
	alias.put("US",	                "ASCII");
	alias.put("IBM367",             "ASCII");
	alias.put("CP367",	        "ASCII");
	alias.put("CPASCII",	        "ASCII");
	useIconv.put("ASCII", "ASCII");

	alias.put("UNICODEBIGUNMARKED",	"UTF-16BE");
	useIconv.put("UTF-16BE", "UTF-16BE");
	alias.put("UNICODELITTLEUNMARKED",	"UTF-16LE");
	useIconv.put("UTF-16LE", "UTF-16LE");

	/* add more here */
    }

    /**
     * Try to look up an alternate encoding for a given encoding name.
     *
     * @param name of the encoding for which an alias is sought.
     * @return alias if found, name if not.
     */
    static String alias(String name) {
	name = name.toUpperCase();
	String alternate = (String)alias.get(name);
	return alternate != null ? alternate : name;
    }

    /**
     * Check whether iconv should be used for the conversion.
     *
     * @param name of the encoding.
     * @return true if iconv should be used for the conversion,
     * false otherwise.
     */
    static boolean shouldUseIconv(String name) {
	name = name.toUpperCase();
	Object obj = useIconv.get(name);
	return obj != null ? true : false;
    }

    /**
     * Returns encoding name to be used for calling iconv.
     *
     * @param name of the encoding.
     * @return Encoding name to be used for calling iconv.
     */
    static String iconvAlias(String name) {
	String iconvEnc = (String)(useIconv.get(name.toUpperCase()));
	return iconvEnc == null ? name : iconvEnc;
    }
}
