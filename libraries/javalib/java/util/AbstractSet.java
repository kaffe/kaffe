
/*
 * Java core library component.
 *
 * Copyright (c) 1999
 *	Archie L. Cobbs.  All rights reserved.
 * Copyright (c) 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Author: Archie L. Cobbs <archie@whistle.com>
 */

package java.util;

public abstract class AbstractSet extends AbstractCollection implements Set {

	protected AbstractSet() {
	}

	public boolean equals(Object o) {
		if (o == this) {
			return true;
		}
		if (!(o instanceof Set)) {
			return false;
		}
		Set that = (Set)o;
		if (that.size() != this.size()) {
			return false;
		}
		return containsAll(that);
	}

	public int hashCode() {
		int sum = 0;
		for (Iterator i = iterator(); i.hasNext(); ) {
			Object next = i.next();
			sum += next == null ? 0 : next.hashCode();
		}
		return sum;
	}

	public boolean removeAll(Collection c) {
		if (size() < c.size())
			return super.removeAll(c);
		boolean status = false;
		for (Iterator i = c.iterator(); i.hasNext(); ) {
			if (remove(i.next()))
				status = true;
		}
		return status;
	}
}

