/**
 * simple test for class finalization
 *
 * @author Godmar Back <gback@cs.utah.edu>
 */
import java.io.*;
import java.lang.reflect.*;

public class ClassGCTest
{
	public static class HObject {
		protected void finalize() throws Throwable {
			if (!ClassGC.gotOneForF) {
				ClassGC.gotOneForF = true;
				System.out.println("Success.");
			}
		}
	}

	public static Object f = new HObject();

	/* Make sure interfaces are GC'd also */
	public interface HInterface {
		void func();
	}

	public static class HImplementor implements HInterface {
		public void func()
		{
		}

		protected void finalize() throws Throwable {
			if (!ClassGC.gotOneForG) {
				ClassGC.gotOneForG = true;
				System.out.println("Success.");
			}
		}
	}

	public static Object g = new HImplementor();
}

