/*
 * Test the ability of the deserialization code to handle
 * odd mutations from the serialized version of the class.
 *
 * Copyright (c) 2002 Pat Tullmann
 * All Rights Reserved.
 *
 * This file is released into the public domain.
 *
 * @author Pat Tullmann <pat_kaffe@tullmann.org>
 */
import java.io.*;

// XXX test arrays
public class TestSerialVersions
{
	// Set to true to get stack traces and other printouts
	private static final boolean DEBUG = false;

	public static final long constantUID = 0x42L;

	private static class Test0001 // "original" (pristine) version
	implements Serializable
	{
		private static final long serialVersionUID = constantUID;
		private int x = 0x042;
		private int y = 0x069;

		public String toString()
		{
			return (this.getClass().getName()+ ": " +x+ ", " +y);
		}
	}
	
	private static class Test0002 // mismatch (widen) field types
	implements Serializable
	{
		private static final long serialVersionUID = constantUID;
		private double x = 0;
		private double y = 0;

		public String toString()
		{
			return (this.getClass().getName()+ ": " +x+ ", " +y);
		}
	}
	
	private static class Test0003 // mismatch (narrow) field types
	implements Serializable
	{
		private static final long serialVersionUID = constantUID;
		private short x = 0;
		private short y = 0;

		public String toString()
		{
			return (this.getClass().getName()+ ": " +x+ ", " +y);
		}
	}
	
	private static class Test0004 // mismatch (incompatible) field types
	implements Serializable
	{
		private static final long serialVersionUID = constantUID;
		private String x = "x";
		private String y = "y";

		public String toString()
		{
			return (this.getClass().getName()+ ": " +x+ ", " +y);
		}
	}
	
	private static class Test0005 // missing x,y fields
	implements Serializable
	{
		private static final long serialVersionUID = constantUID;

		public String toString()
		{
			return (this.getClass().getName()+ ": <no fields>");
		}
	}
	
	private static class Test0052 // missing x (but not y) field
	implements Serializable
	{
		private static final long serialVersionUID = constantUID;
		// private int x;
		private int y;

		public String toString()
		{
			return (this.getClass().getName()+ ": y=" +y);
		}
	}
	
	private static class Test0006 // Wrong serialVersionUID
	implements Serializable
	{
		private static final long serialVersionUID = constantUID + 0x69;
		private int x = 0x042;
		private int y = 0x069;

		public String toString()
		{
			return (this.getClass().getName()+ ": " +x+ ", " +y);
		}
	}
	

	// XXX I don't understand why this silently deserializes Test0001 correctly on JDK1.4...
	// I think the serialPersistentFields are really only used for
	// *writing* objects (in the default)...
	// Kaffe now does the "right thing" (throws an exception on this case).
	private static class Test0007 // Compatible via serialPersistentFields...
	implements Serializable
	{
		private static final long serialVersionUID = constantUID;
		private static final ObjectStreamField[] serialPersistentFields =
		{
			new ObjectStreamField("x", int.class),
			new ObjectStreamField("y", int.class)
		};

		private String x = "x";
		private int y = 0;

		public String toString()
		{
			return (this.getClass().getName()+ ": " +x+ ", " +y);
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
			return (this.getClass().getName()+ ": <no fields>");
		}
	}
	

	private static class Test0009 // Compatible, but bad serialPersistentFields...
	implements Serializable
	{
		private static final long serialVersionUID = constantUID;
		private static final ObjectStreamField[] serialPersistentFields =
		{
			new ObjectStreamField("x", String.class),
			new ObjectStreamField("y", String.class)
		};
		private int x;
		private int y;

		public String toString()
		{
			return (this.getClass().getName()+ ": " +x+ ", " +y);
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
			return (this.getClass().getName()+ ": " +x+ ", " +y);
		}
	}
	

	private static class Test0011 // Compatible, but different (and valid) serialPersistentFields...
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
		private String xodd;
		private String yodd;

		public String toString()
		{
			return (this.getClass().getName()+ ": " +x+ ", " +y);
		}
	}
	
	private static class Test0012 // "Same" as 0011, but with 'transient' keyword
	implements Serializable
	{
		private static final long serialVersionUID = constantUID;
		transient private int x;
		transient private int y;
		private String xodd;
		private String yodd;

		public String toString()
		{
			return (this.getClass().getName()+ ": " +x+ ", " +y);
		}
	}
	
	private static class Test0013 // Battle of the overlapping specs
	implements Serializable
	{
		private static final long serialVersionUID = constantUID;
		private static final ObjectStreamField[] serialPersistentFields =
		{
			new ObjectStreamField("x", int.class),
			new ObjectStreamField("y", int.class)
		};
		transient private int x;
		transient private int y;

		public String toString()
		{
			return (this.getClass().getName()+ ": " +x+ ", " +y);
		}
	}
	

	private static class Test0014 // Type mis-match but with an array
	implements Serializable
	{
		private static final long serialVersionUID = constantUID;
		private int[] x;
		private int[] y;

		public String toString()
		{
			return (this.getClass().getName()+ ": " +x+ ", " +y);
		}
	}
	

	private static void tryRead(Class c, byte[] bytes)
	{
		try
		{
			//System.out.println("Expecting a " +c.getName()+ " in stream...");
			System.out.println(c.getName()+ ":");
			ObjectInputStream ois = new ObjectInputStream(new ByteArrayInputStream(bytes));
			Object o = ois.readObject();
			if (!c.isInstance(o))
				throw new Exception("Object '" +o+ "' is not an instance of " +c);
			System.out.println("  OK. Got: " +o);
		}
		catch (Exception e)
		{
			System.out.println("  Deserialize exception: " +e.getClass().getName());
			if (DEBUG) e.printStackTrace(System.out);
		}

		// Show the fields:
		ObjectStreamClass osc = ObjectStreamClass.lookup(c);
		ObjectStreamField fs[] = osc.getFields();
		System.out.println("  Ser Fields:");
		for (int i = 0; i < fs.length; i++)
		{
			System.out.println("    [" +fs[i].getName()+ ":" +fs[i].getType().getName()+ "]");
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
			System.out.println("    OK.");
			return bo;
		}
		catch (Exception e)
		{
			System.out.println("    Serialization exception: " +e.getClass().getName());
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

		//dumpBytes(mungedOutput);

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
	
		Class[] tests = TestSerialVersions.class.getDeclaredClasses();
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
  Serializing TestSerialVersions$Test0001
    OK.
TestSerialVersions$Test0014:
  Deserialize exception: java.io.InvalidClassException
  Ser Fields:
    [x:[I]
    [y:[I]
  Serializing TestSerialVersions$Test0014
    OK.

TestSerialVersions$Test0013:
  OK. Got: TestSerialVersions$Test0013: 66, 105
  Ser Fields:
    [x:int]
    [y:int]
  Serializing TestSerialVersions$Test0013
    OK.

TestSerialVersions$Test0012:
  OK. Got: TestSerialVersions$Test0012: 0, 0
  Ser Fields:
    [xodd:java.lang.String]
    [yodd:java.lang.String]
  Serializing TestSerialVersions$Test0012
    OK.

TestSerialVersions$Test0011:
  OK. Got: TestSerialVersions$Test0011: 0, 0
  Ser Fields:
    [xodd:java.lang.String]
    [yodd:java.lang.String]
  Serializing TestSerialVersions$Test0011
    OK.

TestSerialVersions$Test0010:
  OK. Got: TestSerialVersions$Test0010: 0, 0
  Ser Fields:
    [xodd:java.lang.String]
    [yodd:java.lang.String]
  Serializing TestSerialVersions$Test0010
    Serialization exception: java.io.InvalidClassException

TestSerialVersions$Test0009:
  Deserialize exception: java.io.InvalidClassException
  Ser Fields:
    [x:java.lang.String]
    [y:java.lang.String]
  Serializing TestSerialVersions$Test0009
    Serialization exception: java.io.InvalidClassException

TestSerialVersions$Test0008:
  OK. Got: TestSerialVersions$Test0008: <no fields>
  Ser Fields:
    [x:int]
    [y:int]
  Serializing TestSerialVersions$Test0008
    Serialization exception: java.io.InvalidClassException

TestSerialVersions$Test0007:
  Deserialize exception: java.io.InvalidClassException
  Ser Fields:
    [x:int]
    [y:int]
  Serializing TestSerialVersions$Test0007
    Serialization exception: java.io.InvalidClassException

TestSerialVersions$Test0006:
  Deserialize exception: java.io.InvalidClassException
  Ser Fields:
    [x:int]
    [y:int]
  Serializing TestSerialVersions$Test0006
    OK.

TestSerialVersions$Test0052:
  OK. Got: TestSerialVersions$Test0052: y=105
  Ser Fields:
    [y:int]
  Serializing TestSerialVersions$Test0052
    OK.

TestSerialVersions$Test0005:
  OK. Got: TestSerialVersions$Test0005: <no fields>
  Ser Fields:
  Serializing TestSerialVersions$Test0005
    OK.

TestSerialVersions$Test0004:
  Deserialize exception: java.io.InvalidClassException
  Ser Fields:
    [x:java.lang.String]
    [y:java.lang.String]
  Serializing TestSerialVersions$Test0004
    OK.

TestSerialVersions$Test0003:
  Deserialize exception: java.io.InvalidClassException
  Ser Fields:
    [x:short]
    [y:short]
  Serializing TestSerialVersions$Test0003
    OK.

TestSerialVersions$Test0002:
  Deserialize exception: java.io.InvalidClassException
  Ser Fields:
    [x:double]
    [y:double]
  Serializing TestSerialVersions$Test0002
    OK.

TestSerialVersions$Test0001:
  OK. Got: TestSerialVersions$Test0001: 66, 105
  Ser Fields:
    [x:int]
    [y:int]
  Serializing TestSerialVersions$Test0001
    OK.

*/
