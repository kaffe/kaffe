/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package java.rmi.server;

import java.net.MalformedURLException;
import java.net.URL;


public interface LoaderHandler {

    String packagePrefix = "";

    Class loadClass(String name) throws MalformedURLException, ClassNotFoundException;

    Class loadClass(URL codebase, String name) throws MalformedURLException, ClassNotFoundException;

    Object getSecurityContext(ClassLoader loader);

}
