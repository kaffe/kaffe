/*
 * Java core library component.
 *
 * Copyright (c) 2001
 *      Andrew Taylor.  All rights reserved.
 * Copyright (c) 2001
 *      Edouard G. Parmelan.  All rights reserverd.
 * Copyright (c) 2001
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Author: Andrew Taylor <andrew.taylor@montage.ca>
 * Modified by Edouard G. Parmelan to support jdk1.3 security behavior.
 * Checked Spec: JDK 1.3
 */

package java.lang.reflect;

import java.lang.SecurityException;
import java.lang.Class;

public class AccessibleObject
{
    static final private java.security.Permission suppressAccessChecks =
	new ReflectPermission("suppressAccessChecks");

    private boolean accessible;

    protected AccessibleObject() {
    }

    public boolean isAccessible() {
	return accessible;
    }

    public void setAccessible(boolean accessible)
	throws SecurityException
    {
	SecurityManager sm = System.getSecurityManager();
	if (sm != null)
	    sm.checkPermission (suppressAccessChecks);
	setAccessible0 (this, accessible);
    }

    public static void setAccessible(AccessibleObject[] objs, boolean accessible)
	throws SecurityException
    {
	SecurityManager sm = System.getSecurityManager();
	if (sm != null)
	    sm.checkPermission (suppressAccessChecks);
	for (int i = 0; i < objs.length; i++)
	    setAccessible0(objs[i], accessible);
    }

    private static void setAccessible0(AccessibleObject obj, boolean accessible)
	throws SecurityException
    {
	if (accessible == true && obj instanceof Constructor) {
	    // JKD1.3 enforce check to not changes java.lang.Class
	    // constructors
	    if (((Constructor)obj).getDeclaringClass() == Class.class) {
		throw new SecurityException ("Can't change accessibility of java.lang.Class constructor");
	    }
	}
	obj.accessible = accessible;
    }
}
