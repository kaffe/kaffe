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
import java.util.Hashtable;
import java.io.IOException;
import java.io.InputStream;
import java.io.ObjectInput;
import java.io.ObjectStreamConstants;
import java.io.StreamCorruptedException;
import java.io.ObjectInputValidation;
import java.io.NotActiveException;
import java.io.OptionalDataException;
import java.io.InvalidObjectException;
import kaffe.io.ObjectInputStreamImpl;

public class ObjectInputStream
  extends InputStream
  implements ObjectInput, ObjectStreamConstants
{

static public abstract class GetField {

abstract public boolean defaulted(String fname) throws IOException, IllegalArgumentException;
abstract public boolean get(String fname, boolean defvalue) throws IOException, IllegalArgumentException;
abstract public byte get(String fname, byte defvalue) throws IOException, IllegalArgumentException;
abstract public char get(String fname, char defvalue) throws IOException, IllegalArgumentException;
abstract public short get(String fname, short defvalue) throws IOException, IllegalArgumentException;
abstract public int get(String fname, int defvalue) throws IOException, IllegalArgumentException;
abstract public float get(String fname, float defvalue) throws IOException, IllegalArgumentException;
abstract public long get(String fname, long defvalue) throws IOException, IllegalArgumentException;
abstract public double get(String fname, double defvalue) throws IOException, IllegalArgumentException;
abstract public Object get(String fname, Object defvalue) throws IOException, IllegalArgumentException;
abstract public ObjectStreamClass getObjectStreamClass();

}

private ObjectInputStreamImpl in;
private boolean enableResolve = false;

private Object currentObject;
private ObjectStreamClass currentStreamClass;

public ObjectInputStream(InputStream inp) throws IOException, StreamCorruptedException
{
	in = ObjectStreamClass.factory.newObjectInputStreamImpl(inp, this);
	readStreamHeader();
}

public int available() throws IOException
{
	return (in.available());
}

public void close() throws IOException
{
	in.close();
}

protected boolean enableResolveObject(boolean enable) throws SecurityException
{
	boolean old = enableResolve;
	enableResolve = enable;
	return (old);
}

public int read() throws IOException
{
	return (in.read());
}

public int read(byte[] buffer, int offset, int count) throws IOException
{
	return (in.read(buffer, offset, count));
}

public boolean readBoolean() throws IOException
{
	return (in.readBoolean());
}

public byte readByte() throws IOException
{
	return (in.readByte());
}

public int readUnsignedByte() throws IOException
{
	return (in.readUnsignedByte());
}

public char readChar() throws IOException
{
	return (in.readChar());
}

public double readDouble() throws IOException
{
	return (in.readDouble());
}

public float readFloat() throws IOException
{
	return (in.readFloat());
}

public void readFully(byte[] buffer) throws IOException
{
	readFully(buffer, 0, buffer.length);
}

public void readFully(byte[] buffer, int offset, int count) throws IOException
{
	in.readFully(buffer, offset, count);
}

public int readInt() throws IOException
{
	return (in.readInt());
}

public String readLine() throws IOException
{
	return (in.readLine());
}

public long readLong() throws IOException
{
	return (in.readLong());
}

public short readShort() throws IOException
{
	return (in.readShort());
}

public int readUnsignedShort() throws IOException
{
	return (in.readUnsignedShort());
}

public String readUTF() throws IOException
{
	return (in.readUTF());
}

protected void readStreamHeader() throws IOException, StreamCorruptedException
{
	in.readStreamHeader();
}

public synchronized void registerValidation(ObjectInputValidation obj, int prio) throws NotActiveException, InvalidObjectException
{
	throw new kaffe.util.NotImplemented();
}

protected Object resolveObject(Object obj) throws IOException
{
	return (obj);
}

public int skipBytes(int count) throws IOException
{
	return (in.skipBytes(count));
}

protected Class resolveClass(ObjectStreamClass desc) throws IOException, ClassNotFoundException
{
	return (Class.forName(desc.getName()));
}

/**
 * Not part of the public interface.
 */
public final Class resolveClassInternal(ObjectStreamClass desc) throws IOException, ClassNotFoundException
{
        return (resolveClass(desc));
}

public final Object readObject() throws OptionalDataException, ClassNotFoundException, IOException
{
	Object obj = in.readObject();
	if (enableResolve) {
		obj = resolveObject(obj);
	}
	return (obj);
}

public void defaultReadObject() throws IOException, ClassNotFoundException, NotActiveException
{
	if (currentObject == null || currentStreamClass == null) {
		throw new NotActiveException();
	}
	in.defaultReadObject(currentObject, currentStreamClass);
}

public ObjectInputStream.GetField readFields() throws IOException, ClassNotFoundException, NotActiveException {
	throw new kaffe.util.NotImplemented();
}

}
