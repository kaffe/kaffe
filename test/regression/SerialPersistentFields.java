/*
 * Test the immutability of the "serialPersistentFields" associated
 * with a class.
 *
 * Copyright (c) 2002 Pat Tullmann
 * All Rights Reserved.
 *
 * This file is released into the public domain.
 *
 * @author Pat Tullmann <pat_kaffe@tullmann.org>
 */
import java.io.*;

public class SerialPersistentFields
implements Serializable
{
	private int i;
	private long l;
	private float f = 0;
	private String str;
	private Integer I;
	private Throwable th = null;

	private static final ObjectStreamField[] serialPersistentFields =
	{
		new ObjectStreamField("i", int.class),
		new ObjectStreamField("f", float.class),
		new ObjectStreamField("I", Integer.class),
		//null
	};

	SerialPersistentFields(String t)
	{
		i = 0xDEADBEEF;
		l = 0xBabeBabeBabeBabeL;
		str = t;
		I = new Integer(0);
	}
	
	void mungeLow()
	{
		SerialPersistentFields.serialPersistentFields[1] = new ObjectStreamField("str", String.class);
	}

	void mungeHigh()
	{
		ObjectStreamClass streamSelf = ObjectStreamClass.lookup(SerialPersistentFields.class);
		ObjectStreamField[] fs = streamSelf.getFields();
		ObjectStreamField hack = new ObjectStreamField("str", String.class);
		fs[1] = hack;
		
		// did it stick?
		fs = streamSelf.getFields();
		if (fs[1] == hack)
			throw new Error("Munge stuck.");
	}
/*
	private void readObject(ObjectInputStream s)
		throws IOException 
	{
	}
	
	private void writeObject(ObjectOutputStream s)
		throws IOException
	{
	}
*/	

	void check()
		throws Exception
	{
		ObjectStreamClass streamSelf = ObjectStreamClass.lookup(SerialPersistentFields.class);

		ObjectStreamField[] fs = streamSelf.getFields();
		for (int i = 0; i < fs.length; i++)
		{
			ObjectStreamField f = fs[i];
			System.out.println(i+ ": "
					   + f.getType().getName()
					   + " "
					   + f.getName()
					   + " // "
					   //+ "@" +f.getOffset()
					   + (f.isUnshared() ? "unshared" : "shared"));
		}
	}
	
	public static void main(String[] args)
		throws Exception
	{
		SerialPersistentFields sf = new SerialPersistentFields(null);

		ObjectOutputStream oos = new ObjectOutputStream(new ByteArrayOutputStream());
		oos.writeObject(sf);

		System.out.println("Initial:");
		sf.check();

		System.out.println("Munge-static:");
		sf.mungeLow();
		sf.check();

		System.out.println("Munge ObjectStreamClass:");
		sf.mungeHigh();
		sf.check();
	}
}

/* Expected Output:
Initial:
0: float f // shared
1: int i // shared
2: java.lang.Integer I // shared
Munge-static:
0: float f // shared
1: int i // shared
2: java.lang.Integer I // shared
Munge ObjectStreamClass:
0: float f // shared
1: int i // shared
2: java.lang.Integer I // shared
*/
