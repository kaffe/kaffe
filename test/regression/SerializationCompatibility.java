/*
 * Test the compatiblity of various serialized objects against other VMs.
 * When run in the Kaffe regression suite, just test that serialize/deserialize
 * of various types is working.  Can be used to generate output on one
 * VM and read it in on another.
 *
 * Copyright (c) 2002, 2003 Pat Tullmann
 * All Rights Reserved.
 *
 * This file is released into the public domain.
 *
 * @author Pat Tullmann <pat_kaffe@tullmann.org>
 */
import java.io.*;
import java.util.*;
import java.math.*;

public class SerializationCompatibility
{
	static final String FILENAME = "serializedFoos.bin";

	static class ObjectCount
	implements Serializable
	{
		int objectCount;
		long secondField;
		
		ObjectCount(int ct)
		{
			this.objectCount = ct;
			this.secondField = 0xBabeBabeBabeBabeL;
		}

		public String toString()
		{
			return "ObjectCount(" +this.objectCount+ ")";
		}

		public boolean equals(Object x)
		{
			if (x instanceof ObjectCount)
			{
				ObjectCount other = (ObjectCount)x;
				return (other.objectCount == this.objectCount)
					&& (other.secondField == this.secondField);
			}
			return false;
		}

	}

	static class DefaultPutFields
	implements Serializable
	{
		private int x = 11;
		private int y = 12;

		public String toString()
		{
			return (this.getClass().getName() + ": " +x+ "," +y);
		}

		private void writeObject(ObjectOutputStream stream) 
			throws IOException, ClassNotFoundException
		{
			this.x = 32;
			this.y = 99;
			ObjectOutputStream.PutField pf = stream.putFields();
			pf.put("x", 49);
			stream.writeFields();
		}

		private void readObject(ObjectInputStream stream) 
			throws IOException, ClassNotFoundException
		{
			stream.defaultReadObject();
		}
	}

	private static class GetTypeMismatch // object typemismatch in get
	implements Serializable
	{
		private Integer x = new Integer(17);
		private Integer y = new Integer(27);

		public String toString()
		{
			return (this.getClass().getName() + ": " +x+ "," +y);
		}

		private void writeObject(ObjectOutputStream stream) 
			throws IOException, ClassNotFoundException
		{
			ObjectOutputStream.PutField pf1 = stream.putFields();
			pf1.put("x", this.x);
			pf1.put("y", this.y);
			stream.writeFields();
		}

		private void readObject(ObjectInputStream stream) 
			throws IOException, ClassNotFoundException
		{
			ObjectInputStream.GetField gf1 = stream.readFields();
			this.x = (Integer)gf1.get("x", new String("Missed X?"));
			this.y = (Integer)gf1.get("y", new String("Missed Y?"));
		}
	}

	private static class PutTypeMismatch // object typemismatch in put
	implements Serializable
	{
		private Integer x = new Integer(11);
		private Integer y = new Integer(32);

		public String toString()
		{
			return (this.getClass().getName() + ": " +x+ "," +y);
		}

		private void writeObject(ObjectOutputStream stream) 
			throws IOException, ClassNotFoundException
		{
			ObjectOutputStream.PutField pf1 = stream.putFields();
			pf1.put("x", new String("whax?"));
			pf1.put("y", new String("whay?"));
			stream.writeFields();
		}

		private void readObject(ObjectInputStream stream) 
			throws IOException, ClassNotFoundException
		{
			ObjectInputStream.GetField gf1 = stream.readFields();
			gf1.get("x", new String("missedx."));
			gf1.get("y", new String("missedy"));
		}
	}

	static String testString(final int id)
	{
		// XXX include some non-ASCII chars
		return "This is a long, but not that compilcated test string for Serialization: " +id;
	}

	static Hashtable testHashtable(final int id)
	{
		Hashtable htab = new Hashtable();
		for (int i = 0; i < 25 + id; i++)
		{
			htab.put("" +i+ "", new Integer(i));
		}
		return htab;
	}

	static HashMap testHashMap(final int id)
	{
		HashMap h = new HashMap();
		for (int i = 0; i < 25 + id; i++)
		{
			h.put("" +i+ "", new Integer(i));
		}
		return h;
	}

	static BigInteger testBigInt(int id)
	{
		return new BigInteger("1010101010101101010101010102102102013103913019301210" + id);
	}

	static void checkFile(String filename)
		throws IOException, ClassNotFoundException, Error
	{
		FileInputStream fis = new FileInputStream (filename);
		ObjectInputStream ois = new ObjectInputStream (fis);

		try
		{
			checkStream(ois);
		}
		catch (java.io.EOFException eofe)
		{
			// okay
		}
	}
	
	private static void check(Object fromStream, Object expected)
		throws Error
	{
		if (fromStream.equals(expected))
			return;
		
		msg("Error: Got object: " +fromStream);
		msg("  but expected: " +expected);
		throw new Error("check failed");
	}


	private static void checkStream(ObjectInputStream ois)
		throws IOException, ClassNotFoundException, Error
	{
		int objCt = 0;
		
		while (true)
		{
			Object o = ois.readObject();
			if (o == null)
			{
				msg("End of stream.");
				ois.close();
				return;
			}

			msg("Read object " +objCt+ ":" +o);

			if (o instanceof String)
				check(o, testString(objCt));
			else if (o instanceof BigInteger)
				check(o, testBigInt(objCt));
			else if (o instanceof Hashtable)
				check(o, testHashtable(objCt));
			else if (o instanceof HashMap)
				check(o, testHashMap(objCt));
			else if (o instanceof ObjectCount)
				check(o, new ObjectCount(objCt));
			else if (o instanceof DefaultPutFields)
				System.out.println(o);
			else if (o instanceof GetTypeMismatch)
				System.out.println(o);
			else if (o instanceof PutTypeMismatch)
				System.out.println(o);
			else
			{
				throw new Error("Unexpected object: " +o);
			}
			objCt++;
		}
	}

	private static void doWrite(ObjectOutputStream oos, Object o)
		throws IOException
	{
		oos.writeObject(o);
		msg("Wrote object: " +o+ "  (osc = " +ObjectStreamClass.lookup(o.getClass())+ ")");
	}

	private static void generateTests(String filename)
		throws IOException
	{
		FileOutputStream fos = new FileOutputStream (filename);
		ObjectOutputStream oos = new ObjectOutputStream (fos);

		int objCt = 0;

		for (int i = 0; i < 120; i++)
		{
			int objType = (i % 9); // XXX randomly select.
			switch (objType)
			{
			case 0: doWrite(oos, testBigInt(objCt));
				break;
			case 1: doWrite(oos, new ObjectCount(objCt)); 
				break;
			case 2: doWrite(oos, testString(objCt));
				break;
			case 3: doWrite(oos, testHashtable(objCt));
				break;
			case 4: doWrite(oos, testHashMap(objCt));
				break;
			case 5: doWrite(oos, new ObjectCount(objCt));
				break;
			case 6: doWrite(oos, new DefaultPutFields());
				break;
			case 7: doWrite(oos, new GetTypeMismatch());
				break;
			case 8: doWrite(oos, new PutTypeMismatch());
				break;
			}
			objCt++;
		}
		oos.flush();
		oos.close();
	}

	private static final void msg(String m)
	{
		System.out.println(m);
	}

	public static void main(String[] args)
		throws IOException, ClassNotFoundException
	{
		/*
		 * No args == run write/read test to default file
		 * 1 arg == write to given file
		 * 2 args == "read" <file>
		 */

		if (args.length == 0)
		{
			generateTests(FILENAME);
			System.out.println();
			checkFile(FILENAME);
			new File(FILENAME).delete();
		}
		else if (args.length == 1)
		{
			generateTests(args[0]);
		}
		else if (args.length == 2)
		{
			checkFile(args[1]);
		}
		else
		{
			System.err.println("Usage:");
			System.err.println("  SerializationCompatibility: do write/read test with default file");
			System.err.println("  SerializationCompatibility <filename>: do write test to given file");
			System.err.println("  SerializationCompatibility read <filename>: do read test from given file");
			System.exit(1);
		}
	}
}
