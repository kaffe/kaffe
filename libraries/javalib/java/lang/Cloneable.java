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

public interface Cloneable {
	/**
	 * This field is checked by the VM to detect whether it loads
	 * the correct java.lang.Cloneable class.
	 *
	 * This is our sanity check to guard against people using the
	 * wrong jar file.
	 */
        final public static int KAFFE_VERSION = 106;
};
