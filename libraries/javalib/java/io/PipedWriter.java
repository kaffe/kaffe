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

public class PipedWriter extends Writer {

  final PipedOutputStream rawOutput = new PipedOutputStream();

  public PipedWriter() {
  }

  public PipedWriter(PipedReader reader) throws IOException {
    connect(reader);
  }

  public void connect(PipedReader reader) throws IOException {
    rawOutput.connect(reader.rawInput);
  }

  public void write(char cbuf[], int off, int len) throws IOException {

    // Convert pairs of bytes to characters
    len = (len << 1);
    byte[] buf = new byte[len];
    for (int i = 0; i < len; off++) {
      buf[i++] = (byte) (cbuf[off] >> 8);
      buf[i++] = (byte) (cbuf[off] & 0xff);
    }

    // Write each character as two bytes big endian
    rawOutput.write(buf, 0, len);
  }

  public void flush() throws IOException {
    rawOutput.flush();
  }

  public void close() throws IOException {
    rawOutput.close();
  }
}

