/*
 * ExecJar.java
 * Execute a Jar file.
 *  This is a Java wrapper which is executed as an application.  The first
 *  argument is the JAR file to actually execute, the rest are parameters.
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.jar;

import java.util.zip.ZipFile;
import java.util.zip.ZipEntry;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.EOFException;
import java.lang.reflect.Method;
import java.lang.Character;
import kaffe.management.Classpath;

public class ExecJar {

private final static String MANIFEST = "META-INF/MANIFEST.MF";
private final static String MAIN = "Main-Class:";

public static void main(String[] args)
{
	ZipFile jar = null;
	try {
		jar = new ZipFile(args[0]);
	}
	catch (IOException _) {
		System.err.println("No such JAR: " + args[0]);
		System.exit(1);
	}

	// Look for manifest
	ZipEntry manifest = jar.getEntry(MANIFEST);
	if (manifest == null) {
		System.err.println("No " + MANIFEST + " found in JAR: " + args[0]);
		System.exit(1);
	}

	String command = null;
	try {
		DataInputStream data = new DataInputStream(jar.getInputStream(manifest));
		while (command == null) {
			String line = data.readLine();
			if (line == null) {
				throw new EOFException();
			}
			if (line.startsWith(MAIN)) {
				char[] cline = line.toCharArray();
				for (int i = MAIN.length(); i < cline.length; i++) {
					if (!Character.isWhitespace(cline[i])) {
						command = new String(cline, i, cline.length-i);
						break;
					}
				}
			}
		}
		jar.close();
	}
	catch (EOFException eof) {
		System.err.println("No " + MAIN + " found in JAR manifest: " + args[0]);
		System.exit(1);
	}
	catch (IOException e) {
		e.printStackTrace();
		System.exit(1);
	}

	/* Add JAR to classpath */
	Classpath.add(args[0]);

	/* Build new argument array */
	String[] nargs = new String[args.length-1];
	for (int i = 1; i < args.length; i++) {
		nargs[i-1] = args[i];
	}

	/* Load in class */
	try {
		Class commandClass = Class.forName(command);

		/* Build main(String[])V */
		Class[] params = new Class[1];
		params[0] = (new String[0]).getClass();

		/* Build the invoke arguments */
		Object[] iargs = new Object[1];
		iargs[0] = (Object)nargs;

		/* Get method and invoke */
		Method meth = commandClass.getDeclaredMethod("main", params);
		meth.invoke(null, iargs);
	}
	catch (Exception misc) {
		misc.printStackTrace();
		System.exit(1);
	}
}

}
