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
 * Checked Spec: JDK 1.3
 */

package java.lang.reflect;

public interface InvocationHandler
{
    public Object invoke(Object proxy, Method method, Object[] args) throws Throwable;
}
