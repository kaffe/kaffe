/*
 * $Id: DTP.java,v 1.2 2004/03/22 11:24:07 dalibor Exp $
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

/**
 * An FTP data transfer process.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 * @version $Revision: 1.2 $ $Date: 2004/03/22 11:24:07 $
 */
interface DTP
{

        /**
	 * Returns an input stream from which a remote file can be read.
	 */
  InputStream getInputStream() throws IOException;

        /**
	 * Returns an output stream to which a local file can be written for
	 * upload.
	 */
  OutputStream getOutputStream() throws IOException;

        /**
	 * Sets the transfer mode to be used with this DTP.
	 */
  void setTransferMode(int mode);

        /**
	 * Marks this DTP completed.
	 * When the current transfer has finished, any resources will be released.
	 */
  void complete();

        /**
	 * Aborts any current transfer and releases all resources held by this
	 * DTP.
	 * @return true if a transfer was interrupted, false otherwise
	 */
  boolean abort();

        /**
	 * Used to notify the DTP that its current transfer is complete.
	 * This occurs either when end-of-stream is reached or a 226 response is
	 * received.
	 */
  void transferComplete();

}
