/*
 * Test various definitions of the semi-magical 'serialPersistentFields' field.
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

public class TestSerialPersistent
{
	private static class Default
	implements Serializable
	{
		private int i;
		private String s;

		// No serialPersistentFields specification
	}

	private static class DefaultTransient
	implements Serializable
	{
		private int i;
		private String s;
		transient private long not_expected_l;

		// No serialPersistentFields specification
	}

	private static class Subset
	implements Serializable
	{
		private int i;
		private String s;
		private long not_expected_l;

		private static final ObjectStreamField[] serialPersistentFields =
		{
			new ObjectStreamField("i", int.class),
			new ObjectStreamField("s", String.class)
		};
	}

	private static class Subclass
	extends Subset
	implements Serializable
	{
		private int xi;
		private String xs;
		private long not_expected_xl;

		private static final ObjectStreamField[] serialPersistentFields =
		{
			new ObjectStreamField("xi", int.class),
			new ObjectStreamField("xs", String.class)
		};
	}

	private static class FieldSort
	implements Serializable
	{
		private int ib;
		private int ia;
		private String sb;
		private int ic;
		private String sa;

		// Real fields should be sorted prim before ref, then alpha
		private static final ObjectStreamField[] serialPersistentFields =
		{
			new ObjectStreamField("ic", int.class),
			new ObjectStreamField("ib", int.class),
			new ObjectStreamField("sa", String.class),
			new ObjectStreamField("sb", String.class),
			new ObjectStreamField("ia", int.class)
		};
	}

	private static class TransientOverlap
	implements Serializable
	{
		private int i;
		transient private String s;
		transient private long not_expected_l;
		
		// Seems this list takes precedence over 'transient' keyword
		private static final ObjectStreamField[] serialPersistentFields =
		{
			new ObjectStreamField("i", int.class),
			new ObjectStreamField("s", String.class)
		};
	}

	private static class ContainsNull
	implements Serializable
	{
		private int i;
		private String s;
		
		// A null entry should make serialization impossible
		private static final ObjectStreamField[] serialPersistentFields =
		{
			null,
			new ObjectStreamField("s", String.class)
		};
	}

	private static class IsNull
	implements Serializable
	{
		private int i;
		private String s;
		
		// A null definition is just ignored?
		private static final ObjectStreamField[] serialPersistentFields = null;
	}

	private static class IsNoFieldsMagic
	implements Serializable
	{
		private int i;
		private String s;
		
		// Check the ObjecStreamClass.NO_FIELDS magic
		private static final ObjectStreamField[] serialPersistentFields = ObjectStreamClass.NO_FIELDS;
	}

	private static class NonPrivate
	implements Serializable
	{
		private int expected;

		// bogus contents should be silently ignored
		public static final ObjectStreamField[] serialPersistentFields =
		{
			null,
			new ObjectStreamField("xxxx", void.class),
		};
	}

	private static class WrongType
	implements Serializable
	{
		private int expected;

		// bogus type should be silently ignored
		public static final Object[] serialPersistentFields =
		{
			null,
			new ObjectStreamField("xxxx", void.class),
		};
	}

	private static class ReallyWrongType
	implements Serializable
	{
		private int expected;

		// bogus type should be silently ignored
		public static final int serialPersistentFields = 0x11;
	}

	private static class Empty
	implements Serializable
	{
		private int not_expected_i;
		private String not_expected_s;
		
		private static final ObjectStreamField[] serialPersistentFields =
		{
		};
	}

	private static class NamesMismatch
	implements Serializable
	{
		private int not_expected_t;
		private String not_expected_s;

		private static final ObjectStreamField[] serialPersistentFields =
		{
			new ObjectStreamField("l", long.class),
			new ObjectStreamField("c", Class.class)
		};
	}

	private static class SubtypeSpec
	implements Serializable
	{
		private int i;
		private String s;

		private static final ObjectStreamField[] serialPersistentFields =
		{
			new ObjectStreamField("i", int.class),
			new ObjectStreamField("s", Object.class) // vs. String.class
		};
	}

	private static class TypesMismatch
	implements Serializable
	{
		private int i;
		private String s;

		private static final ObjectStreamField[] serialPersistentFields =
		{
			new ObjectStreamField("i", String.class),
			new ObjectStreamField("s", int.class)
		};
	}

	// XXX JDK1.4 doesn't complain about this case.
	private static class Duplicates 
	implements Serializable
	{
		private int i;

		private static final ObjectStreamField[] serialPersistentFields =
		{
			new ObjectStreamField("i", int.class),
			new ObjectStreamField("i", int.class),
			new ObjectStreamField("i", int.class),
		};
	}

	// XXX JDK1.4.1 blows up with an internal error in ObjectStreamClass.lookup() on next two test:
	// See bug 4764280 (fixed in 1.4.2??)
	private static class Overlap1 // 2nd is correct
	implements Serializable
	{
		private int i;

		private static final ObjectStreamField[] serialPersistentFields =
		{
			new ObjectStreamField("i", String.class),
			new ObjectStreamField("i", int.class)
		};
	}

	// XXX JDK1.4.1 blows up with an internal error in ObjectStreamClass.lookup() on next two test:
	// See bug 4764280 (fixed in 1.4.2??)
	private static class Overlap2 // 1st is correct
	implements Serializable
	{
		private int i;

		private static final ObjectStreamField[] serialPersistentFields =
		{
			new ObjectStreamField("i", int.class),
			new ObjectStreamField("i", String.class)
		};
	}

	private static void mungeFields(Class c)
	{
		ObjectStreamClass cStreamer = ObjectStreamClass.lookup(c);
		ObjectStreamField[] fs = cStreamer.getFields();
		ObjectStreamField hack = new ObjectStreamField("vec", Vector.class);
		if (fs.length < 1)
			return;
		fs[0] = hack;
		
		// did it stick?
		fs = cStreamer.getFields();
		if (fs[0].equals(hack))
			throw new Error("Munge stuck in " +cStreamer);
	}

	private static void serializeIt(Object o)
		throws IOException
	{
		ByteArrayOutputStream baos= new ByteArrayOutputStream();
		ObjectOutputStream oos = new ObjectOutputStream(baos);
		oos.writeObject(o);
		oos.close();
		System.out.println("  Ok: " +baos.size()+ " bytes.");
	}

	private static void check(Class c)
	{
		try
		{
			System.out.println("Checking " +c.getName()+ ":");
			
			mungeFields(c);

			ObjectStreamClass cStreamer = ObjectStreamClass.lookup(c);

			ObjectStreamField[] fs = cStreamer.getFields();
			for (int i = 0; i < fs.length; i++)
			{
				ObjectStreamField f = fs[i];
				System.out.print("   [" +i+ "]: ");
				if (f == null)
					System.out.println("<null>");
				else
					System.out.println(
						f.getType().getName()
						+ " "
						+ f.getName()
						//+ " // "
						//+ "@" +f.getOffset()
						//+ ", "
						//+ (f.isUnshared() ? "unshared" : "shared")
						);
			}

			System.out.println(" Serialization:");
			serializeIt(c.newInstance());
		}
		catch (Exception e)
		{
			System.out.println("   THREW: " +e.getClass().getName());
			// For debugging:
			//System.out.println("   THREW: " +e);
			//e.printStackTrace(System.out);
		}
	}
	
	
	public static void main(String[] args)
		throws Exception
	{
		Class[] tests = TestSerialPersistent.class.getDeclaredClasses();
		// XXX output assumes returned test array is consistently sorted...
		for (int i = 0; i < tests.length; i++)
		{
			check(tests[i]);
		}
	}
}

/* Expected Output:
Checking TestSerialPersistent$Overlap2:
   [0]: int i
   [1]: java.lang.String i
 Serialization:
   THREW: java.io.InvalidClassException
Checking TestSerialPersistent$Overlap1:
   [0]: int i
   [1]: java.lang.String i
 Serialization:
   THREW: java.io.InvalidClassException
Checking TestSerialPersistent$Duplicates:
   [0]: int i
   [1]: int i
   [2]: int i
 Serialization:
  Ok: 76 bytes.
Checking TestSerialPersistent$TypesMismatch:
   [0]: int s
   [1]: java.lang.String i
 Serialization:
   THREW: java.io.InvalidClassException
Checking TestSerialPersistent$SubtypeSpec:
   [0]: int i
   [1]: java.lang.Object s
 Serialization:
   THREW: java.io.InvalidClassException
Checking TestSerialPersistent$NamesMismatch:
   [0]: long l
   [1]: java.lang.Class c
 Serialization:
   THREW: java.io.InvalidClassException
Checking TestSerialPersistent$Empty:
 Serialization:
  Ok: 47 bytes.
Checking TestSerialPersistent$ReallyWrongType:
   [0]: int expected
 Serialization:
  Ok: 72 bytes.
Checking TestSerialPersistent$WrongType:
   [0]: int expected
 Serialization:
  Ok: 66 bytes.
Checking TestSerialPersistent$NonPrivate:
   [0]: int expected
 Serialization:
  Ok: 67 bytes.
Checking TestSerialPersistent$IsNoFieldsMagic:
 Serialization:
  Ok: 57 bytes.
Checking TestSerialPersistent$IsNull:
   [0]: int i
   [1]: java.lang.String s
 Serialization:
  Ok: 82 bytes.
Checking TestSerialPersistent$ContainsNull:
   THREW: java.lang.NullPointerException
Checking TestSerialPersistent$TransientOverlap:
   [0]: int i
   [1]: java.lang.String s
 Serialization:
  Ok: 92 bytes.
Checking TestSerialPersistent$FieldSort:
   [0]: int ia
   [1]: int ib
   [2]: int ic
   [3]: java.lang.String sa
   [4]: java.lang.String sb
 Serialization:
  Ok: 116 bytes.
Checking TestSerialPersistent$Subclass:
   [0]: int xi
   [1]: java.lang.String xs
 Serialization:
  Ok: 146 bytes.
Checking TestSerialPersistent$Subset:
   [0]: int i
   [1]: java.lang.String s
 Serialization:
  Ok: 82 bytes.
Checking TestSerialPersistent$DefaultTransient:
   [0]: int i
   [1]: java.lang.String s
 Serialization:
  Ok: 92 bytes.
Checking TestSerialPersistent$Default:
   [0]: int i
   [1]: java.lang.String s
 Serialization:
  Ok: 83 bytes.
*/
