/*
 * Java core library component.
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.net;

import java.net.URLStreamHandlerFactory;
import java.net.URLStreamHandler;
import java.util.StringTokenizer;

public class DefaultURLStreamHandlerFactory implements URLStreamHandlerFactory {

final private static String defaultName = "kaffe.net.www.protocol.";

public URLStreamHandler createURLStreamHandler(String protocol)
{
	URLStreamHandler handler;

        String pkgs = System.getProperty("java.protocol.handler.pkgs");
        if (pkgs != null) {
                StringTokenizer tokenizer = new StringTokenizer(pkgs, "|");
                while (tokenizer.hasMoreTokens()) {
                        String theClass = tokenizer.nextToken()+"."+protocol+".Handler";
                        handler = tryClass(theClass);
                        if (handler != null) {
                                return (handler);
                        }
                }
        }

        /* Try the default name */
        String classPath = defaultName + protocol + ".Handler";
        handler = tryClass(classPath);
        if (handler != null) {
                return (handler);
        }

	return (null);
}

private static URLStreamHandler tryClass(String theClass) {
        try {
                Class cl = Class.forName(theClass);
                Object obj = cl.newInstance();
                if (obj instanceof URLStreamHandler) {
                        return (URLStreamHandler )obj;
                }
        }
        catch (UnsatisfiedLinkError e) {
        }
        catch (ClassNotFoundException e) {
        }
        catch (IllegalAccessException e) {
        }
        catch (InstantiationException e) {
        }
        return (null);
}

}
