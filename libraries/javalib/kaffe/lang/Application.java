/*
 * Application -
 *  Kaffe extended library component.
 *  In order to run several applications on a single VM we assign each
 *  one an application.  This is essentially a class loader.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.lang;

import java.lang.String;
import java.lang.Class;
import java.lang.ClassNotFoundException;
import java.lang.Thread;
import java.lang.InterruptedException;
import java.lang.reflect.Method;
import java.lang.ClassLoader;
import java.util.Vector;
import java.util.Enumeration;
import java.io.File;
import java.io.InputStream;
import java.net.URL;
import kaffe.lang.ApplicationException;
import kaffe.lang.ApplicationResource;

public class Application extends ClassLoader implements Runnable {

private Method runMethod;
private String[] arguments;
private Vector resources;

public Application(String cname, String[] args) throws ApplicationException {

	try {
		/* Find the class we're to execute */
		Class clazz = loadClass(cname);

		/* Get main(String[])V */
		runMethod = clazz.getMethod("main", new Class[]{ args.getClass() });
		arguments = args;

		Thread tid = new Thread(null, this, cname);
		tid.start();
	}
	catch (NoSuchMethodException e) {
		throw new ApplicationException("No main() method in " + cname);
	}
	catch (ClassNotFoundException e) {
		throw new ApplicationException("Application " + cname + " class not found");
	}
}

public void run() {

	/* Build the invoke arguments */
	try {
		runMethod.invoke(null, new Object[]{ arguments });
	}
	catch (Throwable e) {
		/* We catch everything and just report it */
		e.printStackTrace();
	}
}

/*************************************************************************/

/**
 * Locate the Application the current thread is running in.
 */
static private Application getApplication() {

	Class[] classes = classStack0();
	for (int i = 0; i < classes.length; i++) {
		ClassLoader loader = classes[i].getClassLoader();
//		System.out.println("Class: " + classes[i].toString() + ", loader: " + loader.toString());
		if (loader instanceof Application) {
			return ((Application)loader);
		}
	}
	return (null);
}

public void freeAllResources() {
	if (resources == null) {
		return;
	}
	Enumeration e = resources.elements();
	while (e.hasMoreElements()) {
		ApplicationResource r = (ApplicationResource)e.nextElement();
		r.freeResource();
	}
}

public static void addResource(ApplicationResource res) {
	Application app = getApplication();
//	System.out.println("Adding resource " + res.toString() + " to app " + (app == null ? "null" : app.toString()));
	if (app != null) {
		app.add(res);
	}
}

public static void removeResource(ApplicationResource res) {
	Application app = getApplication();
//	System.out.println("Removing resource " + res.toString() + " to app " + (app == null ? "null" : app.toString()));
	if (app != null) {
		app.remove(res);
	}
}

private synchronized void add(ApplicationResource res) {
	if (resources == null) {
		resources = new Vector();
	}
	resources.addElement(res);
}

public synchronized void remove(ApplicationResource res) {
	if (resources != null) {
		resources.removeElement(res);
	}
}

/**
 * Terminate an application.  Terminate the threads then tidy any pending
 * resources.
 */
public static boolean exit(int status) {
	Application app = getApplication();
	if (app == null) {
		return (false);
	}
	app.freeAllResources();
	return (true);
}

/*************************************************************************/

public URL getResource(String name) {
	return (getSystemResource(name));
}

public InputStream getResourceAsStream(String name) {
	return (getSystemResourceAsStream(name));
}

public Class loadClass(String name, boolean resolve) throws ClassNotFoundException {
	Class cls;
//	System.out.println("loadClass: " + name);

	cls = findLoadedClass(name);
	if (cls != null) {
		// Found it in the cache
	}
	else if (name.startsWith("java.") || name.startsWith("kaffe.")) {
		cls = findSystemClass(name);
	}
	else {
		try {
			String newname = name.replace('.', '/') + ".class";
			InputStream in = getSystemResourceAsStream(newname);
			byte[] data = new byte[in.available()];
			in.read(data);
			cls = defineClass(null, data, 0, data.length);
		}
		catch (Exception _) {
			throw new ClassNotFoundException(name);
		}
	}
	if (resolve) {
		resolveClass(cls);
	}
	return (cls);
}

private native void exit0();
private static native Class[] classStack0();

}
