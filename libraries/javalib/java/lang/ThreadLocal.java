/*
 * Java core library component.
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.lang;

import java.util.Hashtable;

public class ThreadLocal {

	/*
	 * Since you can't store null as the value in a hashtable,
	 * we use this object to represent a value of null.
	 */
	private static final Object NULL_VALUE = new Object();

	protected Object initialValue() {
		return null;
	}

	public Object get() {
		return get(Thread.currentThread());
	}

	public void set(Object value) {
		set(Thread.currentThread(), value);
	}

	Object get(Thread thread) {
		Hashtable h = thread.getThreadLocals();
		if (!h.containsKey(this)) {
			set(thread, initialValue());
		}
		Object value = h.get(this);
		return value == NULL_VALUE ? null : value;
	}

	void set(Thread thread, Object value) {
		thread.getThreadLocals().put(this,
			value == null ? NULL_VALUE : value);
	}
}

