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
public class MethodDescriptor
  extends FeatureDescriptor
{
	private Method meth;
	private ParameterDescriptor[] params;

public MethodDescriptor(Method method)
	{
	this(method, null);
}

public MethodDescriptor(Method method, ParameterDescriptor parameterDescriptors[])
	{
	meth = method;
	params = parameterDescriptors;
}

public Method getMethod()
	{
	return (meth);
}

public ParameterDescriptor[] getParameterDescriptors()
	{
	return (params);
}
}
