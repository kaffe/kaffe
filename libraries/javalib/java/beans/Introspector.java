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

private static Hashtable cache = new Hashtable();

private static final String ADD = "add";
private static final String GET = "get";
private static final String IS = "is";
private static final String LISTENER = "Listener";
private static final String REMOVE = "remove";
private static final String SET = "set";
private static final int ADD_LENGTH = ADD.length();
private static final int GET_LENGTH = GET.length();
private static final int IS_LENGTH = IS.length();
private static final int LISTENER_LENGTH = LISTENER.length();
private static final int REMOVE_LENGTH = REMOVE.length();
private static final int SET_LENGTH = SET.length();

private static final String BEAN_INFO = "BeanInfo";
private static final String ADD_PROPERTY_CHANGE_LISTENER = ADD + "PropertyChange" + LISTENER;

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

	Object cobj = cache.get(beanClass);
	if (cobj != null) {
		return ((BeanInfo)cobj);
	}

	BeanInfo bean = loadBeanInfo(beanClass);

	// Check stop class is really a superclass of bean class
	if (stopClass != null
	    && (!stopClass.isAssignableFrom(beanClass)
	      || beanClass.isInterface())) {
		throw new IntrospectionException(stopClass.getName()
		    + " not superclass of " + beanClass.getName());
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

	bean = new GenericBeanInfo(desc, props, meths, events, devents, dprops, bean);
	cache.put(beanClass, bean);
	return (bean);
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
			Method method = meths[i];
			int modifiers = method.getModifiers();
			if (Modifier.isPublic(modifiers) && !Modifier.isStatic(modifiers)) {
				String mname = method.getName();
				if (mname.startsWith(ADD) && mname.endsWith(LISTENER)) {
					mname = mname.substring(ADD_LENGTH, mname.length() - LISTENER_LENGTH);
					keys.put(mname, mname);
					/* XXX: Do we need to check that
					 * there's only one parameter?
					 */
					listenerClasses.put(mname, method.getParameterTypes()[0]);
					if (addMethods.get(mname) == null) {
						addMethods.put(mname, method);
					}
				}
				else if (mname.startsWith(REMOVE) && mname.endsWith(LISTENER)) {
					mname = mname.substring(REMOVE_LENGTH, mname.length() - LISTENER_LENGTH);
					keys.put(mname, mname);
					if (removeMethods.get(mname) == null) {
						removeMethods.put(mname, method);
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
			props[i] = new EventSetDescriptor(decapitalize(key), listenerType, listenerType.getDeclaredMethods(), add, remove);

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
				if (mname.startsWith(GET)
				    && !mname.equals(GET)) {
					mname = mname.substring(GET_LENGTH);
					keys.put(mname, mname);
					if (getMethods.get(mname) == null) {
						getMethods.put(mname, meth);
					}
				}
				else if (mname.startsWith(IS)
				    && !mname.equals(IS)) {
					mname = mname.substring(IS_LENGTH);
					keys.put(mname, mname);
					if (getMethods.get(mname) == null) {
						getMethods.put(mname, meth);
					}
				}
				break;

			case 1:
				if (mname.startsWith(SET)
				    && !mname.equals(SET)
				    && meth.getReturnType().equals(Void.TYPE)) {
					mname = mname.substring(SET_LENGTH);
					keys.put(mname, mname);
					if (setMethods.get(mname) == null) {
						setMethods.put(mname, meth);
					}
				}
				else if (mname.startsWith(GET)
				    && !mname.equals(GET)
				    && mclasses[0] == Integer.TYPE) {
					mname = mname.substring(GET_LENGTH);
					keys.put(mname, mname);
					if (getIdxMethods.get(mname) == null) {
						getIdxMethods.put(mname, meth);
					}
				}
				break;

			case 2:
				if (mname.startsWith(SET)
				    && !mname.equals(SET)
				    && meth.getReturnType().equals(Void.TYPE)
				    && mclasses[0] == Integer.TYPE) {
					mname = mname.substring(SET_LENGTH);
					keys.put(mname, mname);
					if (setIdxMethods.get(mname) == null) {
						setIdxMethods.put(mname, meth);
					}
				}
				break;
			}

		}
	}

	// check if the bean fires PropertyChangeEvent
	boolean bound = false;

	try {
	    startClass.getMethod(ADD_PROPERTY_CHANGE_LISTENER, new Class [] {PropertyChangeListener.class});
	    bound = true;
	}
	catch (NoSuchMethodException e) {
	    /* the bean does not allow to add property change listeners,
	     * thus it does not fire poperty change events, i.e. its
	     * properties are not bound.
	     */
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

		if (bound) {
		    props[i].setBound(true);
		}
	}

	return (props);
}

private static BeanInfo loadBeanInfo(Class beanClass) {
	String bname = beanClass.getName();

	// First try to load bean info from package.
	BeanInfo bean = loadNamedBean(beanClass.getClassLoader(),
	    bname + BEAN_INFO);
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
		bean = loadNamedBean(beanClass.getClassLoader(),
		    beansearch[i] + "." + bname + BEAN_INFO);
		if (bean != null) {
			return (bean);
		}
	}

	return (null);
}

private static BeanInfo loadNamedBean(ClassLoader loader, String cl) {
	try {
		return (BeanInfo)Class.forName(cl, true, loader).newInstance();
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
