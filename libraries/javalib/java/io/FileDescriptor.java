/* FileDescriptor.java -- Opaque file handle class
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004
   Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.
 
GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
02111-1307 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version. */


package java.io;

import gnu.classpath.Configuration;

/**
 * This class represents an opaque file handle as a Java class.  It should
 * be used only to pass to other methods that expect an object of this
 * type.  No system specific information can be obtained from this object.
 *
 * @author Aaron M. Renn (arenn@urbanophile.com)
 * @author Tom Tromey (tromey@cygnus.com)
 * @date September 24, 1998 
 */
public final class FileDescriptor
{
  /**
   * A <code>FileDescriptor</code> representing the system standard input
   * stream.  This will usually be accessed through the
   * <code>System.in</code>variable.
   */
  public static final FileDescriptor in = new FileDescriptor();

  /**
   * A <code>FileDescriptor</code> representing the system standard output
   * stream.  This will usually be accessed through the
   * <code>System.out</code>variable.
   */
  public static final FileDescriptor out = new FileDescriptor();

  /**
   * A <code>FileDescriptor</code> representing the system standard error
   * stream.  This will usually be accessed through the
   * <code>System.err</code>variable.
   */
  public static final FileDescriptor err = new FileDescriptor();

  static
    {
      if (Configuration.INIT_LOAD_LIBRARY)
        {
          System.loadLibrary("io");
        }

      nativeInit();
    }

  // These are WHENCE values for seek.
  static final int SET = 0;
  static final int CUR = 1;
  static final int END = 2;

  // These are mode values for open().
  static final int READ   = 1;
  static final int WRITE  = 2;
  static final int APPEND = 4;

  // EXCL is used only when making a temp file.
  static final int EXCL   = 8;
  static final int SYNC   = 16;
  static final int DSYNC  = 32;

  /**
   * This is the actual native file descriptor value
   */
  private long nativeFd = -1L;

  /**
   * This method is used to initialize an invalid FileDescriptor object.
   */
  public FileDescriptor()
  {
  }

  private FileDescriptor(long nativeFd)
  {
    this.nativeFd = nativeFd;
  }

  FileDescriptor(String path, int mode) throws FileNotFoundException
  {
    open(path, mode);
  }

  /**
   * This method forces all data that has not yet been physically written to
   * the underlying storage medium associated with this 
   * <code>FileDescriptor</code>
   * to be written out.  This method will not return until all data has
   * been fully written to the underlying device.  If the device does not
   * support this functionality or if an error occurs, then an exception
   * will be thrown.
   */
  public void sync() throws SyncFailedException
  {
    if (nativeFd == -1L)
      throw new SyncFailedException("Invalid FileDescriptor");

    nativeSync(nativeFd);
  }

  /**
   * This methods tests whether or not this object represents a valid open
   * native file handle.
   *
   * @return <code>true</code> if this object represents a valid 
   * native file handle, <code>false</code> otherwise
   */
  public boolean valid()
  {
    if (nativeFd == -1L)
      return false;

    return nativeValid(nativeFd);
  }

  /**
   * Opens the specified file in the specified mode.  This can be done
   * in one of the specified modes:
   * <ul>
   * <li>r - Read Only
   * <li>rw - Read / Write
   * <li>ra - Read / Write - append to end of file
   * <li>rws - Read / Write - synchronous writes of data/metadata
   * <li>rwd - Read / Write - synchronous writes of data.
   *
   * @param path Name of the file to open
   * @param mode Mode to open
   *
   * @exception IOException If an error occurs.
   */
  void open(String path, int mode) throws FileNotFoundException
  {
    // We don't want fd leakage.
    if (nativeFd != -1L)
      throw new InternalError("FileDescriptor already open");

    // Note that it can be ok to have an empty path.
    // FIXME: verify
    if (path == null)
      throw new NullPointerException("Path cannot be null");

    if ((mode & (READ | WRITE)) == 0)
      throw new InternalError("Invalid mode value: " + mode);

    nativeFd = nativeOpen(path, mode);
  }

  /**
   * Closes this specified file descriptor
   * 
   * @exception IOException If an error occurs 
   */    
  synchronized void close() throws IOException
  {
    if (nativeFd == -1L)
      return;
    try
      {
        nativeClose(nativeFd);
      }
    finally
      {
        nativeFd = -1L;
      }
  }

  /**
   * Writes a single byte to the file
   *
   * @param b The byte to write, encoded in the low eight bits
   *
   * @exception IOException If an error occurs
   */
  void write(int b) throws IOException
  {
    if (nativeFd == -1L)
      throw new IOException("Invalid FileDescriptor");

    nativeWriteByte(nativeFd, (b & 0xFF));
  }

  /**
   * Writes a byte buffer to the file
   *
   * @param buf The byte buffer to write from
   * @param int The offset into the buffer to start writing from
   * @param len The number of bytes to write.
   *
   * @exception IOException If an error occurs
   */
  void write(byte[] buf, int offset, int len) throws IOException
  {
    if (nativeFd == -1L)
      throw new IOException("Invalid FileDescriptor");

    if (len == 0)
      return;

    if ((offset < 0) || (offset > buf.length))
      throw new IllegalArgumentException("Offset invalid: " + offset);

    if ((len < 0) || (len > (buf.length - offset)))
      throw new IllegalArgumentException("Length invalid: " + len);

    // Note that above ops implicitly bomb if buf == null

    nativeWriteBuf(nativeFd, buf, offset, len);
  }

  /**
   * Reads a single byte from the file
   *
   * @return The byte read, in the low eight bits on a long, or -1
   * if end of file
   *
   * @exception IOException If an error occurs
   */
  int read() throws IOException
  {
    if (nativeFd == -1L)
      throw new IOException("Invalid FileDescriptor");

    return nativeReadByte(nativeFd);
  }

  /**
   * Reads a buffer of  bytes from the file
   *
   * @param buf The buffer to read bytes into
   * @param offset The offset into the buffer to start storing bytes
   * @param len The number of bytes to read.
   *
   * @return The number of bytes read, or -1 if end of file.
   *
   * @exception IOException If an error occurs
   */
  int read(byte[] buf, int offset, int len) throws IOException
  {
    if (nativeFd == -1L)
      throw new IOException("Invalid FileDescriptor");

    if (len == 0)
      return(0);

    if ((offset < 0) || (offset > buf.length))
      throw new IllegalArgumentException("Offset invalid: " + offset);

    if ((len < 0) || (len > (buf.length - offset)))
      throw new IllegalArgumentException("Length invalid: " + len);

    // Note that above ops implicitly bomb if buf == null

    return nativeReadBuf(nativeFd, buf, offset, len);
  }

  /**
   * Returns the number of bytes available for reading
   *
   * @return The number of bytes available for reading
   *
   * @exception IOException If an error occurs
   */
  int available() throws IOException
  {
    if (nativeFd == -1L)
      throw new IOException("Invalid FileDescriptor");
    
    return nativeAvailable(nativeFd);
  }

  /**
   * Method to do a "seek" operation on the file
   * 
   * @param offset The number of bytes to seek
   * @param whence The position to seek from, either
   *    SET (0) for the beginning of the file, CUR (1) for the 
   *    current position or END (2) for the end position.
   * @param stopAtEof <code>true</code> to ensure that there is no
   *    seeking past the end of the file, <code>false</code> otherwise.
   *
   * @return The new file position, or -1 if end of file.
   *
   * @exception IOException If an error occurs
   */
  long seek(long offset, int whence, boolean stopAtEof) throws IOException
  {
    if (nativeFd == -1L)
      throw new IOException("Invalid FileDescriptor");

    if ((whence != SET) && (whence != CUR) && (whence != END))
      throw new IllegalArgumentException("Invalid whence value: " + whence);

    return nativeSeek(nativeFd, offset, whence, stopAtEof);
  }

  /**
   * Returns the current position of the file pointer in the file
   *
   * @param fd The native file descriptor
   *
   * @exception IOException If an error occurs
   */
  long getFilePointer() throws IOException
  { 
    if (nativeFd == -1L)
      throw new IOException("Invalid FileDescriptor");

    return nativeGetFilePointer(nativeFd);
  }

  /**
   * Returns the length of the file in bytes
   *
   * @return The length of the file in bytes
   *
   * @exception IOException If an error occurs
   */
  long getLength() throws IOException
  {
    if (nativeFd == -1L)
      throw new IOException("Invalid FileDescriptor");

    return nativeGetLength(nativeFd);
  }

  /**
   * Sets the length of the file to the specified number of bytes
   * This can result in truncation or extension.
   *
   * @param len The new length of the file
   *
   * @exception IOException If an error occurs
   */
  void setLength(long len) throws IOException
  {
    if (nativeFd == -1L)
      throw new IOException("Invalid FileDescriptor");

    if (len < 0)
      throw new IllegalArgumentException("Length cannot be less than zero " +
                                         len);

    nativeSetLength(nativeFd, len);
  }

  // Don't do anything crazy with this
  long getNativeFd()
  {
    return nativeFd;
  }

  private void setNativeFd(long nativeFd)
  {
    this.nativeFd = nativeFd;
  }

  protected void finalize() throws Throwable
  {
    close();
  }

  /*
   * Native FileDescriptor provider interface
   *
   * Platform implementors must implement these methods.  Note that this
   * class guarantees that valid data will be passed to these methods,
   * so basic error checking on input values can be skipped.
   */

  /**
   * This method is called in the class initializer to do any require
   * native library initialization.  It is also responsible for initializing
   * the in, out, and err variables.
   */
  private static native void nativeInit();

  /**
   * Opens the specified file in the specified mode.  This can be done
   * in one of the specified modes:
   * <ul>
   * <li>r - Read Only
   * <li>rw - Read / Write
   * <li>ra - Read / Write - append to end of file
   * <li>rws - Read / Write - synchronous writes of data/metadata
   * <li>rwd - Read / Write - synchronous writes of data.
   *
   * @param path Name of the file to open
   * @param mode Mode to open
   *
   * @return The resulting file descriptor for the opened file, or -1
   * on failure (exception also signaled).
   *
   * @exception IOException If an error occurs.
   */
  private native long nativeOpen(String path, int mode)
    throws FileNotFoundException;

  /**
   * Closes this specified file descriptor
   * 
   * @param fd The native file descriptor to close
   *
   * @return The return code of the native close command.
   *
   * @exception IOException If an error occurs 
   */    
  private native long nativeClose(long fd) throws IOException;
 
  /**
   * Writes a single byte to the file
   *
   * @param fd The native file descriptor to write to
   * @param b The byte to write, encoded in the low eight bits
   *
   * @return The return code of the native write command
   *
   * @exception IOException If an error occurs
   */
  private native long nativeWriteByte(long fd, int b) throws IOException;

  /**
   * Writes a byte buffer to the file
   *
   * @param fd The native file descriptor to write to
   * @param buf The byte buffer to write from
   * @param int The offset into the buffer to start writing from
   * @param len The number of bytes to write.
   *
   * @return The return code of the native write command
   *
   * @exception IOException If an error occurs
   */
  private native long nativeWriteBuf(long fd, byte[] buf, int offset, int len)
    throws IOException;

  /**
   * Reads a single byte from the file
   *
   * @param fd The native file descriptor to read from
   *
   * @return The byte read, in the low eight bits on a long, or -1
   * if end of file
   *
   * @exception IOException If an error occurs
   */
  private native int nativeReadByte(long fd) throws IOException;

  /**
   * Reads a buffer of  bytes from the file
   *
   * @param fd The native file descriptor to read from
   * @param buf The buffer to read bytes into
   * @param offset The offset into the buffer to start storing bytes
   * @param len The number of bytes to read.
   *
   * @return The number of bytes read, or -1 if end of file.
   *
   * @exception IOException If an error occurs
   */
  private native int nativeReadBuf(long fd, byte[] buf, int offset, int len) 
    throws IOException;

  /**
   * Returns the number of bytes available for reading
   *
   * @param fd The native file descriptor
   *
   * @return The number of bytes available for reading
   *
   * @exception IOException If an error occurs
   */
  private native int nativeAvailable(long fd) throws IOException;

  /**
   * Method to do a "seek" operation on the file
   * 
   * @param fd The native file descriptor 
   * @param offset The number of bytes to seek
   * @param whence The position to seek from, either
   *    SET (0) for the beginning of the file, CUR (1) for the 
   *    current position or END (2) for the end position.
   * @param stopAtEof <code>true</code> to ensure that there is no
   *    seeking past the end of the file, <code>false</code> otherwise.
   *
   * @return The new file position, or -1 if end of file.
   *
   * @exception IOException If an error occurs
   */
  private native long nativeSeek(long fd, long offset, int whence, 
                                 boolean stopAtEof)
    throws IOException;

  /**
   * Returns the current position of the file pointer in the file
   *
   * @param fd The native file descriptor
   *
   * @exception IOException If an error occurs
   */
  private native long nativeGetFilePointer(long fd) throws IOException;

  /**
   * Returns the length of the file in bytes
   *
   * @param fd The native file descriptor
   *
   * @return The length of the file in bytes
   *
   * @exception IOException If an error occurs
   */
  private native long nativeGetLength(long fd) throws IOException;

  /**
   * Sets the length of the file to the specified number of bytes
   * This can result in truncation or extension.
   *
   * @param fd The native file descriptor  
   * @param len The new length of the file
   *
   * @exception IOException If an error occurs
   */
  private native void nativeSetLength(long fd, long len) throws IOException;

  /**
   * Tests a file descriptor for validity
   *
   * @param fd The native file descriptor
   *
   * @return <code>true</code> if the fd is valid, <code>false</code> 
   * otherwise
   */
  private native boolean nativeValid(long fd);

  /**
   * Flushes any buffered contents to disk
   *
   * @param fd The native file descriptor
   *
   * @exception IOException If an error occurs
   */
  private native void nativeSync(long fd) throws SyncFailedException;
}
