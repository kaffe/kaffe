import java.io.File;
import java.io.FileOutputStream;

public class DeleteFile
{
	public static void main(String args[]) throws Exception
	{
		FileOutputStream of = new FileOutputStream("deleteOnExit.test");
		of.close();

		File f = new File((File)null, "deleteOnExit.test");
		f.deleteOnExit();

		System.out.println("done.");
		System.exit(0);
	}
}
// java args: DeleteFile || test -f deleteOnExit.test
/* Expected Output:
done.
*/
