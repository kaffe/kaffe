/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.lang.ref;

import java.lang.ref.ReferenceQueue;
import kaffe.util.Ptr;

public abstract class Reference extends Object {

final static protected int PHANTOM = 0;
final static protected int SOFT = 1;
final static protected int WEAK = 2;

private Object obj;		// This must be the first field.
private ReferenceQueue que;
private Reference next;
protected int type;

protected Reference(Object obj) {
	this(obj, null);
}

protected Reference(Object obj, ReferenceQueue que) {
	this.obj = obj;
	this.que = que;
}

public Object get() {
	return (obj);
}

public void clear() {
	obj = null;
}

public boolean isEnqueued() {
	if (que == null) {
		return (false);
	}
	return (que.isEnqueued(this));
}

public boolean enqueue() {
	if (que == null || isEnqueued()) {
		return (false);
	}
	que.enqueue(this);
	return (true);
}

/**
 * This method is called by the VM when the referenced object becomes
 * unreachable.
 */
private void dispatch() {
	clear();
	enqueue();
}

}
