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

public FileInputStream(File file) throws FileNotFoundException {
	this(file.getPath());
}

public FileInputStream(FileDescriptor fdObj) {
	System.getSecurityManager().checkRead(fdObj);
	fd=fdObj;
}

public FileInputStream(String name) throws FileNotFoundException {
	System.getSecurityManager().checkRead(name);		
	try {
		open(name);
	} catch (IOException e) {
		/* Note that open may throw an IOException, but the spec says 
		 * that this constructor throws only FileNotFoundExceptions, 
		 * hence we must map them.
		 */
		throw new FileNotFoundException(name + " " + e.getMessage());
	}
}

native public int available() throws IOException;

native public void close() throws IOException;

protected void finalize() throws Throwable {
	close();
	super.finalize();
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
	return readBytes(b, off, len);
}

native private int readBytes(byte b[], int off, int len);

native public long skip(long n) throws IOException;
}
