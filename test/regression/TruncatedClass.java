/*
 * TruncatedClass.java
 *
 * Test Kaffe's handling of truncated .class "files".
 *
 * Contributed by Pat Tullmann <pat@tullmann.org>
 */

import java.io.IOException;
import java.io.File;
import java.io.FileInputStream;
import java.util.HashSet;

public class TruncatedClass
{
	static class Loader
		extends ClassLoader
	{
		private Class c;
		private Throwable error;

		Loader(String name, byte[] bytes, int offset, int length)
		{
			this.c = null;
			this.error = null;
			try
			{
				this.c = defineClass(name, bytes, offset, length);
			}
			catch (ClassFormatError cfe)
			{
				this.error = cfe;
				return;
			}
			catch (NoClassDefFoundError cfe)
			{
				this.error = cfe;
				return;
			}
			catch (Throwable th)
			{
				System.out.println("Unexpected error loading " +name+ ": " +th);
				System.exit(11);
				// this.error = th;
				// return;
			}
			System.out.println("Succesfully loaded " +name+ ": " +this);
		}

		Throwable error()
		{
			return this.error;
		}
	}

	private static void diffBytes(String msg, byte[] a, byte[] b, int len)
	{
		for (int i = 0; i < len; i++)
		{
			if (a[i] != b[i])
			{
				System.out.println("ERROR: VM changed byte array!!! " + msg);
				System.exit(11);
			}
		}
	}

	private static HashSet seenErrors = new HashSet();

	static class ErrorTrack
	{
		private final Class c;
		private final String msg;
		
		ErrorTrack(Throwable th)
		{
			this.c = th.getClass();
			this.msg = th.getMessage();
		}

		public int hashCode()
		{
			return this.c.hashCode() * this.msg.hashCode();
		}

		public boolean equals(Object x)
		{
			if (x instanceof ErrorTrack)
			{
				ErrorTrack other = (ErrorTrack)x;
				return (this.c.equals(other.c))
					&& (this.msg.equals(other.msg));
			}
			return false;
		}
	}


	private static void showIfNew(String msg, Throwable th)
	{
		ErrorTrack t = new ErrorTrack(th);
		
		if (seenErrors.contains(t))
			return;
		
		seenErrors.add(t);
		
		System.out.println(msg + ":" + th);
	}

	public static void main(String[] args)
		throws IOException
	{
		String codeClassname = "TruncatedClass";
		String codeFilename = "./" +codeClassname+ ".class";
		int rc = 0;

		File codeFile = new File(codeFilename);

		FileInputStream codeStream = new FileInputStream(codeFile);

		byte[] originalBytes = new byte[16 * 1024];

		int len = codeStream.read(originalBytes);
		if (len == -1)
		{
			System.out.println("Failed to read " +codeFile);
			System.exit(2);
		}
			
		int buf = 60;
		byte[] tmpBytes = new byte[len + buf];
		
		System.arraycopy(originalBytes, 0,
				 tmpBytes, 0, len+buf);
		
		System.out.println("Loaded " +len+ 
				   " bytes from " +codeFilename+ ".");

		Loader l;

		System.out.println("Trying complete load.");
		l = new Loader(codeClassname, tmpBytes, 0, len);
		diffBytes("complete load", tmpBytes, originalBytes, len + buf);

		// Try truncated versions of the class
		System.out.println("Trying truncated loads....");
		for (int i = 1; i < len; i++)
		{
			int truncatedLen = len - i;
			l = new Loader(codeClassname, tmpBytes, 0, truncatedLen);
			
			if (l.error() == null)
			{
				System.out.println("Unexpected success! truncatedLen=" + truncatedLen);
				rc = 1;
				
			}
			else
				showIfNew("  ..(" +truncatedLen+ ")", l.error());

			diffBytes("truncatedLen=" + truncatedLen,
				  tmpBytes, originalBytes, len + buf);
		}

		System.exit(rc);
	}
}
