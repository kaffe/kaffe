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

import kaffe.util.IdentityHashtable;

public class ObjectOutputStream
  extends OutputStream
  implements ObjectOutput, ObjectStreamConstants
{
	// --- Static state
	//private static final boolean DOTRACE = Boolean.getBoolean("kaffe.debug.serialout");
	private static final boolean DOTRACE = false;

	// --- Inner classes
	static public abstract class PutField 
	{
		abstract public void put(String fname, boolean fvalue);
		abstract public void put(String fname, byte fvalue);
		abstract public void put(String fname, char fvalue);
		abstract public void put(String fname, short fvalue);
		abstract public void put(String fname, int fvalue);
		abstract public void put(String fname, long fvalue);
		abstract public void put(String fname, float fvalue);
		abstract public void put(String fname, double fvalue);
		abstract public void put(String fname, Object fvalue);
		/** @deprecated use {@link ObjectOutputStream.writeFields}. */
		abstract public void write(ObjectOutput out) throws IOException;
	}

	private static class Output 
		extends DataOutputStream 
	{
		private final ByteArrayOutputStream buffer = new ByteArrayOutputStream(255);
		private boolean buffering;

		Output(OutputStream out) 
		{
			super(out); /* sets inherited this.out */
			this.buffering = false;
		}
		
		// XXX not really "buffering" more like "partition" or "chunking"?
		/*package*/ boolean enableBuffering(boolean state) throws IOException {
			boolean oldstate = buffering;
			//if (DOTRACE) System.out.println("  enableBuffering(new=" +state+ "; old=" +buffering+ ")");
			if (state != oldstate) {
				flush();
				buffering = state;
			}
			return (oldstate);
		}

		public void write(int byteVal) throws IOException {
			if (DOTRACE) System.out.println("buffer.write(0x" +Integer.toHexString(byteVal)+ ")");
			buffer.write(byteVal);
		}
		
		public void write(byte[] b, int off, int len) throws IOException {
			if (DOTRACE) System.out.println("buffer.write(b, off, " +len+ ") ...");
			buffer.write(b, off, len);
		}
		
		public void flush() throws IOException {
			int size = buffer.size();
			if (DOTRACE) System.out.println("  Flushing(size=" +size+ "; bufing=" +buffering+ ")");
			if (size > 0) {
				if (buffering) {
					if (size < 256) {
						this.out.write(ObjectStreamConstants.TC_BLOCKDATA);
						this.out.write(size);
					}
					else {
						this.out.write(ObjectStreamConstants.TC_BLOCKDATALONG);
						this.out.write((size >> 24) & 0xFF);
						this.out.write((size >> 16) & 0xFF);
						this.out.write((size >>  8) & 0xFF);
						this.out.write((size      ) & 0xFF);
					}
				}
				buffer.writeTo(this.out);
				buffer.reset();
			}
		}
		
	}

	// --- Instance state
	
	private final Output out;
	private boolean enableReplace;

	private int nextKey = 0x007e0000;

	// XXX Use a Vector like ObjectInputStream?
	private final IdentityHashtable objectReferences = new IdentityHashtable();

	private Object currentObject;
	private ObjectStreamClass currentStreamClass;

	public ObjectOutputStream(OutputStream ot) 
		throws IOException 
	{
		this.out = new Output(ot);
		writeStreamHeader();
		out.enableBuffering(true);
	}

	// --- Basic output methods

	public void close() throws IOException {
		flush(); // XXX ?
		out.close();
	}

	public void flush() throws IOException {
		out.flush();
	}
	
	public void write(byte[] buffer) throws IOException {
		if (DOTRACE) System.out.println("write(byte[])");
		write(buffer, 0, buffer.length);
	}
	
	public void write(byte[] buffer, int offset, int count) throws IOException {
		if (DOTRACE) System.out.println("write(byte[],...)");
		out.write(buffer, offset, count);
	}
	
	public void writeBoolean(boolean val) throws IOException {
		if (DOTRACE) System.out.println("write(bool)");
		out.writeBoolean(val);
	}
	
	public void writeByte(int val) throws IOException {
		if (DOTRACE) System.out.println("writeByte(" +Integer.toHexString(val)+ ")");
		out.writeByte(val);
	}
	
	public void writeBytes(String str) throws IOException {
		if (DOTRACE) System.out.println("writeBytes(String)");
		out.writeBytes(str);
	}
	
	public void writeChar(int val) throws IOException {
		if (DOTRACE) System.out.println("writeChar()");
		out.writeChar(val);
	}
	
	public void writeChars(String str) throws IOException {
		if (DOTRACE) System.out.println("writeChars()");
		out.writeChars(str);
	}
	
	public void writeDouble(double val) throws IOException {
		if (DOTRACE) System.out.println("writeDouble()");
		out.writeDouble(val);
	}
	
	public void writeFloat(float val) throws IOException {
		if (DOTRACE) System.out.println("writeFloat()");
		out.writeFloat(val);
	}
	
	public void writeInt(int val) throws IOException {
		if (DOTRACE) System.out.println("writeInt()");
		out.writeInt(val);
	}
	
	public void writeLong(long val) throws IOException {
		if (DOTRACE) System.out.println("writeLong(0x" +Long.toHexString(val)+ ")");
		out.writeLong(val);
	}
	
	public void writeShort(int val) throws IOException {
		if (DOTRACE) System.out.println("writeShort(0x" +Integer.toHexString(0x0ffff & val)+ ")");
		out.writeShort(val);
	}
	
	public void writeUTF(String str) throws IOException {
		if (DOTRACE) System.out.println("writeUTF(" +str + ")");
		out.writeUTF(str);
	}
	
	public void write(int oneByte) throws IOException {
		if (DOTRACE) System.out.println("write(int onebyte)");
		out.write(oneByte);
	}
	
	public void reset() throws IOException {
		// XXX check context (e.g., not allowed during writeObject). 
		flush();
		resetObjectReferences();
		writeByte(ObjectStreamConstants.TC_RESET);
	}
	
	protected void drain() throws IOException {
		flush();
	}
	
	// --- 

	protected void annotateClass(Class cl) throws IOException 
	{
		// XXX Does nothing.
	}

	/*package*/ void annotateClassInternal(Class cl) throws IOException 
	{
		annotateClass(cl);
	}

	protected boolean enableReplaceObject(boolean enable) throws SecurityException {
		final boolean old = enableReplace;
		enableReplace = enable;
		return (old);
	}

	protected Object replaceObject(Object obj) throws IOException {
		if (enableReplace)
		{
			/* XXX UNIMPLEMENTED. */
		}
		return (obj);
	}
	
	public final void writeObject(Object obj) throws IOException {
		if (enableReplace) {
			obj = replaceObject(obj);
		}

		boolean restore = this.out.enableBuffering(false);
		
		if (obj == null) {
			writeByte(ObjectStreamConstants.TC_NULL);
			this.out.enableBuffering(restore);
			return;
		}
		
		// It we're writing a class, then we're really writing the classDesc
		if (obj instanceof Class) {
			obj = ObjectStreamClass.lookup((Class)obj);
		}
		
		Class clazz = obj.getClass();
		
		ObjectStreamClass cls = ObjectStreamClass.lookup(clazz);
		// If no stream, then we can't serialize.
		if (cls == null) {
			throw new NotSerializableException();
		}
		
		Integer key = (Integer)objectReferences.get(obj);
		if (key != null) {
			if (DOTRACE) System.out.println("putReference: " + obj);
			writeByte(ObjectStreamConstants.TC_REFERENCE);
			writeInt(key.intValue());
		}
		else if (clazz.isArray()) {
			if (DOTRACE) System.out.println("putArray: " + obj);
			writeByte(ObjectStreamConstants.TC_ARRAY);
			writeObject(clazz);
			cls.putArray(obj, this);
		}
		else if (obj instanceof String) {
			if (DOTRACE) System.out.println("putString: " + (String)obj);
			writeByte(ObjectStreamConstants.TC_STRING);
			writeUTF((String)obj);
			makeObjectReference(obj);
		}
		else if (obj instanceof ObjectStreamClass) {
			while (obj != null) {
				if (DOTRACE) System.out.println("putClass: " + obj);
				writeByte(ObjectStreamConstants.TC_CLASSDESC);
				cls.putClass(obj, this);
				obj = ObjectStreamClass.lookup(((ObjectStreamClass)obj).forClass().getSuperclass());
			}
			if (DOTRACE) System.out.println("putClass: <null>");
			writeObject(null);
		}
		else {
			if (DOTRACE) System.out.println("putObject: " + obj);
			writeByte(ObjectStreamConstants.TC_OBJECT);
			writeObject(clazz);
			cls.putObject(obj, this);
		}
		this.out.enableBuffering(restore);
	}

	public void defaultWriteObject() throws IOException {
		if (currentObject == null || currentStreamClass == null) {
			throw new NotActiveException();
		}

		// Don't block default data
		boolean restore = this.out.enableBuffering(false);
		currentStreamClass.defaultWriteObject(currentObject, this);
		this.out.enableBuffering(restore);
	}
	
	public ObjectOutputStream.PutField putFields() throws IOException 
	{
		if (currentObject == null || currentStreamClass == null) {
			throw new NotActiveException();
		}

		return currentStreamClass.putFields();
	}
	
	public void writeFields() throws IOException 
	{
		if (currentObject == null || currentStreamClass == null) {
			throw new NotActiveException();
		}

		// Don't block "default" data
		boolean restore = this.out.enableBuffering(false);
		currentStreamClass.writeFields(currentObject, this);
		this.out.enableBuffering(restore);
	}

	// --- Internal implementation methods

	/*package*/ boolean enableBuffering(boolean enable)
		throws IOException
	{
		return this.out.enableBuffering(enable);
	}

	private void writeStreamHeader() throws IOException {
		writeShort(ObjectStreamConstants.STREAM_MAGIC);
		writeShort(ObjectStreamConstants.STREAM_VERSION);
		out.flush();
	}

	/*package*/ void makeObjectReference(Object obj) {
		objectReferences.put(obj, new Integer(nextKey));
		if (DOTRACE) System.out.println("makeObjectReference: " + nextKey + " " + obj);
		nextKey++;
	}
	
	private void resetObjectReferences() {
		objectReferences.clear();
	}
}
