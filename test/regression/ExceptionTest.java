/*
 * This file is used by ExceptionTestClassLoader.java
 *
 * @author Benjamin Reed <breed@almaden.ibm.com>
 */ 

import java.util.*;

public class ExceptionTest {
  String foo( int i ) {
    try {
      return (String)v.elementAt( i );
    } catch( Exception e ) {
      System.out.println( "Success 3." );
    } catch( NoClassDefFoundError e ) {
      e.printStackTrace();
    }
    return "Success 4.";
  }
  Vector v = new Vector();
  public ExceptionTest( String hi ) {
    String args[] = { "Success 1." };
    for( int i = 0; i < args.length; i++ ) {
      v.addElement( args[i] );
    }
    for( int i = 0; i <= args.length; i++ ) {
      System.out.println( foo( i ) );
    }
  }
  public static void main(String[] argv) {}
}

/* Expected Output:
*/
