public class LoaderTest_Class3 {
	static {
		try {
			System.loadLibrary("LoaderTest_Class3.lib");
		} catch (UnsatisfiedLinkError e) {
		}
	}
}
