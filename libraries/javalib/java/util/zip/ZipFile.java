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

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.util.Enumeration;
import java.util.Vector;

import kaffe.util.Ptr;

public class ZipFile implements ZipConstants {


  /**
   * Mode flag to open a zip file for reading.
   * From GNU Classpath.
   */
  public static final int OPEN_READ = 0x1;

  /**
   * Mode flag to delete a zip file after reading.
   * From GNU Classpath.
   */
  public static final int OPEN_DELETE = 0x4;

  // Name of this zip file.
  private final String name;

  private Ptr zip;

  private boolean closed;

public ZipFile(String fname) throws IOException
{
	name = fname;
	/* only have one thread at a time attempt to open the zip file */
	zip = openZipFile0(fname);
	if (zip == null) {
		throw new IOException("No such zip file " + fname);
	}
}

public ZipFile(File f) throws ZipException, IOException
{
	this(f.getPath());
}

public ZipFile(File f, int mode) throws ZipException, IOException
{
	// XXX ignoring mode for now
	this(f);
}

private void checkIfClosed()
{
  if (closed) {
    throw new IllegalStateException("Zip file already closed: " + getName());
  }
}

public void close() throws IOException
{
	if (!closed) {
		closeZipFile0(zip);
		closed = true;
	}
}

public Enumeration entries()
{
  checkIfClosed();
  Vector all = getZipEntries0(zip);
  return (all.elements());
}

public ZipEntry getEntry(String zname)
{
  checkIfClosed();
  return (getZipEntry0(zip, zname));
}

public InputStream getInputStream(ZipEntry ze) throws IOException
{
	if (ze == null) {
		throw new NullPointerException();
	}

	if (ze.getSize() == -1) {
		ze = getEntry(ze.getName());
	}

	if (ze == null) {
		return null;
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
  checkIfClosed();
  return getZipFileSize0(zip);
}

protected void finalize() throws IOException
{
	/* We don't need to call super.finalize(),
	 * since super class is java.lang.Object, and
	 * java.lang.Object.finalize() just returns
	 * to caller.
	 */
	close();
}

private static native Ptr openZipFile0(String fname);
private static native void closeZipFile0(Ptr zip);
private static native ZipEntry getZipEntry0(Ptr zip, String zname);
private static native Vector getZipEntries0(Ptr zip);
private static native byte[] getZipData0(Ptr zip, ZipEntry ze);
private static native int getZipFileSize0(Ptr zip);

}
