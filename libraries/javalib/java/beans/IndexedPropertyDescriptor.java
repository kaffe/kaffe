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
 *
 * FIXME: Fix constructors to implement initialization according to
 * Java Class Libraries 2nd Ed. Vol. 2.
 */
public class IndexedPropertyDescriptor
  extends PropertyDescriptor
{
	private Method idxgetter;
	private Method idxsetter;
	private Class idxrettype;

public IndexedPropertyDescriptor(String propertyName, Class beanClass) throws IntrospectionException
{
	super(propertyName, beanClass);

	init(beanClass);
	check();
}

public IndexedPropertyDescriptor(String propertyName, Class beanClass, String getterName, String setterName, String indexedGetterName, String indexedSetterName) throws IntrospectionException
{
	super(propertyName, beanClass, getterName, setterName);

	init(beanClass, indexedGetterName, indexedSetterName);
	check();
}

public IndexedPropertyDescriptor(String propertyName, Method getter, Method setter, Method indexedGetter, Method indexedSetter) throws IntrospectionException
{
	super(propertyName, getter, setter);
	idxgetter = indexedGetter;
	idxsetter = indexedSetter;
	if (idxgetter != null) {
		idxrettype = idxgetter.getReturnType();
	}
	else if (idxsetter != null) {
		idxrettype = idxsetter.getParameterTypes()[1];
	}
	check();
}

private void check() throws IntrospectionException {
	checkGetter(getIndexedReadMethod());
	checkSetter(getIndexedWriteMethod());
	checkGetterAndSetterMatch(getIndexedReadMethod(), getIndexedWriteMethod());
}

private void checkGetterAndSetterMatch(Method getter, Method setter) throws IntrospectionException {
	Class rettype = getIndexedPropertyType();
	if ((getter != null
	     && getter.getReturnType() != rettype)
	    || (setter != null
		&& setter.getParameterTypes()[1] != rettype)) {
		throw new IntrospectionException("Getter method "
						 + getter
						 + " and setter method "
						 + setter
						 + " don't match.");
	}
}

private void checkSetter(Method method) throws IntrospectionException {
	if (method != null) {
		if (method.getParameterTypes().length != 2) {
			throw new IntrospectionException("Setter method "
							 + method
							 + " must accept only two pararmeters.");
		}
		else if (method.getReturnType() != Void.TYPE) {
			throw new IntrospectionException("Setter method "
							 + method
							 + " must return void.");
		}
	}
}

private void checkGetter(Method method) throws IntrospectionException {
	if (method != null) {
		if (method.getParameterTypes().length != 1) {
			throw new IntrospectionException("Getter method "
							 + method
							 + " must accept only one pararmeter.");
		}
		else if (method.getReturnType() == Void.TYPE) {
			throw new IntrospectionException("Getter method "
							 + method
							 + " must not return void.");
		}
	}
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

private void init(Class beanClass) throws IntrospectionException {
	init(beanClass, getReadMethod().getName(), getWriteMethod().getName());
}

private void init(Class beanClass, String indexedGetterName, String indexedSetterName) throws IntrospectionException {
	NoSuchMethodException ex = null;

	try {
		Class[] getParam = new Class[1];
		getParam[0] = Integer.TYPE;

		idxgetter = beanClass.getMethod(indexedGetterName, getParam);
		idxrettype = idxgetter.getReturnType();
	}
	catch (NoSuchMethodException nsme) {
		ex = nsme;
	}
	try {
		Class[] setParam = new Class[2];
		setParam[0] = Integer.TYPE;
		setParam[1] = idxrettype;

		idxsetter = beanClass.getMethod(indexedSetterName, setParam);
	}
	catch (NoSuchMethodException nsme) {
		ex = nsme;
	}

	if (ex != null) {
		throw new IntrospectionException("failed");
	}
}

}
