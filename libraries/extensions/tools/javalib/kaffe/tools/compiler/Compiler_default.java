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

public class Compiler_default
	extends Compiler {

/**
 * The default compiler just say 'false'
 */
public boolean compile(String name) {
	return (false);
}

/**
 * The exception is that we don't have a compiler.
 */
public Exception getException() {
	return (new Exception("no compiler available"));
}

}
