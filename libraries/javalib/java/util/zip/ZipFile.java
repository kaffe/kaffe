/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.util.zip;

import java.io.File;
import java.io.InputStream;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.util.Enumeration;
import java.util.Vector;
import kaffe.util.Ptr;

public class ZipFile implements ZipConstants {

private String name;
private Ptr zip;

public ZipFile(String fname) throws IOException
{
	name = fname;
	zip = openZipFile0(fname);
	if (zip == null) {
		throw new IOException("No such zip file " + fname);
	}
}

public ZipFile(File f) throws ZipException, IOException
{
	this(f.getPath());
}

public void close() throws IOException
{
	if (zip != null) {
		closeZipFile0(zip);
		zip = null;
	}
}

public Enumeration entries()
{
	Vector all = getZipEntries0(zip);
	return (all.elements());
}

public ZipEntry getEntry(String zname)
{
	return (getZipEntry0(zip, zname));
}

public InputStream getInputStream(ZipEntry ze) throws IOException
{
	if (ze == null) {
		throw new NullPointerException();
	}

	byte[] buf = getZipData0(zip, ze);
	if (buf == null) {
		throw new ZipException("no data");
	}
	ByteArrayInputStream strm = new ByteArrayInputStream(buf);
	return (strm);
}

public String getName()
{
	return (name);
}

public int size()
{
	return getZipFileSize0(zip);
}

protected void finalize() throws IOException
{
	try {
		close();
	}
	finally {
		try {
			super.finalize();
		}
		catch (Throwable t) {
			/* just print the stack trace */
			t.printStackTrace();
		}
	}
}

private static native Ptr openZipFile0(String fname);
private static native void closeZipFile0(Ptr zip);
private static native ZipEntry getZipEntry0(Ptr zip, String zname);
private static native Vector getZipEntries0(Ptr zip);
private static native byte[] getZipData0(Ptr zip, ZipEntry ze);
private static native int getZipFileSize0(Ptr zip);

}
