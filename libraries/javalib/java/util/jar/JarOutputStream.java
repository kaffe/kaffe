/* Class : JarOutputStream

  Implementation of the 1.2 Java class JarOutputStream.

  Copyright : Moses DeJong, dejong@cs.umn.edu, 1998.
  Source code licensed under the GPL.
  You can get a copy of the license from www.gnu.org.

  This code is intended for use in the Kaffe project but you can use
  it in other projects as long as you follow the license rules.
*/

package java.util.jar;

import java.io.*;
import java.util.zip.*;


public class JarOutputStream extends ZipOutputStream
{
    public JarOutputStream(OutputStream out, Manifest man) throws IOException
    {
	super(out);

	// FIXME : how do we write a manifest file?
	// is there special processing that needs to be done?
	// what if man is null, the JDK docs do not say anything
	// about a NullPointerException being thrown.

	if (man == null) {
	    throw new NullPointerException("man");
	}

	ZipEntry ze = new ZipEntry("META-INF/MANIFEST.MF");
	ze.setMethod(ZipEntry.DEFLATED); // compressed entry
	putNextEntry(ze);
	man.write(out);
	closeEntry();
    }

    public JarOutputStream(OutputStream out) throws IOException
    {
	super(out);
    }

    public void putNextEntry(ZipEntry ze) throws IOException
    {
	// FIXME : this method must be overridden for some reason.
	// It must do something extra that super.putNextEntry() does not.

	// so far I have been able to figure out that JarOutputStream
	// will use the ZipEntry.setExtra() method to add 4 bytes
	// of data. I have no idea what this data is but it seems to
	// be the same for both uncompressed and compressed archives.

	byte[] data = {-2,-54,0,0};
	ze.setExtra(data);

	super.putNextEntry(ze);
    }
}
