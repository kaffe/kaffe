package java.beans;

import java.lang.reflect.Method;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class EventSetDescriptor
  extends FeatureDescriptor
{
	private final Class sourceClass;
	private final String eventSetName;
	private final Class listenerType;
	private MethodDescriptor[] listenerMethods;
	private Method addListenerMethod;
	private Method removeListenerMethod;
	private boolean unicast;
	private boolean indefault;

public EventSetDescriptor(Class sourceClass, String eventSetName, Class listenerType, String listenerMethodName) throws IntrospectionException
{
	this.sourceClass = sourceClass;
	this.eventSetName = eventSetName;
	this.listenerType = listenerType;

	final String esname = capitalize(eventSetName);
	final String addListenerMethodName = "add"+esname+"Listener";
	final String removeListenerMethodName = "remove"+esname+"Listener";

	listenerMethods = new MethodDescriptor[1];

	// Get the methods on this interface and search out the listener names
	Method meths[] = listenerType.getDeclaredMethods();
	for (int i = 0; i < meths.length; i++) {
		final String mname = meths[i].getName();
		if (mname.equals(listenerMethodName)) {
			listenerMethods[0] = new MethodDescriptor(meths[i]);
		}
		else if (mname.equals(addListenerMethodName)) {
			addListenerMethod = meths[i];
		}
		else if (mname.equals(removeListenerMethodName)) {
			removeListenerMethod = meths[i];
		}
	}
}

public EventSetDescriptor(Class sourceClass, String eventSetName, Class listenerType, String listenerMethodNames[], String addListenerMethodName, String removeListenerMethodName) throws IntrospectionException
{
	this.sourceClass = sourceClass;
	this.eventSetName = eventSetName;
	this.listenerType = listenerType;

	// Get the methods on this interface and search out the listener names
	final Method meths[] = listenerType.getDeclaredMethods();
	listenerMethods = new MethodDescriptor[listenerMethodNames.length];
	for (int i = 0; i < meths.length; i++) {
		final String mname = meths[i].getName();
		for (int j = 0; j < listenerMethodNames.length; j++) {
			if (mname.equals(listenerMethodNames[j])) {
				listenerMethods[j] = new MethodDescriptor(meths[i]);
			}
		}
		if (mname.equals(addListenerMethodName)) {
			addListenerMethod = meths[i];
		}
		else if (mname.equals(removeListenerMethodName)) {
			removeListenerMethod = meths[i];
		}
	}
}

public EventSetDescriptor(String eventSetName, Class listenerType, MethodDescriptor listenerMethodDescriptors[], Method addListenerMethod, Method removeListenerMethod) throws IntrospectionException
{
	this.sourceClass = null;
	this.eventSetName = eventSetName;
	this.listenerType = listenerType;
	this.listenerMethods = listenerMethodDescriptors;
	this.addListenerMethod = addListenerMethod;
	this.removeListenerMethod = removeListenerMethod;
	this.unicast = false;
	this.indefault = false;
	}

public EventSetDescriptor(String eventSetName, Class listenerType, Method listenerMethods[], Method addListenerMethod, Method removeListenerMethod) throws IntrospectionException
{
	this.sourceClass = null;
	this.eventSetName = eventSetName;
	this.listenerType = listenerType;
	this.addListenerMethod = addListenerMethod;
	this.removeListenerMethod = removeListenerMethod;

	this.listenerMethods = new MethodDescriptor[listenerMethods.length];
	for (int i = 0; i < listenerMethods.length; i++) {
		this.listenerMethods[i] = new MethodDescriptor(listenerMethods[i]);
	}

	this.unicast = false;
	this.indefault = false;
	}

public Method getAddListenerMethod()
	{
	return (addListenerMethod);
}

public MethodDescriptor[] getListenerMethodDescriptors()
	{
	return (listenerMethods);
}

public Method[] getListenerMethods()
	{
	Method meths[] = new Method[listenerMethods.length];
	for (int i = 0; i < meths.length; i++) {
		meths[i] = listenerMethods[i].getMethod();
	}
	return (meths);
}

public Class getListenerType()
	{
	return (listenerType);
}

public Method getRemoveListenerMethod()
	{
	return (removeListenerMethod);
}

public boolean isInDefaultEventSet()
	{
	return (indefault);
	}

public boolean isUnicast()
	{
	return (unicast);
}

public void setInDefaultEventSet(boolean inDefaultEventSet)
	{
	indefault = inDefaultEventSet;
	}

public void setUnicast(boolean unicast)
	{
	this.unicast = unicast;
}
}
