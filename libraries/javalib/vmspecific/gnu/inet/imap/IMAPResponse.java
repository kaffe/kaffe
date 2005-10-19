/*
 * IMAPResponse.java
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
 * Linking this library statically or dynamically with other modules is
 * making a combined work based on this library.  Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * As a special exception, the copyright holders of this library give you
 * permission to link this library with independent modules to produce an
 * executable, regardless of the license terms of these independent
 * modules, and to copy and distribute the resulting executable under
 * terms of your choice, provided that you also meet, for each linked
 * independent module, the terms and conditions of the license of that
 * module.  An independent module is a module which is not derived from
 * or based on this library.  If you modify this library, you may extend
 * this exception to your version of the library, but you are not
 * obliged to do so.  If you do not wish to do so, delete this
 * exception statement from your version.
 */

package gnu.inet.imap;

import java.util.List;

/**
 * An IMAP4rev1 server response.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public class IMAPResponse
{

  /**
   * The untagged response tag.
   */
  public static final String UNTAGGED = "*";

  /**
   * The continuation response tag.
   */
  public static final String CONTINUATION = "+";

  /**
   * The tag for this response.
   */
  protected String tag = null;

  /**
   * The response ID.
   */
  protected String id = null;

  /**
   * The message count (for responses returning counts).
   */
  protected int count = -1;

  /**
   * The mailbox (for STATUS responses).
   */
  protected String mailbox = null;

  /**
   * The response code.
   */
  protected List code = null;

  /**
   * The human-readable text.
   */
  protected String text;

  public String getTag()
  {
    return tag;
  }

  public boolean isTagged()
  {
    return (tag != UNTAGGED && tag != CONTINUATION);
  }

  public boolean isUntagged()
  {
    return (tag == UNTAGGED);
  }

  public boolean isContinuation()
  {
    return (tag == CONTINUATION);
  }

  public String getID()
  {
    return id;
  }

  public int getCount()
  {
    return count;
  }

  public List getResponseCode()
  {
    return code;
  }

  public String getText()
  {
    return text;
  }

  /**
   * ANSI-coloured toString for debugging.
   */
  public String toANSIString()
  {
    StringBuffer buffer = new StringBuffer();
    buffer.append(tag);
    if (count != -1)
      {
        buffer.append(" \u001b[00;31m");
        buffer.append(count);
        buffer.append("\u001b[00m");
      }
    if (!isContinuation())
      {
        buffer.append(" \u001b[01m");
        buffer.append(id);
        buffer.append("\u001b[00m");
      }
    if (mailbox != null)
      {
        buffer.append(" \u001b[00;35m");
        buffer.append(mailbox);
        buffer.append("\u001b[00m");
      }
    if (code != null)
      {
        buffer.append(" \u001b[00;36m");
        buffer.append(code);
        buffer.append("\u001b[00m");
      }
    if (text != null)
      {
        buffer.append(" \u001b[00;33m");
        buffer.append(text);
        buffer.append("\u001b[00m");
      }
    return buffer.toString();
  }

  public String toString()
  {
    StringBuffer buffer = new StringBuffer();
    buffer.append(tag);
    if (count != -1)
      {
        buffer.append(' ');
        buffer.append(count);
      }
    if (!isContinuation())
      {
        buffer.append(' ');
        buffer.append(id);
      }
    if (mailbox != null)
      {
        buffer.append(' ');
        buffer.append(mailbox);
      }
    if (code != null)
      {
        buffer.append(' ');
        buffer.append(code);
      }
    if (text != null)
      {
        buffer.append(' ');
        buffer.append(text);
      }
    return buffer.toString();
  }

}

