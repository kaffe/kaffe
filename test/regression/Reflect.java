import java.lang.*;
import java.lang.reflect.*;

public class Reflect {

  public static void main(String[] argv)
  {
    String name;
    try {
      name = argv[0];
    }
    catch (ArrayIndexOutOfBoundsException _) {
      name = "java.lang.System";
    }
    try {
      Class cls = Class.forName(name);
      boolean isfinal = false;

      // In a final class, methods legally may or may not be marked final.
      // To produce consistent output, remove the "final" keywords
      if (Modifier.isFinal(cls.getModifiers())) {
	isfinal = true;
      }

      Method[] meth = cls.getMethods();
      for (int i = 0; i < meth.length; i++) {
        System.out.println("Method: "
	  + (isfinal ? stripFinal(meth[i].toString()) : meth[i].toString()));
      }

      Constructor[] cnst = cls.getConstructors();
      for (int i = 0; i < cnst.length; i++) {
        System.out.println("Constructor: " + cnst[i]);
      }

      Field[] fld = cls.getFields();
      for (int i = 0; i < fld.length; i++) {
        System.out.println("Field: " + fld[i]);
      }
    }
    catch (ClassNotFoundException _) {
      System.out.println("Class not found");
    }
  }

  private static String stripFinal(String s) {
    int i = s.indexOf("final ");
    if (i == -1) {
      return(s);
    }
    return s.substring(0, i) + s.substring(i + 6);
  }
}

// Sort output
/* Expected Output:
Field: public static final java.io.InputStream java.lang.System.in
Field: public static final java.io.PrintStream java.lang.System.err
Field: public static final java.io.PrintStream java.lang.System.out
Method: public boolean java.lang.Object.equals(java.lang.Object)
Method: public java.lang.String java.lang.Object.toString()
Method: public native int java.lang.Object.hashCode()
Method: public native java.lang.Class java.lang.Object.getClass()
Method: public native void java.lang.Object.notify()
Method: public native void java.lang.Object.notifyAll()
Method: public static java.lang.SecurityManager java.lang.System.getSecurityManager()
Method: public static java.lang.String java.lang.System.getProperty(java.lang.String)
Method: public static java.lang.String java.lang.System.getProperty(java.lang.String,java.lang.String)
Method: public static java.lang.String java.lang.System.getenv(java.lang.String)
Method: public static java.lang.String java.lang.System.mapLibraryName(java.lang.String)
Method: public static java.lang.String java.lang.System.setProperty(java.lang.String,java.lang.String)
Method: public static java.util.Properties java.lang.System.getProperties()
Method: public static native int java.lang.System.identityHashCode(java.lang.Object)
Method: public static native long java.lang.System.currentTimeMillis()
Method: public static native void java.lang.System.arraycopy(java.lang.Object,int,java.lang.Object,int,int)
Method: public static void java.lang.System.exit(int)
Method: public static void java.lang.System.gc()
Method: public static void java.lang.System.load(java.lang.String)
Method: public static void java.lang.System.loadLibrary(java.lang.String)
Method: public static void java.lang.System.runFinalization()
Method: public static void java.lang.System.runFinalizersOnExit(boolean)
Method: public static void java.lang.System.setErr(java.io.PrintStream)
Method: public static void java.lang.System.setIn(java.io.InputStream)
Method: public static void java.lang.System.setOut(java.io.PrintStream)
Method: public static void java.lang.System.setProperties(java.util.Properties)
Method: public static void java.lang.System.setSecurityManager(java.lang.SecurityManager)
Method: public void java.lang.Object.wait() throws java.lang.InterruptedException
Method: public void java.lang.Object.wait(long) throws java.lang.InterruptedException
Method: public void java.lang.Object.wait(long,int) throws java.lang.InterruptedException
*/
