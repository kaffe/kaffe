/*
 * Classpath.java
 * Interface to KVM class path management.
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.management;

public class Classpath {

static {
	System.loadLibrary("management");
}

public native static void add(String pathelem);

}
