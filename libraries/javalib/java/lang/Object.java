/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.lang;


public class Object
{
native protected Object clone() throws CloneNotSupportedException;

public boolean equals(Object obj) {
	return (this==obj);  /* Well either its equal or it aint! */
}

protected void finalize() throws Throwable {
	/* Does nothing for Object class */
}

final native public Class  getClass();

native public int    hashCode();

final native public void   notify();

final native public void   notifyAll();

public String toString() {
	return getClass().getName()+'@'+Integer.toHexString(hashCode());
}

final public void wait() throws InterruptedException {
	/* Just wait forever */
	wait(0);
}

final public void wait(long timeout) throws InterruptedException {
	if (Thread.interrupted()) {
		throw new InterruptedException();
	}
	if (timeout < 0) {
		throw new IllegalArgumentException("timeout is negative");
	}
	wait0(timeout);
	if (Thread.interrupted()) {
		throw new InterruptedException();
	}
}

final public void wait(long timeout, int nanos) throws InterruptedException {
	/* Ignore nanos, except avoid clipping a non-zero quantity to zero */
	if (nanos < 0 || nanos > 999999) {
		throw new IllegalArgumentException("nanos out or range");
	}
	if (timeout == 0 && nanos > 0) {
		timeout++;
	}
	wait(timeout);    
}

final native private void wait0(long timeout);
}
