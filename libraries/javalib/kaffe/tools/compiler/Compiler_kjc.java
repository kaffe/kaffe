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

import kaffe.lang.Application;
import kaffe.lang.ApplicationException;

public class Compiler_kjc extends Compiler {

public boolean compile(String name) {
	String[] args;
	String classpath = System.getProperty("java.class.path", ".");
	if (destination != null) {
		args = new String[] {
				"-classpath", classpath,
				"-d", destination,
				name };
	}
	else {
		args = new String[] {
				"-classpath", classpath,
				name };
	}
	Application app;
	try {
		app = new Application("at.dms.kjc.Main", args);
	}
	catch (ApplicationException _) {
		return (false);
	}
	try {
		app.waitFor();
	}
	catch (InterruptedException _) {
	}
	int code = app.exitValue();
	if (code == 0) {
		return (true);
	}
	Throwable thrw = app.exitException();
	if (thrw instanceof Exception) {
		exception = (Exception)thrw;
	}
	else {
		exception = new Exception("exited with errorcode " + code);
	}
	return (false);
}

}
