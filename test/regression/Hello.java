/*
 * Sample class for ClassLoader tests
 *
 * Written by Godmar Back <gback@cs.utah.edu>
 */
/* in order to be usable by class loader, both 
 * class and constructor must be public 
 */
public class Hello
{
	public Hello()
	{
		System.out.println("Hello World");
	}
}
