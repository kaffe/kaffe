class IllegalWait {
  public static void main (String[] args) throws Exception {
    new Object().wait(100);
  }
}

/* Expected Output:
java.lang.IllegalMonitorStateException
	at x.main(IllegalWait.java:6)
*/

