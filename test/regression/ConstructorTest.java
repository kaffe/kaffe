/**
 * test that a class whose objects are instantiated via newInstance
 * do in fact have a default, no-arg constructor.  If they don't,
 * a NoSuchMethodError must be thrown.
 */
public class ConstructorTest {
  public ConstructorTest(int i) {}
  public static void main(String[]argv) 
  throws IllegalAccessException, InstantiationException
  {
    try {
	Object o = ConstructorTest.class.newInstance();
    } catch (NoSuchMethodError e) {
	System.out.println("Success.");
	return;
    }
    System.out.println("Failure.");
  }
}

