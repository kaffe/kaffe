/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.lang.reflect;

import java.lang.Class;
import java.lang.String;

public class Constructor
  implements Member
{
	private Class clazz;
	private int slot;
	private Class[] parameterTypes;
	private Class[] exceptionTypes;

public boolean equals(Object obj)
	{
	// Catch the simple case where they're really the same
	if ((Object)this == obj) {
		return (true);
	}

	// if obj is null then they are not the same
	if (obj == null) {
		return (false);
	}

	Constructor cobj;
	try {
		cobj = (Constructor)obj;
	}
	catch (ClassCastException _) {
		return (false);
	}

	if (clazz != cobj.clazz) {
		return (false);
	}
	if (parameterTypes.length != cobj.parameterTypes.length) {
		return (false);
	}   
	for (int i = 0; i < parameterTypes.length; i++) {
		if (parameterTypes[i] != cobj.parameterTypes[i]) {
			return (false);
		}
	}
	return (true);
}

public Class getDeclaringClass()
	{
	return (clazz);
}

public Class[] getExceptionTypes()
	{
	return (exceptionTypes);
}

native public int getModifiers();

public String getName()
	{
	return (clazz.getName());
}

public Class[] getParameterTypes()
	{
	return (parameterTypes);
}

public int hashCode()
	{
	return (clazz.hashCode());
}

native public Object newInstance(Object initargs[]) throws InstantiationException, IllegalAccessException, IllegalArgumentException, InvocationTargetException;

public String toString()
	{
	StringBuffer str = new StringBuffer();
	int mod = getModifiers();

	// Modifier
	if (Modifier.isPublic(mod)) {
		str.append("public ");
	}
	else if (Modifier.isPrivate(mod)) {
		str.append("private ");
	}
	else if (Modifier.isProtected(mod)) {
		str.append("protected ");
	}

	// Class name.
	str.append(clazz.getName());
	str.append("(");

	// Signature
	for (int i = 0; i < parameterTypes.length; i++) {
		str.append(Method.getPrettyName(parameterTypes[i]));
		if (i+1 < parameterTypes.length) {
			str.append(",");
		}
	}
	str.append(")");
	
        if (exceptionTypes.length > 0) {
	        str.append(" throws ");
                for (int i = 0; i < exceptionTypes.length; i++) {
                        str.append(exceptionTypes[i].getName());
                        if (i+1 < exceptionTypes.length) {
			        str.append(",");
                        }
                }
        }

	return (new String(str));
	}
}
