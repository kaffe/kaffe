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

import kaffe.util.NotImplemented;

public class RuleBasedCollator extends Collator {
	private final String rules;

	public RuleBasedCollator(String rules) throws ParseException {
		this.rules = rules;
	}

	public Object clone() {
		return (super.clone());
	}

	public int compare(String src, String target) {
		/*
		 * XXX - this provide simple comparisons because
		 * we don't implement this yet.
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
		RuleBasedCollator that = (RuleBasedCollator)obj;

		// Now we have to do the hard stuff ...
		return this.rules.equals(that.rules);
	}

	public CollationElementIterator getCollationElementIterator(String s) {
		return new CollationElementIterator(s);
	}

	public CollationElementIterator getCollationElementIterator(
			CharacterIterator source) {
		return new CollationElementIterator(source);
	}

	public CollationKey getCollationKey(String src) {
		return new CollationKey(this, src);
	}

	public String getRules() {
		return rules;
	}

	public int hashCode() {
		return (System.identityHashCode(this));
	}

}
