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

public class PipedReader extends Reader {

  final PipedInputStream rawInput = new PipedInputStream();

  public PipedReader() {
  }

  public PipedReader(PipedWriter writer) throws IOException {
    connect(writer);
  }

  public void connect(PipedWriter writer) throws IOException {
    rawInput.connect(writer.rawOutput);
  }

  public int read(char cbuf[], int off, int len) throws IOException {

    // Read each character as two bytes big endian
    len = (len << 1) & 0xfff;
    byte[] buf = new byte[len];
    int num = rawInput.read(buf, 0, len);
    if (num == -1) {
      return(-1);
    }

    // XXX Assume we won't ever read an odd length
    if ((num & 0x1) != 0) {
      throw new Error("rec'd odd length");
    }

    // Convert pairs of bytes to characters
    for (int i = 0; i < num; i += 2) {
      cbuf[off++] = (char) ((buf[i] << 8) | (buf[i + 1] & 0xff));
    }
    return (num >> 1);
  }

  public void close() throws IOException {
    rawInput.close();
  }

}


