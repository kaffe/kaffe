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

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

public class Compiler_pizza
	extends Compiler {

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
	try {
		Class mainclass = Class.forName("pizza.compiler.Main");
		Method mainmeth = mainclass.getMethod("main", new Class[] {String[].class});
		mainmeth.invoke(null, args);
	}
	catch (ClassNotFoundException e) {
	    e.printStackTrace();
	    return false;
	}
	catch (IllegalAccessException e) {
	    e.printStackTrace();
	    return false;
	}
	catch (InvocationTargetException e) {
	    e.printStackTrace();
	    return false;
	}
	catch (NoSuchMethodException e) {
	    e.printStackTrace();
	    return false;
	}

	return true;
}

}
