/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.io;


public class FileInputStream
  extends InputStream {

private FileDescriptor fd = new FileDescriptor();

static {
        System.loadLibrary("io");
}

public FileInputStream(FileDescriptor fdObj) {
	if (fdObj == null) {
		throw new NullPointerException();
	}

	SecurityManager sm = System.getSecurityManager();
	if (sm != null)
		sm.checkRead(fdObj);
	// XXX check if fd is invalid?  Or points to a non-file object?
	fd=fdObj;
}

public FileInputStream(String name) throws FileNotFoundException {
	this(new File(name));
}

public FileInputStream(File file) throws FileNotFoundException {
	final String fname = file.getPath();

	// Note File.isDirectory() will do the required SecurityManager 
	// canRead() check for us.
	if (file.isDirectory())
		throw new FileNotFoundException(fname+ ": Is a directory");

	try {
		open(fname);
	} catch (IOException e) {
		/* Note that open may throw an IOException, but the spec says
		 * that this constructor throws only FileNotFoundExceptions,
		 * hence we must map them.
		 */
		throw new FileNotFoundException(fname + ": " + e.getMessage());
	}
}



native public int available() throws IOException;

native public void close() throws IOException;

protected void finalize() throws IOException {
	close();
	try {
	    super.finalize();
	}
	catch(Throwable e){
	    throw new IOException(e.getMessage());
	}
}

final public FileDescriptor getFD() throws IOException {
	return fd;
}

native private void open(String name) throws IOException;

native public int read() throws IOException;

public int read(byte b[]) throws IOException {
	return read(b, 0, b.length);
}

public int read(byte b[], int off, int len) throws IOException {
	if (off < 0 || len < 0 || off + len > b.length) {
	   throw new IndexOutOfBoundsException();
	}

        return readBytes(b, off, len);
}

native private int readBytes(byte b[], int off, int len);

native public long skip(long n) throws IOException;
}
