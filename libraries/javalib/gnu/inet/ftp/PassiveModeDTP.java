/*
 * $Id: PassiveModeDTP.java,v 1.3 2004/07/25 22:46:18 dalibor Exp $
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

import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.Socket;

/**
 * A passive mode FTP data transfer process.
 * This connects to the host specified and proxies the resulting socket's
 * input and output streams.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 * @version $Revision: 1.3 $ $Date: 2004/07/25 22:46:18 $
 */
final class PassiveModeDTP implements DTP
{

  final String address;
  final int port;
  Socket socket;
  DTPInputStream in;
  DTPOutputStream out;
  boolean completed;
  boolean inProgress;
  int transferMode;

  PassiveModeDTP (String address, int port, InetAddress localhost)
    throws IOException
    {
      this.address = address;
      this.port = port;
      completed = false;
      inProgress = false;
      socket = new Socket (address, port, localhost, port + 1);
    }

  /**
   * Returns an input stream from which a remote file can be read.
   */
  public InputStream getInputStream () throws IOException
    {
      if (inProgress)
        {
          throw new IOException ("Transfer in progress");
        }
      switch (transferMode)
        {
        case FTPConnection.MODE_STREAM:
          in = new StreamInputStream (this, socket.getInputStream ());
          break;
        case FTPConnection.MODE_BLOCK:
          in = new BlockInputStream (this, socket.getInputStream ());
          break;
        case FTPConnection.MODE_COMPRESSED:
          in = new CompressedInputStream (this, socket.getInputStream ());
          break;
        default:
          throw new IllegalStateException ("Invalid transfer mode");
        }
      in.setTransferComplete (false);
      return in;
    }

  /**
   * Returns an output stream to which a local file can be written for
   * upload.
   */
  public OutputStream getOutputStream () throws IOException
    {
      if (inProgress)
        {
          throw new IOException ("Transfer in progress");
        }
      switch (transferMode)
        {
        case FTPConnection.MODE_STREAM:
          out = new StreamOutputStream (this, socket.getOutputStream ());
          break;
        case FTPConnection.MODE_BLOCK:
          out = new BlockOutputStream (this, socket.getOutputStream ());
          break;
        case FTPConnection.MODE_COMPRESSED:
          out = new CompressedOutputStream (this, socket.getOutputStream ());
          break;
        default:
          throw new IllegalStateException("Invalid transfer mode");
        }
      out.setTransferComplete (false);
      return out;
    }

  public void setTransferMode (int mode)
    {
      transferMode = mode;
    }

  public void complete ()
    {
      completed = true;
      if (!inProgress)
        {
          transferComplete ();
        }
    }

  public boolean abort ()
    {
      completed = true;
      transferComplete ();
      return inProgress;
    }

  /*
   * Called by DTPInputStream or DTPOutputStream when end of
   * stream is reached.
   */
  public void transferComplete ()
    {
      if (in != null)
        {
          in.setTransferComplete (true);
        }
      if (out != null)
        {
          out.setTransferComplete (true);
        }
      inProgress = false;
      completed = completed || (transferMode == FTPConnection.MODE_STREAM);
      if (completed && socket != null)
        {
          try
            {
              socket.close ();
            }
          catch (IOException e)
            {
            }
        }
    }

}
