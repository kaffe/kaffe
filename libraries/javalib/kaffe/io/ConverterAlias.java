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
    private static Hashtable alias = new Hashtable();

    static {
	alias.put("ISO-8859-1", 	"8859_1");
	/* add more here */
    }

    /**
     * Try to look up an alternate encoding for a given encoding name.
     *
     * @param name of the encoding for which an alias is sought.
     * @return alias if found, name if not.
     */
    static String alias(String name) {
	String alternate = (String)alias.get(name);
	if (alternate == null) {
	    return (name);
	} else {
	    return (alternate);
	}
    }
}
