/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.io;

import java.io.Serializable;
import java.util.Hashtable;
import kaffe.util.NotImplemented;

public class ObjectInputStream
  extends InputStream
  implements ObjectInput, ObjectStreamConstants
{

private InputStream in;
private byte[] buffer = new byte[255];
private int pos = 0;
private int len = 0;
private boolean doBlocking = false;
private Hashtable objectsDone = new Hashtable();
private int nextKey = 0x007e0000;
private Object currObject;
private ObjectStreamClass currStreamClass;
private boolean enableResolve = false;


public ObjectInputStream(InputStream inp) throws IOException, StreamCorruptedException
{
	in = inp;
	enableBuffering(false);
	readStreamHeader();
	enableBuffering(true);
}

public int available() throws IOException
{
	throw new NotImplemented();
}

public void close() throws IOException
{
	in.close();
}

protected final boolean enableResolveObject(boolean enable) throws SecurityException
{
	boolean oldResolve = enableResolve;
	enableResolve = enable;
	return (oldResolve);
}

public int read() throws IOException
{
	if (doBlocking == false) {
		return (in.read());
	}
	if (pos >= len) {
		if (in.read() != ObjectStreamConstants.TC_BLOCKDATA) {
			throw new StreamCorruptedException("expected blockdata");
		}
		int size = in.read();
		len = in.read(buffer, 0, size);
		if (len != size) {
			throw new StreamCorruptedException("bad blockdata size");
		}
		pos = 0;
	}
	return ((int)buffer[pos++] & 0xFF);
}

public int read(byte[] buffer, int offset, int count) throws IOException
{
	int i;
	for (i = 0; i < count; i++) {
		int val = read();
		if (val == -1) {
			break;
		}
		buffer[offset+i] = (byte)val;
	}
	return (i);
}

public boolean readBoolean() throws IOException
{
	if (readWithEOF() == 0) {
		return (false);
	}
	else {
		return (true);
	}
}

public byte readByte() throws IOException
{
	return ((byte)readWithEOF());
}

public int readUnsignedByte() throws IOException
{
	return (readWithEOF());
}

public char readChar() throws IOException
{
	int b1 = readWithEOF();
	int b2 = readWithEOF();
	return ((char)((b1 << 8) | b2));
}

public double readDouble() throws IOException
{
	return (Double.longBitsToDouble(readLong()));
}

public float readFloat() throws IOException
{
	return (Float.intBitsToFloat(readInt()));
}

public void readFully(byte[] buffer) throws IOException
{
	readFully(buffer, 0, buffer.length);
}

public void readFully(byte[] buffer, int offset, int count) throws IOException
{
	int cnt = 0;
	while (count > 0) {
		cnt = read(buffer, offset, count);
		if (cnt == -1) {
			throw new EOFException();
		}
		count -= cnt;
		offset += cnt;
	}
}

public int readInt() throws IOException
{
	int b1 = readWithEOF();
	int b2 = readWithEOF();
	int b3 = readWithEOF();
	int b4 = readWithEOF();
	return ((b1 << 24) | (b2 << 16) | (b3 << 8) | b4);
}

public String readLine() throws IOException
{
	throw new NotImplemented();
}

public long readLong() throws IOException
{
	long l1 = (long)readInt();
	long l2 = (long)readInt() & 0xFFFFFFFFL;
	return ((l1 << 32) | l2);
}

public short readShort() throws IOException
{
	int b1 = readWithEOF();
	int b2 = readWithEOF();
	return ((short)((b1 << 8) | b2));
}

public int readUnsignedShort() throws IOException
{
	int b1 = readWithEOF();
	int b2 = readWithEOF();
	return ((b1 << 8) | b2);
}

public final Object readObject() throws OptionalDataException, ClassNotFoundException, IOException
{
	synchronized (this) {

		boolean oldBuffering = enableBuffering(false);

		Object oldObject = currObject;
		ObjectStreamClass oldStreamClass = currStreamClass;

		int tok = readUnsignedByte();
		if (tok == ObjectStreamConstants.TC_REFERENCE) {
			Integer key = new Integer(readInt());
			currObject = objectsDone.get(key);
			if (currObject == null) {
				throw new StreamCorruptedException("reference to unknown object");
			}
		}
		else if (tok == ObjectStreamConstants.TC_NULL) {
			currObject = null;
		}
		else {
			switch (tok) {
			case ObjectStreamConstants.TC_OBJECT:
				try {
					currStreamClass = (ObjectStreamClass)readObject();
				}
				catch (ClassCastException _) {
					throw new StreamCorruptedException("expected class desc");
				}
				currObject = allocateNewObject(currStreamClass.clazz, null);
				if ((currStreamClass.method & ObjectStreamConstants.SC_EXTERNALIZABLE) != 0) {
                                        try {
                                                ((Externalizable)currObject).readExternal(this);
                                        }
                                        catch (ClassCastException _) {
                                        }
				}
				else if ((currStreamClass.method & ObjectStreamConstants.SC_WRRD_METHODS) != 0) {
					invokeObjectReader(currObject, ObjectStreamClass.class);
				}
				else if ((currStreamClass.method & ObjectStreamConstants.SC_SERIALIZABLE) != 0) {
					defaultReadObject();
				}
				else {
					throw new StreamCorruptedException("unknown method type: " + currStreamClass.method);
				}
				break;

			case ObjectStreamConstants.TC_ARRAY:
				try {
					currStreamClass = (ObjectStreamClass)readObject();
				}
				catch (ClassCastException _) {
					throw new StreamCorruptedException("expected class desc");
				}
				currObject = readArray();
				break;

			case ObjectStreamConstants.TC_CLASSDESC:
				ObjectStreamClass cls = new ObjectStreamClass();
				currObject = (Object)cls;
				invokeObjectReader(currObject, ObjectStreamClass.class);
				cls.clazz = resolveClass(cls);
				cls.buildFieldsAndOffset();
				break;

			case ObjectStreamConstants.TC_STRING:
				currObject = readUTF();
				break;

			case ObjectStreamConstants.TC_RESET:
				pos = 0;
				len = 0;
				objectsDone = new Hashtable();
				enableBuffering(true);
				break;

			default:
				throw new StreamCorruptedException("unexpected token: " + tok);
			}

			// Insert into hash table.
			Integer key = new Integer(++nextKey);
			objectsDone.put(currObject, key);
		}

		Object obj = currObject;
		if (enableResolve) {
			obj = resolveObject(obj);
		}

		currObject = oldObject;
		currStreamClass = oldStreamClass;

		enableBuffering(oldBuffering);

		return (obj);
	}
}

public final void defaultReadObject() throws IOException, ClassNotFoundException, NotActiveException
{
	readDataFields(currStreamClass, currObject);
}

private void readDataFields(ObjectStreamClass strm, Object obj)
{
	if (strm != null) {
		readDataFields(strm.superclazzStream, obj);
		inputClassFields(obj, strm.clazz, strm.fieldRdWr);
	}
}

protected void readStreamHeader() throws IOException, StreamCorruptedException
{
	expectShort(ObjectStreamConstants.STREAM_MAGIC, "bad magic nr");
	expectShort(ObjectStreamConstants.STREAM_VERSION, "bad version nr");
}

public String readUTF() throws IOException
{
        synchronized (this) {
		int len = readShort();
		byte[] buf = new byte[len];
		readFully(buf);
		return (unmakeUTF(buf));
	}
}

private Object readArray() throws IOException, ClassNotFoundException
{
	int len = readInt();
	Object obj = allocateNewArray(currStreamClass.clazz, len);
	Class elem = currStreamClass.clazz.getComponentType();

	if (elem == Object.class) {
		Object[] arr = (Object[])obj;
		for (int i = 0; i < len; i++) {
			arr[i] = readObject();
		}
	}
	else if (elem == Character.TYPE) {
		char[] arr = (char[])obj;
		for (int i = 0; i < len; i++) {
			arr[i] = readChar();
		}
	}
	else if (elem == Short.TYPE) {
		short[] arr = (short[])obj;
		for (int i = 0; i < len; i++) {
			arr[i] = readShort();
		}
	}
	else if (elem == Integer.TYPE) {
		int[] arr = (int[])obj;
		for (int i = 0; i < len; i++) {
			arr[i] = readInt();
		}
	}
	else if (elem == Boolean.TYPE) {
		boolean[] arr = (boolean[])obj;
		for (int i = 0; i < len; i++) {
			arr[i] = readBoolean();
		}
	}
	else if (elem == Long.TYPE) {
		long[] arr = (long[])obj;
		for (int i = 0; i < len; i++) {
			arr[i] = readLong();
		}
	}
	else if (elem == Double.TYPE) {
		double[] arr = (double[])obj;
		for (int i = 0; i < len; i++) {
			arr[i] = readDouble();
		}
	}
	else if (elem == Float.TYPE) {
		float[] arr = (float[])obj;
		for (int i = 0; i < len; i++) {
			arr[i] = readFloat();
		}
	}

	return (obj);
}

public synchronized void registerValidation(ObjectInputValidation obj, int prio) throws NotActiveException, InvalidObjectException
{
	throw new NotImplemented();
}

protected Class resolveClass(ObjectStreamClass desc) throws IOException, ClassNotFoundException
{
	Class cls = loadClass0(null, desc.getName());
	return (cls);
}

protected Object resolveObject(Object obj) throws IOException
{
	return (obj);
}

public int skipBytes(int count) throws IOException
{
	throw new NotImplemented();
}

// ---------------------------------------------------------------------------

private String unmakeUTF(byte[] buf) throws UTFDataFormatException
{
	StringBuffer buffer = new StringBuffer();
	for (int pos = 0; pos < buf.length; ) {
                int data = buf[pos++] & 0xFF;

                if ((data & 0x80) == 0x80) {
                        /* Hi-bit set, multi byte char */
                        if ((data & 0xE0) == 0xC0) {
                                /* Valid 2 byte string '110' */
                                int data2 = buf[pos++] & 0xFF;
                                if ((data2 & 0xC0) == 0x80) {
                                        /* Valid 2nd byte */
                                        char toAdd = (char)(((data & 0x1F) << 6) | (data2 & 0x3F));
                                        buffer.append(toAdd);
                                }
                                else {
					throw new UTFDataFormatException();
				}
                        }
			else if ((data & 0xF0) == 0xE0) {
                                /* Valid 3 byte string '1110' */
                                int data2 = buf[pos++] & 0xFF;
                                if ((data2 & 0xC0) == 0x80) {
                                        /* Valid 2nd byte */
                                        int data3 = buf[pos++] & 0xFF;
                                        if ((data3 & 0xC0) == 0x80) {
                                                /* Valid 3rd byte */
                                                char toAdd = (char)(((data & 0x0F) << 12) | ((data2 & 0x3F) << 6) | (data3 & 0x3F));
                                                buffer.append(toAdd);
                                        }
                                        else {
						throw new UTFDataFormatException();
					}
                                }
                                else {
					throw new UTFDataFormatException();
				}
                        }
                        else {
				throw new UTFDataFormatException();
			}
                }
                else {
                        buffer.append((char)data);
                }
	}
	return (buffer.toString());
}

int readWithEOF() throws IOException
{
	int val = read();
	if (val == -1) {
		throw new EOFException();
	}
	return (val);
}

void expectShort(short val, String mess) throws IOException
{
	int b1 = readWithEOF();
	int b2 = readWithEOF();
	if (val != (short)((b1<<8)|b2)) {
		throw new StreamCorruptedException(mess);
	}
}

void expectByte(byte val, String mess) throws IOException
{
	int b1 = readWithEOF();
	if (val != (byte)b1) {
		throw new StreamCorruptedException(mess);
	}
}

private boolean enableBuffering(boolean flag) throws IOException
{
        if (flag != doBlocking) {
		if (pos < len) {
			throw new StreamCorruptedException("blockdata buffer not empty");
		}
                doBlocking = flag;
                return (!flag);
        }
        return (flag);
}

native private Class loadClass0(Class clazz, String name);
native private void inputClassFields(Object obj, Class clazz, int[] arr);
native private static Object allocateNewObject(Class clazz1, Class clazz2);
native private static Object allocateNewArray(Class clazz, int size);
native private boolean invokeObjectReader(Object obj, Class clazz);

}
