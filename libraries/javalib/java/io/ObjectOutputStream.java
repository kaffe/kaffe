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

public class ObjectOutputStream
  extends OutputStream
  implements ObjectOutput, ObjectStreamConstants
{

private Object currObject;
private ObjectStreamClass currStreamClass;

private OutputStream out;
private byte[] outBuf = new byte[255];
private int pos = 0;
private Hashtable objectsDone = new Hashtable();
private int nextKey = 0x007e0000;
private boolean doBlocking = false;
private boolean enableReplace = false;

public ObjectOutputStream(OutputStream ot) throws IOException
{
	out = ot;
	enableBuffering(false);
	writeStreamHeader();
	enableBuffering(true);
}

protected void annotateClass(Class cl) throws IOException
{
	// Does nothing by default
}

public void close() throws IOException
{
	flush();
	out.close();
}

protected void drain() throws IOException
{
	synchronized(this) {
		if (pos > 0) {
			if (doBlocking == true) {
				out.write(ObjectStreamConstants.TC_BLOCKDATA);
				out.write(pos);
			}
			out.write(outBuf, 0, pos);
			pos = 0;
		}
	}
}

protected final boolean enableReplaceObject(boolean enable) throws SecurityException
{
	boolean oldEnable = enableReplace;
	enableReplace = enable;
	return (oldEnable);
}

public void flush() throws IOException
{
	drain();
	out.flush();
}

protected Object replaceObject(Object obj) throws IOException
{
	return (obj);
}

public void reset() throws IOException
{
	if (currObject != null) {
		throw new IOException("currently serializing object");
	}
	enableBuffering(false);
	objectsDone = new Hashtable();
	pos = 0;
	writeByte(ObjectStreamConstants.TC_RESET);
	enableBuffering(false);
}

public void write(byte[] buffer) throws IOException
{
	write(buffer, 0, buffer.length);
}

public void write(byte[] buffer, int offset, int count) throws IOException
{
	synchronized(this) {
		for (int i = 0; i < count; i++) {
			write(buffer[offset+i]);
		}
	}
}

public void writeBoolean(boolean val) throws IOException
{
	if (val) {
		write(1);
	}
	else {
		write(0);
	}
}

public void writeByte(int val) throws IOException
{
	write(val);
}

public void writeBytes(String str) throws IOException
{
	char[] buf = str.toCharArray();
	int len = buf.length;
	synchronized(this) {
		for (int i = 0; i < len; i++) {
			write((int)(buf[i] & 0xFF));
		}
	}
}

public void writeChar(int val) throws IOException
{
	synchronized(this) {
		write((val >> 8) & 0xFF);
		write(val & 0xFF);
	}
}

public void writeChars(String str) throws IOException
{
	char[] buf = str.toCharArray();
	synchronized(this) {
		for (int i = 0; i < buf.length; i++) {
			write((buf[i] >> 8) & 0xFF);
			write(buf[i] & 0xFF);
		}
	}
}

public void writeDouble(double val) throws IOException
{
	long nval = Double.doubleToLongBits(val);
	synchronized (this) {
		write((int)(nval >> 56) & 0xFF);
		write((int)(nval >> 48) & 0xFF);
		write((int)(nval >> 40) & 0xFF);
		write((int)(nval >> 32) & 0xFF);
		write((int)(nval >> 24) & 0xFF);
		write((int)(nval >> 16) & 0xFF);
		write((int)(nval >> 8) & 0xFF);
		write((int)nval & 0xFF);
	}
}

public void writeFloat(float val) throws IOException
{
	int nval = Float.floatToIntBits(val);
	synchronized (this) {
		write((nval >> 24) & 0xFF);
		write((nval >> 16) & 0xFF);
		write((nval >> 8) & 0xFF);
		write(nval & 0xFF);
	}
}

public void writeInt(int val) throws IOException
{
	synchronized (this) {
		write((val >> 24) & 0xFF);
		write((val >> 16) & 0xFF);
		write((val >> 8) & 0xFF);
		write(val & 0xFF);
	}
}

public void writeLong(long val) throws IOException
{
	synchronized (this) {
		write((int)(val >> 56) & 0xFF);
		write((int)(val >> 48) & 0xFF);
		write((int)(val >> 40) & 0xFF);
		write((int)(val >> 32) & 0xFF);
		write((int)(val >> 24) & 0xFF);
		write((int)(val >> 16) & 0xFF);
		write((int)(val >> 8) & 0xFF);
		write((int)val & 0xFF);
	}
}

public void writeShort(int val) throws IOException
{
	synchronized (this) {
		write((val >> 8) & 0xFF);
		write(val & 0xFF);
	}
}

public final void writeObject(Object obj) throws IOException
{
	synchronized (this) {

		if (enableReplace) {
			obj = replaceObject(obj);
		}

		drain();	/* Drain any pending data first */

		boolean oldBuffering = enableBuffering(false);

		if (obj == null) {
			writeByte(ObjectStreamConstants.TC_NULL);
			enableBuffering(oldBuffering);
			return;
		}

		Object oldObject = currObject;
		ObjectStreamClass oldStreamClass = currStreamClass;

		currObject = obj;
		Class cls = obj.getClass();

		currStreamClass = ObjectStreamClass.lookup(cls);
		if (currStreamClass != null) {

			// Have we serialized this object before?
			Integer key = (Integer)objectsDone.get(obj);
			if (key != null) {
				writeByte(ObjectStreamConstants.TC_REFERENCE);
				writeInt(key.intValue());
			}
			// Otherwise we'd better do it now.
			else {
				if (obj instanceof ObjectStreamClass) {
					writeByte(ObjectStreamConstants.TC_CLASSDESC);
				}
				else if (obj instanceof String) {
					writeByte(ObjectStreamConstants.TC_STRING);
				}
				else if (cls.isArray()) {
					writeByte(ObjectStreamConstants.TC_ARRAY);
					writeObject(currStreamClass);
				}
				else {
					writeByte(ObjectStreamConstants.TC_OBJECT);
					writeObject(currStreamClass);
				}

				// Insert into hash table so we don't do it again.
				key = new Integer(++nextKey);
				objectsDone.put(obj, key);

				switch (currStreamClass.method) {
				case ObjectStreamConstants.SC_EXTERNALIZABLE:
					try {
						((Externalizable)obj).writeExternal(this);
					}
					catch (ClassCastException _) {
					}
					break;

				case ObjectStreamConstants.SC_WRRD_METHODS:
					invokeObjectWriter(obj, cls);
					break;

				case ObjectStreamConstants.SC_SERIALIZABLE:
					defaultWriteObject();
					break;

				case ObjectStreamConstants.SC_STRING:
					writeUTF((String)obj);
					break;

				default:
					throw new StreamCorruptedException("unknown method type");
				}
				// Write any annotations.
				annotateClass(currStreamClass.clazz);
			}

		}

		currObject = oldObject;
		currStreamClass = oldStreamClass;

		enableBuffering(oldBuffering);
	}
}

public final void defaultWriteObject() throws IOException
{
	synchronized (this) {
		if (currObject == null) {
			throw new NotActiveException();
		}
		if (currStreamClass.clazz.isArray()) {
			writeArray(currStreamClass, currObject);
		}
		else {
			writeDataFields(currStreamClass, currObject);
		}
	}
}

private void writeDataFields(ObjectStreamClass strm, Object obj)
{
	if (strm == null) {
		return;
	}
	writeDataFields(strm.superclazzStream, obj);
	outputClassFields(obj, strm.clazz, strm.fieldRdWr);
}

private void writeArray(ObjectStreamClass strm, Object obj) throws IOException
{
	Class elem = strm.clazz.getComponentType();

	if (elem == Object.class) {
		Object[] arr = (Object[])obj;
		int len = arr.length;
		writeInt(len);
		for (int i = 0; i < len; i++) {
			writeObject(arr[i]);
		}
	}
	else if (elem == Character.TYPE) {
		char[] arr = (char[])obj;
		int len = arr.length;
		writeInt(len);
		for (int i = 0; i < len; i++) {
			writeChar(arr[i]);
		}
	}
	else if (elem == Short.TYPE) {
		short[] arr = (short[])obj;
		int len = arr.length;
		writeInt(len);
		for (int i = 0; i < len; i++) {
			writeShort(arr[i]);
		}
	}
	else if (elem == Integer.TYPE) {
		int[] arr = (int[])obj;
		int len = arr.length;
		writeInt(len);
		for (int i = 0; i < len; i++) {
			writeInt(arr[i]);
		}
	}
	else if (elem == Boolean.TYPE) {
		boolean[] arr = (boolean[])obj;
		int len = arr.length;
		writeInt(len);
		for (int i = 0; i < len; i++) {
			writeBoolean(arr[i]);
		}
	}
	else if (elem == Long.TYPE) {
		long[] arr = (long[])obj;
		int len = arr.length;
		writeInt(len);
		for (int i = 0; i < len; i++) {
			writeLong(arr[i]);
		}
	}
	else if (elem == Double.TYPE) {
		double[] arr = (double[])obj;
		int len = arr.length;
		writeInt(len);
		for (int i = 0; i < len; i++) {
			writeDouble(arr[i]);
		}
	}
	else if (elem == Float.TYPE) {
		float[] arr = (float[])obj;
		int len = arr.length;
		writeInt(len);
		for (int i = 0; i < len; i++) {
			writeFloat(arr[i]);
		}
	}
}

protected void writeStreamHeader() throws IOException
{
	synchronized (this) {
		writeShort(ObjectStreamConstants.STREAM_MAGIC);
		writeShort(ObjectStreamConstants.STREAM_VERSION);
	}
}

public void writeUTF(String str) throws IOException
{
	String result = makeUTF(str);
	synchronized (this) {
		writeShort(result.length());
		writeBytes(result);
	}
}

public void write(int oneByte) throws IOException
{
	synchronized (this) {
		outBuf[pos] = (byte)oneByte;
		pos++;
		if (pos == outBuf.length) {
			drain();
		}
	}
}

private static String makeUTF(String str)
{
	char buf[] = str.toCharArray();
	StringBuffer out = new StringBuffer();

	for (int i = 0; i < buf.length; i++) {
		char chr = buf[i];
		if (chr >= '\u0001' && chr <= '\u007F') {
			out.append(chr);
		}
		else if ((chr >= '\u0080' && chr <= '\u07FF') | chr=='\u0000') {
			out.append(((chr & 0x07C0) >> 6) | 0xC0);
			out.append((chr & 0x003F) | 0x80);
		}
		else {
			out.append(((chr & 0xF000) >> 12) | 0xE0);
			out.append(((chr & 0x0FC0) >> 6) | 0x80);
			out.append((chr & 0x003F) | 0x80);
		}
	}

	return (out.toString());
}

private boolean enableBuffering(boolean flag) throws IOException
{
	if (flag != doBlocking) {
		drain();
		doBlocking = flag;
		return (!flag);
	}
	return (flag);
}

private native boolean invokeObjectWriter(Object obj, Class cls);
private native void outputClassFields(Object obj, Class cls, int[] types);

}
