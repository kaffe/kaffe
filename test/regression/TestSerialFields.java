/*
 * Tests of ObjectInputStream.GetField and ObjectOutputStream.PutField.
 * Doesn't test too aggressively (for example, none of the tests make
 * sure what they say gets written out, actually gets written out.)
 * See SerializationCompatibility for more aggressive testing.
 *
 * Copyright (c) 2002 Pat Tullmann
 * All Rights Reserved.
 *
 * This file is released into the public domain.
 *
 * @author Pat Tullmann <pat_kaffe@tullmann.org>
 */
import java.io.*;
import java.util.*;

public class TestSerialFields
{
	// Set to true to get stack traces and byte-dumps:
	private static final boolean DEBUG = false;

	public static final long constantUID = 0x42L;

	private static final String[] allFieldNames = 
	{
		"x", "y",
		"xd", "yd",
		"xodd", "yodd",
	};
	
	private static void checkGetBool(ObjectInputStream.GetField inputFields, String fname)
		throws IOException
	{
		try
		{
			boolean val = inputFields.get(fname, false);
			System.out.print(" bool=" +val);
		}
		catch (IllegalArgumentException iae)
		{
			// ignore
			if (DEBUG) System.out.print(" /*not boolean*/");
		}
	}

	private static void checkGetObject(ObjectInputStream.GetField inputFields, String fname)
		throws IOException
	{
		try
		{
			Object val = inputFields.get(fname, null);
			System.out.print(" Object=" +val);
		}
		catch (IllegalArgumentException iae)
		{
			// ignore
			if (DEBUG) System.out.print(" /*not Object*/");
		}
	}

	private static void checkGetInt(ObjectInputStream.GetField inputFields, String fname)
		throws IOException
	{
		try
		{
			int val = inputFields.get(fname, 0);
			System.out.print(" int=" +val);
		}
		catch (IllegalArgumentException iae)
		{
			// ignore
			if (DEBUG) System.out.print(" /*not int*/");
		}
	}

	private static void checkGetDouble(ObjectInputStream.GetField inputFields, String fname)
		throws IOException
	{
		try
		{
			double val = inputFields.get(fname, 0.0);
			System.out.print(" double=" +val+ " ");
		}
		catch (IllegalArgumentException iae)
		{
			// ignore
			if (DEBUG) System.out.print(" /*not double*/");
		}
	}

	private static boolean checkGetDefaulted(ObjectInputStream.GetField inputFields, String fname)
		throws IOException
	{
		try
		{
			boolean defaulted = inputFields.defaulted(fname);
			System.out.print("    " +fname+ ":");
			if (defaulted)
				System.out.print(" (defaulted)");
			return true;
		}
		catch (IllegalArgumentException iae)
		{
			return false;
		}
	}

	private static void checkGetFields(ObjectInputStream.GetField inputFields)
		throws IOException
	{
		for (int i = 0; i < allFieldNames.length; i++)
		{
			String fname = allFieldNames[i];
			if (!checkGetDefaulted(inputFields, fname))
				continue;
			checkGetBool(inputFields, fname);
			checkGetObject(inputFields, fname);
			checkGetInt(inputFields, fname);
			checkGetDouble(inputFields, fname);
			System.out.println();
		}
	}
	
	private static String checkPutBool(ObjectOutputStream.PutField outputFields, String fname)
		throws IOException
	{
		try
		{
			outputFields.put(fname, true);
			return "bool=true ";
		}
		catch (IllegalArgumentException iae)
		{
			if (DEBUG) return "/*not boolean*/ ";
			return "";
		}
	}

	private static String checkPutString(ObjectOutputStream.PutField outputFields, String fname)
		throws IOException
	{
		try
		{
			outputFields.put(fname, new String("putfield test string"));
			return "string='putfield test string' ";
		}
		catch (IllegalArgumentException iae)
		{
			if (DEBUG) return "/*not Object*/ ";
			return "";
		}
	}

	private static String checkPutInt(ObjectOutputStream.PutField outputFields, String fname)
		throws IOException
	{
		try
		{
			outputFields.put(fname, 42);
			return "int=42 ";
		}
		catch (IllegalArgumentException iae)
		{
			if (DEBUG) return "/*not int*/ ";
			return "";
		}
	}

	private static String checkPutDouble(ObjectOutputStream.PutField outputFields, String fname)
		throws IOException
	{
		try
		{
			outputFields.put(fname, 6.9);
			return "double=6.9 ";
		}
		catch (IllegalArgumentException iae)
		{
			if (DEBUG) return "/*not double*/ ";
			return "";
		}
	}

	private static void checkPutFields(ObjectOutputStream.PutField outputFields)
		throws IOException
	{
		for (int i = 0; i < allFieldNames.length; i++)
		{
			String fname = allFieldNames[i];
			String msg = "";
			msg += checkPutBool(outputFields, fname);
			msg += checkPutString(outputFields, fname);
			msg += checkPutInt(outputFields, fname);
			msg += checkPutDouble(outputFields, fname);

			if (!msg.equals(""))
			{
				System.out.print("    put " +fname+ ": ");
				System.out.print(msg);
				System.out.println();
			}
		}
	}


	private static class Test0001 // "original" (pristine) version
	implements Serializable
	{
		private static final long serialVersionUID = constantUID;
		private int x = 0x042;
		private int y = 0x069;

		// Garbage fields that won't be read by anyone
		private int[] iarray = new int[] { 1, 2, 3};
		private Integer X = new Integer(44);
		private long foo = 11;
		private double xf = 32.45;

		public String toString()
		{
			return (this.getClass().getName() + " x=" +x+ "; y=" +y+ "; foo=" +foo);
		}

		private void writeObject(ObjectOutputStream stream) 
			throws IOException, ClassNotFoundException
		{
			TestSerialFields.checkPutFields(stream.putFields());
			stream.writeFields();
		}

		private void readObject(ObjectInputStream stream) 
			throws IOException, ClassNotFoundException
		{
			TestSerialFields.checkGetFields(stream.readFields());
		}

	}
	
	private static class Test0002 // contains doubles
	implements Serializable
	{
		private static final long serialVersionUID = constantUID;
		private double xd = 0;
		private double yd = 0;

		public String toString()
		{
			return (this.getClass().getName() + " instance");
		}

		private void writeObject(ObjectOutputStream stream) 
			throws IOException, ClassNotFoundException
		{
			TestSerialFields.checkPutFields(stream.putFields());
			stream.writeFields();
		}

		private void readObject(ObjectInputStream stream) 
			throws IOException, ClassNotFoundException
		{
			TestSerialFields.checkGetFields(stream.readFields());
		}
	}
	
	private static class Test0005 // missing x,y fields
	implements Serializable
	{
		private static final long serialVersionUID = constantUID;

		public String toString()
		{
			return (this.getClass().getName() + " instance");
		}

		private void writeObject(ObjectOutputStream stream) 
			throws IOException, ClassNotFoundException
		{
			TestSerialFields.checkPutFields(stream.putFields());
			stream.writeFields();
		}

		private void readObject(ObjectInputStream stream) 
			throws IOException, ClassNotFoundException
		{
			TestSerialFields.checkGetFields(stream.readFields());
		}
	}
	

	private static class Test0008 // Compatible via serialPersistentFields...
	implements Serializable
	{
		private static final long serialVersionUID = constantUID;
		private static final ObjectStreamField[] serialPersistentFields =
		{
			new ObjectStreamField("x", int.class),
			new ObjectStreamField("y", int.class)
		};

		public String toString()
		{
			return (this.getClass().getName() + " instance");
		}

		private void writeObject(ObjectOutputStream stream) 
			throws IOException, ClassNotFoundException
		{
			TestSerialFields.checkPutFields(stream.putFields());
			stream.writeFields();
		}

		private void readObject(ObjectInputStream stream) 
			throws IOException, ClassNotFoundException
		{
			TestSerialFields.checkGetFields(stream.readFields());
		}
	}
	

	private static class Test0010 // Compatible, but different (and invalid) serialPersistentFields...
	implements Serializable
	{
		private static final long serialVersionUID = constantUID;
		private static final ObjectStreamField[] serialPersistentFields =
		{
			new ObjectStreamField("xodd", String.class),
			new ObjectStreamField("yodd", String.class)
		};
		private int x;
		private int y;

		public String toString()
		{
			return (this.getClass().getName() + " instance");
		}

		private void writeObject(ObjectOutputStream stream) 
			throws IOException, ClassNotFoundException
		{
			TestSerialFields.checkPutFields(stream.putFields());
			stream.writeFields();
		}

		private void readObject(ObjectInputStream stream) 
			throws IOException, ClassNotFoundException
		{
			TestSerialFields.checkGetFields(stream.readFields());
		}
	}
	

	private static class Test0011 // Compatible, broken writeObject
	implements Serializable
	{
		private static final long serialVersionUID = constantUID;
		private int x;
		private int y;

		public String toString()
		{
			return (this.getClass().getName() + " instance");
		}

		private void writeObject(ObjectOutputStream stream) 
			throws IOException, ClassNotFoundException
		{
			//Object ignored = stream.putFields();
			stream.writeFields();
		}

		private void readObject(ObjectInputStream stream) 
			throws IOException, ClassNotFoundException
		{
			stream.readFields();
		}
	}

	private static class Test0012 // check defaultReadObject
	implements Serializable
	{
		private static final long serialVersionUID = constantUID;
		private int x = 11;
		private int y = 12;

		public String toString()
		{
			return (this.getClass().getName() + ": " +x+ "," +y);
		}

		private void writeObject(ObjectOutputStream stream) 
			throws IOException, ClassNotFoundException
		{
			Object x = stream.putFields();
			stream.writeFields();
		}

		private void readObject(ObjectInputStream stream) 
			throws IOException, ClassNotFoundException
		{
			stream.defaultReadObject();
		}
	}

	private static class Test0014 // bogus reads/puts
	implements Serializable
	{
		private static final long serialVersionUID = constantUID;
		private int x = 11;
		private int y = 12;

		public String toString()
		{
			return (this.getClass().getName() + ": " +x+ "," +y);
		}

		private void writeObject(ObjectOutputStream stream) 
			throws IOException, ClassNotFoundException
		{
			ObjectOutputStream.PutField pf = stream.putFields();
			pf.put("totallyBogus", true);
			stream.writeFields();
		}

		private void readObject(ObjectInputStream stream) 
			throws IOException, ClassNotFoundException
		{
			ObjectInputStream.GetField gf = stream.readFields();
			gf.get("totallyBogus", true);
		}
	}

	private static class Test0015 // impossible field names
	implements Serializable
	{
		private static final long serialVersionUID = constantUID;
		private int x = 11;
		private int y = 12;

		private static final ObjectStreamField[] serialPersistentFields =
		{
			new ObjectStreamField("x", int.class),
			new ObjectStreamField("No $!#%ing way!", String.class),
			new ObjectStreamField("y", int.class),
		};

		public String toString()
		{
			return (this.getClass().getName() + ": " +x+ "," +y);
		}

		private void writeObject(ObjectOutputStream stream) 
			throws IOException, ClassNotFoundException
		{
			ObjectOutputStream.PutField pf = stream.putFields();
			pf.put("No $!#%ing way!", "way");
			stream.writeFields();
		}

		private void readObject(ObjectInputStream stream) 
			throws IOException, ClassNotFoundException
		{
			ObjectInputStream.GetField gf = stream.readFields();
			gf.get("No $!#%ing way!", "way");
		}
	}

	// XXX JDK1.4 and Kaffe differ on this one (JDK blows up with multiple 
	// readFields()) both Kaffe and JDK1.4 are fine with multiple putFields.
	private static class Test0016 // multiple gets/sets
	implements Serializable
	{
		private static final long serialVersionUID = constantUID;
		private int x = 11;
		private int y = 12;

		public String toString()
		{
			return (this.getClass().getName() + ": " +x+ "," +y);
		}

		private void writeObject(ObjectOutputStream stream) 
			throws IOException, ClassNotFoundException
		{
			ObjectOutputStream.PutField pf1 = stream.putFields();
			ObjectOutputStream.PutField pf2 = stream.putFields();
			if (pf1 != pf2)
				throw new Error("Two different PutFields?!");
			stream.writeFields();
		}

		private void readObject(ObjectInputStream stream) 
			throws IOException, ClassNotFoundException
		{
			ObjectInputStream.GetField gf1 = stream.readFields();
			ObjectInputStream.GetField gf2 = stream.readFields();
			if (gf1 != gf2)
				throw new Error("Two different GetFields?!");
			this.x = gf1.get("x", 0);
			this.y = gf1.get("y", 0);
		}
	}
	private static void tryRead(Class c, byte[] bytes)
	{
		System.out.println(c.getName()+ ":");
		try
		{
			System.out.println("  Deserializing from Test0001 stream...");
			//System.out.println("Expecting a " +c.getName()+ " in stream...");
			ObjectInputStream ois = new ObjectInputStream(new ByteArrayInputStream(bytes));
			Object o = ois.readObject();
			if (!c.isInstance(o))
				throw new Exception("Object '" +o+ "' is not an instance of " +c);
			System.out.println("    OK. Got: " +o);
		}
		catch (Exception e)
		{
			System.out.println("    Exception: " +e.getClass().getName());
			if (DEBUG) e.printStackTrace(System.out);
		}
	}

	private static ByteArrayOutputStream generateBytes(Class c)
	{
		try
		{
			System.out.println("  Serializing " +c.getName());
			ByteArrayOutputStream bo = new ByteArrayOutputStream(256);
			ObjectOutputStream oos = new ObjectOutputStream(bo);
			oos.writeObject(c.newInstance());
			oos.close();
			System.out.println("    OK: " +bo.size()+ " bytes");
			return bo;
		}
		catch (Exception e)
		{
			System.out.println("    Exception: " +e.getClass().getName());
			if (DEBUG) e.printStackTrace(System.out);
			return null;
		}
	}
	
	private static void dumpBytes(byte[] b)
	{
		for (int i = 0; i < b.length; i++)
		{
			if ((0x00FF&b[i]) < 16)
				System.out.print("0");
			System.out.print(Integer.toHexString(b[i] & 0x00FF));
			System.out.print(" ");
			if (((i + 1) % 15) == 0)
				System.out.println();
		}
		System.out.println();
	}

	private static byte[] mungeBytes(byte[] base, String srcStr, String replStr)
	{
		byte[] src = srcStr.getBytes();
		byte[] repl = replStr.getBytes();
		
		if (src.length != repl.length)
			throw new Error("Can't handle different length src/repl ("+src+ ", "+repl+")");

		for (int i = 0; i < base.length; i++)
		{
			if (i > base.length - src.length)
				throw new Error("Didn't find it... ran out of room.");

			// found start
			if (base[i] == src[0])
			{
				//System.out.println("Checking " +i+ "...");
				int s = 0;
				while (s < src.length)
				{
					if (base[i+s] != src[s])
						break;
					s++;
				}
				
				// Found full match
				if (s == src.length)
				{
					//System.out.println("Found src at " +i+ "...");
					for (int r = 0; r < repl.length; r++)
					{
						base[i+r] = repl[r];
					}
					return base;
				}
			}
		}
		return null; // Never Reached
	}

	private static ByteArrayOutputStream canonicalOutput;
	private static Class canonicalClass;

	private static void checkClass(Class c)
	{
		// "fix" the canonical output so it looks like it was
		// a serialization of the given class c.  Does the
		// munge on a *copy* of the canonical bytes.
		byte[] mungedOutput = mungeBytes(canonicalOutput.toByteArray(), 
						 canonicalClass.getName(), 
						 c.getName());
		
		if (DEBUG) dumpBytes(mungedOutput);
		
		// Expect an instance of c from mungedOutput
		tryRead(c, mungedOutput);

		// Hmm.. just for kicks
		generateBytes(c);
		System.out.println();
	}

	public static void main(String[] args)
	{
		// Canonical, correct format:
		System.out.println("Generating canonical stream");
		canonicalClass = Test0001.class;
		canonicalOutput = generateBytes(Test0001.class);
	
		String canonName = canonicalClass.getName();
		String checkName = canonName.substring(0, canonName.length() - 4); // strip version no.
	
		Class[] tests = TestSerialFields.class.getDeclaredClasses();
		// XXX output assumes returned test array is consistently sorted...
		for (int i = 0; i < tests.length; i++)
		{
			if (tests[i].getName().startsWith(checkName))
				checkClass(tests[i]);
		}
	}
}

/* Expected Output:
Generating canonical stream
  Serializing TestSerialFields$Test0001
    put x: int=42 
    put y: int=42 
    OK: 132 bytes
TestSerialFields$Test0016:
  Deserializing from Test0001 stream...
    OK. Got: TestSerialFields$Test0016: 42,42
  Serializing TestSerialFields$Test0016
    OK: 63 bytes

TestSerialFields$Test0015:
  Deserializing from Test0001 stream...
    OK. Got: TestSerialFields$Test0015: 0,0
  Serializing TestSerialFields$Test0015
    OK: 108 bytes

TestSerialFields$Test0014:
  Deserializing from Test0001 stream...
    Exception: java.lang.IllegalArgumentException
  Serializing TestSerialFields$Test0014
    Exception: java.lang.IllegalArgumentException

TestSerialFields$Test0012:
  Deserializing from Test0001 stream...
    OK. Got: TestSerialFields$Test0012: 42,42
  Serializing TestSerialFields$Test0012
    OK: 63 bytes

TestSerialFields$Test0011:
  Deserializing from Test0001 stream...
    OK. Got: TestSerialFields$Test0011 instance
  Serializing TestSerialFields$Test0011
    Exception: java.io.NotActiveException

TestSerialFields$Test0010:
  Deserializing from Test0001 stream...
    x: int=42
    y: int=42
    xodd: (defaulted) Object=null
    yodd: (defaulted) Object=null
    OK. Got: TestSerialFields$Test0010 instance
  Serializing TestSerialFields$Test0010
    put xodd: string='putfield test string' 
    put yodd: string='putfield test string' 
    OK: 133 bytes

TestSerialFields$Test0008:
  Deserializing from Test0001 stream...
    x: int=42
    y: int=42
    OK. Got: TestSerialFields$Test0008 instance
  Serializing TestSerialFields$Test0008
    put x: int=42 
    put y: int=42 
    OK: 63 bytes

TestSerialFields$Test0005:
  Deserializing from Test0001 stream...
    x: int=42
    y: int=42
    OK. Got: TestSerialFields$Test0005 instance
  Serializing TestSerialFields$Test0005
    OK: 47 bytes

TestSerialFields$Test0002:
  Deserializing from Test0001 stream...
    x: int=42
    y: int=42
    xd: (defaulted) double=0.0 
    yd: (defaulted) double=0.0 
    OK. Got: TestSerialFields$Test0002 instance
  Serializing TestSerialFields$Test0002
    put xd: double=6.9 
    put yd: double=6.9 
    OK: 73 bytes

TestSerialFields$Test0001:
  Deserializing from Test0001 stream...
    x: int=42
    y: int=42
    OK. Got: TestSerialFields$Test0001 x=0; y=0; foo=0
  Serializing TestSerialFields$Test0001
    put x: int=42 
    put y: int=42 
    OK: 132 bytes

*/
