public class LoaderTest_Class2 {
	static {
		try {
			System.loadLibrary("LoaderTest_Class2.lib");
		} catch (UnsatisfiedLinkError e) {
		}
	}
}
