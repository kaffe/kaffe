/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.rmi.server;

import java.net.MalformedURLException;
import java.net.URL;


public interface LoaderHandler {

public static final String packagePrefix = "";

public Class loadClass(String name) throws MalformedURLException, ClassNotFoundException;

public Class loadClass(URL codebase, String name) throws MalformedURLException, ClassNotFoundException;

public Object getSecurityContext(ClassLoader loader);

}
