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

public class PhantomReference extends Reference {

public PhantomReference(Object obj) {
	this(obj, null);
}

public PhantomReference(Object obj, ReferenceQueue que) {
	super(obj, que);
	type = PHANTOM;
}

public Object get() {
	return (null);
}

}
