public class ArrayForName {

    public static void testLoadArray() throws Exception {

	// Loading by built-in type ID is not allowed
	// int          != I
	// boolean      != Z
	// long         != J
	// float        != F
	// double       != D
	// byte         != B
        // short        != S
	// char         != C
	// void         != V

	expect("I", "Exception");
	expect("Z", "Exception");
	expect("J", "Exception");
	expect("F", "Exception");
	expect("D", "Exception");
	expect("B", "Exception");
	expect("S", "Exception");
	expect("C", "Exception");
	expect("V", "Exception");

	// Not possible to load by builtin type name

	expect("int",     "Exception");
	expect("boolean", "Exception");
	expect("long",    "Exception");
	expect("float",   "Exception");
	expect("double",  "Exception");
	expect("byte",    "Exception");
	expect("short",   "Exception");
	expect("char",    "Exception");
	expect("void",    "Exception");

	// Test loading an array by built-in type id
	// int[]        == [I
	// int[][]      == [[I
	// boolean[]    == [Z
	// boolean[][]  == [[Z
	// long[]       == [J
	// long[][]     == [[J
	// float[]      == [F
	// float[][]    == [[F
	// double[]     == [D
	// double[][]   == [[D
	// byte[]       == [B
	// byte[][]     == [[B
	// short[]      == [S
	// short[][]    == [[S
	// char[]       == [C
	// char[][]     == [[C

	expect("[I",  "int[]");
	expect("[[I", "int[][]");
	expect("[Z",  "boolean[]");
	expect("[[Z", "boolean[][]");
	expect("[J",  "long[]");
	expect("[[J", "long[][]");
	expect("[F",  "float[]");
	expect("[[F", "float[][]");
	expect("[D",  "double[]");
	expect("[[D", "double[][]");
	expect("[B",  "byte[]");
	expect("[[B", "byte[][]");
	expect("[S",  "short[]");
	expect("[[S", "short[][]");
	expect("[C",  "char[]");
	expect("[[C", "char[][]");

	// Array of type void is not allowed

	expect("[V",    "Exception");
	expect("[[V",   "Exception");
	expect("[[[V",  "Exception");

	// When loading an array using the built-in
	// type id, id must be at end of string

	expect("[II",   "Exception");
	expect("[ZZ",   "Exception");
	expect("[JJ",   "Exception");
	expect("[FF",   "Exception");
	expect("[DD",   "Exception");
	expect("[BB",   "Exception");
	expect("[SS",   "Exception");
	expect("[CC",   "Exception");
	expect("[ZZ",   "Exception");
	expect("[C;",   "Exception");
	expect("[C\0;", "Exception");

	// [L + Class + ;
	// Primitive Class name is not valid 

	expect("[Lint;",     "Exception");
	expect("[Lboolean;", "Exception");
	expect("[Llong;",    "Exception");
	expect("[Lfloat;",   "Exception");
	expect("[Ldouble;",  "Exception");
	expect("[Lbyte;",    "Exception");
	expect("[Lshort;",   "Exception");
	expect("[Lchar;",    "Exception");
	expect("[Lvoid;",    "Exception");

	// java.lang.Object[]     == [Ljava.lang.Object;
	// java.lang.Object[][]   == [[Ljava.lang.Object;
	// java.lang.String[]     == [Ljava.lang.String;
	// java.lang.String[][]   == [[Ljava.lang.String;

	expect("[Ljava.lang.Object;",  "java.lang.Object[]");
	expect("[[Ljava.lang.Object;", "java.lang.Object[][]");
	expect("[Ljava.lang.String;",  "java.lang.String[]");
	expect("[[Ljava.lang.String;", "java.lang.String[][]");

	// L + Class must follow 0-N [ characters

	expect("Ljava.lang.Object;", "Exception");
	expect("Ljava.lang.String;", "Exception");

	// Misc invalid class names

	expect("L",          "Exception");
	expect("L;",         "Exception");
	expect("LS;",        "Exception");
	expect("[LObject;",  "Exception");
	expect("[[LObject;", "Exception");
	expect("[LString;",  "Exception");
	expect("[[LString;", "Exception");
	expect("[[String;",  "Exception");
	expect("[[Object;",  "Exception");
	expect("[[int;",     "Exception");
	expect("LString;",   "Exception");
	expect("L;",         "Exception");
	expect("[[Q",        "Exception");  // array of primitive type is not allowed
	expect("[void",      "Exception");  // array of void is not allowed
	expect("[[Ljava/lang/Object;", "Exception"); // classes must use . as seperator
	expect("[[Ljava.lang.String",  "Exception"); // need ; at the end of class name
	expect("[[java.lang.String;",  "Exception"); // need L after [
	expect("",           "Exception");
    }

    static void expect(String clsName, String expected) throws Exception {
	String result = loadByName(clsName);

	if (! result.equals(expected)) {
	    StringBuffer msg = new StringBuffer();

	    msg.append("for clsName \"" + clsName + "\" expected \"" +
		       expected + "\" but got \"" + result + "\"");

	    System.err.println(msg.toString());
	}
    }

    static String loadByName(String clsName) throws Exception {
    	try {
	    Class c = Class.forName(clsName);
	    if (c == null) {
		// This should never happen
		throw new NullPointerException("Class.forName(" +
		    clsName + ") returned null");
	    }
	    if (!c.isArray() && !c.isPrimitive()) {
		// In this case we tried to load a primitive
		// type by name (like "int"), but we got
		// an actual class that used that name.
		// We only try to load arrays and primitive
		// types in this set of tests, so just pretend
		// that we did not find a class named "int".
		return "Exception";
	    }
		
	    return getNameFromClass( c );
	} catch (IllegalArgumentException e) { // JDK 1.1
	    return "Exception";
	} catch (ClassNotFoundException e) { // JDK 1.2
	    return "Exception";
	}
    }

    static String getNameFromClass(Class type) {
	StringBuffer name = new StringBuffer();
    
	while (type.isArray()) {
	    name.append("[]");
	    type = type.getComponentType();
	}
	name.insert(0,type.getName());
	return name.toString();
    }
    
    public static void main(String[] argv) throws Exception {
	testLoadArray();	
	System.out.println("done");
    }
}

/* Expected Output:
done
*/
