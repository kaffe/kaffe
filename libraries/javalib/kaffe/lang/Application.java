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

import java.lang.reflect.Method;
import java.lang.reflect.InvocationTargetException;
import java.util.Vector;
import java.util.Enumeration;
import java.io.InputStream;
import java.io.FilterInputStream;
import java.io.PrintStream;
import java.net.URL;

public class Application extends ClassLoader implements Runnable {

private Method runMethod;
private String[] arguments;
private Vector resources;
private ResourceReader reader;
private ApplicationResource initResource;
private Thread tid;
private Throwable exception;
private int exitcode;
private static boolean sysio;

/**
 * 1. create an application
 * 2. start the application in separate thread
 *
 * @param cname	Name of main class
 * @param args  argv[] argument passed to main()
 */
public Application(String cname, String[] args) throws ApplicationException {

	try {
		/* Find the class we're to execute */
		Class clazz = loadClass(cname);

		/* Get main(String[])V */
		runMethod = clazz.getMethod("main", new Class[]{ args.getClass() });
		arguments = args;

		tid = new Thread(null, this, cname);

		setupSystemIO();
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
		if (e instanceof InvocationTargetException) {
			e = ((InvocationTargetException)e).getTargetException();
		}
		if (!(e instanceof ThreadDeath)) {
			/* We catch everything and just report it */
			e.printStackTrace();
			exception = e;
			exitcode = 1;
		}
	}

	// if the application didn't add any other resources (threads or 
	// frames or whatever), and a final resource was given, free that 
	// final resource now.
	// This way, it can be used to detect whether the application
	// simply returned from its main() function
	if (resources != null && resources.size() == 1 
		&& resources.elementAt(0) == initResource) {
		initResource.freeResource();
	}
}

/*************************************************************************/

/**
 * Locate the Application the current thread is running in.
 */
public static Application getApplication() {

	Class[] classes = classStack0();
	for (int i = 0; i < classes.length; i++) {
		ClassLoader loader = classes[i].getClassLoader();
		//System.out.println("Class: " + classes[i].toString() + ", loader: " + loader);
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
//		System.out.println("Freeing resource " + r);
		r.freeResource();
	}
}

public static void addResource(ApplicationResource res) {
	Application app = getApplication();
	if (app != null) {
		app.add(res);
	}
}

public static void removeResource(ApplicationResource res) {
	Application app = getApplication();
	if (app != null) {
		app.remove(res);
	}
}

private synchronized void add(ApplicationResource res) {
	if (resources == null) {
		resources = new Vector();
	}
//	System.out.println("Adding resource " + res.toString() + " to app " + this);
	resources.addElement(res);
}

public synchronized void remove(ApplicationResource res) {
	if (resources != null) {
//		System.out.println("Removing resource " + res.toString() + " from app " + this);
		resources.removeElement(res);
	}
}

private static synchronized void setupSystemIO() {
	if (sysio) {
		return;
	}
	try {
		System.setIn(new FilterInputStream(System.in){ public void close() { } });
		System.setOut(new PrintStream(System.out){ public void close() { flush(); } });
		System.setErr(new PrintStream(System.err){ public void close() { flush(); } });
		sysio = true;
	}
	catch (SecurityException _) {
	}
}

/**
 * Terminate an application.  Terminate the threads then tidy any pending
 * resources.
 */
public static boolean exit(int status) {
	Application app = getApplication();
	if (app == null) {
		return (false);		// returning false causes VM to exit
	}
	app.exitcode = status;
	app.freeAllResources();
	return (true);
}

public int waitFor() throws InterruptedException {
	tid.join();
	return (exitcode);
}

public Throwable exitException() {
	return (exception);
}

public int exitValue() {
	return (exitcode);
}

private native void exit0();
private static native Class[] classStack0();

}
