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

    // All aliases should be upper case
    static {
	alias.put("DEFAULT",		"Default");
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
	alias.put("ISO-8859-2",		"8859_2");
	alias.put("ISO-8859-3",		"8859_3");
	alias.put("ISO-8859-4",		"8859_4");
	alias.put("ISO-8859-5",		"8859_5");
	alias.put("ISO-8859-6",		"8859_6");
	alias.put("ISO-8859-7",		"8859_7");
	alias.put("ISO-8859-8",		"8859_8");
	alias.put("ISO-8859-9",		"8859_9");
	alias.put("EBCDIC",		"CP1046");
	alias.put("UTF-8",		"UTF8");
	alias.put("KOI8-R",		"KOI8_R");
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
}
