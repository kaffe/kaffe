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

public class PipedReader extends Reader {

  CircularBuffer buffer;

  private static final int PIPE_SIZE = 2048;

  public PipedReader() {
  }

  public PipedReader(PipedWriter writer) throws IOException {
    connect(writer);
  }

  private void checkIfConnected() throws IOException {
    synchronized(this) {
      if (!isConnected()) {
	throw new IOException("Pipe not connected");
      }
    }
  }

  public void connect(PipedWriter writer) throws IOException {
    synchronized(this) {
      if (isConnected()) {
	throw new IOException("Already connected");
      }
      else {
	writer.buffer = buffer = new CircularBuffer(PIPE_SIZE);
      }
    }
  }

  private boolean isConnected() {
    return buffer != null;
  }

  public int read() throws IOException {
    return super.read();
  }

  public int read(char cbuf[], int off, int len) throws IOException {
    checkIfConnected();

    synchronized(buffer) {
      int nread;
      while ((nread = buffer.read(cbuf, off, len)) == 0) {
	try {
	  buffer.wait();
	}
	catch (InterruptedException e) {
	  continue;
	}
      }

      buffer.notify(); // notify the writer.
      return nread;
    }
  }

  public boolean ready() throws IOException {
    checkIfConnected();

    return !buffer.isEmpty();
  }

  public void close() throws IOException {
    synchronized(buffer) {
      buffer.close();
      buffer.notifyAll(); // notify reader and writer.
    }
  }
}


