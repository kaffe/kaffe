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

import java.io.DataInputStream;
import java.io.ObjectStreamConstants;
import java.io.StreamCorruptedException;
import java.io.InputStream;
import java.io.IOException;
import java.io.OptionalDataException;
import java.io.ObjectInputStream;
import java.io.NotActiveException;
import java.io.Externalizable;
import java.lang.ClassNotFoundException;
import java.util.Vector;
import java.io.ObjectStreamClass;

public class ObjectInputStreamImpl extends DataInputStream {

static private ObjectStreamClassImpl defaultClassDesc = new ObjectStreamClassImpl();

private byte[] buffer = new byte[255];
private int pos;
private int len;
private boolean buffering = false;

private static int firstKey = 0x007e0000;
private Vector objectReferences = new Vector();

private ObjectInputStream parent;

class Input extends InputStream {

private InputStream in;
int leftinblock;

Input(InputStream in) {
        this.in = in;
}

public void close() throws IOException {
	in.close();
}

public int read() throws IOException {
	if (!buffering) {
		int v = in.read();
//System.out.println("read " + v);
		return (v);
	}
	if (pos < len) {
		return ((int)(buffer[pos++] & 0xFF));
	}

	/*
	 * we're reading plain data and are still working on a long 
	 * block that required multiple reads with our short buffer
	 */ 
	if (leftinblock > 0) {
		int size = buffer.length;
		if (size > leftinblock) {
			size = leftinblock;
		}
		len = in.read(buffer, 0, size);
		if (len != size) {
			throw new StreamCorruptedException("bad blockdata size, read " + len + " expected " + size);
		}
		pos = 1;
		leftinblock -= len;
		return ((int)(buffer[0] & 0xFF));
	}

	// begin of plain data
	int b = in.read();
	int size;
	switch (b) {
	case ObjectStreamConstants.TC_BLOCKDATA:
		/* byte following tag indicates number of bytes */
		leftinblock = in.read();
		return (read());	// tail recursive call

	case ObjectStreamConstants.TC_BLOCKDATALONG:
		/* The doc says: 
		 * long following tag indicates number of bytes 
		 * but what they actually write is an int following the tag.
		 */
		int v1 = 0;
		for (int i = 24; i >= 0; i -= 8) {
			b = in.read();
			if (b == -1) {
				throw new StreamCorruptedException(
					"reached eof while reading block size");
			}
			v1 |= (int)b << i;
		}
		leftinblock = v1;
		return (read());	// tail recursive call

	default:
		(new Throwable()).printStackTrace();	// debug
		throw new StreamCorruptedException("expected blockdata: " + b);
	}
}

}

public ObjectInputStreamImpl(InputStream in, ObjectInputStream p) {
	super(null);
	this.in = new Input(in);
	parent = p;
}

public boolean enableBuffering(boolean state) {
        boolean oldstate = buffering;
        if (state != oldstate) {
		// If buffering, flush any buffered data
                if (buffering) {
			len = 0;
			pos = 0;
                }
                buffering = state;
        }
        return (oldstate);
}

public Object readObject() throws OptionalDataException, ClassNotFoundException, IOException {

	boolean restore = enableBuffering(false);

	Object currObject = null;

	int tok = readUnsignedByte();
//System.out.println("readObject: tok = " + tok);
	switch (tok) {
	case ObjectStreamConstants.TC_NULL:
		currObject = null;
		break;

	case ObjectStreamConstants.TC_STRING:
		currObject = readUTF();
		makeObjectReference(currObject);
		break;

	case ObjectStreamConstants.TC_REFERENCE:
		currObject = getObjectFromReference(readInt());
		break;

	case ObjectStreamConstants.TC_CLASSDESC:
		currObject = getStreamClass();
		break;

	case ObjectStreamConstants.TC_OBJECT:
		currObject = getObject();
		break;

	case ObjectStreamConstants.TC_ARRAY:
		currObject = getArray();
		break;

	case ObjectStreamConstants.TC_RESET:
		resetObjectReferences();
		return (readObject());

	case ObjectStreamConstants.TC_BLOCKDATA:
		int l = read();
		if (l == -1) {
			throw new StreamCorruptedException("incomplete block count");
		}
		throw new OptionalDataException(l);

	case ObjectStreamConstants.TC_BLOCKDATALONG:
		int l0 = read();
		int l1 = read();
		int l2 = read();
		int l3 = read();
		if (l0 == -1 || l1 == -1 || l2 == -1 || l3 == -1) {
			throw new StreamCorruptedException("incomplete block count");
		}
		int ll = ((l0 << 24) | (l1 << 16) | (l2 << 8) | l3);
		throw new OptionalDataException(ll);

	default:
		throw new StreamCorruptedException("unexpected token: " + tok);
	}

	enableBuffering(restore);

	return (currObject);
}

public void defaultReadObject(Object obj, ObjectStreamClass cls) throws IOException, StreamCorruptedException {
	// We don't buffer default object data
	boolean restore = enableBuffering(false);
	cls.defaultReadObject(obj, parent);
	enableBuffering(restore);
}

public void readStreamHeader() throws IOException, StreamCorruptedException {
	int i = readShort();
	if (i != ObjectStreamConstants.STREAM_MAGIC) {
		throw new StreamCorruptedException("bad magic nr: " + i);
	}
	i = readShort();
	if (i != ObjectStreamConstants.STREAM_VERSION) {
		throw new StreamCorruptedException("bad version nr: " + i);
	}
}

private Object getObjectFromReference(int key) throws StreamCorruptedException {
	try {
		return (objectReferences.elementAt(key - firstKey));
	}
	catch (ArrayIndexOutOfBoundsException _) {
		throw new StreamCorruptedException("reference to unknown object");
	}
}

public void makeObjectReference(Object obj) {
	objectReferences.addElement(obj);
}

private void resetObjectReferences() {
	objectReferences.removeAllElements();
}

private Object getObject() throws StreamCorruptedException, OptionalDataException {
	try {
		ObjectStreamClassImpl cls = (ObjectStreamClassImpl)parent.readObject();
		Object obj = cls.getObject(parent, this);
		return (obj);
	}
	catch (ClassNotFoundException e1) {
		throw new StreamCorruptedException("error getting object: " + e1);
	}
	catch (IOException e2) {
		throw new StreamCorruptedException("error getting object: " + e2);
	}
	catch (ClassCastException e3) {
		throw new StreamCorruptedException("error getting object: " + e3);
	}
}

private Object getStreamClass() throws StreamCorruptedException {
	try {
		Object obj = defaultClassDesc.getClass(parent, this);
//System.out.println("getStreamClass: " + obj);
		return (obj);
	}
	catch (IOException e2) {
		throw new StreamCorruptedException("error getting class: " + e2);
	}
	catch (ClassCastException e3) {
		e3.printStackTrace();
		throw new StreamCorruptedException("error getting class: " + e3);
	}
}

private Object getArray() throws StreamCorruptedException {
	try {
		ObjectStreamClassImpl cls = (ObjectStreamClassImpl)parent.readObject();
		Object obj = cls.getArray(parent, this);
		return (obj);
	}
	catch (ClassNotFoundException e1) {
		throw new StreamCorruptedException("bad array: " + e1);
	}
	catch (IOException e2) {
		throw new StreamCorruptedException("bad array: " + e2);
	}
	catch (ClassCastException e3) {
		e3.printStackTrace();
		throw new StreamCorruptedException("bad array: " + e3);
	}
}

}
