/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package gnu.java.net;

import java.io.FileDescriptor;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.net.SocketException;
import java.net.SocketImpl;
import java.net.SocketOptions;
import java.net.UnknownHostException;

/* XXX: This class should not be public!
 * It's public right now cause it's accessed from kaffe.net.
 */
public class PlainSocketImpl
  extends SocketImpl {

private InputStream in;
private OutputStream out;
protected int timeout;
private boolean closed;
private boolean blocking;
private boolean connecting;
private int native_fd;
  /**
   * Indicates whether a channel initiated whatever operation
   * is being invoked on this socket.
   */
  private boolean inChannelOperation;

  /**
   * Indicates whether we should ignore whether any associated
   * channel is set to non-blocking mode. Certain operations
   * throw an <code>IllegalBlockingModeException</code> if the
   * associated channel is in non-blocking mode, <i>except</i>
   * if the operation is invoked by the channel itself.
   */
  public final boolean isInChannelOperation()
  {
    return inChannelOperation;
  }
  
  /**
   * Sets our indicator of whether an I/O operation is being
   * initiated by a channel.
   */
  public final void setInChannelOperation(boolean b)
  {
    inChannelOperation = b;
  }
 
static {
	System.loadLibrary("net");
}

public PlainSocketImpl() {
        timeout = -1; // As defined in jsyscall.h
	blocking = true;
	native_fd = -1;
}

public int getNativeFD() {
	return native_fd;
}

protected void accept(SocketImpl s) throws IOException {
	if (!(s instanceof PlainSocketImpl))
	  throw new IllegalArgumentException("Kaffe only supports PlainSocketImpl in accept.");
	socketAccept(s);
}

protected synchronized int available() throws IOException {
	return closed ? 0 : socketAvailable();
}

protected void bind(InetAddress address, int lport) throws IOException {
	socketBind(address, lport);
}

protected synchronized void close() throws IOException {
	if( !closed )
		socketClose();
	closed = true;
	in = null;
	out = null;
}

protected void connect(String host, int port) throws IOException {
	connect(InetAddress.getByName(host), port);
}

protected void connect(InetAddress address, int port) throws IOException {
	socketConnect(address, port, this.timeout);
}

protected void connect(SocketAddress address, int timeout) throws IOException {
        InetSocketAddress iaddr = (InetSocketAddress)address;
	InetAddress addr = iaddr.getAddress();

	if (addr == null)
	   throw new IllegalArgumentException("address is unresolved: " + iaddr);

	if (timeout < 0)
	   throw new IllegalArgumentException("negative timeout");
	
	if (timeout == 0)
	    timeout = -1;

	socketConnect(addr, iaddr.getPort(), timeout);
}

protected void create(boolean stream) throws IOException {
	socketCreate(stream);
}

protected void finalize() throws Throwable {
	close();
	super.finalize();
}

protected synchronized InputStream getInputStream() throws IOException {
	if (in == null) {
		in = new SocketInputStream(this); 
	}
	return (in);
}

protected synchronized OutputStream getOutputStream() throws IOException {
	if (out == null) {
		out = new SocketOutputStream(this);
	}
	return (out);
}

protected void listen(int count) throws IOException {
	socketListen(count);
}

protected void setTcpNoDelay(boolean on) throws SocketException {
	socketSetOption(TCP_NODELAY, new Integer(on ? 1 : 0));
}

protected boolean getTcpNoDelay() throws SocketException {
	return socketGetOption(TCP_NODELAY) != 0;
}

protected void setSoLinger(boolean on, int howlong) throws SocketException {
	socketSetOption(SO_LINGER, new Integer(on ? howlong : 0));
}

protected int getSoLinger() throws SocketException {
	return socketGetOption(SO_LINGER);
}

protected void setSoTimeout(int timeout) throws SocketException {
	setOption(SocketOptions.SO_TIMEOUT, new Integer(timeout));
}

protected int getSoTimeout() throws SocketException {
	return ((Integer)getOption(SocketOptions.SO_TIMEOUT)).intValue();
}

protected void setSendBufferSize(int size) throws SocketException {
	socketSetOption(SO_SNDBUF, new Integer(size));
}

protected int getSendBufferSize() throws SocketException {
	return socketGetOption(SO_SNDBUF);
}

protected void setReceiveBufferSize(int size) throws SocketException {
	socketSetOption(SO_RCVBUF, new Integer(size));
}

protected int getReceiveBufferSize() throws SocketException {
	return socketGetOption(SO_RCVBUF);
}

public void setOption(int option, Object data) throws SocketException {
	final boolean disable = (data instanceof Boolean) && !((Boolean) data).booleanValue();
	int timo;

	if (disable) {
		data = new Integer(0);			// the common case
	}
	switch (option) {
	case SO_SNDBUF:
	case SO_RCVBUF:
	case SO_LINGER:
	case SO_REUSEADDR:
		break;
	case TCP_NODELAY:
		data = new Integer(disable ? 0 : 1);
		break;
	case SO_TIMEOUT:
		timo = ((Integer)data).intValue();
		if (timo < 0) {
			throw new IllegalArgumentException("timeout < 0");
		}
		// This is for our infinite timeout, 0 is reserved for
		// some non-blocking operation.
		if (timo == 0)
			this.timeout = -1;
		else
			this.timeout = timo;
		return;

	case SO_BINDADDR:
		throw new SocketException("Read-only socket option");
	case IP_MULTICAST_IF:
	default:
		throw new SocketException("Unknown socket option");
	}
	socketSetOption(option, data);
}

public Object getOption(int option) throws SocketException {
	switch (option) {
	case SO_SNDBUF:
	case SO_RCVBUF:
	case SO_LINGER:
	case SO_REUSEADDR:
		return new Integer(socketGetOption(option));
	case TCP_NODELAY:
		return new Boolean(socketGetOption(option) != 0);
	case SO_TIMEOUT:
		if (timeout == -1)
			return new Integer(0);
		else
			return new Integer(timeout);
	case SO_BINDADDR:
		int val = socketGetOption(option);
		try {
			return InetAddress.getByName(
			    ((int) ((val >> 24) & 0xff)) + "." +
			    ((int) ((val >> 16) & 0xff)) + "." +
			    ((int) ((val >>  8) & 0xff)) + "." +
			    ((int) ((val      ) & 0xff)) );
		} catch (UnknownHostException e) {
			throw new Error("impossible result");
		}
	case IP_MULTICAST_IF:
	default:
		throw new SocketException("Unknown socket option");
	}
}

protected int read(byte[] buf, int offset, int len) throws IOException {
	if (closed)
		return -1;
	int r = socketRead(buf, offset, len);
	if (r > 0 || len == 0) {
		return (r);
	}
	else {
		return (-1);
	}
}

protected void write(byte[] buf, int offset, int len) throws IOException {
	if (closed)
		throw new IOException("socket closed");
	socketWrite(buf, offset, len);
}

protected void sendUrgentData(int data) throws IOException {
	// TODO: replace with a real function.
}

public synchronized native void socketSetOption(int option, Object data) throws SocketException;
public synchronized native int socketGetOption(int option) throws SocketException;
protected synchronized native void socketAccept(SocketImpl sock);
protected synchronized native int  socketAvailable();
protected synchronized native void socketBind(InetAddress addr, int port);
protected synchronized native void socketClose();
protected synchronized native void socketConnect(InetAddress addr, int port, int timeout);
protected synchronized native void socketCreate(boolean stream);
protected synchronized native void socketListen(int count);
protected synchronized native int socketRead(byte[] buf, int offset, int len) throws IOException;
protected synchronized native void socketWrite(byte[] buf, int offset, int len) throws IOException;

// This function are principally for the NIO implementation of sockets.
protected synchronized native void waitForConnection() throws IOException;
protected synchronized native void setBlocking(boolean blocking);

/* Taken from GNU Classpath. */

  /**
   * This class contains an implementation of <code>InputStream</code> for
   * sockets.  It in an internal only class used by <code>PlainSocketImpl</code>.
   */
  class SocketInputStream extends InputStream
  {
    /**
     * The PlainSocketImpl object this stream is associated with
     */
    private PlainSocketImpl impl;

    /**
     * Builds an instance of this class from a PlainSocketImpl object
     */
    protected SocketInputStream (PlainSocketImpl impl)
    {
      this.impl = impl;
    }

    /**
     * Returns the number of bytes available to be read before blocking
     */
    public int available() throws IOException
    {
      return impl.available();
    }

    /**
     * Determines if "mark" functionality is supported on this stream.  For
     * sockets, this is always false.  Note that the superclass default is
     * false, but it is overridden out of safety concerns and/or paranoia.
     */
    public boolean markSupported()
    {
      return false;
    }

    /**
     * Do nothing mark method since we don't support this functionality.  Again,
     * overriding out of paranoia.
     *
     * @param readlimit In theory, the number of bytes we can read before the mark becomes invalid
     */
    public void mark (int readlimit)
    {
    }

    /**
     * Since we don't support mark, this method always throws an exception
     *
     * @exception IOException Everytime since we don't support this functionality
     */
    public void reset() throws IOException
    {
      throw new IOException ("Socket InputStreams do not support mark/reset");
    }

    /**
     * This method not only closes the stream, it closes the underlying socket
     * (and thus any connection) and invalidates any other Input/Output streams
     * for the underlying impl object
     */
    public void close() throws IOException
    {
      impl.close();
    }

    /**
     * Reads the next byte of data and returns it as an int.
     *
     * @return The byte read (as an int) or -1 if end of stream);
     *
     * @exception IOException If an error occurs.
     */
    public int read() throws IOException
    {
      byte buf[] = new byte [1];
      int bytes_read = read (buf, 0, buf.length);

      if (bytes_read != -1)
        return buf[0] & 0xFF;
      else
        return -1;
    }

    /**
     * Reads up to buf.length bytes of data into the caller supplied buffer.
     *
     * @return The actual number of bytes read or -1 if end of stream
     *
     * @exception IOException If an error occurs.
     */
    public int read (byte[] buf) throws IOException
    {
      return read (buf, 0, buf.length);
    }

    /**
     * Reads up to len bytes of data into the caller supplied buffer starting
     * at offset bytes from the start of the buffer
     *
     * @return The number of bytes actually read or -1 if end of stream
     *
     * @exception IOException If an error occurs.
     */
    public int read (byte[] buf, int offset, int len) throws IOException
    {
      int bytes_read = impl.read (buf, offset, len);

      if (bytes_read == 0)
        return -1;

      return bytes_read;
    }

  } // class SocketInputStream

  /**
   * This class is used internally by <code>PlainSocketImpl</code> to be the
   * <code>OutputStream</code> subclass returned by its
   * <code>getOutputStream method</code>.  It expects only to  be used in that
   * context.
   */
  class SocketOutputStream extends OutputStream
  {
    /**
     * The PlainSocketImpl object this stream is associated with
     */
    private PlainSocketImpl impl;

    /**
     * Build an instance of this class from a PlainSocketImpl object
     */
    protected SocketOutputStream (PlainSocketImpl impl)
    {
      this.impl = impl;
    }

    /**
     * This method closes the stream and the underlying socket connection. This
     * action also effectively closes any other InputStream or OutputStream
     * object associated with the connection.
     *
     * @exception IOException If an error occurs
     */
    public void close() throws IOException
    {
      impl.close();
    }

    /**
     * Hmmm, we don't seem to have a flush() method in Socket impl, so just
     * return for now, but this might need to be looked at later.
     *
     * @exception IOException Can't happen
     */
    public void flush() throws IOException
    {
    }

    /**
     * Writes a byte (passed in as an int) to the given output stream
     *
     * @param b The byte to write
     *
     * @exception IOException If an error occurs
     */
    public void write (int b) throws IOException
    {
      byte buf[] = new byte [1];
      Integer i = new Integer (b);

      buf [0] = i.byteValue();
      write (buf, 0, buf.length);
    }

    /**
     * Write an array of bytes to the output stream
     *
     * @param buf The array of bytes to write
     *
     * @exception IOException If an error occurs
     */
    public void write (byte[] buf) throws IOException
    {
      write (buf, 0, buf.length);
    }

    /**
     * Writes len number of bytes from the array buf to the stream starting
     * at offset bytes into the buffer.
     *
     * @param buf The buffer
     * @param offset Offset into the buffer to start writing from
     * @param len The number of bytes to write
     */
    public void write (byte[] buf, int offset, int len) throws IOException
    {
      impl.write (buf, offset, len);
    }

  } // class SocketOutputStream

}
