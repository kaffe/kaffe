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
public class PropertyDescriptor
  extends FeatureDescriptor
{
	private Class rettype;
	private Method getter;
	private Method setter;
	private boolean bound;
	private boolean constrained;
	private Class editor;

public PropertyDescriptor(String propertyName, Class beanClass) throws IntrospectionException
{
	super(propertyName);

	final String capitalized = capitalize(propertyName);
	String name = "set" + capitalized;
	final Method [] meths = beanClass.getMethods();
	for (int i = 0; i < meths.length; ++i) {
		Method method = meths[i];
		if (name.equals(method.getName()) && method.getParameterTypes().length == 1) {
			checkSetter(method);
			setter = method;
			rettype = setter.getParameterTypes()[0];
			break;
		}
	}
	if (setter == null) {
		throw new IntrospectionException("Class "
						 + beanClass.getName()
						 + " does not have a setter method "
						 + name);

	}
	if (rettype == Boolean.TYPE) {
		name = "is" + capitalized;
		try {
			getter = beanClass.getMethod(name, null);
			checkGetter(getter);
		}
		catch (NoSuchMethodException e) {
		}
	}

	if (getter == null) {
		name = "get" + capitalized;
		try {
			getter = beanClass.getMethod(name, null);
			checkGetter(getter);
		}
		catch (NoSuchMethodException e) {
			throw new IntrospectionException(e.getMessage());
		}
	}

	checkGetterAndSetterMatch();
}

public PropertyDescriptor(String propertyName, Class beanClass, String getterName, String setterName) throws IntrospectionException
{
	super(propertyName);

	Method meths[] = beanClass.getMethods();
	for (int i = 0; i < meths.length; i++) {
		Method method = meths[i];
		if (getterName != null
		    && getter == null
		    && getterName.equals(method.getName())
		    && method.getParameterTypes().length == 0) {
			checkGetter(method);
			getter = method;
			rettype = getter.getReturnType();
		}
		else if (setterName != null
			 && setter == null
			 && setterName.equals(method.getName())
			 && method.getParameterTypes().length == 1) {
			checkSetter(method);
			setter = method;
			rettype = setter.getReturnType();
		}
	}

	checkGetterAndSetterMatch();
}

public PropertyDescriptor(String propertyName, Method getter, Method setter) throws IntrospectionException
{
	super(propertyName);
	this.getter = getter;
	this.setter = setter;
	if (getter != null) {
		rettype = getter.getReturnType();
	}
	else if (setter != null) {
		rettype = setter.getParameterTypes()[0];
	}

	checkGetterAndSetterMatch();
}

private void checkGetterAndSetterMatch() throws IntrospectionException {
	if ((getter != null && getter.getReturnType() != rettype)
	    || (setter != null && setter.getParameterTypes()[0] != rettype)) {
		throw new IntrospectionException("Getter method "
						 + getter
						 + " and setter method "
						 + setter
						 + " don't match.");
	}
}

private void checkSetter(Method method) throws IntrospectionException {
	if (method.getParameterTypes().length != 1) {
		throw new IntrospectionException("Setter method "
						 + method
						 + " must accept only one pararmeter.");
	}
	else if (method.getReturnType() != Void.TYPE) {
			throw new IntrospectionException("Setter method "
						 + method
						 + " must return void.");
	}
}

private void checkGetter(Method method) throws IntrospectionException {
	if (method.getParameterTypes().length != 0) {
		throw new IntrospectionException("Getter method "
						 + method
						 + " must not accept any pararmeters.");
	}
	else if (method.getReturnType() == Void.TYPE) {
		throw new IntrospectionException("Getter method "
						 + method
						 + " must not return void.");
	}
}

public Class getPropertyEditorClass()
	{
	return (editor);
}

public Class getPropertyType()
	{
	return (rettype);
}

public Method getReadMethod()
	{
	return (getter);
}

public Method getWriteMethod()
	{
	return (setter);
}

public boolean isBound()
	{
	return (bound);
}

public boolean isConstrained()
	{
	return (constrained);
}

public void setBound(boolean bound)
	{
	this.bound = bound;
}

public void setConstrained(boolean constrained)
	{
	this.constrained = constrained;
}

public void setPropertyEditorClass(Class propertyEditorClass)
	{
	editor = propertyEditorClass;
}
}
