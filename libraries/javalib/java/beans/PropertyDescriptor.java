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
	this(propertyName, beanClass, "get"+capitalize(propertyName), "set"+capitalize(propertyName));
}

public PropertyDescriptor(String propertyName, Class beanClass, String getterName, String setterName) throws IntrospectionException
{
	super(propertyName);
	this.getter = null;
	this.setter = null;
	this.rettype = null;
	this.bound = false;
	this.constrained = false;
	this.editor = null;

	Method meths[] = beanClass.getMethods();
	for (int i = 0; i < meths.length; i++) {
		String mname = meths[i].getName();
		if (getterName != null && getterName.equals(mname)) {
			getter = meths[i];
			rettype = getter.getReturnType();
		}
		else if (setterName != null && setterName.equals(mname)) {
			setter = meths[i];
			rettype = setter.getReturnType();
		}
	}
}

public PropertyDescriptor(String propertyName, Method getter, Method setter) throws IntrospectionException
{
	super(propertyName);
	this.getter = getter;
	this.setter = setter;
	rettype = null;
	bound = false;
	constrained = false;
	editor = null;
	if (getter != null) {
		rettype = getter.getReturnType();
	}
	else if (setter != null) {
		rettype = setter.getParameterTypes()[0];
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
