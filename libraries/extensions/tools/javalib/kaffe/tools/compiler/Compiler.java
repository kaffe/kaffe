/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.tools.compiler;

abstract public class Compiler {

private static final String prefix = "kaffe.tools.compiler.Compiler_";

protected Exception exception;
protected String destination;

abstract public boolean compile(String name);

public Exception getException() {
	return (exception);
}

public void setDestination(String dest) {
	destination = dest;
}

public static Compiler getInstance() {
	return (getInstance(System.getProperty("kaffe.compiler", "default")));
}

public static Compiler getInstance(String name) {
	try {
		Class cls = Class.forName(prefix + name);
		return ((Compiler)cls.newInstance());
	}
	catch (ClassNotFoundException _) {
	}
	catch (IllegalAccessException _) {
	}
	catch (InstantiationException _) {
	}
	return (new Compiler_default());
}

}
