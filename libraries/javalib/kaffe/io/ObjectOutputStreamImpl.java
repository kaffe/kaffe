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

import java.io.DataOutputStream;
import java.io.OutputStream;
import java.io.IOException;
import java.io.ObjectStreamConstants;
import java.io.ObjectOutputStream;
import java.io.ObjectStreamClass;
import java.io.NotSerializableException;
import kaffe.io.ObjectStreamClassImpl;
import kaffe.util.IdentityHashtable;

public class ObjectOutputStreamImpl extends DataOutputStream {

private static ObjectStreamClassImpl classDesc = new ObjectStreamClassImpl();

private boolean buffering = false;

private ObjectOutputStream parent;

private int nextKey = 0x007e0000;
private IdentityHashtable objectReferences = new IdentityHashtable();

/*
 * XXX this is wrong.
 *
 * It does not handle the case of "write(new byte[2000]);"
 * The problem is that the stream is not put in buffering mode by default.
 * However, the BLOCKDATA token preceding plain data is only written if
 * buffering is set.  See flush()
 */
class Output extends OutputStream {

private OutputStream out;
private byte[] buffer = new byte[255];
private int pos = 0;

Output(OutputStream out) {
	this.out = out;
}

public void write(int byteVal) throws IOException {
	buffer[pos] = (byte)byteVal;
	pos++;
	if (pos == buffer.length) {
		flush();
	}
}

public void flush() throws IOException {
	if (pos > 0) {
		if (buffering) {
			out.write(ObjectStreamConstants.TC_BLOCKDATA);
			out.write(pos);
		}
		out.write(buffer, 0, pos);
		pos = 0;
	}
}

}

public ObjectOutputStreamImpl(OutputStream out, ObjectOutputStream p) {
	super(null);
	this.out = new Output(out);
	parent = p;
}

public void writeStreamHeader() throws IOException {
	writeShort(ObjectStreamConstants.STREAM_MAGIC);
	writeShort(ObjectStreamConstants.STREAM_VERSION);
}

public void drain() throws IOException {
	out.flush();
}

public boolean enableBuffering(boolean state) throws IOException {
	boolean oldstate = buffering;
	if (state != oldstate) {
		drain();
		buffering = state;
	}
	return (oldstate);
}

public void reset() throws IOException {
	flush();
	resetObjectReferences();
	writeByte(ObjectStreamConstants.TC_RESET);
}

public void writeObject(Object obj) throws IOException {

	boolean restore = enableBuffering(false);

	if (obj == null) {
		writeByte(ObjectStreamConstants.TC_NULL);
		enableBuffering(restore);
		return;
	}

	// It we're writing a class, then we're really writing the classDesc
	if (obj instanceof Class) {
		obj = ObjectStreamClass.lookup((Class)obj);
	}

	Class clazz = obj.getClass();

	ObjectStreamClassImpl cls = (ObjectStreamClassImpl)ObjectStreamClass.lookup(clazz);
	// If no stream, then we can't serialize.
	if (cls == null) {
		throw new NotSerializableException();
	}

	Integer key = (Integer)objectReferences.get(obj);
	if (key != null) {
//System.out.println("putReference: " + obj);
		writeByte(ObjectStreamConstants.TC_REFERENCE);
		writeInt(key.intValue());
	}
	else if (clazz.isArray()) {
//System.out.println("putArray: " + obj);
		writeByte(ObjectStreamConstants.TC_ARRAY);
		writeObject(clazz);
		cls.putArray(obj, parent, this);
	}
	else if (obj instanceof String) {
//System.out.println("putString: " + (String)obj);
		writeByte(ObjectStreamConstants.TC_STRING);
		writeUTF((String)obj);
		makeObjectReference(obj);
	}
	else if (obj instanceof ObjectStreamClass) {
//System.out.println("putClass: " + obj);
		while (obj != null) {
			writeByte(ObjectStreamConstants.TC_CLASSDESC);
			cls.putClass(obj, parent, this);
			obj = ObjectStreamClass.lookup(((ObjectStreamClass)obj).forClass().getSuperclass());
		}
		writeObject(null);
	}
	else {
//System.out.println("putObject: " + obj);
		writeByte(ObjectStreamConstants.TC_OBJECT);
		writeObject(clazz);
		cls.putObject(obj, parent, this);
	}
	enableBuffering(restore);
}

public void defaultWriteObject(Object obj, ObjectStreamClass cls) throws IOException {
	// Don't block default data
	boolean restore = enableBuffering(false);
	cls.defaultWriteObject(obj, parent);
	enableBuffering(restore);
}

public void makeObjectReference(Object obj) {
        objectReferences.put(obj, new Integer(nextKey));
//System.out.println("makeObjectReference: " + nextKey + " " + obj);
	nextKey++;
}

private void resetObjectReferences() {
        objectReferences = new IdentityHashtable();
}

}
