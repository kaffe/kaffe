package java.beans;

import java.io.IOException;
import java.io.InputStream;
import java.io.ObjectInputStream;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class Beans
{
	private static boolean designtime = false;
	private static boolean guiavailable = false;

public Beans()
	{
}

public static Object getInstanceOf(Object bean, Class targetType)
	{
	return (bean);
}

public static Object instantiate(ClassLoader cls, String beanName) throws IOException, ClassNotFoundException
{
	InputStream in = cls.getSystemResourceAsStream(beanName.replace('.', '/') + ".ser");

	Object obj;
	if (in != null) {
		obj = (new ObjectInputStream(in)).readObject();
	}
	else {
		try {
			obj = cls.loadClass(beanName).newInstance();
		}
		catch (InstantiationException _) {
			throw new ClassNotFoundException(beanName);
		}
		catch (IllegalAccessException _) {
			throw new ClassNotFoundException(beanName);
		}
	}
	return (obj);
}

public static boolean isDesignTime()
	{
	return (designtime);
}

public static boolean isGuiAvailable()
	{
	return (guiavailable);
}

public static boolean isInstanceOf(Object bean, Class targetType)
	{
	return (targetType.isInstance(bean));
}

public static void setDesignTime(boolean isDesignTime) throws SecurityException
{
	// System.getSecurityManager().check???
	designtime = isDesignTime;
}

public static void setGuiAvailable(boolean isGuiAvailable) throws SecurityException
{
	// System.getSecurityManager().check???
	guiavailable = isGuiAvailable;
}
}
