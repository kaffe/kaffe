/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.tools.javap;

import java.io.*;
import java.util.*;
import java.lang.reflect.*;

/**
 * JavaP prints structural information about classes.
 * For each class, all public fields and methods are listed.
 * "Reflectance" is used to look up the information.
 *
 * @author      Ian Darwin, Ian@DarwinSys.com
 * @version     $Id: JavaP.java,v 1.1 1999/10/16 01:56:23 tim Exp $
 */
public class JavaP {

/** Simple main program, construct self, process each class name
 * found in argv.
 */
public static void main(String argv[]) {
	JavaP xref = new JavaP();

	if (argv.length == 0) {
		usage();
		System.exit(1);
	}
	else {
		for (int i=0; i<argv.length; i++) {
			xref.doClass(argv[i]);
		}
	}
}

/** Format the fields and methods of one class, given its name.
 */
protected void doClass(String className) {
	try {
		Class c = Class.forName(className);
		System.out.print(Modifier.toString(c.getModifiers()) + ' ' + c);
		Class sc = c.getSuperclass();
		if (sc != null) {
			System.out.print(" extends " + sc.getName());
		}
		Class ifs[] = c.getInterfaces();
		if (ifs.length > 0) {
			System.out.print(" implements ");
			for (int i = 0; i < ifs.length; i++) {
				System.out.print(ifs[i].getName());
				if (i + 1 < ifs.length) {
					System.out.print(", ");
				}
			}
		}
		System.out.println(" {");
		int i, mods;
		Field fields[] = c.getDeclaredFields();
		for (i = 0; i < fields.length; i++) {
			if (!Modifier.isPrivate(fields[i].getModifiers())
			 && !Modifier.isProtected(fields[i].getModifiers()))
				System.out.println("\t" + fields[i]);
		}

		Method methods[] = c.getDeclaredMethods();
		for (i = 0; i < methods.length; i++) {
			if (!Modifier.isPrivate(methods[i].getModifiers())
			 && !Modifier.isProtected(methods[i].getModifiers()))
				System.out.println("\t" + methods[i]);
		}
		System.out.println("}");
	}
	catch (ClassNotFoundException e) {
		System.err.println("Error: Class " + 
			className + " not found!");
	}
	catch (Exception e) {
		System.err.println(e);
	}
}

private static void usage() {
	System.err.print(
"usage: javap [-options] classes\n" +
"Options are:\n" +
"	-b *			Backwards compatible with javap in JDK 1.1\n" +
"	-c *			Disassemble bytecode\n" +
"	-classpath <path> *	Specify where to find classes\n" +
"	-help *			Prints this usage message\n" +
"	-l *			Prints line number information\n" +
"	-public *		Show only public classes & members\n" +
"	-protected *		Show protected/public classes & members\n" +
"	-package *		SHow package/protected/public classes & members\n" +
"	-private *		Show all classes & members\n" +
"	-s *			Print internal type signatures\n" +
"	-verbose *		Print stack size, locals and arguments\n" +
"  * Option not implemented\n"
);
}

}
