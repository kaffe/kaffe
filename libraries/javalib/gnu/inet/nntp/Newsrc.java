/*
 * $Id: Newsrc.java,v 1.3 2004/10/04 19:34:01 robilad Exp $
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

import java.util.Iterator;

/**
 * Interface for a .newsrc configuration.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 * @version $Revision: 1.3 $ $Date: 2004/10/04 19:34:01 $
 */
public interface Newsrc
{

  /**
   * Returns an iterator over the names of the subscribed newsgroups.
   * Each item returned is a String.
   */
  public Iterator list ();

  /**
   * Indicates whether a newsgroup is subscribed in this newsrc.
   */
  public boolean isSubscribed (String newsgroup);

  /**
   * Sets whether a newsgroup is subscribed in this newsrc.
   */
  public void setSubscribed (String newsgroup, boolean subs);

  /**
   * Indicates whether an article is marked as seen in the specified newsgroup.
   */
  public boolean isSeen (String newsgroup, int article);

  /**
   * Sets whether an article is marked as seen in the specified newsgroup.
   */
  public void setSeen (String newsgroup, int article, boolean seen);

  /**
   * Closes the configuration, potentially saving any changes.
   */
  public void close ();

}
