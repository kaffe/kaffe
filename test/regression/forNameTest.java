class forNameTest {
       public static void main(String argv[]) {
	      try {
		     Class.forName("loadThis");
		     Class c = Class.forName("loadThis", false, 
		        new ClassLoader() {
			  public Class loadClass(String n)
				throws ClassNotFoundException {
				return findSystemClass(n);
			  }
		        });
		     System.out.println("constructor not called");
		     c.newInstance();
	      }
	      catch( Exception e ) {
		      System.out.println(e.getMessage());
		      e.printStackTrace();
	      }
       }
}

class loadThis {
      static {
	      try {
		      new loadThis();
	      }
	      catch( Exception e ) {
		      System.out.println(e.getMessage());
		      e.printStackTrace();
	      }
      }

      public loadThis() {
	      System.out.println("constructor called");
      }
}

/* Expected Output:
constructor called
constructor not called
constructor called
*/
