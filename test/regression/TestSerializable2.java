/**
 * Test Serialization when a super class is not Serializable.
 *
 * @author Edouard G. Parmelan <egp@free.fr>
 */
import java.io.*;
import java.util.*;
import java.lang.reflect.*;


public class TestSerializable2
{
    // this class is serializable as <init>() is not private
    static class NotSerialized {
	static int count = 0;
	public int magic = 123;

	public NotSerialized(int dummy) {
	    setMagic(321);
	}

	NotSerialized() {
	    setMagic(999);
	}

	private void setMagic(int magic) {
	    this.magic = magic;
	}
    }

    static class Serialized
	extends NotSerialized
	implements Serializable
    {
	static int count = 0;
	public int i;

	public Serialized(int i) {
	    super(i);
	    this.i = i;
	}

	public String toString() {
	    return "Serialized-" + i + " magic " + magic;
	}
    }

    // this class is not serializable as it does not have <init>()
    static class NotSerializable1 {
	static int count = 0;
	public int magic = 123;

	public NotSerializable1(int dummy) {
	    setMagic(321);
	}

	private void setMagic(int magic) {
	    this.magic = magic;
	}
    }

    static class Serialized1
	extends NotSerializable1
	implements Serializable
    {
	static int count = 0;
	public int i;

	public Serialized1(int i) {
	    super(i);
	    this.i = i;
	}

	public String toString() {
	    return "Serialized1-" + i + " magic " + magic;
	}
    }

 
    // this class is not serializable as <init>() is private
    static class NotSerializable2 {
	static int count = 0;
	public int magic = 123;

	public NotSerializable2(int dummy) {
	    setMagic(321);
	}

	private NotSerializable2() {
	    setMagic(999);
	}

	private void setMagic(int magic) {
	    this.magic = magic;
	}
    }

    static class Serialized2
	extends NotSerializable2
	implements Serializable
    {
	static int count = 0;
	public int i;

	public Serialized2(int i) {
	    super(i);
	    this.i = i;
	}

	public String toString() {
	    return "Serialized2-" + i + " magic " + magic;
	}
    }


    static void test(Object a) {
	System.out.println ("a = " + a);
	try {
	    FileOutputStream fos = new FileOutputStream ("frozen_serial");
	    ObjectOutputStream oos = new ObjectOutputStream (fos);
	    oos.writeObject (a);
	    oos.flush ();

	    FileInputStream fis = new FileInputStream ("frozen_serial");
	    ObjectInputStream ois = new ObjectInputStream (fis);
	    Object b = ois.readObject ();

	    System.out.println ("b = " + b);
	}
	catch (Exception e) {
	    System.out.println (e);
	}
    }


    public static void main (String argv[]) {
	test(new Serialized(0));
	test(new Serialized1(10));
	test(new Serialized2(10));
    }
}

/* Expected Output:
a = Serialized-0 magic 321
b = Serialized-0 magic 999
a = Serialized1-10 magic 321
java.io.InvalidClassException: TestSerializable2$NotSerializable1; Missing no-arg constructor for class
a = Serialized2-10 magic 321
java.io.InvalidClassException: TestSerializable2$NotSerializable2; IllegalAccessException
*/
