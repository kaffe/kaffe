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

import kaffe.io.CircularBuffer;

public class PipedWriter extends Writer {

  CircularBuffer buffer;

  public PipedWriter() {
  }

  public PipedWriter(PipedReader reader) throws IOException {
    connect(reader);
  }

  private void checkIfStillOpen() throws IOException {
    if (buffer.isClosed()) {
      throw new IOException("Pipe closed");
    }
  }

  private void checkIfConnected() throws IOException {
    synchronized(this) {
      if (!isConnected()) {
	throw new IOException("Pipe not connected");
      }
    }
  }

  public void connect(PipedReader reader) throws IOException {
    synchronized(this) {
      /* connect using the reader to avoid racing conditions. */
      reader.connect(this);
    }
  }

  private boolean isConnected() {
    return buffer != null;
  }

  public void write(int c) throws IOException {
    super.write(c);
  }

  public void write(char cbuf[], int off, int len) throws IOException {
    checkIfConnected();

    synchronized(buffer) {
      while(len > 0) {
	checkIfStillOpen();

	if (buffer.isFull()) {
	  try {
	    buffer.wait();
	  }
	  catch (InterruptedException e) {
	    continue;
	  }
	}
	else {
	  int written = buffer.write(cbuf, off, len);
	  off += written;
	  len -= written;

	  buffer.notify(); // notify the reader.
	}
      }
    }
  }

  public void flush() throws IOException {
    if (isConnected()) {
      synchronized(buffer) {
	checkIfStillOpen();

	buffer.notify();
      }
    }
  }

  public void close() throws IOException {
    synchronized(buffer) {
      buffer.close();
      buffer.notifyAll(); // notify reader and writer.
    }
  }
}

