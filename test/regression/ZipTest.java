/**
 * Kaffe fails to create compressed zip files.
 *
 * @author Carlos Valiente <yo@virutass.net>
 */

import java.io.*;
import java.util.zip.*;


public class ZipTest {

	public static void main(String[] args)
		throws Exception
	{

		buildZip("ziptest1.zip", false);
		buildZip("ziptest2.zip", true);
	}

	static void buildZip(String zipFile, boolean compress)
		throws Exception
	{

		System.out.println("****");
		System.out.print("Creating " + zipFile + " (");
		if(!compress)
			System.out.print("un");
		System.out.println("compressed)");

		ZipOutputStream zOut = new ZipOutputStream(new FileOutputStream(zipFile));
		if(compress)
			zOut.setMethod(ZipOutputStream.DEFLATED);
		else
			zOut.setMethod(ZipOutputStream.STORED);

		addFile(zOut, "ZipTest.class", compress);

		if(zOut != null)
			zOut.close();

	}

	static void addFile(ZipOutputStream zOut, String file, boolean compress)
		throws Exception
	{

		InputStream in = new FileInputStream(file);
		ZipEntry ze = new ZipEntry(file);

		if(!compress) {
			long size = 0;
			CRC32 cal = new CRC32();
			if(!in.markSupported()) {
				ByteArrayOutputStream bos = new ByteArrayOutputStream();

				byte[] buffer = new byte[8 * 1024];
				int count = 0;
				do {
					size += count;
					cal.update(buffer, 0, count);
					bos.write(buffer, 0, count);
					count = in.read(buffer, 0, buffer.length);
				}
				while(count != -1);
				in = new ByteArrayInputStream(bos.toByteArray());
			}
			else {
				in.mark(Integer.MAX_VALUE);
				byte[] buffer = new byte[8 * 1024];
				int count = 0;
				do {
					size += count;
					cal.update(buffer, 0, count);
					count = in.read(buffer, 0, buffer.length);
				}
				while(count != -1);
				in.reset();
			}
			ze.setSize(size);
			ze.setCrc(cal.getValue());
		}

		zOut.putNextEntry(ze);

		byte[] buffer = new byte[8 * 1024];
		int count = 0;
		do {
			zOut.write(buffer, 0, count);
			count = in.read(buffer, 0, buffer.length);
		}
		while(count != -1);
	}
}
/* Expected Output:
****
Creating ziptest1.zip (uncompressed)
****
Creating ziptest2.zip (compressed)
*/
