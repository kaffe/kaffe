/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package kaffe.rmi.server;

import java.io.ObjectOutputStream;
import java.io.OutputStream;
import java.io.IOException;
import java.rmi.server.RMIClassLoader;

public class RMIObjectOutputStream
	extends ObjectOutputStream {

public RMIObjectOutputStream(OutputStream strm) throws IOException {
	super(strm);
}

protected void annotateClass(Class cls) throws IOException {
//System.out.println("Annotating class: " + cls);
	writeObject(RMIClassLoader.getClassAnnotation(cls));
}

}
