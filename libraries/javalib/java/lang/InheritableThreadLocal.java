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

public class InheritableThreadLocal extends ThreadLocal {
	protected Object childValue(Object parentValue) {
		return parentValue;
	}
}

