class forNameTest {
       public static void main(String argv[]) {
	      try {
		     Class.forName("loadThis");
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
