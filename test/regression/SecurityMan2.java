import java.net.*;
import java.security.*;

public class SecurityMan2 
{
	static class mytest
	{
		mytest()
		{
		}
	}

	static class MySM extends SecurityManager
	{
		public void checkPermission(java.security.Permission perm)
		{
			if (perm.getName().equals("exitVM"))
			{
				throw new SecurityException("no exit !");
			}
		}
	}

	static public void main(String args[]) throws Exception
	{
		System.setSecurityManager(new MySM());
		
		URLClassLoader cl = (URLClassLoader)SecurityMan2.class.getClassLoader();
		URLClassLoader cl2 = new URLClassLoader(cl.getURLs());
		Class c = Class.forName("SecurityMan2$mytest", true, cl2);

		c.newInstance();
		System.out.println("Ok");
	}
}
/* Expected Output:
Ok
*/
