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
import java.io.OutputStream;
import java.io.ObjectOutput;
import java.io.ObjectStreamConstants;
import java.io.IOException;
import java.io.NotActiveException;
import kaffe.io.ObjectOutputStreamImpl;

public class ObjectOutputStream
  extends OutputStream
  implements ObjectOutput, ObjectStreamConstants
{

private ObjectOutputStreamImpl out;
private boolean enableReplace = false;

private Object currentObject;
private ObjectStreamClass currentStreamClass;

public ObjectOutputStream(OutputStream ot) throws IOException {
	out = ObjectStreamClass.factory.newObjectOutputStreamImpl(ot, this);
	writeStreamHeader();
}

protected void annotateClass(Class cl) throws IOException {
	// Does nothing.
}

public void close() throws IOException {
	flush();
	out.close();
}

protected final boolean enableReplaceObject(boolean enable) throws SecurityException {
	boolean old = enableReplace;
	enableReplace = enable;
	return (old);
}

public void flush() throws IOException {
	drain();
	out.flush();
}

protected Object replaceObject(Object obj) throws IOException {
	return (obj);
}

public void write(byte[] buffer) throws IOException {
	write(buffer, 0, buffer.length);
}

public void write(byte[] buffer, int offset, int count) throws IOException {
	out.write(buffer, offset, count);
}

public void writeBoolean(boolean val) throws IOException {
	out.writeBoolean(val);
}

public void writeByte(int val) throws IOException {
	out.writeByte(val);
}

public void writeBytes(String str) throws IOException {
	out.writeBytes(str);
}

public void writeChar(int val) throws IOException {
	out.writeChar(val);
}

public void writeChars(String str) throws IOException {
	out.writeChars(str);
}

public void writeDouble(double val) throws IOException {
	out.writeDouble(val);
}

public void writeFloat(float val) throws IOException {
	out.writeFloat(val);
}

public void writeInt(int val) throws IOException {
	out.writeInt(val);
}

public void writeLong(long val) throws IOException {
	out.writeLong(val);
}

public void writeShort(int val) throws IOException {
	out.writeShort(val);
}

protected void writeStreamHeader() throws IOException {
	out.writeStreamHeader();
}

public void writeUTF(String str) throws IOException {
	out.writeUTF(str);
}

public void write(int oneByte) throws IOException {
	out.write(oneByte);
}

public void reset() throws IOException {
	out.reset();
}

protected void drain() throws IOException {
	out.drain();
}

public final void writeObject(Object obj) throws IOException {
	if (enableReplace) {
		obj = replaceObject(obj);
	}
	out.writeObject(obj);
}

public final void defaultWriteObject() throws IOException {
	if (currentObject == null || currentStreamClass == null) {
                throw new NotActiveException();
        }
	out.defaultWriteObject(currentObject, currentStreamClass);
}

}
