/*
 * $Id: ArticleStream.java,v 1.3 2004/10/04 19:34:01 robilad Exp $
 * Copyright (C) 2002 The Free Software Foundation
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

package gnu.inet.nntp;

import java.io.FilterInputStream;
import java.io.InputStream;
import java.io.IOException;

/**
 * A stream that can be told to read to the end of its data.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 * @version $Revision: 1.3 $ Date$
 */
public final class ArticleStream
  extends FilterInputStream implements PendingData
{

  ArticleStream (InputStream in)
  {
    super (in);
  }

  /**
   * Read to the end of the article data.
   */
  public void readToEOF () throws IOException
  {
    if (in.available () == 0)
      {
        return;
      }
    byte[] buf = new byte[4096];
    int ret = 0;
    while (ret != -1)
      {
        ret = in.read (buf);
      }
  }

}
