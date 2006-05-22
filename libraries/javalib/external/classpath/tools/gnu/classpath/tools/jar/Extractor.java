/* Extractor.java - action to extract from a jar file
 Copyright (C) 2006 Free Software Foundation, Inc.

 This file is part of GNU Classpath.

 GNU Classpath is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2, or (at your option)
 any later version.

 GNU Classpath is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with GNU Classpath; see the file COPYING.  If not, write to the
 Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 02110-1301 USA.

 Linking this library statically or dynamically with other modules is
 making a combined work based on this library.  Thus, the terms and
 conditions of the GNU General Public License cover the whole
 combination.

 As a special exception, the copyright holders of this library give you
 permission to link this library with independent modules to produce an
 executable, regardless of the license terms of these independent
 modules, and to copy and distribute the resulting executable under
 terms of your choice, provided that you also meet, for each linked
 independent module, the terms and conditions of the license of that
 module.  An independent module is a module which is not derived from
 or based on this library.  If you modify this library, you may extend
 this exception to your version of the library, but you are not
 obligated to do so.  If you do not wish to do so, delete this
 exception statement from your version. */


package gnu.classpath.tools.jar;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Iterator;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

public class Extractor
    extends Action
{
  // This is a set of all the items specified on the command line.
  // It is null if none were specified.
  private HashSet allItems;

  private void copyFile(InputStream input, File output) throws IOException
  {
    FileOutputStream os = new FileOutputStream(output);
    byte[] buffer = new byte[1024];
    while (true)
      {
        int len = input.read(buffer);
        if (len == - 1)
          break;
        os.write(buffer, 0, len);
      }
    os.close();
  }

  private void initSet(ArrayList entries)
  {
    if (entries == null || entries.isEmpty())
      return;
    allItems = new HashSet();
    Iterator it = entries.iterator();
    while (it.hasNext())
      {
        Entry entry = (Entry) it.next();
        int len = entry.name.length();
        while (len > 0 && entry.name.charAt(len - 1) == '/')
          --len;
        String name = entry.name.substring(0, len);
        allItems.add(name);
      }
  }

  private boolean shouldExtract(String filename)
  {
    if (allItems == null)
      return true;
    while (filename.length() > 0)
      {
        if (allItems.contains(filename))
          return true;
        int index = filename.lastIndexOf('/');
        if (index == -1)
          break;
        filename = filename.substring(0, index);
      }
    return false;
  }

  public void run(Main parameters) throws IOException
  {
    // Figure out what we want to extract.
    initSet(parameters.entries);
    // Open the input file.
    ZipInputStream zis;
    File zfile = parameters.archiveFile;
    if (zfile == null || "-".equals(zfile.getName()))
      zis = new ZipInputStream(System.in);
    else
      {
        InputStream ins = new BufferedInputStream(new FileInputStream(zfile));
        zis = new ZipInputStream(ins);
      }
    // Extract stuff.
    while (true)
      {
        ZipEntry entry = zis.getNextEntry();
        if (entry == null)
          break;
        if (! shouldExtract(entry.getName()))
          continue;
        File file = new File(entry.getName());
        if (entry.isDirectory())
          {
            if (file.mkdirs())
              {
                if (parameters.verbose)
                  System.err.println("  created: " + file);
              }
            continue;
          }

        File parent = file.getParentFile();
        if (parent != null)
          parent.mkdirs();

        copyFile(zis, file);

        if (parameters.verbose)
          {
            String leader = (entry.getMethod() == ZipEntry.STORED
                             ? " extracted" : "  inflated");
            System.err.println(leader + ": " + file);
          }
      }
  }
}
