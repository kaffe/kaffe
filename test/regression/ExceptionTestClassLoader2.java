/*
 * Test what happens if classloaders don't properly supply exceptions.
 *
 * This test needs ExceptionTest.java
 *
 * Kaffe-specific test.
 *
 * NB:  this tests depends on the lazyness with which Kaffe resolves
 * symbols.  (The JLS leaves this open.)
 * This test fails under both JDK 1.1 and 1.2 (for different reasons.)
 *
 * @author Benjamin Reed <breed@almaden.ibm.com>
 */
import java.io.*;
import java.util.*;
import java.lang.reflect.*;

public class ExceptionTestClassLoader2 extends ClassLoader {
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
    /* don't respond */ 
    if (name.equals("java.lang.Exception")) {
       System.out.println("Success 2.");
       return (null);
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
    ClassLoader cl = new ExceptionTestClassLoader2();

    try {
      Class classArgs[] = new Class[1];
      classArgs[0] = String.class;
      Constructor con = 
	cl.loadClass( "ExceptionTest" ).getConstructor( classArgs );
      Object instArgs[] = new Object[1];
      instArgs[0] = "me";
      con.newInstance( instArgs );
    } catch ( InvocationTargetException e ) {
      Throwable t = e.getTargetException();
      // JDK 1.1 throws a ClassNotFoundException here, which is wrong.
      // JDK 1.2 loads so eagerly that we don't see the "Success 1." print
      // Also, JDK 1.2 asks for the exception in the call to getConstructor
      // which is pretty darn early.  This means we don't even get an
      // InvocationTargetException.
      if (t instanceof NoClassDefFoundError) {
	    System.out.println("Success 3.");
      } else {
	  System.out.println(t);
      }
    } catch ( Exception e ) {
      e.printStackTrace();
    }
    System.out.println("Success 4.");
  }
}






/* Expected Output:
Success 2.
Success 3.
Success 4.
*/
