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

import java.lang.ref.Reference;
import java.util.Vector;
import java.lang.InterruptedException;
import java.lang.IllegalArgumentException;

public class ReferenceQueue extends Object {

private Vector que;

public ReferenceQueue() {
	que = new Vector();
}

public Reference poll() {
	synchronized (this) {
		if (!que.isEmpty()) {
			Reference ref = (Reference)que.firstElement();
			que.removeElement(ref);
			return (ref);
		}
		return (null);
	}
}

public Reference remove(long timeout) throws IllegalArgumentException, InterruptedException {
	synchronized (this) {
		for (;;) {
			if (!que.isEmpty()) {
				Reference ref = (Reference)que.firstElement();
				que.removeElement(ref);
				return (ref);
			}
			wait(timeout);
			if (timeout > 0) {
				return (null);
			}
		}
	}
}

public Reference remove() throws InterruptedException {
	return (remove(0));
}

boolean isEnqueued(Reference ref) {
	synchronized (this) {
		if (que.contains(ref)) {
			return (true);
		}
		else {
			return (false);
		}
	}
}

void enqueue(Reference ref) {
	synchronized (this) {
		que.addElement(ref);
		notify();
	}
}

}
