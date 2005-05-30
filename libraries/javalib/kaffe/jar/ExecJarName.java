/*
 * ExecJarName.java
 * Execute a Jar file with a given named starting class.
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.jar;

import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.jar.JarFile;

public class ExecJarName {

public static JarFile getJar(String path) {
	JarFile jar = null;
	try {
		jar = new JarFile(path);
	}
	catch (IOException e) {
		System.err.println("Can't access JAR file ``"
		    + path + "'': " + e);
		System.exit(1);
	}
	return jar;
}

public static void main(String[] args)
{
	/* Check JAR file OK */
	getJar(args[0]);

	/* Get command to call */
	String command = args[1];

	/* Build new argument array */
	String[] nargs = new String[args.length-2];
	for (int i = 2; i < args.length; i++) {
		nargs[i-2] = args[i];
	}

	/* Load in class */
	try {
		Class commandClass;

		commandClass = Class.forName (args[1], true, ClassLoader.getSystemClassLoader());

		/* Build main(String[])V */
		Class[] params = new Class[1];
		params[0] = (new String[0]).getClass();

		/* Build the invoke arguments */
		Object[] iargs = new Object[1];
		iargs[0] = nargs;

		/* Get method and invoke */
		Method meth = commandClass.getDeclaredMethod("main", params);
		meth.invoke(null, iargs);
	} catch (InvocationTargetException e) {
		e.getTargetException().printStackTrace();
		System.exit(1);
	} catch (Exception e) {
		e.printStackTrace();
		System.exit(1);
	}
}

}
