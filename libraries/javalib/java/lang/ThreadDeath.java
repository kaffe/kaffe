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

public class ThreadDeath extends Error {

/* NB: Sun's does not hardwire this, but we must because our computed
 * hash will differ from Sun's because of the package local constructor
 * below.
 */
private static final long serialVersionUID = -4417128565033088268L;

public ThreadDeath() {
}

/**
 * This constructor is for internal VM use only (SignalError)
 */
ThreadDeath(String s) {
	super(s);
}

}
