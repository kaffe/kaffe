/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.rmi.server;

import java.io.ObjectStreamClass;
import java.io.ObjectInputStream;
import java.io.InputStream;
import java.io.IOException;
import java.net.URL;
import java.net.MalformedURLException;
import java.rmi.server.RMIClassLoader;

public class RMIObjectInputStream
	extends ObjectInputStream {

UnicastConnectionManager manager;

public RMIObjectInputStream(InputStream strm, UnicastConnectionManager man) throws IOException {
	super(strm);
	manager = man;
	enableResolveObject(true);
}

protected Class resolveClass(ObjectStreamClass desc) throws IOException, ClassNotFoundException {
//System.out.println("Resolving class: " + desc.getName());
	String annotation = (String)readObject();
	if (annotation == null) {
		return (super.resolveClass(desc));
	}
	else {
		try {
			return (RMIClassLoader.loadClass(new URL(annotation), desc.getName()));
		}
		catch (MalformedURLException _) {
			throw new ClassNotFoundException(desc.getName());
		}
	}
}

}
