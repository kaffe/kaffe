/*
 * Sample class for ClassLoader tests
 *
 * Written by Godmar Back <gback@cs.utah.edu>
 */
/* in order to be usable by class loader, both 
 * class and constructor must be public 
 */
public class ClassGCTest
{
	public static class HObject {
		protected void finalize() throws Throwable {
			if (!ClassGC.gotOne) {
				ClassGC.gotOne = true;
				System.out.println("Success.");
			}
		}
	}

	public static Object f = new HObject();
}
