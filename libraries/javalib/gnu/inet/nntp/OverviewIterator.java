/*
 * $Id: OverviewIterator.java,v 1.1 2004/07/25 22:46:23 dalibor Exp $
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

import java.io.IOException;
import java.util.NoSuchElementException;

/**
 * An iterator over an overview listing.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 * @version $Revision: 1.1 $ $Date: 2004/07/25 22:46:23 $
 */
public class OverviewIterator extends LineIterator
{

  OverviewIterator (NNTPConnection connection)
    {
      super (connection);
    }

  /**
   * Returns the next overview entry.
   */
  public Object next ()
    {
      try
        {
          return nextOverview ();
        }
      catch (IOException e)
        {
          throw new NoSuchElementException ("I/O error: " + e.getMessage ());
        }
    }

  /**
   * Returns the next overview entry.
   */
  public Overview nextOverview () throws IOException
    {
      String line = nextLine ();

      // Parse line
      int start = 0, end;
      end = line.indexOf ('\t', start);
      int articleNumber = Integer.parseInt (line.substring (start, end));
      start = end + 1;
      Overview overview = new Overview (articleNumber);
      end = line.indexOf ('\t', start);
      while (end > -1)
        {
          String entry = line.substring (start, end);
          overview.add (entry);
          start = end + 1;
          end = line.indexOf ('\t', start);
        }
      String entry = line.substring (start);
      overview.add (entry);

      return overview;
    }

}
