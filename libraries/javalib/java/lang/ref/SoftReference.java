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

public class SoftReference extends Reference {

public SoftReference(Object obj) {
	this(obj, null);
}

public SoftReference(Object obj, ReferenceQueue que) {
	super(obj, que);
	type = SOFT;
}

public Object get() {
	return (super.get());
}

}
