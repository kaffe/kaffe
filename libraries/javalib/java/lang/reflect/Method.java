/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998, 2001, 2002
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Checked Spec: JDK 1.3
 */

package java.lang.reflect;


public final class Method
    extends AccessibleObject
    implements Member
{

static private final Object[] nullArgs = new Object[0];
private Class clazz;
private int slot;
private String name;
private Class returnType;
private Class[] parameterTypes;
private Class[] exceptionTypes;

static {
	init0();
}

native private static void init0();

private Method() {
}

/* used by Constructor.newInstance */
Method(Class clazz, int slot, String name, Class returnType,
       Class [] parameterTypes, Class [] exceptionTypes)
{
	this.clazz = clazz;
	this.slot = slot;
	this.name = name;
	this.returnType = returnType;
	this.parameterTypes = parameterTypes;
	this.exceptionTypes = exceptionTypes;
}

public boolean equals(Object obj) {
	// Catch the simple case where they're really the same
	if (this == obj) {
		return (true);
	}
	// if obj is null or not Method, then they are not the same
	if (!(obj instanceof Method)) {
		return (false);
	}

	Method mobj = (Method)obj;

	if (clazz != mobj.clazz ||
	    returnType != mobj.returnType ||
	    parameterTypes.length != mobj.parameterTypes.length ||
	    ! name.equals(mobj.name)) {
		return (false);
	}

	for (int i = parameterTypes.length; i-- > 0; ) {
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

public Object invoke(Object obj, Object args[]) 
	throws IllegalAccessException,
	IllegalArgumentException,
	InvocationTargetException {

	if (args == null) {
		args = nullArgs;
	}

	if (!Modifier.isStatic (getModifiers())) {
		if (obj == null && !"<init>".equals(name)) {
			throw new NullPointerException("Null object pointer");
		}

		if (obj != null && !getDeclaringClass().isInstance(obj)) {
			throw new IllegalArgumentException("Object type doesn't match method's class");
		}
	}

	if (args.length != getParameterTypes().length) {
		throw new IllegalArgumentException("wrong number of arguments");
	}

	/* Process arguments to get them 'correct' */
	for (int i = 0; i < args.length; i++) {
		Class pt = parameterTypes[i];
		Object arg = args[i];
		if (pt.isPrimitive()) { 
			/* Might need fixup */
			if (pt == Boolean.TYPE) {
				if (arg instanceof Boolean) {
				}
				else {
					throw new IllegalArgumentException();
				}
			}
			else if (pt == Byte.TYPE) {
				if (arg instanceof Byte) {
				}
				else {
					throw new IllegalArgumentException();
				}
			}
			else if (pt == Short.TYPE) {
				if (arg instanceof Short) {
				}
				else if (arg instanceof Byte) {
					args[i] = new Short((short)((Byte)arg).byteValue());
				}
				else {
					throw new IllegalArgumentException();
				}
			}
			else if (pt == Character.TYPE) {
				if (arg instanceof Character) {
				}
				else {
					throw new IllegalArgumentException();
				}
			}
			else if (pt == Integer.TYPE) {
				if (arg instanceof Integer) {
				}
				else if (arg instanceof Short) {
					args[i] = new Integer((int)((Short)arg).shortValue());
				}
				else if (arg instanceof Character) {
					args[i] = new Integer((int)((Character)arg).charValue());
				}
				else if (arg instanceof Byte) {
					args[i] = new Integer((int)((Byte)arg).byteValue());
				}
				else {
					throw new IllegalArgumentException();
				}
			}
			else if (pt == Long.TYPE) {
				if (arg instanceof Long) {
				}
				else if (arg instanceof Integer) {
					args[i] = new Long((long)((Integer)arg).intValue());
				}
				else if (arg instanceof Short) {
					args[i] = new Long((long)((Short)arg).shortValue());
				}
				else if (arg instanceof Character) {
					args[i] = new Long((long)((Character)arg).charValue());
				}
				else if (arg instanceof Byte) {
					args[i] = new Long((long)((Byte)arg).byteValue());
				}
				else {
					throw new IllegalArgumentException();
				}
			}
			else if (pt == Float.TYPE) {
				if (arg instanceof Float) {
				}
				else if (arg instanceof Long) {
					args[i] = new Float((float)((Long)arg).longValue());
				}
				else if (arg instanceof Integer) {
					args[i] = new Float((float)((Integer)arg).intValue());
				}
				else if (arg instanceof Short) {
					args[i] = new Float((float)((Short)arg).shortValue());
				}
				else if (arg instanceof Character) {
					args[i] = new Float((float)((Character)arg).charValue());
				}
				else if (arg instanceof Byte) {
					args[i] = new Float((float)((Byte)arg).byteValue());
				}
				else {
					throw new IllegalArgumentException();
				}
			}
			else if (pt == Double.TYPE) {
				if (arg instanceof Double) {
				}
				else if (arg instanceof Float) {
					args[i] = new Double((double)((Float)arg).floatValue());
				}
				else if (arg instanceof Long) {
					args[i] = new Double((double)((Long)arg).longValue());
				}
				else if (arg instanceof Integer) {
					args[i] = new Double((double)((Integer)arg).intValue());
				}
				else if (arg instanceof Short) {
					args[i] = new Double((double)((Short)arg).shortValue());
				}
				else if (arg instanceof Character) {
					args[i] = new Double((double)((Character)arg).charValue());
				}
				else if (arg instanceof Byte) {
					args[i] = new Double((double)((Byte)arg).byteValue());
				}
				else {
					throw new IllegalArgumentException();
				}
			}
			else {
				throw new Error("cannot happen");
			}
		}
		else if (arg!=null && !pt.isAssignableFrom(arg.getClass())) {
			throw new IllegalArgumentException("incompatible argument");
		}
	}

	return (invoke0(obj, args));
}

native Object invoke0(Object obj, Object args[]) throws IllegalAccessException, IllegalArgumentException, InvocationTargetException;

public String toString() {
	StringBuffer str = new StringBuffer();
	int mod = getModifiers();

	// Modifier
	if (mod != 0) {
		str.append(Modifier.toString(mod));
		str.append(' ');
	}

	// Return type
	str.append(getPrettyName(returnType));
	str.append(' ');

	// Class name
	str.append(clazz.getName());
	str.append('.');

	// Method name
	str.append(name);
	str.append('(');

	// Signature
	for (int i = 0; i < parameterTypes.length; i++) {
		str.append(getPrettyName(parameterTypes[i]));
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

static String getPrettyName(Class cls) {
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
