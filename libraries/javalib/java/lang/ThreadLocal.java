
/*
 * Java core library component.
 *
 * Copyright (c) 1998, 1999
 *      Archie L. Cobbs.  All rights reserved.
 * Copyright (c) 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Author: Archie L. Cobbs <archie@whistle.com>
 */

package java.lang;

import java.util.HashMap;

public class ThreadLocal {

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
		HashMap map = thread.getThreadLocals();
		synchronized (map) {
			if (!map.containsKey(this)) {
				set(thread, initialValue());
			}
			return map.get(this);
		}
	}

	void set(Thread thread, Object value) {
		HashMap map = thread.getThreadLocals();
		synchronized (map) {
			map.put(this, value);
		}
	}
}

