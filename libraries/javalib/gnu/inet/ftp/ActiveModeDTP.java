/*
 * $Id: ActiveModeDTP.java,v 1.2 2004/03/22 11:24:07 dalibor Exp $
 * Copyright (C) 2003 The Free Software Foundation
 * 
 * This file is part of GNU inetlib, a library.
 * 
 * GNU inetlib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * GNU inetlib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 * As a special exception, if you link this library with other files to
 * produce an executable, this library does not by itself cause the
 * resulting executable to be covered by the GNU General Public License.
 * This exception does not however invalidate any other reasons why the
 * executable file might be covered by the GNU General Public License.
 */

package gnu.inet.ftp;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;

/**
 * An active mode FTP data transfer process.
 * This starts a server on the specified port listening for a data
 * connection. It converts the socket input into a file stream for reading.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 * @version $Revision: 1.2 $ $Date: 2004/03/22 11:24:07 $
 */
final class ActiveModeDTP implements DTP, Runnable
{

  ServerSocket server;
  Socket socket;
  DTPInputStream in;
  DTPOutputStream out;
  boolean completed;
  boolean inProgress;
  int transferMode;
  IOException exception;
  Thread acceptThread;

    ActiveModeDTP(InetAddress localhost, int port) throws IOException
  {
    completed = false;
    inProgress = false;
    server = new ServerSocket(port, 1, localhost);
    acceptThread = new Thread(this, "ActiveModeDTP");
    acceptThread.start();
  }

        /**
	 * Start listening.
	 */
  public void run()
  {
    try
    {
      socket = server.accept();
      //System.err.println("Accepted connection from "+socket.getInetAddress()+":"+socket.getPort());
    }
    catch(IOException e)
    {
      exception = e;
    }
  }

        /**
	 * Waits until a client has connected.
	 */
  public void waitFor() throws IOException
  {
    long timeout = 6000;        // TODO review this
      try
    {
      acceptThread.join(timeout);
    }
    catch(InterruptedException e)
    {
    }
    if (exception != null)
      throw exception;
    if (socket == null)
    {
      server.close();
      throw new IOException("client did not connect before timeout");
    }
    acceptThread = null;
  }

        /**
	 * Returns an input stream from which a remote file can be read.
	 */
  public InputStream getInputStream() throws IOException
  {
    if (inProgress)
      throw new IOException("Transfer in progress");
    if (acceptThread != null)
        waitFor();
    switch (transferMode)
    {
    case FTPConnection.MODE_STREAM:
      in = new StreamInputStream(this, socket.getInputStream());
      break;
      case FTPConnection.MODE_BLOCK:in =
        new BlockInputStream(this, socket.getInputStream());
      break;
      case FTPConnection.MODE_COMPRESSED:in =
        new CompressedInputStream(this, socket.getInputStream());
      break;
      default:throw new IllegalStateException("invalid transfer mode");
    }
    in.setTransferComplete(false);
    return in;
  }

        /**
	 * Returns an output stream to which a local file can be written for
	 * upload.
	 */
  public OutputStream getOutputStream() throws IOException
  {
    if (inProgress)
      throw new IOException("Transfer in progress");
    if (acceptThread != null)
        waitFor();
    switch (transferMode)
    {
    case FTPConnection.MODE_STREAM:
      out = new StreamOutputStream(this, socket.getOutputStream());
      break;
      case FTPConnection.MODE_BLOCK:out =
        new BlockOutputStream(this, socket.getOutputStream());
      break;
      case FTPConnection.MODE_COMPRESSED:out =
        new CompressedOutputStream(this, socket.getOutputStream());
      break;
      default:throw new IllegalStateException("invalid transfer mode");
    }
    out.setTransferComplete(false);
    return out;
  }

  public void setTransferMode(int mode)
  {
    transferMode = mode;
  }

  public void complete()
  {
    completed = true;
    if (!inProgress)
      transferComplete();
  }

  public boolean abort()
  {
    completed = true;
    transferComplete();
    return inProgress;
  }

  public void transferComplete()
  {
    if (socket == null)
      return;
    in.setTransferComplete(true);
    out.setTransferComplete(true);
    completed = completed || (transferMode == FTPConnection.MODE_STREAM);
    if (completed)
    {
      try
      {
        socket.close();
      }
      catch(IOException e)
      {
      }
      try
      {
        server.close();
      }
      catch(IOException e)
      {
      }
    }
  }

}
