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
import kaffe.util.NotImplemented;

public class RuleBasedCollator extends Collator {

public RuleBasedCollator(String rules) throws ParseException {
	throw new kaffe.util.NotImplemented(RuleBasedCollator.class.getName() + ".<init>(String)");
}

public Object clone() {
	return (super.clone());
}

public int compare(String src, String target) {
	/*
 	 * XXX - this provide simple comparisons before we don't implement
	 * this yet.
	 */
	return (src.compareTo(target));
}

public boolean equals(Object obj) {
	// Do the simple stuff first.
	if (obj == (Object)this) {
		return (true);
	}
	if (!(obj instanceof RuleBasedCollator)) {
		return (false);
	}
	RuleBasedCollator other = (RuleBasedCollator)obj;

	// Now we have to do the hard stuff ...

	return (false);
}

public CollationElementIterator getCollationElementIterator(String src) {
	throw new kaffe.util.NotImplemented(RuleBasedCollator.class.getName() + ".getCollationElementIterator(String)");
}

public CollationKey getCollationKey(String src) {
	throw new kaffe.util.NotImplemented(RuleBasedCollator.class.getName() + ".getCollationKey(String)");
}

public String getRules() {
	throw new kaffe.util.NotImplemented(RuleBasedCollator.class.getName() + ".getRules()");
}

public int hashCode() {
	return (System.identityHashCode(this));
}

}
