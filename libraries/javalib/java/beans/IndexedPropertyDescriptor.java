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
public class IndexedPropertyDescriptor
  extends PropertyDescriptor
{
	private Method idxgetter;
	private Method idxsetter;
	private Class idxrettype;

public IndexedPropertyDescriptor(String propertyName, Class beanClass) throws IntrospectionException
{
	this(propertyName, beanClass, "get"+capitalize(propertyName), "set"+capitalize(propertyName), "get"+capitalize(propertyName), "set"+capitalize(propertyName));
}

public IndexedPropertyDescriptor(String propertyName, Class beanClass, String getterName, String setterName, String indexedGetterName, String indexedSetterName) throws IntrospectionException
{
	super(propertyName, beanClass, getterName, setterName);

	try {
		Class[] getParam = new Class[1];
		getParam[0] = Integer.TYPE;

		idxgetter = beanClass.getMethod(getterName, getParam);
		idxrettype = idxgetter.getReturnType();

		Class[] setParam = new Class[2];
		setParam[0] = Integer.TYPE;
		setParam[1] = idxrettype;

		idxsetter = beanClass.getMethod(setterName, setParam);
	}
	catch (NoSuchMethodException _) {
		throw new IntrospectionException("failed");
	}
}

public IndexedPropertyDescriptor(String propertyName, Method getter, Method setter, Method indexedGetter, Method indexedSetter) throws IntrospectionException
{
	super(propertyName, getter, setter);
	idxgetter = indexedGetter;
	idxsetter = indexedSetter;
	idxrettype = idxgetter.getReturnType();
}

public Class getIndexedPropertyType()
	{
	return (idxrettype);
}

public Method getIndexedReadMethod()
	{
	return (idxgetter);
}

public Method getIndexedWriteMethod()
	{
	return (idxsetter);
}
}
