
/*
 * Checks that our primordial loader can properly find resources. 
 */

import java.net.URL;
import java.net.JarURLConnection;

public class PrimordialLoaderTest
{

	public static void main (String[] args) throws java.io.IOException
	{
		/* get a resource that's somewhere in the bootclasspath */
		URL url = ClassLoader.getSystemResource("java/lang/Object.class");
 
		System.out.println (url.getProtocol()+" "+((JarURLConnection)url.openConnection()).getEntryName());

		java.util.Enumeration enum = ClassLoader.getSystemResources("java/lang/Object.class");

		while (enum.hasMoreElements())
		{
			url = (URL)enum.nextElement();

			System.out.println (url.getProtocol()+" "+((JarURLConnection)url.openConnection()).getEntryName());
		}

		System.out.println (ClassLoader.getSystemResourceAsStream("java/lang/Object.class").getClass());
	}

}


/* Expected Output:
jar java/lang/Object.class
jar java/lang/Object.class
class java.util.jar.JarFile$EntryInputStream
*/
