package java.beans;

import java.io.IOException;
import java.io.InputStream;
import java.io.ObjectInputStream;
import kaffe.io.ClassLoaderObjectInputStream;
import kaffe.lang.SystemClassLoader;

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

public Beans() {
}

public static Object getInstanceOf(Object bean, Class targetType) {
	return (bean);
}

public static Object instantiate(ClassLoader cld, String beanName) throws IOException, ClassNotFoundException {
System.out.println("Beans:instantiate: name=" + beanName + ", loader=" + cld);
	if (cld == null) {
		cld = SystemClassLoader.getClassLoader();
	}

	InputStream in = cld.getResourceAsStream(beanName.replace('.', '/') + ".ser");

	Object obj;
	if (in != null) {
System.out.println("Reading serialized object");
		obj = (new ClassLoaderObjectInputStream(in, cld)).readObject();
	}
	else {
		try {
System.out.println("Creating new instance");
			obj = cld.loadClass(beanName).newInstance();
		}
		catch (InstantiationException _) {
System.out.println("InstantiationException");
			throw new ClassNotFoundException(beanName);
		}
		catch (IllegalAccessException _) {
System.out.println("IllegalAccessException");
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
