/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 * Copyright (c) 2002
 *	Patrick Tullmann, <pat_kaffe@tullmann.org>
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.io;

import java.util.Hashtable;
import java.util.ArrayList;

/*
 * TODO:
 *
 * support useProtocolVersion 
 * writeReplace / readResolve
 * enforce context restrictions on reset()
 * "unshared" objects
 * Strings larger than 64K
 * Better exception chaining/reporting
 * Security checks
 * Support overriding implementation: writeObjectOverride, no-arg constructor, etc...
 * Support for dynamic proxy classes??
 * "AnnotateClass" support
 * Do inner classes serialize correctly?
 * Replace all the native methods with pure Java (and lots of reflection).
 */
public class ObjectStreamClass 
implements Serializable 
{
	// --- Static state 

	public static ObjectStreamField[] NO_FIELDS = new ObjectStreamField[0];

	//private static final boolean DOTRACE = Boolean.getBoolean("kaffe.debug.serialout") || Boolean.getBoolean("kaffe.debug.serialin");
	private static final boolean DOTRACE = false;

	private static final long serialVersionUID = -6120832682080437368L;

	// Mapping from Class -> ObjectStreamClass for that class.
	private static final Hashtable streamClasses = new Hashtable();

	static {
		init0();
	}
	
	// -- Inner Classes

	/** 
	 * Temporary type used for reading in and processing in-stream
	 * type descriptions.
	 */
	private static class InStreamTypeDescriptor
	{
		String fieldName;
		String typeName;
		byte typeCode;
	}

	/** @see #putField */
	private static class ConcretePutField
	extends ObjectOutputStream.PutField
	{
		private static abstract class Value
		{
			abstract void put(ObjectOutputStream out) throws IOException;
		}

		private final ObjectStreamField[] fields;

		/** Value to write for each field. */
		private final Value[] values;

		ConcretePutField(ObjectStreamField[] fields)
		{
			// share fields array with creator
			this.fields = fields;
			this.values = new Value[fields.length];
		}

		private int findIndex(String fname)
			throws IllegalArgumentException
		{
			if (fname == null)
				throw new NullPointerException("null field name");

			for (int i = 0; i < fields.length; i++)
			{
				if (fields[i].getName().equals(fname))
					return i;
			}
			
			throw new IllegalArgumentException(
				"Field `" +fname+ "' is not a serializable field.");
		}

		private static void requireType(ObjectStreamField f, Class c)
			throws IllegalArgumentException
		{
			if (! f.getType().equals(c))
				throw new IllegalArgumentException("Field `" +f.getName()+ "' has type " +c.getName());
		}

		public void put(String fname, final boolean fvalue)
		{
			int i = findIndex(fname);
			requireType(fields[i], boolean.class);
			values[i] = new Value() {
					void put(ObjectOutputStream out) throws IOException
					{
						out.writeBoolean(fvalue);
					}
				};
		}
		
		public void put(String fname, final byte fvalue)
		{
			int i = findIndex(fname);
			requireType(fields[i], byte.class);
			values[i] = new Value() {
					void put(ObjectOutputStream out) throws IOException
					{
						out.writeByte(fvalue);
					}
				};
		}
		
		public void put(String fname, final char fvalue)
		{
			int i = findIndex(fname);
			requireType(fields[i], char.class);
			values[i] = new Value() {
					void put(ObjectOutputStream out) throws IOException
					{
						out.writeChar(fvalue);
					}
				};
		}
		
		public void put(String fname, final short fvalue)
		{
			int i = findIndex(fname);
			requireType(fields[i], short.class);
			values[i] = new Value() {
					void put(ObjectOutputStream out) throws IOException
					{
						out.writeShort(fvalue);
					}
				};
		}
		
		public void put(String fname, final int fvalue)
		{
			int i = findIndex(fname);
			if (DOTRACE) System.out.println("PutField.put(" +fname+ ", (int)" +fvalue+ ") -> f=" +fields[i]);
			requireType(fields[i], int.class);
			if (DOTRACE) System.out.println("PutField.put(" +fname+ ", (int)" +fvalue+ ") -> OK!");
			values[i] = new Value() {
					void put(ObjectOutputStream out) throws IOException
					{
						out.writeInt(fvalue);
					}
				};
		}
		
		public void put(String fname, final long fvalue)
		{
			int i = findIndex(fname);
			requireType(fields[i], long.class);
			values[i] = new Value() {
					void put(ObjectOutputStream out) throws IOException
					{
						out.writeLong(fvalue);
					}
				};
		}
		
		public void put(String fname, final float fvalue)
		{
			int i = findIndex(fname);
			requireType(fields[i], float.class);
			values[i] = new Value() {
					void put(ObjectOutputStream out) throws IOException
					{
						out.writeFloat(fvalue);
					}
				};
		}
		
		public void put(String fname, final double fvalue)
		{
			int i = findIndex(fname);
			requireType(fields[i], double.class);
			values[i] = new Value() {
					void put(ObjectOutputStream out) throws IOException
					{
						out.writeDouble(fvalue);
					}
				};
		}
		
		public void put(String fname, final Object fvalue)
		{
			int i = findIndex(fname);
			if (fields[i].getType().isPrimitive())
				throw new IllegalArgumentException("Field `" +fname+ "' has primitive type "
								   +fields[i].getType().getName());
			values[i] = new Value() {
					void put(ObjectOutputStream out) throws IOException
					{
						out.writeObject(fvalue);
					}
				};
		}

		/** @deprecated use {@link ObjectOutputStream.writeFields}. */
		public void write(ObjectOutput out) throws IOException
		{
			throw new kaffe.util.NotImplemented(
				"ObjectOutputStream.PutField.write() -- its deprecated anyway.");
		}

		/*package*/ void writeTo(ObjectOutputStream out) throws IOException
		{
			for (int i = 0; i < values.length; i++)
			{
				if (values[i] != null)
				{
					if (DOTRACE) System.out.println("Writing " +fields[i].getName());
					values[i].put(out); // heheheh, he said "put out"
				}
				else
				{
					// If the field wasn't given a value by
					// the user, write the default for that type
					// into the stream...
					switch(fields[i].getTypeCode())
					{
					case '[':
					case 'L': out.writeObject(null);
						break;
					case 'I': out.writeInt(0);
						break;
					case 'J': out.writeLong(0L);
						break;
					case 'Z': out.writeBoolean(false);
						break;
					case 'B': out.writeByte(0);
						break;
					case 'S': out.writeShort(0);
						break;
					case 'C': out.writeChar(0);
						break;
					case 'F': out.writeFloat(0.0f);
						break;
					case 'D': out.writeDouble(0.0);
						break;
					case 'V': 
					default:
						throw new InternalError("Illegal typecode in " +fields[i]);
					}
				}
			}
		}
	}

	private static class ConcreteGetField
	extends ObjectInputStream.GetField
	{
		private static class Field
		{
			final String fname;

			Field(String fname)
			{
				this.fname = fname;
			}

			boolean getBool() { throw new IllegalArgumentException("Field '" +fname+ "' is not a boolean"); }
			byte getByte() { throw new IllegalArgumentException("Field '" +fname+ "' is not a byte"); }
			char getChar() { throw new IllegalArgumentException("Field '" +fname+ "' is not a char"); }
			short getShort() { throw new IllegalArgumentException("Field '" +fname+ "' is not a short"); }
			int getInt() { throw new IllegalArgumentException("Field '" +fname+ "' is not a int"); }
			long getLong() { throw new IllegalArgumentException("Field '" +fname+ "' is not a long"); }
			float getFloat() { throw new IllegalArgumentException("Field '" +fname+ "' is not a float"); }
			double getDouble() { throw new IllegalArgumentException("Field '" +fname+ "' is not a double"); }
			Object getObject() { throw new IllegalArgumentException("Field '" +fname+ "' is not an object reference"); }
		}

		private final ObjectStreamClass objectStreamClass;
		private final Field[] loadedFields;
		private final ObjectStreamField[] defaultedFields;
	
		ConcreteGetField(ObjectStreamClass objectStreamClass, ObjectInputStream in)
			throws IOException, ClassNotFoundException
		{
			this.objectStreamClass = objectStreamClass;
			this.loadedFields = new Field[objectStreamClass.readableFields.length];

			if (DOTRACE) System.out.println("ConcreteGetField: fct="+loadedFields.length);
			
			// Read in all the fields defined instream (regardless of
			// whether they match in-VM types, or have in-VM offsets).
			for (int i = 0; i < loadedFields.length; i++)
			{
				ObjectStreamField f = objectStreamClass.readableFields[i];
				//f.forceTypeResolution();
				String fname = f.getName();
				char tcode = f.getTypeCode();
				if (DOTRACE) System.out.println("GetField.readField("+fname+","+tcode+")");
				loadedFields[i] = readField(in, fname, tcode);
			}

			// Now figure out which fields are "defaulted": that is the
			// set of serializable fields in a class that are not in
			// the in-stream representation.  Most of the time, this
			// list is empty.

			ArrayList defaultedFieldsArray = new ArrayList();
			if (DOTRACE) System.out.println("ConcreteGetField: finding default fields.");
			for (int i = 0; i < objectStreamClass.serializableFields.length; i++)
			{
				ObjectStreamField f = objectStreamClass.serializableFields[i];
				int fi = findLoadedField(f.getName());
				if (fi == -1)
					defaultedFieldsArray.add(f);
			}
				
			this.defaultedFields = (ObjectStreamField[])defaultedFieldsArray.toArray(new ObjectStreamField[0]);
		}
		
		private int findLoadedField(String fname)
		{
			for (int i = 0; i < this.loadedFields.length; i++)
			{
				if (this.loadedFields[i].fname.equals(fname))
					return i;
			}

			return -1;
		}
		
		private int findDefaultedField(String fname)
		{
			for (int i = 0; i < this.defaultedFields.length; i++)
			{
				if (this.defaultedFields[i].getName().equals(fname))
					return i;
			}

			return -1;
		}
		
		private static Field readField(ObjectInputStream in,
					       String fname,
					       char tcode)
			throws IOException, ClassNotFoundException
		{
			if (tcode == '[' || tcode == 'L')
			{
				final Object val = in.readObject();
				return new Field(fname) {
						Object getObject() {
							return val;
						}
					};
			}

			// Primitive type
			switch(tcode)
			{
			case 'I': final int ival = in.readInt();
				return new Field(fname) {
					int getInt() { return ival; }
				};
			case 'J': final long jval = in.readLong();
				return new Field(fname) {
					long getLong() { return jval; }
				};
			case 'Z': final boolean zval = in.readBoolean();
				return new Field(fname) {
					boolean getBoolean() { return zval; }
				};
			case 'B': final byte bval = in.readByte();
				return new Field(fname) {
					byte getByte() { return bval; }
				};
			case 'C': final char cval = in.readChar();
				return new Field(fname) {
					char getChar() { return cval; }
				};
			case 'S': final short sval = in.readShort();
				return new Field(fname) {
					short getShort() { return sval; }
				};
			case 'F': final float fval = in.readFloat();
				return new Field(fname) {
					float getFloat() { return fval; }
				};
			case 'D': final double dval = in.readDouble();
				return new Field(fname) {
					double getDouble() { return dval; }
				};
			case 'V': // shouldn't occur
			default:
				return new Field(fname);
			}
			
			// not reached
		}

		public boolean defaulted(String fname) throws IOException, IllegalArgumentException
		{
			int fi = findLoadedField(fname);
			if (fi != -1)
				return false;
			
			fi = findDefaultedField(fname);
			if (fi != -1)
				return true;
			
			throw new IllegalArgumentException("No such field `" +fname+ "'");
		}

		private static void requireFieldCode(ObjectStreamField f, char typeCode, String typename)
		{
			if (f.getTypeCode() != typeCode)
				throw new IllegalArgumentException("Field '" +f.getName()+ "' is not a " +typename);
		}

		public boolean get(String fname, boolean defvalue) throws IOException, IllegalArgumentException
		{
			int fi = findLoadedField(fname);
			if (fi != -1)
				return this.loadedFields[fi].getBool();
			fi = findDefaultedField(fname);
			if (fi != -1)
			{
				requireFieldCode(this.defaultedFields[fi], 'Z', "boolean");
				return defvalue;
			}
			throw new IllegalArgumentException("No field named `" +fname+ "'");
		}
		
		public byte get(String fname, byte defvalue) throws IOException, IllegalArgumentException
		{
			int fi = findLoadedField(fname);
			if (fi != -1)
				return this.loadedFields[fi].getByte();
			fi = findDefaultedField(fname);
			if (fi != -1)
			{
				requireFieldCode(this.defaultedFields[fi], 'B', "byte");
				return defvalue;
			}
			throw new IllegalArgumentException("No field named `" +fname+ "'");
		}
	
		public char get(String fname, char defvalue) throws IOException, IllegalArgumentException
		{
			int fi = findLoadedField(fname);
			if (fi != -1)
				return this.loadedFields[fi].getChar();
			fi = findDefaultedField(fname);
			if (fi != -1)
			{
				requireFieldCode(this.defaultedFields[fi], 'C', "char");
				return defvalue;
			}
			throw new IllegalArgumentException("No field named `" +fname+ "'");
		}
		
		public short get(String fname, short defvalue) throws IOException, IllegalArgumentException
		{
			int fi = findLoadedField(fname);
			if (fi != -1)
				return this.loadedFields[fi].getShort();
			fi = findDefaultedField(fname);
			if (fi != -1)
			{
				requireFieldCode(this.defaultedFields[fi], 'S', "short");
				return defvalue;
			}
			throw new IllegalArgumentException("No field named `" +fname+ "'");
		}
		
		public int get(String fname, int defvalue) throws IOException, IllegalArgumentException
		{
			int fi = findLoadedField(fname);
			if (fi != -1)
				return this.loadedFields[fi].getInt();
			fi = findDefaultedField(fname);
			if (fi != -1)
			{
				requireFieldCode(this.defaultedFields[fi], 'I', "int");
				return defvalue;
			}
			throw new IllegalArgumentException("No field named `" +fname+ "'");
		}
		
		public float get(String fname, float defvalue) throws IOException, IllegalArgumentException
		{
			int fi = findLoadedField(fname);
			if (fi != -1)
				return this.loadedFields[fi].getFloat();
			fi = findDefaultedField(fname);
			if (fi != -1)
			{
				requireFieldCode(this.defaultedFields[fi], 'F', "float");
				return defvalue;
			}
			throw new IllegalArgumentException("No field named `" +fname+ "'");
		}
		
		public long get(String fname, long defvalue) throws IOException, IllegalArgumentException
		{
			int fi = findLoadedField(fname);
			if (fi != -1)
				return this.loadedFields[fi].getLong();
			fi = findDefaultedField(fname);
			if (fi != -1)
			{
				requireFieldCode(this.defaultedFields[fi], 'J', "long");
				return defvalue;
			}
			throw new IllegalArgumentException("No field named `" +fname+ "'");
		}
		
		public double get(String fname, double defvalue) throws IOException, IllegalArgumentException
		{
			int fi = findLoadedField(fname);
			if (fi != -1)
				return this.loadedFields[fi].getDouble();
			fi = findDefaultedField(fname);
			if (fi != -1)
			{
				requireFieldCode(this.defaultedFields[fi], 'D', "double");
				return defvalue;
			}
			throw new IllegalArgumentException("No field named `" +fname+ "'");
		}
		
		public Object get(String fname, Object defvalue) throws IOException, IllegalArgumentException
		{
			int fi = findLoadedField(fname);
			if (fi != -1)
				return this.loadedFields[fi].getObject();
			fi = findDefaultedField(fname);
			if (fi != -1)
			{
				requireFieldCode(this.defaultedFields[fi], 'L', "object reference");
				return defvalue;
			}
			throw new IllegalArgumentException("No field named `" +fname+ "'");
		}
		
		public ObjectStreamClass getObjectStreamClass()
		{
			return this.objectStreamClass;
		}
	}
	
	// --- Instance state

	private /*final*/ Class clazz;
	private /*final*/ int method; // The serialization method used for this class
	private /*final*/ long clazz_serialVersionUID; // Beware of static 'serialVersionUID'!
	private ObjectStreamClass superstream; // not really a "stream"
	
	/** Fields to write out when serializing instances of clazz.  In canonical order.
	 * Passed to native outputClassFields0. */
	private ObjectStreamField[] serializableFields;

	/** Fields to read in when deserializing instances of clazz.  In stream-specified
	 * order.  See resolveVMFieldsSerialFields().  Passed to native inputClassFields0. */
	private ObjectStreamField[] readableFields = null;

	// XXX the following two fields are related to a specific object, and need
	// to be re-set for each new instance written out...

	/** Allocated on demand in putFields() */
	private ConcretePutField userPutField;
	/** Allocated on demand in readFields() */
	private ConcreteGetField userGetField;


	private ObjectStreamClass(Class clazz, int meth) {
		if (clazz == null)
			throw new NullPointerException();
		this.clazz = clazz;
		this.method = meth;
		this.clazz_serialVersionUID = getSerialVersionUID0(clazz);
		this.superstream = lookup(clazz.getSuperclass());
		this.serializableFields = findSerializableFields0(this.clazz);
	}
	
	/**
	 * No-arg constructor generates an ObjectStreamClass for ObjectStreamClass.
	 * Serialization of an ObjectStreamClass is a special, but still ends up
	 * invoking writeObject().
	 */
	/*package*/ ObjectStreamClass() {
		this.clazz = ObjectStreamClass.class;
		this.method = ObjectStreamConstants.SC_WRITE_METHOD|ObjectStreamConstants.SC_SERIALIZABLE;
		this.clazz_serialVersionUID = ObjectStreamClass.serialVersionUID;
		this.superstream = null;
		this.serializableFields = NO_FIELDS;
	}
	
	public Class forClass() {
		return clazz;
	}
	
	public String getName() {
		return clazz.getName();
	}
	
	public String toString() {
		return getName()
			+ ": static final long serialVersionUID = "
			+ Long.toString(getSerialVersionUID()) 
			+ "L;";
	}
	
	public static ObjectStreamClass lookup(Class cl) {
		if (cl == null) {
			return (null);
		}
		
		if (DOTRACE) System.out.println("Looking up " + cl);
		// First check hash table for match - return what's found
		ObjectStreamClass osc = (ObjectStreamClass)streamClasses.get(cl);
		if (osc != null) {
			if (DOTRACE) System.out.println("Found in cache");
			return (osc);
		}
		
		// Otherwise we work out how the class should be serialized and
		// make an entry for it.  If the class can't be serialized we
		// make a null entry.
		int method;
		if (Externalizable.class.isAssignableFrom(cl)) {
			method = ObjectStreamConstants.SC_SERIALIZABLE | ObjectStreamConstants.SC_EXTERNALIZABLE;
			if (DOTRACE) System.out.println(" is externalizable");
		}
		else if (Serializable.class.isAssignableFrom(cl)) {
			method = ObjectStreamConstants.SC_SERIALIZABLE;
			if (hasWriteObject(cl)) {
				method |= ObjectStreamConstants.SC_WRITE_METHOD;
				if (DOTRACE) System.out.println(" has read/write");
			}
			if (DOTRACE) System.out.println(" is serializable");
		}
		else {
			if (DOTRACE) System.out.println("Cannot be serialized");
			// Cannot be serialized
			return (null);
		}
		
		osc = new ObjectStreamClass(cl, method);
		streamClasses.put(cl, osc);
		
		return (osc);
	}
	
	public ObjectStreamField getField(String name) {
		for (int i = 0; i < this.serializableFields.length; i++)
		{
			if (name.equals(this.serializableFields[i].getName()))
				return this.serializableFields[i];
		}
		return null;
	}
	
	public ObjectStreamField[] getFields() {
		// Return a copy of the fields, so users can't muck with real info
		ObjectStreamField[] userf = new ObjectStreamField[serializableFields.length];
		System.arraycopy(serializableFields, 0,
				 userf, 0, serializableFields.length);
		return userf;
	}
		
	public long getSerialVersionUID() {
		if (clazz_serialVersionUID == 0)
			clazz_serialVersionUID = getSerialVersionUID0(this.clazz);
		return clazz_serialVersionUID;
	}
	

	// Implementation-specific methods:

	/*package*/ ObjectOutputStream.PutField putFields() 
		throws IOException
	{
		// XXX synchronization??

		if (this.userPutField == null)
			this.userPutField = new ConcretePutField(this.serializableFields);
		return this.userPutField;
	}

	/*package*/ void writeFields(Object obj, ObjectOutputStream out) 
		throws IOException
	{
		if (this.userPutField == null)
			throw new NotActiveException("no current PutField object (use putFields())");

		// XXX oddly, the object being serialized is irrelevant
		this.userPutField.writeTo(out);
	}

	/*package*/ ObjectInputStream.GetField readFields(ObjectInputStream ois) 
		throws IOException, ClassNotFoundException
	{
		if (this.userGetField == null)
			this.userGetField = new ConcreteGetField(this, ois);
		return this.userGetField;
	}

	/*package*/ Object getObject(ObjectInputStream in)
		throws StreamCorruptedException, InvalidClassException, OptionalDataException 
	{
		Object obj = allocateNewObject();
		in.makeObjectReference(obj);
		getObjectWithoutSuper(obj, in);
		return (obj);
	}

	private void getObjectWithoutSuper(Object obj, ObjectInputStream in)
		throws StreamCorruptedException, InvalidClassException, OptionalDataException 
	{
		// Get the data in our superclasses first (if they're serializable)
		if (superstream != null) 
		{
			superstream.getObjectWithoutSuper(obj, in);
		}
		
		try {
			if ((method & ObjectStreamConstants.SC_EXTERNALIZABLE) != 0) {
				try
				{
					((Externalizable)obj).readExternal(in);
				}
				catch (IOException ioe)
				{
					throw new StreamCorruptedException("error in readExternal" +ioe);
				}
			}
			else if ((method & ObjectStreamConstants.SC_WRITE_METHOD) != 0) {
				boolean restore = in.enableBuffering(true);
				this.userGetField = null;
				invokeObjectReader0(obj, in);
				in.enableBuffering(restore);
				getEndOfDataBlock(in);
			}
			else if ((method & ObjectStreamConstants.SC_SERIALIZABLE) != 0) {
				defaultReadObject(obj, in);
			}
			else {
				throw new StreamCorruptedException("unknown method type");
			}
		}
		catch (StreamCorruptedException sce) {
			throw sce;
		}
		catch (ClassCastException e1) {
			//e1.printStackTrace(); // XXXX DEBUGGING CRUFT
			throw new StreamCorruptedException("error getting object: " + e1);
		}
		catch (ClassNotFoundException e2) {
			throw new StreamCorruptedException("error getting object: " + e2);
		}
	}
	
	private void getEndOfDataBlock(ObjectInputStream in)
		throws StreamCorruptedException 
	{
		byte read = 0;

		try {
			read = in.readByte();
		}
		catch (IOException e) {
			StreamCorruptedException sce = new StreamCorruptedException("failed to read endblockdata: " + e);
			sce.initCause(e);
			throw sce;
		}
		
		if (read != ObjectStreamConstants.TC_ENDBLOCKDATA) {
			throw new StreamCorruptedException("failed to read endblockdata: no marker found");
		}
	}

	/*package*/ Object getClass(ObjectInputStream in)
		throws StreamCorruptedException, InvalidClassException, OptionalDataException 
	{
		try {
			if (DOTRACE) System.out.println("Getting class");
			Object obj = allocateNewObject();
			in.makeObjectReference(obj);
			this.userGetField = null;
			invokeObjectReader0(obj, in);
			// Get any class annotation
			getEndOfDataBlock(in);
			// Get the superclass stream.
			if (DOTRACE) System.out.println("Getting superclass");
			((ObjectStreamClass)obj).superstream = (ObjectStreamClass)in.readObject();
			if (DOTRACE) System.out.println("Done");
			return (obj);
		}
		catch (InvalidClassException ice) {
			throw ice; 
		}
		catch (StreamCorruptedException sce) {
			throw sce; 
		}
		catch (IOException e1) {
			throw new StreamCorruptedException("error getting object: " + e1);
		}
		catch (ClassNotFoundException e2) {
			throw new StreamCorruptedException("error getting object: " + e2);
		}
	}
	
	/*package*/ Object getArray(ObjectInputStream in)
		throws StreamCorruptedException, InvalidClassException
	{
		try {
			Class elem = clazz.getComponentType();
			int len = in.readInt();
			Object obj = allocateNewArray(len);
			in.makeObjectReference(obj);
			
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
			//e1.printStackTrace();
			throw new StreamCorruptedException("bad array: " + e1);
		}
		catch (ClassNotFoundException e2) {
			throw new StreamCorruptedException("bad array: " + e2);
		}
		catch (InvalidClassException ice) {
			throw ice; 
		}
		catch (StreamCorruptedException sce) {
			throw sce; 
		}
		catch (IOException e3) {
			throw new StreamCorruptedException("bad array: " + e3);
		}
	}
	
	/*package*/ void putObject(Object obj, ObjectOutputStream out)
		throws IOException 
	{
		out.makeObjectReference(obj);
		putObjectWithoutSuper(obj, out);
	}

	private void putObjectWithoutSuper(Object obj, ObjectOutputStream out)
		throws IOException {

		// Put the data in our superclasses first (if they're serializable)
		if (superstream != null) {
			superstream.putObjectWithoutSuper(obj, out);
		}
		
		if ((method & ObjectStreamConstants.SC_EXTERNALIZABLE) != 0) {
			if (DOTRACE) System.out.println("...writeExternal");
			((Externalizable)obj).writeExternal(out);
		}
		else if ((method & ObjectStreamConstants.SC_WRITE_METHOD) != 0) {
			if (DOTRACE) System.out.println("...invokeObjectWriter");
			boolean restore = out.enableBuffering(true);
			this.userPutField = null;
			invokeObjectWriter0(obj, out);
			out.enableBuffering(restore);
			out.writeByte(ObjectStreamConstants.TC_ENDBLOCKDATA);
		}
		else if ((method & ObjectStreamConstants.SC_SERIALIZABLE) != 0) {
			if (DOTRACE) System.out.println("...defaultWriteObject");
			defaultWriteObject(obj, out);
		}
		else {
			throw new StreamCorruptedException("unknown method type");
		}
	}

	/*package*/ void putClass(Object obj, ObjectOutputStream out)
		throws IOException {

		out.makeObjectReference(obj);
		this.userPutField = null;
		invokeObjectWriter0(obj, out);
		out.annotateClassInternal(((ObjectStreamClass)obj).clazz);
		out.writeByte(ObjectStreamConstants.TC_ENDBLOCKDATA);
	}

	/*package*/ void putArray(Object obj, ObjectOutputStream out)
		throws IOException {

		Class elem = obj.getClass().getComponentType();
		
		out.makeObjectReference(obj);
		
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
	
	/*package*/ void defaultReadObject(Object obj, ObjectInputStream in) {
		inputClassFields0(this.clazz, this.readableFields, obj, in);
	}

	/*package*/ void defaultWriteObject(Object obj, ObjectOutputStream out) {
		outputClassFields0(this.clazz, this.serializableFields, obj, out);
	}

	/*package*/ void readObject(ObjectInputStream in) 
		throws IOException, ClassNotFoundException 
	{
		String name = in.readUTF();
		if (DOTRACE) System.out.println("readObject()-> name=" + name);
		this.clazz_serialVersionUID = in.readLong();
		if (DOTRACE) System.out.println("readObject()-> serialVersionUID=" + clazz_serialVersionUID);
		this.method = in.readUnsignedByte();
		if (DOTRACE) System.out.println("readObject()-> \"method\"=" + method);
		
		// Resolve the class, XXX convert exceptions?
		this.clazz = in.resolveClassInternal(name);

		// Figure out what this class inVM-representation expects 
		// its serialized form to contain...
		this.serializableFields = findSerializableFields0(clazz);

		// Now read in the serialized description of the fields of the class.
		int len = in.readShort();

		// We'll track the types the instream class has:
		InStreamTypeDescriptor[] inStreamFields = new InStreamTypeDescriptor[len];

		if (DOTRACE) System.out.println("readObject()-> field ct=" + len);
		for (int i = 0; i < len; i++) {
			inStreamFields[i] = new InStreamTypeDescriptor();
			inStreamFields[i].typeCode = (byte)in.readUnsignedByte();
			inStreamFields[i].fieldName = in.readUTF();
			if (DOTRACE) System.out.println("  " +inStreamFields[i].typeCode+ ": " +inStreamFields[i].fieldName);

			if ((inStreamFields[i].typeCode == 'L')
			    || (inStreamFields[i].typeCode == '[')) {
				try {
					inStreamFields[i].typeName = ((String)in.readObject()).intern();
				}
				catch (ClassNotFoundException _) {
					// XXX better error message
					throw new StreamCorruptedException("bad type");
				}
			}
			else
			{
				inStreamFields[i].typeName = null;
			}
		}
		
		// Check for matching serial version UIDs
		long localUID = getSerialVersionUID0(clazz);
		if (clazz_serialVersionUID != localUID) {
			throw new InvalidClassException(
				"mismatched serial version UIDs: expected " 
				+ clazz_serialVersionUID 
				+ " but local class `" + clazz.getName() 
				+ "' has UID " + localUID);
		}

		resolveVMFieldsSerialFields(inStreamFields);
	}

	/**
	 * The given list of inStreamFields needs to be tweaked to "match"
	 * the set of serially persistent fields defined in the in-VM 
	 * class (i.e., this.serializableFields).  We only have something
	 * interesting to do if a class has evolved and thus the in-VM
	 * version is different from the in-stream version.  Also, folks who
	 * play nasty games with Serialization (i.e., to test it :) might
	 * cause problems here.
	 *
	 * For each in-stream field there are several things that might happen:
	 *   - its good.  We'll read it in and set the appropriate field in
	 *     the object
	 *   - its irrelevant.  The in-VM repr doesn't have such a field, we
	 *     silently ignore such changes.
	 *   - its a mismatch.  The in-MV repr has a field of the same name
	 *     but a different type.  To be useful with 'readFields' any errors
	 *     are thrown during the read...
	 *
	 * If the in-stream fields match the in-VM fields, then we just use
	 * the inVM field descriptor.
	 *
	 * The common case is that the new readableFields array is a copy of
	 * the serializableFields array (sharing the objects).
	 */
	private void resolveVMFieldsSerialFields(InStreamTypeDescriptor[] inStreamFields)
	{
		this.readableFields = new ObjectStreamField[inStreamFields.length];

		/* 
		 * XXX this is O(N*M).  (Most times, N and M < 4, though)
		 */
		for (int i = 0; i < inStreamFields.length; i++)
		{
			InStreamTypeDescriptor inStreamField = inStreamFields[i];

			int v = 0; // XXX should be able to bump start each round
			while ((v < this.serializableFields.length)
			       && (this.readableFields[i] == null))
			{
				ObjectStreamField inVMField = this.serializableFields[v];
				this.readableFields[i] = compareFields(inStreamField, inVMField);
				v++;
			}

			if (this.readableFields[i] == null)
			{
				if (DOTRACE) System.out.println("  " +inStreamField.fieldName+ 
								" (" +inStreamField.typeName+ ")"
								+ " not part of inVM class. ignored.");
				// Didn't find a match
				this.readableFields[i] = new ObjectStreamField(inStreamField.fieldName,
									       inStreamField.typeCode,
									       inStreamField.typeName);
				this.readableFields[i].setIgnored();
			}
		}
	}

	/** Compare in-stream field definition of a field to inVM definition. */
	private static ObjectStreamField compareFields(InStreamTypeDescriptor inStreamField,
						       ObjectStreamField inVMField)
	{
		if (inStreamField.fieldName.equals(inVMField.getName()))
		{
			String inStreamTypeName = inStreamField.typeName;
			String inVMTypeName = inVMField.getTypeString();
			boolean sameTypes = false;

			if ((inStreamTypeName == null)
			    && (inVMTypeName == null)
			    && (inStreamField.typeCode == inVMField.getTypeCode()))
				// Primitives of the same type
				sameTypes = true;
			else if ((inStreamTypeName != null)
				 && (inVMTypeName != null)
				 && (inStreamTypeName.equals(inVMTypeName)))
				// References of the same type
				sameTypes = true;

			if (sameTypes)
			{
				if (DOTRACE) System.out.println("  " +inStreamField.fieldName+ " matches inVM class field.");
				// Its the same as the declared inVM name,
				// note however, that the declared inVM repr might
				// mismatch the actual inVM repr...
				return inVMField;
			}
			else
			{
				if (DOTRACE) System.out.println("  " +inStreamField.fieldName+ " mis-match: " 
								+inStreamField.typeCode+ "("
								+inStreamField.typeName+ ") vs. inVM type " 
								+inVMField.getTypeCode()+ "("
								+inVMField.getTypeString()+ ")");
				// its a type mismatch, against the declared inVM 
				// repr.  Perhaps it matches the actual underlying
				// field type, but that's too bad.  Force a mismatch:
				ObjectStreamField osf = new ObjectStreamField(inStreamField.fieldName,
									      inStreamField.typeCode,
									      inStreamField.typeName);
				osf.setMismatch(true);
				return osf;
			}
		}

		// no match, try next
		return null;
	}


	private void writeObject(ObjectOutputStream out) 
		throws IOException {

		if (DOTRACE) System.out.println("ObjectStreamClass.writeObject: " +this);
		if (DOTRACE) System.out.println("...write name: writeUTF(" +getName()+ ")");
		out.writeUTF(getName());
		if (DOTRACE) System.out.println("...write UID: writeLong(" +clazz_serialVersionUID+ ")");
		out.writeLong(clazz_serialVersionUID);
		if (DOTRACE) System.out.println("...write \"method\": writeByte(" +method+ ")");
		out.writeByte(method);
		if (DOTRACE) System.out.println("...write field ct: writeShort(" +this.serializableFields.length+ ")");
		//outputClassFieldInfo(out);
		out.writeShort(this.serializableFields.length);

		/* 
		 * We write out the serializableFields info *assuming
		 * its correct*.  (Offsets and typeMismatches may
		 * occur later), but if the user overrides
		 * readObject/writeObject those errors won't occur...
		 */
		for (int i = 0; i < this.serializableFields.length; i++)
		{
			ObjectStreamField f = this.serializableFields[i];

			if (DOTRACE) System.out.println("...write type code: writeShort(" +f.getTypeCode()+ ")");
			out.writeByte((byte)f.getTypeCode());
			if (DOTRACE) System.out.println("...write field name: writeUTF(" +f.getName()+ ")");
			out.writeUTF(f.getName());

			String fullTypeName = f.getTypeString(); // null for primitives
			if (fullTypeName != null)
			{
				if (DOTRACE) System.out.println("...write type (string) writeObject(" +fullTypeName+ ")");
				// Append the full typename, in "field descriptor format"
				// as an object...
				out.writeObject(fullTypeName);
			}
		}
		if (DOTRACE) System.out.println("...Done");
	}

	private static boolean hasWriteObject(Class cls) {
		return (hasWriteObject0(cls));
	}

	private native Object allocateNewObject();
	private native Object allocateNewArray(int size);
	private native static boolean hasWriteObject0(Class cls);
	private native boolean invokeObjectReader0(Object obj, ObjectInputStream in);
	private native boolean invokeObjectWriter0(Object obj, ObjectOutputStream out);
	//private native void outputClassFieldInfo(ObjectOutputStream out);

// --- XXXX XXX 
	private static native void init0();
	private static native long getSerialVersionUID0(Class cls); // XXX "compute" not get
	private static native ObjectStreamField[] findSerializableFields0(Class clazz);
	private static native void outputClassFields0(Class clazz,
						      ObjectStreamField[] writeableFields,
						      Object obj,
						      ObjectOutputStream out);
	private static native void inputClassFields0(Class clazz, 
						     ObjectStreamField[] readableFields, 
						     Object obj, 
						     ObjectInputStream in);
}
