/*
 * Utility class for piped reader & writer.
 *
 * Copyright (c) 2001
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2001
 *      Dalibor Topic
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.io;

public class CircularBuffer {
    /* the buffer where chars are stored. */
    char [] buffer;

    /* the index right after the last char that has been read, or 0 if we
     * are at buffer's end.
     */
    int in;

    /* the index right after the last char that has been written, or 0 if we
     * are at buffer's end.
     */
    int out;

    /* a flag to distinguish full from empty buffers. If it is true the buffer
     * is empty, if it is false the buffer may be full.
     */
    boolean empty = true;

    /* a flag to denote a closed circular buffer. It can still be read from, as
     * long as it is not empty. When it is empty, the buffer returns EOF to all
     * read requests.
     */

    boolean closed;

    /* creates a new buffer of the given size. */
    public CircularBuffer(int size) {
	buffer = new char[size];
    }

    /* returns the number of chars available for reading. */
    public int available() {
	if (isFull()) {
	    return buffer.length;
	}
	else {
	    return (in - out) % buffer.length;
	}
    }

    /* returns the capacity available for writing. */
    public int capacity() {
	return buffer.length - available();
    }

    /* closes this buffer, so it can no longer be written to. */
    public void close() {
	closed = true;
    }

    /* true if this buffer is closed. */
    public boolean isClosed() {
	return closed;
    }

    /* returns true if the buffer is empty. */
    public boolean isEmpty() {
	    return (in == out) && empty;
    }

    /* returns true if the buffer is full. */
    public boolean isFull() {
	return (in == out) && !empty;
    }

    /* reads at max len chars from buffer into buffer buf starting at
       offset off. reads at least one char.
    */
    public int read(char [] buf, int off, int len) {
	if (isClosed() && isEmpty()) {
	    return -1;
	}
	else {
	    int maxreadlen = Math.min(buffer.length - out,
				      Math.min(len, available()));
	    if (maxreadlen > 0) {
		System.arraycopy(buffer, out, buf, off, maxreadlen);
		out = (out + maxreadlen) % buffer.length;
		if (in == out) {
		    empty = true;
		}
	    }

	    return maxreadlen;
	}
    }

    /* writes at max len chars from the buffer buf starting at offset off. */ 
    public int write(char [] buf, int off, int len) {
      int maxwritelen = Math.min(buffer.length - in,
				 Math.min(len, capacity()));
      if (maxwritelen > 0) {
	System.arraycopy(buf, off, buffer, in, maxwritelen);
	off += maxwritelen;
	len -= maxwritelen;
	in = (in + maxwritelen) % buffer.length;
	empty = false;
      }

      return maxwritelen;
    }
}
