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

public class WeakReference extends Reference {

public WeakReference(Object obj) {
	this(obj, null);
}

public WeakReference(Object obj, ReferenceQueue que) {
	super(obj, que);
	type = WEAK;
}

}
