/**
 * test that a class whose objects are instantiated via newInstance
 * do in fact have a default, no-arg constructor.  If they don't,
 * an InstantiationException must be thrown.
 */
public class ConstructorTest {
  public ConstructorTest(int i) {}
  public static void main(String[]argv) 
  throws IllegalAccessException, InstantiationException
  {
    try {
	Object o = ConstructorTest.class.newInstance();
    } catch (InstantiationException e) {
	System.out.println("Success.");
	return;
    }
    System.out.println("Failure.");
  }
}


/* Expected Output:
Success.
*/
