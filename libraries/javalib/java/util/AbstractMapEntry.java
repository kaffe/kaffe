
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

abstract class AbstractMapEntry implements Map.Entry {
	protected final Object key;
	protected Object value;

	AbstractMapEntry(Object key, Object value) {
		this.key = key;
		this.value = value;
	}

	public Object getKey() {
		return key;
	}

	public Object getValue() {
		return value;
	}

	public Object setValue(Object newValue) {
		Object oldValue = value;
		changeValue(newValue);
		value = newValue;
		return oldValue;
	}

	// This should write the new value back through to the underlying Map
	protected abstract void changeValue(Object newValue);

	public boolean equals(Object o) {
		if (!(o instanceof Map.Entry)) {
			return false;
		}
		Map.Entry me = (Map.Entry)o;
		return (this.key == null ? me.getKey() == null
			: this.key.equals(me.getKey()))
		    && (this.value == null ? me.getValue() == null
			: this.value.equals(me.getValue()));
	}

	public int hashCode() {
		return (key == null ? 0 : key.hashCode())
		    ^ (value == null ? 0 : value.hashCode());
	}
}

