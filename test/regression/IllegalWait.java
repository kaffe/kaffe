class IllegalWait {
  public static void main (String[] args) throws Exception {
    boolean caught = false;
    try {
      new Object().wait(100);
    } catch (IllegalMonitorStateException e) {
      caught = true;
    }
    System.out.println(caught ? "Success." : "Failure.");
  }
}

/* Expected Output:
Success.
*/

