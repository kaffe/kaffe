/*
 * FileNewsrc.java
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

package gnu.inet.nntp;

import java.io.BufferedReader;
import java.io.BufferedOutputStream;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStreamReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

/**
 * A .newsrc configuration on a filesystem.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public class FileNewsrc
  implements Newsrc
{

  private static final String NEWSRC_ENCODING = "US-ASCII";

  protected File file;

  protected List subs = null;
  protected List groups = null;
  protected Map lines = null;
  protected boolean dirty;
  protected boolean debug;

  /**
   * Constructor.
   * @param file the disk file
   * @param debug for debugging information on stderr
   */
  public FileNewsrc(File file, boolean debug)
  {
    this.file = file;
    this.debug = debug;
  }

  public void close()
  {
    if (!dirty)
      {
        return;
      }
    save();
  }
  
  /**
   * Load the file.
   */
  void load()
  {
    long fs = file.length();
    long max = (long) Integer.MAX_VALUE;
    int bs = (int) (fs > max ? max : fs);
    
    groups = new LinkedList();
    lines = new HashMap(bs / 20);
    subs = new LinkedList();
    
    // Load
    try
      {
        long t1 = System.currentTimeMillis();
        if (debug)
          {
            System.err.println("DEBUG: nntp: newsrc loading " +
                               file.getPath());
          }
        
        FileInputStream fr = new FileInputStream(file);
        InputStreamReader ir = new InputStreamReader(fr, NEWSRC_ENCODING);
        BufferedReader reader = new BufferedReader(ir, bs);
        String line = reader.readLine();
        while (line != null)
          {
            int cp = line.indexOf(':');
            if (cp > -1)
              {
                // Subscribed newsgroup
                String name = line.substring(0, cp);
                groups.add(name);
                subs.add(name);
                cp++;
                if (cp < line.length())
                  {
                    String tail = line.substring(cp).trim();
                    if (tail.length() > 0)
                      {
                        lines.put(name, tail);
                      }
                  }
              }
            else
              {
                int pp = line.indexOf('!');
                if (pp > -1)
                  {
                    // Unsubscribed newsgroup
                    String name = line.substring(0, pp);
                    groups.add(name);
                    pp++;
                    if (pp < line.length())
                      {
                        String tail = line.substring(pp).trim();
                        if (tail.length() > 0)
                          {
                            lines.put(name, tail);
                          }
                      }
                  }
                // else ignore - comments etc will not be saved!
              }
            line = reader.readLine();
          }
        reader.close();
        long t2 = System.currentTimeMillis();
        if (debug)
          {
            System.err.println("DEBUG: nntp: newsrc load: " +
                               groups.size() + " groups in " +
                               (t2 - t1) + "ms");
          }
      }
    catch (FileNotFoundException e)
      {
      }
    catch (IOException e)
      {
        System.err.println("WARNING: nntp: unable to read newsrc file");
        if (debug)
          {
            e.printStackTrace(System.err);
          }
      }
    catch (SecurityException e)
      {
        System.err.println("WARNING: nntp: " +
                           "no read permission on newsrc file");
      }
    dirty = false;
  }
  
  /**
   * Save the file.
   */
  void save()
  {
    try
      {
        long t1 = System.currentTimeMillis();
        if (debug)
          {
            System.err.println("DEBUG: nntp: newsrc saving " +
                               file.getPath());
          }

        int bs = (groups.size() * 20);    // guess an average line length
        FileOutputStream fw = new FileOutputStream(file);
        BufferedOutputStream writer = new BufferedOutputStream(fw, bs);
        for (Iterator i = groups.iterator(); i.hasNext();)
          {
            String group = (String) i.next();
            StringBuffer buffer = new StringBuffer(group);
            if (subs.contains(group))
              {
                buffer.append(':');
              }
            else
              {
                buffer.append('!');
              }
            Object r = lines.get(group);
            if (r instanceof String)
              {
                buffer.append((String) r);
              }
            else
              {
                RangeList ranges = (RangeList) r;
                if (ranges != null)
                  {
                    buffer.append(ranges.toString());
                  }
              }
            buffer.append('\n');

            byte[] bytes = buffer.toString().getBytes(NEWSRC_ENCODING);
            writer.write(bytes);
          }
        writer.flush();
        writer.close();

        long t2 = System.currentTimeMillis();
        if (debug)
          {
            System.err.println("DEBUG: nntp: newsrc save: " +
                               groups.size() + " groups in " +
                               (t2 - t1) + "ms");
          }
      }
    catch (IOException e)
      {
        System.err.println("WARNING: nntp: unable to save newsrc file");
        if (debug)
          {
            e.printStackTrace(System.err);
          }
      }
    dirty = false;
  }

  /**
   * Returns an iterator over the names of the currently subscribed
   * newsgroups.
   */
  public Iterator list()
  {
    if (subs == null)
      {
        load();
      }
    return subs.iterator();
  }

  public boolean isSubscribed(String newsgroup)
  {
    if (subs == null)
      {
        load();
      }
    return (subs.contains(newsgroup));
  }

  public void setSubscribed(String newsgroup, boolean flag)
  {
    if (subs == null)
      {
        load();
      }
    if (flag && !groups.contains(newsgroup))
      {
        groups.add(newsgroup);
      }
    boolean subscribed = subs.contains(newsgroup);
    if (flag && !subscribed)
      {
        subs.add(newsgroup);
        dirty = true;
      }
    else if (!flag && subscribed)
      {
        subs.remove(newsgroup);
        dirty = true;
      }
  }

  public boolean isSeen(String newsgroup, int article)
  {
    if (subs == null)
      {
        load();
      }
    Object value = lines.get(newsgroup);
    if (value instanceof String)
      {
        value = new RangeList((String) value);
      }
    RangeList ranges = (RangeList) value;
    if (ranges != null)
      {
        return ranges.isSeen(article);
      }
    return false;
  }

  public void setSeen(String newsgroup, int article, boolean flag)
  {
    if (subs == null)
      {
        load();
      }
    Object value = lines.get(newsgroup);
    if (value instanceof String)
      {
        value = new RangeList((String) value);
      }
    RangeList ranges = (RangeList) value;
    if (ranges == null)
      {
        ranges = new RangeList();
        lines.put(newsgroup, ranges);
        dirty = true;
      }
    if (ranges.isSeen(article) != flag)
      {
        ranges.setSeen(article, flag);
        dirty = true;
      }
  }

  /**
   * A RangeList holds a series of ranges that are ordered and
   * non-overlapping.
   */
  static class RangeList
  {

    List seen;

    RangeList()
    {
      seen = new ArrayList();
    }

    RangeList(String line)
    {
      this();
      try
        {
          // Parse the line at comma delimiters.
          int start = 0;
          int end = line.indexOf(',');
          while (end > start)
            {
              String token = line.substring(start, end);
              addToken(token);
              start = end + 1;
              end = line.indexOf(',', start);
            }
          addToken(line.substring(start));
        }
      catch (NumberFormatException e)
        {
          System.err.println("ERROR: nntp: bad newsrc format: " + line);
        }
    }

    /*
     * Used during initial parse.
     */
    private void addToken(String token) throws NumberFormatException
    {
      int hp = token.indexOf('-');
      if (hp > -1)
        {
          // Range
          String fs = token.substring(0, hp);
          String ts = token.substring(hp + 1);
          int from = Integer.parseInt(fs);
          int to = Integer.parseInt(ts);
          if (from > -1 && to > -1)
            {
              insert(from, to);
            }
        }
      else
        {
          // Single number
          int number = Integer.parseInt(token);
          if (number > -1)
            {
              insert(number);
            }
        }
    }

    /**
     * Indicates whether the specified article is seen.
     */
    public boolean isSeen(int num)
    {
      int len = seen.size();
      Range[] r = new Range[len];
      seen.toArray(r);
      for (int i = 0; i < len; i++)
        {
          if (r[i].contains(num))
            {
              return true;
            }
        }
      return false;
    }

    /**
     * Sets whether the specified article is seen.
     */
    public void setSeen(int num, boolean flag)
    {
      if (flag)
        {
          insert(num);
        }
      else
        {
          remove(num);
        }
    }

    /*
     * Find the index within seen to insert the specified article.
     * The range object at the returned index may already contain num.
     */
    int indexOf(int num)
    {
      int len = seen.size();
      Range[] r = new Range[len];
      seen.toArray(r);
      for (int i = 0; i < len; i++)
        {
          if (r[i].contains(num))
            {
              return i;
            }
          if (r[i].from > num)
            {
              return i;
            }
          if (r[i].to == num - 1)
            {
              return i;
            }
        }
      return len;
    }

    void insert(int start, int end)
    {
      Range range = new Range(start, end);
      int i1 = indexOf(range.from);
      // range is at end
      if (i1 == seen.size())
        {
          seen.add(range);
          return;
        }
      Range r1 = (Range) seen.get(i1);
      // range is before r1
      if (range.to < r1.from)
        {
          seen.add(i1, range);
          return;
        }
      // range is a subset of r1
      if (r1.from <= range.from && r1.to >= range.to)
        {
          return;
        }
      // range is a superset of r1
      int i2 = indexOf(range.to);
      Range r2 = (Range) seen.get(i2);
      System.err.println("r2 " + r2 + " i2 " + i2);
      // remove all ranges between
      for (int i = i2; i >= i1; i--)
        {
          seen.remove(i);
        }
      // merge
      int f = (range.from < r1.from) ? range.from : r1.from;
      int t = (range.to > r2.to) ? range.to : r2.to;
      range = new Range(f, t);
      seen.add(i1, range);
    }

    void insert(int num)
    {
      insert(num, num);
    }

    void remove(int num)
    {
      int i = indexOf(num);
      Range r = (Range) seen.get(i);
      seen.remove(i);
      // num == r
      if ((r.from == r.to) &&(r.to == num))
        {
          return;
        }
      // split r
      if (r.to > num)
        {
          Range r2 = new Range(num + 1, r.to);
          seen.add(i, r2);
        }
      if (r.from < num)
        {
          Range r2 = new Range(r.from, num - 1);
          seen.add(i, r2);
        }
    }

    public String toString()
    {
      StringBuffer buf = new StringBuffer();
      int len = seen.size();
      for (int i = 0; i < len; i++)
        {
          Range range = (Range) seen.get(i);
          if (i > 0)
            {
              buf.append(',');
            }
          buf.append(range.toString());
        }
      return buf.toString();
    }

  }

  /**
   * A range is either a single integer or a range between two integers.
   */
  static class Range
  {
    int from;
    int to;

    public Range(int i)
    {
      from = to = i;
    }

    public Range(int f, int t)
    {
      if (f > t)
        {
          from = t;
          to = f;
        }
      else
        {
          from = f;
          to = t;
        }
    }

    public boolean contains(int num)
    {
      return (num >= from && num <= to);
    }

    public String toString()
    {
      if (from != to)
        {
          return new StringBuffer()
            .append(from)
            .append('-')
            .append(to)
            .toString();
        }
      else
        {
          return Integer.toString(from);
        }
    }

  }

}

