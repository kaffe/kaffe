/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.io;

import java.lang.String;
import java.io.Serializable;
import java.io.Externalizable;
import java.util.Hashtable;
import java.io.ObjectStreamConstants;
import java.io.StreamCorruptedException;
import java.io.IOException;
import java.io.OptionalDataException;
import kaffe.io.ObjectInputStreamImpl;
import kaffe.io.ObjectOutputStreamImpl;
import kaffe.io.ObjectStreamClassImpl;
import kaffe.io.SerializationFactory;
import kaffe.io.DefaultSerializationFactory;

public class ObjectStreamClass implements Serializable {

private static final long serialVersionUID = -6120832682080437368L;

public static SerializationFactory factory = new DefaultSerializationFactory();

private static Hashtable streamClasses = new Hashtable();

protected String name;
protected Class clazz;

public Class forClass() {
	return (clazz);
}

public String getName() {
	return (name);
}

public String toString() {
	return (name + ": static final long serialVersionUID = " + Long.toString(getSerialVersionUID()));
}

public static ObjectStreamClass lookup(Class cl) {

	if (cl == null) {
		return (null);
	}

//System.out.println("Looking up " + cl);
	// First check hash table for match - return what's found
	ObjectStreamClass osc = (ObjectStreamClass)streamClasses.get(cl);
	if (osc != null) {
//System.out.println("Found in cache");
		return (osc);
	}

	// Otherwise we work out how the class should be serialized and
	// make an entry for it.  If the class can't be serialized we
	// make a null entry.
	int method;
	if (Externalizable.class.isAssignableFrom(cl)) {
		method = ObjectStreamConstants.SC_SERIALIZABLE | ObjectStreamConstants.SC_EXTERNALIZABLE;
//System.out.println(" is externalizable");
	}
	else if (Serializable.class.isAssignableFrom(cl)) {
		method = ObjectStreamConstants.SC_SERIALIZABLE;
		if (factory.hasRdWrMethods(cl)) {
			method |= ObjectStreamConstants.SC_WRITE_METHOD;
//System.out.println(" has read/write");
                }
//System.out.println(" is serializable");
        }
	else {
//System.out.println("Cannot be serialized");
		// Cannot be serialized
		return (null);
        }

	osc = factory.newObjectStreamClass(cl, method);
	streamClasses.put(cl, osc);

	return (osc);
}

/**
 * The following functions are overridden by the implementation.
 */

public void defaultReadObject(Object obj, ObjectInputStream in) {
	// These are redefined by the implementation
}

public void defaultWriteObject(Object obj, ObjectOutputStream out) {
	// These are redefined by the implementation
}

public long getSerialVersionUID() {
	// These are redefined by the implementation
	return (0);
}

}
