/*
 * Java core library component.
 *
 * Copyright (c) 2001
 *      Edouard G. Parmelan.  All rights reserverd.
 * Copyright (c) 2001
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Author: Edouard G. Parmelan <egp@free.fr>
 * Checked Spec: JDK 1.3 - NotImplemented
 */

package java.lang.reflect;

import java.io.Serializable;

public class Proxy
    implements Serializable
{
    protected InvocationHandler h;

    private Proxy() {
    }

    protected Proxy(InvocationHandler h) {
	this.h = h;
    }

    public static Class getProxyClass(ClassLoader loader, Class[] interfaces)
	throws IllegalArgumentException
    {
	throw new kaffe.util.NotImplemented("java.lang.reflect.Proxy");
    }

    public static Object newProxyInstance(ClassLoader loader, Class[] interfaces, InvocationHandler h)
	throws IllegalArgumentException
    {
	throw new kaffe.util.NotImplemented("java.lang.reflect.Proxy");
    }

    public static boolean isProxyClass(Class cl)
    {
	throw new kaffe.util.NotImplemented("java.lang.reflect.Proxy");
    }

    public static InvocationHandler getInvocationHandler(Object proxy)
	throws IllegalArgumentException
    {
	throw new kaffe.util.NotImplemented("java.lang.reflect.Proxy");
    }
}
