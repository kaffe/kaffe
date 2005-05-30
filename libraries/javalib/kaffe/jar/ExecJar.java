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

import java.io.IOException;
import java.util.StringTokenizer;
import java.util.jar.Attributes;
import java.util.jar.JarFile;
import java.util.jar.Manifest;

public class ExecJar {

public static void main(String[] args)
{
	// Get JAR file
	JarFile jar = ExecJarName.getJar(args[0]);

	// Look for manifest
	Manifest manifest = null;
	try {
		manifest = jar.getManifest();
		if (manifest == null) {
			System.err.println("No manifest found in ``"
			    + args[0] + "''");
			System.exit(1);
		}
	} catch (IOException e) {
		System.err.println("Can't access manifest in ``"
		    + args[0] + "'': " + e);
		System.exit(1);
	}

	// Get "Main-Class" attribute
	String mainClass = manifest.getMainAttributes().getValue(
					Attributes.Name.MAIN_CLASS);
	if (mainClass == null) {
		System.err.println("No ``Main-Class'' found in manifest");
		System.exit(1);
	}

	// Now call ExecJarName's main()
	String[] newArgs = new String[args.length + 1];
	System.arraycopy(args, 1, newArgs, 2, args.length - 1);
	newArgs[0] = args[0];
	newArgs[1] = mainClass.replace('/', '.');
	ExecJarName.main(newArgs);
}

}

