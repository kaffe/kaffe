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

import java.lang.String;
import java.io.Serializable;
import java.util.Locale;
import java.util.ResourceBundle;

public abstract class Collator implements Cloneable, Serializable {

public final static int NO_DECOMPOSITION = 0;
public final static int CANONICAL_DECOMPOSITION = 1;
public final static int FULL_DECOMPOSITION = 2;
public final static int IDENTICAL = 3;
public final static int PRIMARY = 0;
public final static int SECONDARY = 1;
public final static int TERTIARY = 2;

private int mode;
private int strength;

protected Collator() {
	mode = NO_DECOMPOSITION;
	strength = PRIMARY;
}

public Object clone() {
	try {
		return (super.clone());
	}
	catch (CloneNotSupportedException _) {
		return (null);
	}
}

public abstract int compare(String src, String target);

public boolean equals(Object obj) {
	try {
		Collator other = (Collator)obj;
		if (mode == other.mode && strength == other.strength) {
			return (true);
		}
	}
	catch (ClassCastException _) {
	}
	return (false);
}

public boolean equals(String src, String target) {
	return (src.equals(target));
}

public static synchronized Locale[] getAvailableLocales() {
        return (Format.getAvailableLocales("collator"));
}

public abstract CollationKey getCollationKey(String src);

public synchronized int getDecomposition() {
	return (mode);
}

public static synchronized Collator getInstance() {
	return (getInstance(Locale.getDefault()));
}

public static synchronized Collator getInstance(Locale loc) {
        ResourceBundle bundle = Format.getResources("collator", loc);
        return (new RuleBasedCollator(""));
}

public synchronized int getStrength() {
	return (strength);
}

public abstract int hashCode();

public synchronized void setDecomposition(int mode) {
	this.mode = mode;
}

public synchronized void setStrength(int strength) {
	this.strength = strength;
}

}
