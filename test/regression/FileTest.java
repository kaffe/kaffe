import java.io.File;

public class FileTest {

    public static void exists(File f) {
      String type = "File";
      if (f.isDirectory()) { type = "Directory"; }
      System.out.print(type + " \"" + f.getPath() );
      if (f.exists()) {
          System.out.println("\" does exist");
      } else {
          System.out.println("\" does not exist");
      }
    }

    public static void main(String[] argv) throws Exception {
        File g1 = new File("/", "tmp"); exists(g1);
        File g2 = new File("/"); exists(g2);
        File g3 = new File(g2, "tmp"); exists(g3);
        File g4 = new File(""); exists(g4);
        File g5 = new File("","NotExist2"); exists(g5);
	File g6 = new File((String)null,"NotExist2"); exists(g6);

        try {
        File b1 = new File(null); exists(b1);
        } catch (NullPointerException e) {System.out.println("caught 1");}

        try {
        File b2 = new File("/", null); exists(b2);
        } catch (NullPointerException e) {System.out.println("caught 2");}

	try {
	File b3 = new File((File)null,"NotExist2"); exists(b3);
        } catch (NullPointerException e) {System.out.println("caught 3");}

    }
}
/**********

JDK 1.1 output

Directory "/tmp" does exist
Directory "/" does exist
Directory "/tmp" does exist
File "" does not exist
File "/file2" does not exist
File "file2" does not exist
caught 1
caught 2
caught 3


JDK 1.2 output

Directory "/tmp" does exist
Directory "/" does exist
Directory "/tmp" does exist
File "" does not exist
File "/file2" does not exist
File "file2" does not exist
caught 1
caught 2
File "file2" does not exist


*************/


/* Expected Output:
Directory "/tmp" does exist
Directory "/" does exist
Directory "/tmp" does exist
File "" does not exist
File "/NotExist2" does not exist
File "NotExist2" does not exist
caught 1
caught 2
File "NotExist2" does not exist
*/
