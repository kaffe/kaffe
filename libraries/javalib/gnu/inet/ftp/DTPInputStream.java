/*
 * $Id: DTPInputStream.java,v 1.5 2004/10/04 19:33:56 robilad Exp $
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

import java.io.FilterInputStream;
import java.io.InputStream;
import java.io.IOException;

/**
 * An input stream that notifies a DTP on completion.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 * @version $Revision: 1.5 $ $Date: 2004/10/04 19:33:56 $
 */
abstract class DTPInputStream extends FilterInputStream
{

  DTP dtp;
  boolean transferComplete;

  /**
   * Constructor.
   * @param dtp the controlling data transfer process
   * @param in the underlying socket stream
   */
  DTPInputStream (DTP dtp, InputStream in)
    {
      super (in);
      this.dtp = dtp;
      transferComplete = false;
    }

  /**
   * Marks this input stream complete.
   * This is called by the DTP.
   */
  void setTransferComplete (boolean flag)
    {
      transferComplete = flag;
    }

  /**
   * Notifies the controlling DTP that this stream has completed transfer.
   */
  public void close () throws IOException
    {
      dtp.transferComplete ();
    }

}
