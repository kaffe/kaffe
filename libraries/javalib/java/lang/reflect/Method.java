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

public class Method implements Member {

	private Class clazz;
	private int slot;
	private String name;
	private Class returnType;
	private Class[] parameterTypes;
	private Class[] exceptionTypes;

public boolean equals(Object obj) {
	// Catch the simple case wher they're really the same
	if ((Object)this == obj) {
		return (true);
	}

	Method mobj;
	try {
		mobj = (Method)obj;
	}
	catch (ClassCastException _) {
		return (false);
	}

	if (clazz != mobj.clazz) {
		return (false);
	}
	if (parameterTypes.length != mobj.parameterTypes.length) {
		return (false);
	}
	for (int i = 0; i < parameterTypes.length; i++) {
		if (parameterTypes[i] != mobj.parameterTypes[i]) {
			return (false);
		}
	}
	return (true);
}

public Class getDeclaringClass() {
	return (clazz);
}

public Class[] getExceptionTypes() {
	return (exceptionTypes);
}

native public int getModifiers();

public String getName() {
	return (name);
}

public Class[] getParameterTypes() {
	return (parameterTypes);
}

public Class getReturnType() {
	return (returnType);
}

public int hashCode() {
	return (clazz.getName().hashCode() ^ name.hashCode());
}

native public Object invoke(Object obj, Object args[]) throws IllegalAccessException, IllegalArgumentException, InvocationTargetException;

public String toString() {
	StringBuffer str = new StringBuffer();
	int mod = getModifiers();

	if (Modifier.isPublic(mod)) {
		str.append("public ");
	}
	else if (Modifier.isPrivate(mod)) {
		str.append("private ");
	}
	else if (Modifier.isProtected(mod)) {
		str.append("protected ");
	}

	if (Modifier.isAbstract(mod)) {
		str.append("abstract ");
	}
	if (Modifier.isStatic(mod)) {
		str.append("static ");
	}
	if (Modifier.isFinal(mod)) {
		str.append("final ");
	}
	if (Modifier.isSynchronized(mod)) {
		str.append("synchronized ");
	}
	if (Modifier.isNative(mod)) {
		str.append("native ");
	}

	// Return type
	str.append(fullClassName(returnType));
	str.append(" ");

	// Class name
	str.append(clazz.getName());
	str.append(".");

	// Method name
	str.append(name);
	str.append("(");

	// Signature
	for (int i = 0; i < parameterTypes.length; i++) {
		str.append(fullClassName(parameterTypes[i]));
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

	return (str.toString());
}

private String fullClassName(Class cls) {
        StringBuffer str = new StringBuffer();

        for (int count = 0;; count++) {
                if (!cls.isArray()) {
                        str.append(cls.getName());
                        for (; count > 0; count--) {
                                str.append("[]");
                        }
                        return (str.toString());
                }
                cls = cls.getComponentType();
        }
}


}
