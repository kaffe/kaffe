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
	SecurityManager sm = System.getSecurityManager();
	if (sm != null)
		sm.checkRead(fdObj);
	fd=fdObj;
}

public FileInputStream(String name) throws FileNotFoundException {
	final SecurityManager sm = System.getSecurityManager();
	if (sm != null)
		sm.checkRead(name);
	try {
		open(name);
	} catch (IOException e) {
		/* Note that open may throw an IOException, but the spec says
		 * that this constructor throws only FileNotFoundExceptions,
		 * hence we must map them.
		 */
		throw new FileNotFoundException(name + ": " + e.getMessage());
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
	if (off >= 0 && off + len <= b.length) {
		return readBytes(b, off, len);
	}
	else {
		throw new ArrayIndexOutOfBoundsException();
	}
}

native private int readBytes(byte b[], int off, int len);

native public long skip(long n) throws IOException;
}
