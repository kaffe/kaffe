/*
 * Java core library component.
 *
 * Copyright (c) 2002
 *      Patrick Tullmann <pat_kaffe@tullmann.org>
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.io;

/**
 * Try to keep this class relatively light-weight since its gets pulled in by
 * lots of core classes that use it to define Sun-compatible serialization.
 */
public class ObjectStreamField
implements Comparable
{
	private final Class type;
	private final String name;
	private final boolean unshared; // XXX unimplemented

	// Filled in on demand when getTypeString() is invoked
	private String typeStr;

	// XXX this is 
	// a byte so that native code can use it without thinking too much.
	private final byte typeCode;

	// These are set in ObjectStreamClassImpl.c:
	int offset;
	boolean typeMismatch;

	// NOTE: Native code in ObjectStreamClassImpl.c:deepCopyFixup() creates instances
	// of this class without using the constructor.

	public ObjectStreamField(String name, Class type)
	{
		this(name, type, false);
	}
	
	// unshared controls use of readUnshared/writeUnshared vs. readObject/writeObject
	public ObjectStreamField(String name, Class type, boolean unshared)
	{
		if (name == null)
			throw new NullPointerException();
		this.type = type;
		this.name = name;
		this.unshared = unshared;
		this.offset = -1;
		this.typeMismatch = true; // irrelevant in user-created Fields

		// Compute the typeCode
		if (this.type.isArray())
			this.typeCode = (byte)'[';
		else if (this.type.isPrimitive())
		{
			if (this.type == boolean.class)
				this.typeCode = (byte)'Z';
			else if (this.type == long.class)
				this.typeCode = (byte)'J';
			else if (this.type == int.class)
				this.typeCode = (byte)'I';
			else if (this.type == short.class)
				this.typeCode = (byte)'S';
			else if (this.type == byte.class)
				this.typeCode = (byte)'B';
			else if (this.type == char.class)
				this.typeCode = (byte)'C';
			else if (this.type == float.class)
				this.typeCode = (byte)'F';
			else if (this.type == double.class)
				this.typeCode = (byte)'D';
			else 
				// Shouldn't ever happen:
				this.typeCode = (byte)'V';
		}
		else
			this.typeCode = (byte)'L';

		// this is filled in on demand:
		this.typeStr = null;
	}

	// Package-private constructor used by ObjectStreamClass to represent
	// the fields encoded instream.
	/*package*/ ObjectStreamField(String name, byte typeCode, String typeStr)
	{
		if (name == null)
			throw new NullPointerException();
		if (typeCode == 0)
			throw new InternalError("Illegal typeCode (0)");
		this.name = name;
		this.type = null; // XXX
		this.typeStr = typeStr; // will be null for primitive types
		this.typeCode = typeCode;
		this.typeMismatch = false; // setMismatch will override
		this.offset = -1;
		this.unshared = false;
	}

	public String getName()
	{
		return this.name;
	}
	
	/*package*/ void setMismatch(boolean m)
	{
		this.typeMismatch = m;
	}

	/*package*/ void setIgnored()
	{
		this.offset = -1;
	}

	public Class getType()
	{
		return this.type;
	}
	
	public char getTypeCode()
	{
		return (char)this.typeCode;
	}

	public String getTypeString()
	{
		if (this.type == null)
			return this.typeStr;

		if (this.type.isPrimitive())
			return null;
		
		if (this.typeStr == null)
		{
			// XXX this is pretty lame because internally, Kaffe
			// stores the class name the right format, and it
			// will allocate a temporary object to return to us!
			String baseStr;
			if (this.type.isArray())
				baseStr = this.type.getName();
			else // reference type
				baseStr = "L" + this.type.getName() + ";";
			// See JDC bug 4405949 (for intern())
			this.typeStr = baseStr.replace('.','/').intern();
		}

		return this.typeStr;
	}

	public boolean isUnshared()
	{
		return this.unshared;
	}

	protected void setOffset(int o)
	{
		this.offset = o;
	}

	public int getOffset()
	{
		return this.offset;
	}

	public int compareTo(Object obj)
	{
		ObjectStreamField other = (ObjectStreamField)obj;

		/*
		 * References beat primitives.  Name is secondary.
		 */
		if (other.type.isPrimitive()
		    && (!this.type.isPrimitive()))
			return 1;
		else if (this.type.isPrimitive())
			return -1;
		return this.name.compareTo(other.name);
	}

	public String toString()
	{
		return this.type.getName() + " " +this.getName() 
			+ "; //"
			+ " @" +offset
			+ (this.unshared ? ", unshared" : "");
	}
}
