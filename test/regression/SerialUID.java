/**
 * Test various serialVersionUID fields type.
 * Based on kaffe.tools.serivalver.SerialVer class.
 *
 * @author Edouard G. Parmelan <egp@free.fr>
 */
import java.io.ObjectStreamClass;

class SerialUIDByte
    implements java.io.Serializable
{
    public static final byte serialVersionUID = 1;
    public static final byte trash1 = 0x11;
    public static final byte trash2 = 0x22;
    public static final byte trash3 = 0x33;

    // KJC allways emit <clinit>() when there is a static initialized field
    // so force <clinit>() for all compilers
    static { SerialUID0.gen_clinit(); };
}

class SerialUIDChar
    implements java.io.Serializable
{
    public static final char serialVersionUID = '\u0001';
    public static final byte trash2 = 0x22;
    public static final byte trash3 = 0x33;

    // KJC allways emit <clinit>() when there is a static initialized field
    // so force <clinit>() for all compilers
    static { SerialUID0.gen_clinit(); };
}

class SerialUIDShort
    implements java.io.Serializable
{
    public static final short serialVersionUID = 0x0001;
    public static final byte trash2 = 0x22;
    public static final byte trash3 = 0x33;

    // KJC allways emit <clinit>() when there is a static initialized field
    // so force <clinit>() for all compilers
    static { SerialUID0.gen_clinit(); };
}

class SerialUIDInt
    implements java.io.Serializable
{
    public static final int serialVersionUID = 0x0001;
    public static final byte trash2 = 0x22;
    public static final byte trash3 = 0x33;

    // KJC allways emit <clinit>() when there is a static initialized field
    // so force <clinit>() for all compilers
    static { SerialUID0.gen_clinit(); };
}


class SerialUIDFloat
    implements java.io.Serializable
{
    public static final float serialVersionUID = 0.1f;
    public static final byte trash2 = 0x22;
    public static final byte trash3 = 0x33;

    // KJC allways emit <clinit>() when there is a static initialized field
    // so force <clinit>() for all compilers
    static { SerialUID0.gen_clinit(); };
}

class SerialUIDDouble
    implements java.io.Serializable
{
    public static final double serialVersionUID = 0.1;

    // KJC allways emit <clinit>() when there is a static initialized field
    // so force <clinit>() for all compilers
    static { SerialUID0.gen_clinit(); };
}


class SerialUIDObject
    implements java.io.Serializable
{
    public static final Long serialVersionUID = new Long(1L);

    // KJC allways emit <clinit>() when there is a static initialized field
    // so force <clinit>() for all compilers
    static { SerialUID0.gen_clinit(); };
}


class SerialUID0
    implements java.io.Serializable
{
    public static final long serialVersionUID = 1234567890123456789L;

    // KJC allways emit <clinit>() when there is a static initialized field
    // This method will force <clinit>() for all compilers
    static void gen_clinit() { }
}


public class SerialUID
    extends SerialUID0
{
    
    public long longField;

    public static void main(String[] args) {
	if (args.length == 0) {
	    args = new String[] { "SerialUID0", "SerialUID",
				  "SerialUIDByte", "SerialUIDChar",
				  "SerialUIDShort", "SerialUIDInt",
				  "SerialUIDFloat", "SerialUIDDouble",
				  "SerialUIDObject" };
	}
	for (int i = 0; i < args.length; i++) {
	    try {
		Class clz = Class.forName(args[i]);
		ObjectStreamClass stream = ObjectStreamClass.lookup(clz);
		if (stream != null) {
		    System.out.println(args[i] + ": static final long serialVersionUID = " + stream.getSerialVersionUID() + "L;");
		}
		else {
		    System.out.println("Class " + args[i] + " is not Serializable.");
		}
	    }
	    catch (ClassNotFoundException _) {
		System.out.println("Class " + args[i] + " not found.");
	    }
	    catch (Throwable t) {
		System.out.println(args[i] + ": " + t.toString());
	    }
	}
    }
}

/* Expected Output:
SerialUID0: static final long serialVersionUID = 1234567890123456789L;
SerialUID: static final long serialVersionUID = -6331010889134945863L;
SerialUIDByte: static final long serialVersionUID = 5112146195474272476L;
SerialUIDChar: static final long serialVersionUID = 8699205732610606234L;
SerialUIDShort: static final long serialVersionUID = 5164061412271023960L;
SerialUIDInt: static final long serialVersionUID = -5906151353932303441L;
SerialUIDFloat: static final long serialVersionUID = 4977706161661264981L;
SerialUIDDouble: static final long serialVersionUID = -4581434795115610980L;
SerialUIDObject: static final long serialVersionUID = -2830734355633622104L;
*/
