/*
 * $Id: ListEntry.java,v 1.4 2004/10/04 19:34:00 robilad Exp $
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

package gnu.inet.imap;

import java.util.List;

/**
 * An item in an IMAP LIST or LSUB response.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 * @version $Revision: 1.4 $ $Date: 2004/10/04 19:34:00 $
 */
public class ListEntry implements IMAPConstants
{

  private String mailbox;

  private char delimiter;

  private boolean noinferiors;
  private boolean noselect;
  private boolean marked;
  private boolean unmarked;

  /*
   * Constructor.
   * The list entry is otherwise immutable.
   */
  ListEntry (String mailbox, char delimiter, boolean noinferiors,
             boolean noselect, boolean marked, boolean unmarked)
    {
      this.mailbox = mailbox;
      this.delimiter = delimiter;
      this.noinferiors = noinferiors;
      this.noselect = noselect;
      this.marked = marked;
      this.unmarked = unmarked;
    }

  /**
   * Returns the mailbox this list entry refers to.
   */
  public String getMailbox ()
    {
      return mailbox;
    }

  /**
   * Returns the mailbox hierarchy delimiter.
   */
  public char getDelimiter ()
    {
      return delimiter;
    }

  /**
   * If true: it is not possible for any child levels of hierarchy to
   * exist under this name; no child levels exist now and none can be
   * created in the future.
   */
  public boolean isNoinferiors ()
    {
      return noinferiors;
    }

  /**
   * If true: it is not possible to use this name as a selectable
   * mailbox.
   */
  public boolean isNoselect ()
    {
      return noselect;
    }

  /**
   * If true: the mailbox has been marked "interesting" by the server;
   * the mailbox probably contains messages that have been added since
   * the last time the mailbox was selected.
   */
  public boolean isMarked ()
    {
      return marked;
    }

  /**
   * If true: the mailbox does not contain any additional messages since
   * the last time the mailbox was selected.
   */
  public boolean isUnmarked ()
    {
      return unmarked;
    }

  /**
   * Debugging
   */
  public String toString()
    {
      StringBuffer buffer = new StringBuffer ();
      if (noinferiors || noselect || marked || unmarked)
        {
          buffer.append ("(\u001b[00;35m");
          boolean seq = false;
          seq = conditionalAppend (buffer, seq, noinferiors, LIST_NOINFERIORS);
          seq = conditionalAppend (buffer, seq, noselect, LIST_NOSELECT);
          seq = conditionalAppend (buffer, seq, marked, LIST_MARKED);
          seq = conditionalAppend (buffer, seq, unmarked, LIST_UNMARKED);
          buffer.append ("\u001b[00m) ");
        }
      buffer.append ("\"\u001b[00;31m");
      buffer.append (delimiter);
      buffer.append ("\u001b[00m\" ");
      buffer.append (mailbox);
      return buffer.toString ();
    }

  private static boolean conditionalAppend (StringBuffer buffer,
                                            boolean seq, boolean test,
                                            String value)
    {
      if (test)
        {
          if (seq)
            {
              buffer.append (' ');
            }
          buffer.append (value);
          seq = true;
        }
      return seq;
    }

}
