package com.ms.lang;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import com.ms.lang.MulticastNotSupportedException;

public class Delegate implements Cloneable {

Method methodTarget;
Object objTarget;

static {
	System.loadLibrary("microsoft");
}

protected Delegate(Object o, String m, String s) {
	objTarget = o;
	methodTarget = getMethod0(o, m, s);
}

Delegate(Object o, Method m) {
	objTarget = o;
	methodTarget = m;
}

private static native Method getMethod0(Object o, String name, String sig);

public final Method getMethod() {
	return (methodTarget);
}

public final Object getTarget() {
	return (objTarget);
}

public Delegate[] getInvocationList() {
	return (new Delegate[]{ this });
}

public int hashCode() {
	return (getTarget().hashCode() ^ getMethod().hashCode());
}

public Object clone() {
	try {
		return (super.clone());
	}
	catch (CloneNotSupportedException _) {
		return (null);
	}
}

public boolean equals(Object o) {
	if (o == this) {
		return (true);
	}
	if (o instanceof Delegate) {
		Delegate del = (Delegate)o;
		if (del.getInvocationList().length == getInvocationList().length && del.methodTarget == methodTarget && del.objTarget == objTarget) {
			return (true);
		}
	}
	return (false);
}

public final Object dynamicInvoke(Object[] args) throws IllegalArgumentException, InvocationTargetException {
	try {
		return(methodTarget.invoke(objTarget, args));
	}
	catch (IllegalAccessException _) {
		throw new IllegalArgumentException();
	}
}

protected final Object invokeHelper(Object[] args) throws Throwable {
	try {
		return(dynamicInvoke(args));
	}
	catch (InvocationTargetException e) {
		throw e.getTargetException();
	}
}

Delegate combine(Delegate d) throws MulticastNotSupportedException {
	throw new MulticastNotSupportedException();
}

Delegate remove(Delegate d) {
	if (this == d) {
		return (null);
	}
	return (this);
}

public static final Delegate combine(Delegate a, Delegate b) {
	if (a == null) {
		return (b);
	}
	else if (b == null) {
		return (a);
	}
	else if (a.getClass() != b.getClass()) {
		throw new IllegalArgumentException("cannot combine Delegates of different types");
	}
	else {
		try {
			return (b.combine(a));
		}
		catch (MulticastNotSupportedException _) {
			throw new IllegalArgumentException("multicast Delegates not supported");
		}
	}
}

public static final Delegate combine(Delegate[] dels) {
	Delegate base = dels[0];
	for (int i = 1; i < dels.length; i++) {
		base = combine(base, dels[i]);
	}
	return (base);
}

public static final Delegate remove(Delegate a, Delegate b) {
	if (a == null) {
		return (null);
	}
	else if (b == null) {
		return (a);
	}
	else {
		return (a.remove(b));
	}
}

}
