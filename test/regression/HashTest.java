
import java.io.*;
import java.security.*;

class HashTest {
  private static final String INPUT = "Let there be Java";

  public static void main (String[] args) throws Exception {
    String algorithm = (args.length == 0) ? "SHA" : args[0];

    // Create byte array from the String
    byte[] a = new byte[INPUT.length()];
    for (int i = 0; i < a.length; i++) {
      a[i] = (byte)INPUT.charAt(i);
    }

    // Get digest input stream
    DigestInputStream dis = new DigestInputStream(
      new ByteArrayInputStream(a), MessageDigest.getInstance(algorithm));

    // Process input and get digest
    byte[] buf = new byte[100];
    while (dis.read(buf, 0, buf.length) != -1);
    byte[] digest = dis.getMessageDigest().digest();

    // Display digest
    System.out.print(algorithm + " = ");
    for (int i = 0; i < digest.length; i++) {
      System.out.print(Integer.toHexString(digest[i] & 0xff));
    }
    System.out.println();
  }
}

/* Expected Output:
SHA = 276b20b73453a6f1815b4ae4654a734699c835
*/

