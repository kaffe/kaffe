/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.io;

import java.lang.String;
import java.io.Serializable;
import java.io.Externalizable;
import java.util.Hashtable;
import java.io.ObjectStreamConstants;
import java.io.StreamCorruptedException;
import java.io.IOException;
import java.io.OptionalDataException;
import java.io.ObjectStreamClass;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import kaffe.io.ObjectInputStreamImpl;
import kaffe.io.ObjectOutputStreamImpl;

public class ObjectStreamClassImpl extends ObjectStreamClass {

private static Hashtable streamClasses = new Hashtable();

private int method;
private long serialVersionUID;
private ObjectStreamClassImpl superstream;
private Class iclazz;
private kaffe.util.Ptr[] fields;

static {
	init();
}

public ObjectStreamClassImpl(Class cl, int meth) {
	name = cl.getName();
	clazz = cl;
	method = meth;
	serialVersionUID = getSerialVersionUID0(cl);
	superstream = (ObjectStreamClassImpl)lookup(cl.getSuperclass());
	iclazz = null;
}

public ObjectStreamClassImpl() {
	clazz = ObjectStreamClassImpl.class;
	name = clazz.getName();
	method = ObjectStreamConstants.SC_WRITE_METHOD|ObjectStreamConstants.SC_SERIALIZABLE;
	serialVersionUID = 0;
	superstream = null;
	iclazz = null;
}

public Object getObject(ObjectInputStream in, ObjectInputStreamImpl impl) throws StreamCorruptedException, OptionalDataException {
	Object obj = allocateNewObject();
	impl.makeObjectReference(obj);
	getObjectWithoutSuper(obj, in, impl);
	return (obj);
}

public void getObjectWithoutSuper(Object obj, ObjectInputStream in, ObjectInputStreamImpl impl) throws StreamCorruptedException, OptionalDataException {

	// Get the data in our superclasses first (if they're serializable)
	if (superstream != null) {
		superstream.getObjectWithoutSuper(obj, in, impl);
	}

        try {
                if ((method & ObjectStreamConstants.SC_EXTERNALIZABLE) != 0) {
                        ((Externalizable)obj).readExternal(in);
                }
                else if ((method & ObjectStreamConstants.SC_WRITE_METHOD) != 0) {
			boolean restore = impl.enableBuffering(true);
                        invokeObjectReader0(obj, in);
			impl.enableBuffering(restore);
			getEndOfDataBlock(in, impl);
                }
                else if ((method & ObjectStreamConstants.SC_SERIALIZABLE) != 0) {
                        defaultReadObject(obj, in);
                }
                else {
                        throw new StreamCorruptedException("unknown method type");
                }
        }
        catch (ClassCastException e1) {
		e1.printStackTrace();
		throw new StreamCorruptedException("error getting object: " + e1);
        }
        catch (ClassNotFoundException e2) {
		throw new StreamCorruptedException("error getting object: " + e2);
        }
        catch (IOException e3) {
		throw new StreamCorruptedException("error getting object: " + e3);
        }
}

private void getEndOfDataBlock(ObjectInputStream in, ObjectInputStreamImpl impl) throws StreamCorruptedException {
	try {
		if (impl.readByte() == ObjectStreamConstants.TC_ENDBLOCKDATA) {
			return;
		}
	}
	catch (IOException _) {
	}
	throw new StreamCorruptedException("failed to read endblockdata");
}

public Object getClass(ObjectInputStream in, ObjectInputStreamImpl impl) throws StreamCorruptedException, OptionalDataException {
        try {
//System.out.println("Getting class");
                Object obj = allocateNewObject();
		impl.makeObjectReference(obj);
		invokeObjectReader0(obj, in);
		// Get any class annotation
		getEndOfDataBlock(in, impl);
		// Get the superclass stream.
//System.out.println("Getting superclass");
		((ObjectStreamClassImpl)obj).superstream = (ObjectStreamClassImpl)in.readObject();
//System.out.println("Done");
                return (obj);
        }
        catch (IOException e1) {
		throw new StreamCorruptedException("error getting object: " + e1);
        }
	catch (ClassNotFoundException e2) {
		throw new StreamCorruptedException("error getting object: " + e2);
	}
}

/**
 * Not part of the public interface.
 */
public Object getArray(ObjectInputStream in, ObjectInputStreamImpl impl) throws StreamCorruptedException {
        try {
                Class elem = clazz.getComponentType();
                int len = in.readInt();
                Object obj = allocateNewArray(len);
		impl.makeObjectReference(obj);

                if (elem == Character.TYPE) {
                        char[] arr = (char[])obj;
                        for (int i = 0; i < len; i++) {
                                arr[i] = in.readChar();
                        }
                }
                else if (elem == Short.TYPE) {
                        short[] arr = (short[])obj;
                        for (int i = 0; i < len; i++) {
                                arr[i] = in.readShort();
                        }
                }
                else if (elem == Integer.TYPE) {
                        int[] arr = (int[])obj;
                        for (int i = 0; i < len; i++) {
                                arr[i] = in.readInt();
                        }
                }
                else if (elem == Boolean.TYPE) {
                        boolean[] arr = (boolean[])obj;
                        for (int i = 0; i < len; i++) {
                                arr[i] = in.readBoolean();
                        }
                }
                else if (elem == Long.TYPE) {
                        long[] arr = (long[])obj;
                        for (int i = 0; i < len; i++) {
                                arr[i] = in.readLong();
                        }
                }
                else if (elem == Double.TYPE) {
                        double[] arr = (double[])obj;
                        for (int i = 0; i < len; i++) {
                                arr[i] = in.readDouble();
                        }
                }
                else if (elem == Float.TYPE) {
                        float[] arr = (float[])obj;
                        for (int i = 0; i < len; i++) {
                                arr[i] = in.readFloat();
                        }
                }
                else if (elem == Byte.TYPE) {
                        byte[] arr = (byte[])obj;
                        for (int i = 0; i < len; i++) {
                                arr[i] = in.readByte();
                        }
                }
                else { // Must be an array of objects.
                        Object[] arr = (Object[])obj;
                        for (int i = 0; i < len; i++) {
                                arr[i] = in.readObject();
                        }
                }
                return (obj);
        }
        catch (ClassCastException e1) {
		e1.printStackTrace();
		throw new StreamCorruptedException("bad array: " + e1);
        }
        catch (ClassNotFoundException e2) {
		throw new StreamCorruptedException("bad array: " + e2);
        }
        catch (IOException e3) {
		throw new StreamCorruptedException("bad array: " + e3);
        }
}

/**
 * Not part of the public interface.
 */
public void putObject(Object obj, ObjectOutputStream out, ObjectOutputStreamImpl impl) throws IOException {
	impl.makeObjectReference(obj);
	putObjectWithoutSuper(obj, out, impl);
}

public void putObjectWithoutSuper(Object obj, ObjectOutputStream out, ObjectOutputStreamImpl impl) throws IOException {
	// Put the data in our superclasses first (if they're serializable)
	if (superstream != null) {
		superstream.putObjectWithoutSuper(obj, out, impl);
	}

	if ((method & ObjectStreamConstants.SC_EXTERNALIZABLE) != 0) {
//System.out.println("...writeExternal");
		((Externalizable)obj).writeExternal(out);
	}
	else if ((method & ObjectStreamConstants.SC_WRITE_METHOD) != 0) {
//System.out.println("...invokeObjectWriter");
		boolean restore = impl.enableBuffering(true);
		invokeObjectWriter0(obj, out);
		impl.enableBuffering(restore);
		impl.writeByte(ObjectStreamConstants.TC_ENDBLOCKDATA);
	}
	else if ((method & ObjectStreamConstants.SC_SERIALIZABLE) != 0) {
//System.out.println("...defaultWriteObject");
		defaultWriteObject(obj, out);
	}
	else {
		throw new StreamCorruptedException("unknown method type");
	}
}

public void putClass(Object obj, ObjectOutputStream out, ObjectOutputStreamImpl impl) throws IOException {
	impl.makeObjectReference(obj);
	invokeObjectWriter0(obj, out);
	out.annotateClassInternal(((ObjectStreamClassImpl)obj).clazz);
	impl.writeByte(ObjectStreamConstants.TC_ENDBLOCKDATA);
}

/**
 * Not part of the public interface.
 */
public void putArray(Object obj, ObjectOutputStream out, ObjectOutputStreamImpl impl) throws IOException {
	Class elem = obj.getClass().getComponentType();

	impl.makeObjectReference(obj);

        if (elem == Character.TYPE) {
                char[] arr = (char[])obj;
                int len = arr.length;
                out.writeInt(len);
                for (int i = 0; i < len; i++) {
                        out.writeChar(arr[i]);
                }
        }
        else if (elem == Byte.TYPE) {
                byte[] arr = (byte[])obj;
                int len = arr.length;
                out.writeInt(len);
                for (int i = 0; i < len; i++) {
                        out.writeByte(arr[i]);
                }
        }
        else if (elem == Short.TYPE) {
                short[] arr = (short[])obj;
                int len = arr.length;
                out.writeInt(len);
                for (int i = 0; i < len; i++) {
                        out.writeShort(arr[i]);
                }
        }
        else if (elem == Integer.TYPE) {
                int[] arr = (int[])obj;
                int len = arr.length;
                out.writeInt(len);
                for (int i = 0; i < len; i++) {
                        out.writeInt(arr[i]);
                }
        }
        else if (elem == Boolean.TYPE) {
                boolean[] arr = (boolean[])obj;
                int len = arr.length;
                out.writeInt(len);
                for (int i = 0; i < len; i++) {
                        out.writeBoolean(arr[i]);
                }
        }
        else if (elem == Long.TYPE) {
                long[] arr = (long[])obj;
                int len = arr.length;
                out.writeInt(len);
                for (int i = 0; i < len; i++) {
                        out.writeLong(arr[i]);
                }
        }
        else if (elem == Double.TYPE) {
                double[] arr = (double[])obj;
                int len = arr.length;
                out.writeInt(len);
                for (int i = 0; i < len; i++) {
                        out.writeDouble(arr[i]);
                }
        }
        else if (elem == Float.TYPE) {
                float[] arr = (float[])obj;
                int len = arr.length;
                out.writeInt(len);
                for (int i = 0; i < len; i++) {
                        out.writeFloat(arr[i]);
                }
        }
        else { // Must be an array of objects
                Object[] arr = (Object[])obj;
                int len = arr.length;
                out.writeInt(len);
                for (int i = 0; i < len; i++) {
                        out.writeObject(arr[i]);
                }
        }
}

public void defaultReadObject(Object obj, ObjectInputStream in) {
	inputClassFields(obj, in);
}

public void defaultWriteObject(Object obj, ObjectOutputStream out) {
	outputClassFields(obj, out);
}

private void readObject(ObjectInputStream in) throws IOException, ClassNotFoundException {
        name = in.readUTF();
//System.out.println("name = " + name);
        serialVersionUID = in.readLong();
//System.out.println("serialVersionUID = " + serialVersionUID);
        method = in.readUnsignedByte();
//System.out.println("method = " + method);

        int len = in.readShort();
//System.out.println("len = " + len);
        for (int i = 0; i < len; i++) {
                int type = in.readUnsignedByte();
		String tname = in.readUTF();
//System.out.println("  " + tname);
		// int tmod = in.readShort();
                if (type == 'L' || type == '[') {
                        try {
                                String stype = (String)in.readObject();
                        }
                        catch (ClassNotFoundException _) {
                                throw new StreamCorruptedException("bad type");
                        }
                }
        }

	// Resolve the class
	clazz = in.resolveClassInternal(this);

	// Check for matching serial version UIDs
	long localUID = getSerialVersionUID0(clazz);
	if (serialVersionUID != localUID) {
		throw new StreamCorruptedException(
			"mismatched serial version UIDs: expected " 
			+ serialVersionUID 
			+ " but local class `" + clazz.getName() 
			+ "' has UID " + localUID);
	}
}

private void writeObject(ObjectOutputStream out) throws IOException {
//System.out.println("ObjectStreamClassImpl.writeObject: " + name);
//System.out.println("...writeUTF");
        out.writeUTF(name);
//System.out.println("...writeLong");
        out.writeLong(serialVersionUID);
//System.out.println("...writeByte");
        out.writeByte(method);
//System.out.println("...writeFields");
	outputClassFieldInfo(out);
//System.out.println("...Done");
}

public long getSerialVersionUID() {
	return (getSerialVersionUID0(clazz));
}

public static boolean hasWriteObject(Class cls) {
	return (hasWriteObject0(cls));
}

private native static void init();
private native Object allocateNewObject();
private native Object allocateNewArray(int size);
private native static boolean hasWriteObject0(Class cls);
private native void inputClassFields(Object obj, ObjectInputStream in);
private native void outputClassFields(Object obj, ObjectOutputStream out);
private native boolean invokeObjectReader0(Object obj, ObjectInputStream in);
private native boolean invokeObjectWriter0(Object obj, ObjectOutputStream out);
private native void outputClassFieldInfo(ObjectOutputStream out);
private native static long getSerialVersionUID0(Class cls);

}
