package com.ms.lang;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Vector;
import java.util.Enumeration;

public class MulticastDelegate extends Delegate {

private Vector list = new Vector();

protected MulticastDelegate(Object o, String m, String s) {
	super(o, m, s);
}

MulticastDelegate(Object o, Method m) {
	super(o, m);
}

public Delegate[] getInvocationList() {
	Delegate[] dels = new Delegate[list.size()];
	int i = 0;
	for (Enumeration e = list.elements(); e.hasMoreElements(); ) {
		dels[i++] = (Delegate)e.nextElement();
	}
	return (dels);
}

public int hashCode() {
	return (super.hashCode());
}

public boolean equals(Object o) {
	if (o == this) {
		return (true);
	}
	throw new kaffe.util.NotImplemented();
}

protected final void invokeHelperMulticast(Object[] args) throws Throwable {
        try {
		for (Enumeration e = list.elements(); e.hasMoreElements(); ) {
			((Delegate)e.nextElement()).dynamicInvoke(args);
		}
        }
        catch (InvocationTargetException e) {
                throw e.getTargetException();
        }

}

Delegate combine(Delegate a) {
	MulticastDelegate mdel = new MulticastDelegate(objTarget, methodTarget);
	for (Enumeration e = list.elements(); e.hasMoreElements(); ) {
		mdel.list.addElement(e.nextElement());
	}
	if (a instanceof MulticastDelegate) {
		for (Enumeration e = ((MulticastDelegate)a).list.elements(); e.hasMoreElements(); ) {
			mdel.list.addElement(e.nextElement());
		}
	}
	else {
		mdel.list.addElement(a);
	}
	return (mdel);
}

Delegate remove(Delegate a) {
	MulticastDelegate mdel = new MulticastDelegate(objTarget, methodTarget);
	for (Enumeration e = list.elements(); e.hasMoreElements(); ) {
		Object o = e.nextElement();
		if (o != a) {
			mdel.list.addElement(o);
		}
	}
	return (mdel);
}

}
