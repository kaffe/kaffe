/*
 * Java core library component.
 *
 * Copyright (c) 2002
 *      Dalibor Topic.  All rights reserved.
 * Copyright (c) 2002
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Author: Dalibor Topic <robilad@yahoo.com>
 */

package kaffe.lang;

/* this class implements utility methods for packages */
public class PackageHelper {
    public static final String getPackageName(Class clazz) {
	String name = clazz.getName();
	/* skip over array depth */
	int start = name.lastIndexOf('[');
	if (start != -1) {
	    /* Array of Lclass; or Linterface; or Lprimitive; */
	    ++start;
	    if (name.charAt(start) == 'L') {
		/* it's a class or interface */
		++start;
	    }
	}
	else {
	    /* it is a simple class of interface,
	     * or a primitive class.
	     */
	    start = 0;
	}

	int end = name.lastIndexOf('.');
	if (end < 0) {
	    end = 0;
	}

	return name.substring(start, end);
    }
}
