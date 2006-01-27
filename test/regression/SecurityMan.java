import java.security.Permission;

class SecurityMan
{
  static class TestSecurityManager extends SecurityManager
  {
    public void checkPermission(Permission perm) {
      try {
	Class.forName("java.security.RuntimePermission");
      }
      catch (Throwable t) {
      }
    }
  }
  
  public static void main(String[] args) {
    try {
      SecurityManager sm = new TestSecurityManager();
      System.setSecurityManager(sm);
      sm.checkRead("/");
    }
    catch (Throwable t) {
      t.printStackTrace();
    }
    System.out.println("Ok");
  }
}
// Skip Run
/* Expected Output:
Ok
*/
