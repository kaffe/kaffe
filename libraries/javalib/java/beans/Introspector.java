package java.beans;

import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.TooManyListenersException;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class Introspector {

private static String[] beansearch = { "kaffe.beans.infos" };

private Introspector() {}
    
public static String decapitalize(String name) {
	if (name.length() > 2 && Character.isUpperCase(name.charAt(0)) && Character.isUpperCase(name.charAt(1))) {
		return (name);
	}
	else if (name.length() > 0) {        /* empty string */
		return (Character.toLowerCase(name.charAt(0)) + name.substring(1));
	}
	else {
		return (name);
	}
}

public static BeanInfo getBeanInfo(Class beanClass) throws IntrospectionException {
	return (getBeanInfo(beanClass, null));
}

public static BeanInfo getBeanInfo(Class beanClass, Class stopClass) throws IntrospectionException {
	BeanInfo bean = loadBeanInfo(beanClass);

	// Okay, we must now introspect to fill in the blanks.
	if (stopClass != null) {
		stopClass = stopClass.getSuperclass();
	}
	if (stopClass != null && !beanClass.isInstance(stopClass)) {
		throw new IntrospectionException("Illegal stop class");
	}

	BeanDescriptor desc = null;
	PropertyDescriptor[] props = null;
	MethodDescriptor[] meths = null;
	EventSetDescriptor[] events = null;
	int dprops = -1;
	int devents = -1;

	// If we found a bean info, use as much of that as there is.
	if (bean != null) {
		desc = bean.getBeanDescriptor();
		props = bean.getPropertyDescriptors();
		meths = bean.getMethodDescriptors();
		events = bean.getEventSetDescriptors();
		dprops = bean.getDefaultPropertyIndex();
		devents = bean.getDefaultEventIndex();
	}

	// Introspect the rest.
	if (desc == null) {
		desc = new BeanDescriptor(beanClass);
	}
	if (props == null) {
		props = getProperties(beanClass, stopClass);
	}
	if (meths == null) {
		meths = getMethods(beanClass, stopClass);
	}
	if (events == null) {
		events = getListeners(beanClass, stopClass);
	}

	return (new GenericBeanInfo(desc, props, meths, events, devents, dprops, bean));
}

public static String[] getBeanInfoSearchPath() {
	return (beansearch);
}

private static EventSetDescriptor[] getListeners(Class startClass, Class stopClass) throws IntrospectionException {
	Hashtable addMethods = new Hashtable();
	Hashtable removeMethods = new Hashtable();
	Hashtable keys = new Hashtable();
	Hashtable listenerClasses = new Hashtable();

	for (Class cls = startClass; cls != stopClass; cls = cls.getSuperclass()) {
		Method[] meths = cls.getDeclaredMethods();
		for (int i = 0; i < meths.length; i++) {
			if (Modifier.isPublic(meths[i].getModifiers()) && !Modifier.isStatic(meths[i].getModifiers())) {
				String mname = meths[i].getName();
				if (mname.startsWith("add") && mname.endsWith("Listener")) {
					mname = mname.substring(3, mname.length() - 8);
					keys.put(mname, mname);
					/* XXX: Do we need to check that
					 * there's only one parameter?
					 */
					listenerClasses.put(mname, meths[i].getParameterTypes()[0]);
					if (addMethods.get(mname) == null) {
						addMethods.put(mname, meths[i]);
					}
				}
				else if (mname.startsWith("remove") && mname.endsWith("Listener")) {
					mname = mname.substring(6, mname.length() - 8);
					keys.put(mname, mname);
					if (removeMethods.get(mname) == null) {
						removeMethods.put(mname, meths[i]);
					}
				}
			}
		}
	}

	// Now look through add/remove methods and create descriptors.
	EventSetDescriptor props[] = new EventSetDescriptor[keys.size()];
	Enumeration k = keys.elements();
	for (int i = 0; i < props.length; i++) {
		String key = (String)k.nextElement();
		Method add = (Method)addMethods.get(key);
		Method remove = (Method)removeMethods.get(key);
		Class listenerType = (Class)listenerClasses.get(key);
		if (add != null && remove != null) {
			props[i] = new EventSetDescriptor(decapitalize(key), listenerType, new Method[]{}, add, remove);
			Class except[] = add.getExceptionTypes();
			if (except != null) {
				for (int j = 0; j < except.length; j++) {
					try {
						if (except[j].newInstance() instanceof TooManyListenersException) {
							props[i].setUnicast(true);
							break;
						}
					}
					catch (IllegalAccessException _) {
				}
					catch (InstantiationException _) {
					}
				}
			}
		}
	}

	return (props);
}

private static MethodDescriptor[] getMethods(Class startClass, Class stopClass) throws IntrospectionException {
	Hashtable hash = new Hashtable();

	for (Class cls = startClass; cls != stopClass; cls = cls.getSuperclass()) {
		Method[] meths = cls.getDeclaredMethods();
		for (int i = 0; i < meths.length; i++) {

			if (!Modifier.isPublic(meths[i].getModifiers())) {
				continue;
			}

			String mname = meths[i].toString();
			if (hash.get(mname) == null) {
				hash.put(mname, meths[i]);
			}
		}
	}

	MethodDescriptor[] methdesc = new MethodDescriptor[hash.size()];
	Enumeration e = hash.elements();
	for (int i = 0; i < methdesc.length; i++) {
		methdesc[i] = new MethodDescriptor((Method)e.nextElement());
	}

	return (methdesc);
}

private static PropertyDescriptor[] getProperties(Class startClass, Class stopClass) throws IntrospectionException {
	Hashtable setMethods = new Hashtable();
	Hashtable getMethods = new Hashtable();
	Hashtable setIdxMethods = new Hashtable();
	Hashtable getIdxMethods = new Hashtable();
	Hashtable keys = new Hashtable();

	for (Class cls = startClass; cls != stopClass; cls = cls.getSuperclass()) {
		Method[] meths = cls.getDeclaredMethods();
		for (int i = 0; i < meths.length; i++) {

			Method meth = meths[i];

			// We should only consider public non-statics.
			int mod = meth.getModifiers();
			if (!Modifier.isPublic(mod) || Modifier.isStatic(mod)) {
				continue;
			}

			String mname = meth.getName();
			Class[] mclasses = meth.getParameterTypes();

			switch (mclasses.length) {
			case 0:
				if (mname.startsWith("get")
				    && !mname.equals("get")) {
					mname = mname.substring(3);
					keys.put(mname, mname);
					if (getMethods.get(mname) == null) {
						getMethods.put(mname, meth);
					}
				}
				else if (mname.startsWith("is")
				    && !mname.equals("is")) {
					mname = mname.substring(2);
					keys.put(mname, mname);
					if (getMethods.get(mname) == null) {
						getMethods.put(mname, meth);
					}
				}
				break;

			case 1:
				if (mname.startsWith("set")
				    && !mname.equals("set")
				    && meth.getReturnType().equals(Void.TYPE)) {
					mname = mname.substring(3);
					keys.put(mname, mname);
					if (setMethods.get(mname) == null) {
						setMethods.put(mname, meth);
					}
				}
				else if (mname.startsWith("get")
				    && !mname.equals("get")
				    && mclasses[0] == Integer.TYPE) {
					mname = mname.substring(3);
					keys.put(mname, mname);
					if (getIdxMethods.get(mname) == null) {
						getIdxMethods.put(mname, meth);
					}
				}
				break;

			case 2:
				if (mname.startsWith("set")
				    && !mname.equals("set")
				    && meth.getReturnType().equals(Void.TYPE)
				    && mclasses[0] == Integer.TYPE) {
					mname = mname.substring(3);
					keys.put(mname, mname);
					if (setIdxMethods.get(mname) == null) {
						setIdxMethods.put(mname, meth);
					}
				}
				break;
			}

		}
	}


	// Now look through set/get methods and create desciptors.
	// Eliminate any methods that don't make sense.
	PropertyDescriptor props[] = new PropertyDescriptor[keys.size()];

	Enumeration k = keys.elements();
	for (int i = 0; i < props.length; i++) {
		String key = (String)k.nextElement();
		Method set = (Method)setMethods.get(key);
		Method get = (Method)getMethods.get(key);
		Method setidx = (Method)setIdxMethods.get(key);
		Method getidx = (Method)getIdxMethods.get(key);
		if (setidx == null && getidx == null) {
			if (get != null && set != null) {
				if (!set.getReturnType().equals(Void.TYPE))
					set = null;
				else if (!get.getReturnType().equals(
				    set.getParameterTypes()[0]))
					get = null;
			}
			props[i] = new PropertyDescriptor(
			    decapitalize(key), get, set);
		}
		else {
			if (getidx != null && setidx != null) {
				if (!setidx.getReturnType().equals(Void.TYPE))
					setidx = null;
				else if (!getidx.getReturnType().equals(
				    setidx.getParameterTypes()[1]))
					getidx = null;
			}
			props[i] = new IndexedPropertyDescriptor(
			    decapitalize(key), get, set, getidx, setidx);
		}
	}

	return (props);
}

private static BeanInfo loadBeanInfo(Class beanClass) {
	String bname = beanClass.getName();

	// First try to load bean info from package.
	BeanInfo bean = loadNamedBean(bname + "BeanInfo");
	if (bean != null) {
		return (bean);
	}

	// Extract the name of the bean without package information.
	// We make allowances for both '.' and '/' seperators.
	int pos = bname.lastIndexOf('.');
	int spos = bname.lastIndexOf('/');
	if (spos > pos) {
		pos = spos;
	}
	bname = bname.substring(pos+1);

	// Next try the search paths
	for (int i = 0; i < beansearch.length; i++) {
		bean = loadNamedBean(beansearch[i] + "." + bname + "BeanInfo");
		if (bean != null) {
			return (bean);
		}
	}

	return (null);
}

private static BeanInfo loadNamedBean(String bname) {
	try {
		return ((BeanInfo)Class.forName(bname).newInstance());
	}
	catch (ClassNotFoundException _) {
	}
	catch (ClassCastException _) {
	}
	catch (IllegalAccessException _) {
	}
	catch (InstantiationException _) {
	}
	return (null);
}

public static void setBeanInfoSearchPath(String path[]) {
	beansearch = path;
}

}
