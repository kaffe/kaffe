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
      Method[] meth = cls.getMethods();
      for (int i = 0; i < meth.length; i++) {
        System.out.println("Method: " + meth[i].toString());
      }
      Constructor[] cnst = cls.getConstructors();
      for (int i = 0; i < cnst.length; i++) {
        System.out.println("Constructor: " + cnst[i].toString());
      }
      Field[] fld = cls.getFields();
      for (int i = 0; i < fld.length; i++) {
        System.out.println("Field: " + fld[i].toString());
      }
    }
    catch (ClassNotFoundException _) {
      System.out.println("Class not found");
    }
  }

}
