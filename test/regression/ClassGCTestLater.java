/*
 * Sample class for ClassLoader tests
 *
 * Written by Godmar Back <gback@cs.utah.edu>
 */
/* in order to be usable by class loader, both 
 * class and constructor must be public 
 */
import java.lang.reflect.*;

public class ClassGCTestLater
{
    public ClassGCTestLater() throws Exception
    {
	Class c = ClassGCTest.class;
	String s = c.getName();
	if (!s.equals("ClassGCTest"))
	    System.out.println("Failure: name is " + s);
	Constructor cc = c.getConstructor(new Class [] {});
	if (!cc.toString().equals("public ClassGCTest()"))
	    System.out.println("Failure: name is " + cc.toString());
	if (!cc.newInstance(new Object[] {}).
		toString().startsWith("ClassGCTest")) {
	    System.out.println("Failure newInstance.");
	}
    }
}
