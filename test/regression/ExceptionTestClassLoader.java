/*
 * Test that classloader can properly supply exceptions.
 *
 * This test needs ExceptionTest.java
 *
 * @author Benjamin Reed <breed@almaden.ibm.com>
 */
import java.io.*;
import java.util.*;
import java.lang.reflect.*;

public class ExceptionTestClassLoader extends ClassLoader {
  private Class myLoadClass( String name ) throws ClassNotFoundException {
    try {
      if ( name.equals( "ExceptionTest" ) ) {
	File file = new File( "ExceptionTest" );
	FileInputStream fis = new FileInputStream( file );
	byte buffer[] = new byte[(int)file.length()];
	int size = 0;
	while( size < file.length() ) {
	  int rc = fis.read( buffer, size, (int)file.length() - size );
	  size += rc;
	}
	return defineClass( null, buffer, 0, size );
      }
    } catch( Exception e ) {}
    throw new ClassNotFoundException( name );
  }

  protected Class loadClass( String name, boolean resolve ) throws ClassNotFoundException {
    if (name.equals("java.lang.Exception")) {
       System.out.println("Success 2.");
    }
    Class theclass = findLoadedClass( name );
    if ( theclass == null ) {
      try {
	theclass = findSystemClass( name );
      } catch( Exception e ) { // Yes Exception is a bit too general
	theclass = myLoadClass( name );
      }
    }
    if ( theclass != null && resolve ) {
      resolveClass( theclass );
    }
    return theclass;
  }

  public static void main( String args[] ) {
    new File("ExceptionTest.class").renameTo(new File("ExceptionTest"));
    Vector v = new Vector();
    ClassLoader cl = new ExceptionTestClassLoader();
    try {
      Class classArgs[] = new Class[1];
      classArgs[0] = String.class;
      Constructor con = 
	cl.loadClass( "ExceptionTest" ).getConstructor( classArgs );
      Object instArgs[] = new Object[1];
      instArgs[0] = "me";
      con.newInstance( instArgs );
    } catch ( Exception e ) {
      e.printStackTrace();
    }
    System.out.println("Success 5.");
  }
}





// Sort output
/* Expected Output:
Success 1.
Success 2.
Success 3.
Success 4.
Success 5.
*/
