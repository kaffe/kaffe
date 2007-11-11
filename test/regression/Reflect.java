import java.lang.reflect.*;

public class Reflect {

  public static void main(String[] argv)
  {
    String name;
    try {
      name = argv[0];
    }
    catch (ArrayIndexOutOfBoundsException _) {
      name = "java.lang.Runnable";
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
Method: public abstract void java.lang.Runnable.run()
*/
