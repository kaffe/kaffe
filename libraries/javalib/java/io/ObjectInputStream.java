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

import java.util.ArrayList;

public class ObjectInputStream
  extends InputStream
  implements ObjectInput, ObjectStreamConstants
{
	// --- Static State

	//private static final boolean DOTRACE = Boolean.getBoolean("kaffe.debug.serialin");
	private static final boolean DOTRACE = false;

	private static final ObjectStreamClass defaultClassDesc = new ObjectStreamClass();
	private static final int firstKey = 0x007e0000;

	// --- Inner classes

	/** The spec requires this be abstract. */
	public abstract static class GetField {
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

	/** 
	 * Wraps real input stream of serialized data with blocking (i.e., TC_BLOCKDATA).
	 */
	private static class BlockDataInputStream
	extends FilterInputStream
	{
		private final byte[] buffer = new byte[255];
		private int pos;
		private int len;
		private boolean buffering;
	
		private int leftinblock;
		
		BlockDataInputStream(InputStream in) 
		{
			super(in); /* sets inherited this.in */
		}
		
		// XXX more than an "enable", not really "buffering" either...
		boolean enableBuffering(boolean state) {
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

		public int read(byte[] b) throws IOException
		{
			return this.read(b, 0, b.length);
		}
		
		public int read(byte[] buf, int off, int len) throws IOException
		{
			if (DOTRACE) System.out.println("read(buf,off,"+len+")-> ...");

			if (!buffering)
				return in.read(buf, off, len);
			
			// Gah...
			for (int i = 0; i < len; i++)
			{
				// read() will never return -1, just throws exceptions
				buf[off + i] = (byte)this.read();
			}
			return (len);
		}
		
		public int read() throws IOException {
			if (!buffering) {
				int v = in.read();
				if (DOTRACE) System.out.println("read-> 0x" +Integer.toHexString(v)
								+ " "
								+ (Character.isISOControl((char)v) ?
								   "--"
								   : ( "'" +((char)v)+ "'")));
				return (v);
			}

			if (DOTRACE) System.out.println("read-> (buffered)");

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
					throw new StreamCorruptedException(
						"bad blockdata size, read " 
						+len+ " expected " + size);
				}
				pos = 1;
				leftinblock -= len;
				return ((int)(buffer[0] & 0xFF));
			}
			
			// begin of plain data
			int b = in.read();
			
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
				//(new Throwable("hacky, but real stack trace")).printStackTrace();	// debug
				throw new StreamCorruptedException("expected blockdata start marker, but got : 0x" + Integer.toHexString(b));
			}
		}
	} // end class BlockDataInputStream

	// --- Instance state

	private final ArrayList objectReferences = new ArrayList();
	
	private final BlockDataInputStream blockDataInputStream;
	private final DataInputStream formattedInputStream;
	private boolean enableResolve;

	private Object currentObject;
	private ObjectStreamClass currentStreamClass;

	public ObjectInputStream(InputStream baseStream) 
		throws IOException, StreamCorruptedException
	{
		this.blockDataInputStream = new BlockDataInputStream(baseStream);
		this.formattedInputStream = new DataInputStream(this.blockDataInputStream);
		this.readStreamHeader();
		this.blockDataInputStream.enableBuffering(true);
	}

	protected ObjectInputStream()
	{
		this.blockDataInputStream = null; // to shut KJC up
		this.formattedInputStream = null; // to shut KJC up
		// XXX are we even setup for subclassing?
		// XXX security checks?
		throw new kaffe.util.NotImplemented(
			ObjectInputStream.class.getName() + ".<init>()");
	}

	// --- The basic IO primitives

	public void close() throws IOException
	{
		this.formattedInputStream.close();
	}

	public int available() throws IOException
	{
		return this.formattedInputStream.available();
	}
	
	public int read() throws IOException
	{
		return (this.formattedInputStream.read());
	}

	public boolean readBoolean() throws IOException
	{
		return (this.formattedInputStream.readBoolean());
	}
	
	public byte readByte() throws IOException
	{
		return (this.formattedInputStream.readByte());
	}
	
	public int readUnsignedByte() throws IOException
	{
		return (this.formattedInputStream.readUnsignedByte());
	}
	
	public char readChar() throws IOException
	{
		return (this.formattedInputStream.readChar());
	}
	
	public double readDouble() throws IOException
	{
		return (this.formattedInputStream.readDouble());
	}
	
	public float readFloat() throws IOException
	{
		return (this.formattedInputStream.readFloat());
	}
	
	public void readFully(byte[] buffer) throws IOException
	{
		this.formattedInputStream.readFully(buffer, 0, buffer.length);
	}
	
	public void readFully(byte[] buffer, int offset, int count) throws IOException
	{
		this.formattedInputStream.readFully(buffer, offset, count);
	}
	
	public int readInt() throws IOException
	{
		return (this.formattedInputStream.readInt());
	}
	
	public String readLine() throws IOException
	{
		return (this.formattedInputStream.readLine());
	}
	
	public long readLong() throws IOException
	{
		return (this.formattedInputStream.readLong());
	}
	
	public short readShort() throws IOException
	{
		return (this.formattedInputStream.readShort());
	}
	
	public int readUnsignedShort() throws IOException
	{
		return (this.formattedInputStream.readUnsignedShort());
	}
	
	public String readUTF() throws IOException
	{
		return (this.formattedInputStream.readUTF());
	}
	
	public int skipBytes(int count) throws IOException
	{
		return (this.formattedInputStream.skipBytes(count));
	}
	
	// --- The serialization-specific methods


	protected void readStreamHeader() throws IOException, StreamCorruptedException
	{
		int i = readShort();
		if (i != ObjectStreamConstants.STREAM_MAGIC) {
			throw new StreamCorruptedException("bad magic in stream header ("
							   +i+", expected " +STREAM_MAGIC+ ")");
		}
		i = readShort();
		if (i != ObjectStreamConstants.STREAM_VERSION) {
			throw new StreamCorruptedException("bad version in stream header (" 
							   +i+ ", expected " +STREAM_VERSION+ ")");
		}
	}
	
	protected boolean enableResolveObject(boolean enable) throws SecurityException
	{
		final boolean old = enableResolve;
		enableResolve = enable;
		return (old);
	}
	
	protected Object resolveObject(Object obj) throws IOException
	{
		if (enableResolve)
		{
			// XXX This is broken.  Must invoke per-object "readResolve" method, if it exists.
			// XXX needs security checks, too.
		}

		return (obj);
	}
	
	protected Class resolveClass(ObjectStreamClass desc) 
		throws IOException, ClassNotFoundException
	{
		// XXX This is broken .. the spec says:
		//
		//   "By default the class name is resolved relative to the
		//    class that called readObject."
		//
		return (resolveClassInternal(desc.getName()));
	}
	
	/*package*/ Class resolveClassInternal(String name) 
		throws IOException, ClassNotFoundException
	{
		return (Class.forName(name));
	}
	
	public final Object readObject() 
		throws OptionalDataException, ClassNotFoundException, IOException
	{
		Object obj = readObjectInternal();
		if (enableResolve) {
			obj = resolveObject(obj);
		}
		return (obj);
	}
	
	private Object readObjectInternal() 
		throws OptionalDataException, ClassNotFoundException, IOException 
	{
		boolean restore = this.blockDataInputStream.enableBuffering(false);
		
		Object currObject = null;
		
		int tok = readUnsignedByte();
		if (DOTRACE) System.out.println("readObject: tok=0x" + Integer.toHexString(tok));
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
			
		case ObjectStreamConstants.TC_CLASS:
			currObject = getClassOnly();
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
			throw new StreamCorruptedException("unexpected ObjectStreamConstant TC_ token: 0x" 
							   +Integer.toHexString(tok));
		}
		
		this.blockDataInputStream.enableBuffering(restore);
		
		return (currObject);
	}

	// --- Methods for use inside a readObject(), writeObject(), etc...

	public synchronized void registerValidation(ObjectInputValidation obj, int prio) 
		throws NotActiveException, InvalidObjectException
	{
		throw new kaffe.util.NotImplemented(
			ObjectInputStream.class.getName() + ".registerValidation()");
	}
	
	public void defaultReadObject()
		throws IOException, ClassNotFoundException, NotActiveException
	{
		if (currentObject == null || currentStreamClass == null) {
			throw new NotActiveException();
		}

		// We don't buffer default object data
		boolean restore = this.blockDataInputStream.enableBuffering(false);
		currentStreamClass.defaultReadObject(currentObject, this);
		this.blockDataInputStream.enableBuffering(restore);
	}

	public ObjectInputStream.GetField readFields() 
		throws IOException, ClassNotFoundException, NotActiveException 
	{
		if (currentObject == null || currentStreamClass == null) {
			throw new NotActiveException();
		}

		// No blocking
		boolean restore = this.blockDataInputStream.enableBuffering(false);
		GetField rc = currentStreamClass.readFields(this);
		this.blockDataInputStream.enableBuffering(restore);
		return rc;
	}

	// -- Internal implementation methods

	private Object getObjectFromReference(int key) 
		throws StreamCorruptedException {
		try {
			return (objectReferences.get(key - firstKey));
		}
		catch (ArrayIndexOutOfBoundsException _) {
			throw new StreamCorruptedException("reference to unknown object");
		}
	}

	/*package*/ void makeObjectReference(Object obj) {
		objectReferences.add(obj);
	}

	private void resetObjectReferences() {
		objectReferences.clear();
	}
	
	/*package*/ boolean enableBuffering(boolean enable) 
	{
		return this.blockDataInputStream.enableBuffering(enable);
	}
	
	private Object getObject() 
		throws StreamCorruptedException, OptionalDataException, InvalidClassException 
	{
		try {
			ObjectStreamClass cls;
			try {
				cls = (ObjectStreamClass)this.readObject();
			} 
			catch (InvalidClassException ice) {
				throw ice;
			}
			catch (StreamCorruptedException sce) {
				throw sce;
			}
			catch (IOException ioe) {
				throw new StreamCorruptedException("Error reading object header: " +ioe);
			}

			Object obj = cls.getObject(this);
			return (obj);
		}
		catch (ClassNotFoundException e1) {
			throw new StreamCorruptedException("error getting object: " + e1);
		}
		catch (ClassCastException e3) {
			throw new StreamCorruptedException("error getting object: " + e3);
		}
	}

	private Object getClassOnly() 
		throws StreamCorruptedException, OptionalDataException, InvalidClassException 
	{
		try {
			Object obj = (ObjectStreamClass)this.readObject();
			makeObjectReference(obj);
			return (obj);
		}
		catch (InvalidClassException ice) {
			throw ice;
		}
		catch (StreamCorruptedException sce) {
			throw sce;
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

	private Object getStreamClass() 
		throws StreamCorruptedException, InvalidClassException
	{
		try {
			Object obj = defaultClassDesc.getClass(this);
			if (DOTRACE) System.out.println("getStreamClass: " + obj);
			return (obj);
		}
		catch (StreamCorruptedException sce) {
			throw sce;
		}
		catch (InvalidClassException ice) {
			throw ice;
		}
		catch (IOException e2) {
			throw new StreamCorruptedException("error getting class: " + e2);
		}
		catch (ClassCastException e3) {
			e3.printStackTrace();
			throw new StreamCorruptedException("error getting class: " + e3);
		}
	}

	private Object getArray() 
		throws StreamCorruptedException, InvalidClassException
	{
		try {
			ObjectStreamClass cls = (ObjectStreamClass)this.readObject();
			Object obj = cls.getArray(this);
			return (obj);
		}
		catch (StreamCorruptedException sce) {
			throw sce;
		}
		catch (InvalidClassException ice) {
			throw ice;
		}
		catch (ClassNotFoundException e1) {
			throw new StreamCorruptedException("error getting array: " + e1);
		}
		catch (IOException e2) {
			throw new StreamCorruptedException("error getting array: " + e2);
		}
		catch (ClassCastException e3) {
			e3.printStackTrace();
			throw new StreamCorruptedException("error getting array: " + e3);
		}
	}
}

