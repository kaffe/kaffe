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

public class JarOutputStream extends ZipOutputStream {

    	public JarOutputStream(OutputStream out, Manifest manifest)
			throws IOException {
		super(out);
		if (manifest == null) {
			return;
		}
		JarEntry ent = new JarEntry(JarFile.MANIFEST_NAME);
		putNextEntry(ent);
		manifest.write(new OutputStream() {
			public void write(int b) throws IOException {
				JarOutputStream.this.write(b);
			}
			public void write(byte[] buf, int off, int len)
					throws IOException {
				JarOutputStream.this.write(buf, off, len);
			}
			public void flush() throws IOException {
				JarOutputStream.this.flush();
			}
		});
		closeEntry();
	}

	public JarOutputStream(OutputStream out) throws IOException {
		super(out);
	}

	// Why is this method here?
	public void putNextEntry(ZipEntry ze) throws IOException {
		super.putNextEntry(ze);
	}
}

