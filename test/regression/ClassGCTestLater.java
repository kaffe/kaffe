/**
 * simple test for class finalization
 *
 * @author Godmar Back <gback@cs.utah.edu>
 */
import java.io.*;
import java.lang.reflect.*;

public class ClassGCTestLater
{
    public ClassGCTestLater() throws Exception
    {
	Class c = ClassGCTest.class;
	String s = c.getName();
	if (!s.equals("ClassGCTest"))
	  System.out.println("Failure: name is " + s
			     + " (in class " + c + ")");
	/* I think getConstructor should be enough, since we're in the
           same package, but it fails :-(  -oliva */
	Constructor cc = c.getConstructor(new Class [] {});
	if (!cc.toString().equals("public ClassGCTest()"))
	    System.out.println("Failure: name is " + cc.toString()
			       + " (in ctor " + cc + ")");
	if (!cc.newInstance(new Object[] {}).
		toString().startsWith("ClassGCTest")) {
	    System.out.println("Failure newInstance.");
	}
    }
}
