/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.rmi.server;

import java.lang.reflect.Method;
import java.lang.Class;

/**
 * A little helper class to calculate the method hash numbers.
 *  It's quicker to go native than to do this from Java.
 */
public class RMIHashes {

public static native long getMethodHash(Method meth);
public static native long getInterfaceHash(Class clazz);

}
