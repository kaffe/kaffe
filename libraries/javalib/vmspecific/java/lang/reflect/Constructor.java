/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998, 2001
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Checked Spec: JDK 1.3
 */

package java.lang.reflect;


public final class Constructor
  extends AccessibleObject
  implements Member
{
	private Class clazz;
	private int slot;
	private Class[] parameterTypes;
	private Class[] exceptionTypes;

private Constructor() {
}

public boolean equals(Object obj)
	{
	// Catch the simple case where they're really the same
	if (this == obj) {
		return (true);
	}

	// if obj is null or not Constructor then they are not the same
	if (!(obj instanceof Constructor)) {
		return (false);
	}

	Constructor cobj = (Constructor)obj;

	if (clazz != cobj.clazz) {
		return (false);
	}
	if (parameterTypes.length != cobj.parameterTypes.length) {
		return (false);
	}
	for (int i = parameterTypes.length; i-- > 0; ) {
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
	return (clazz.getName().hashCode());
}

public Object newInstance(Object initargs[]) throws InstantiationException, IllegalAccessException, IllegalArgumentException, InvocationTargetException {
	Method meth = new Method(clazz,
				 slot,
				 "<init>",
				 clazz,
				 parameterTypes,
				 exceptionTypes);
	return (meth.invoke(null, initargs));
}

public String toString()
	{
	StringBuffer str = new StringBuffer();
	int mod = getModifiers();

	// Modifier
	if (mod != 0) {
		str.append(Modifier.toString(mod));
		str.append(' ');
	}

	// Class name.
	str.append(clazz.getName());
	str.append('(');

	// Signature
	for (int i = 0; i < parameterTypes.length; i++) {
		str.append(Method.getPrettyName(parameterTypes[i]));
		if (i+1 < parameterTypes.length) {
			str.append(',');
		}
	}
	str.append(')');

        if (exceptionTypes.length > 0) {
	        str.append(" throws ");
                for (int i = 0; i < exceptionTypes.length; i++) {
                        str.append(exceptionTypes[i].getName());
                        if (i+1 < exceptionTypes.length) {
			        str.append(',');
                        }
                }
        }

	return (str.toString());
}
}
