/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.tools.serialver;

import java.io.ObjectStreamClass;

public class SerialVer {

public static void main(String[] args) {
	if (args.length == 0) {
		System.err.println("Usage: serialver [classname ...]");
		System.exit(1);
	}
	for (int i = 0; i < args.length; i++) {
		try {
			Class clz = Class.forName(args[i]);
			ObjectStreamClass stream = ObjectStreamClass.lookup(clz);
			if (stream != null) {
				System.out.println(args[i] + ": static final long serialVersionUID = " + stream.getSerialVersionUID() + "L;");
			}
			else {
				System.out.println("Class " + args[i] + " is not Serializable.");
			}
		}
		catch (ClassNotFoundException _) {
			System.out.println("Class " + args[i] + " not found.");
		}
	}
}

}
