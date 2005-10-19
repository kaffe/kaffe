/*
 * gnu.classpath.tools.Util Copyright (C) 2001 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Classpath.
 * 
 * GNU Classpath is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2, or (at your option) any later version.
 * 
 * GNU Classpath is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * GNU Classpath; see the file COPYING. If not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 */
package gnu.classpath.tools;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.InputStream;
import java.io.IOException;
import java.util.StringTokenizer;
import java.util.zip.ZipFile;
import java.util.zip.ZipEntry;
import java.util.zip.ZipException;

public class Util
{
  private String bootclasspath;
  private String classpath;
  private String userclasspath;

  public Util()
  {
    classpath = System.getProperty("java.class.path");
    bootclasspath = System.getProperty("sun.boot.class.path");
  }

  /**
   * Searches from the current working directory for a file
   * package/classname.class unless the user supplied a classpath. If so then
   * searches through the user supplied classpath. Finally searches the system
   * classpath and bootclasspath.
   * 
   * @param className the class to find
   * @return a valid InputStream for reading the given class's class file
   * @throws ClassNotFoundException if the class cannot be found
   */
  public InputStream findClass(String className) throws ClassNotFoundException
  {
    InputStream is = null;
    String filename = getFileNameFromClassName(className);
    if (userclasspath != null)
      {
        String path = getSearchPath();
        is = findClass(filename, path);
      }
    else
      {
        is = findFile(filename);
	if (is == null)
	  {
            String path = getSearchPath();
            is = findClass(filename, path);
	  }
      }
    if (is != null)
      return is;
    throw new ClassNotFoundException(className + " not found");
  }

  /**
   * Attempt to get an InputStream for the specified filename.
   * 
   * @param filename the filename to find in the specified path
   * @param classpath the path or paths to search for the specified file
   * @return <code>null</code> if the file cannot be found
   */
  private InputStream findClass(String filename, String classpath)
  {
    StringTokenizer st = new StringTokenizer(classpath, File.pathSeparator);
    while (st.hasMoreTokens())
      {
        String path = st.nextToken();
        File f = new File(path);
        if (f.exists() && f.isDirectory())
          {
            f = new File(f, filename);
            if (f.exists())
              {
                try
                  {
                    FileInputStream fis = new FileInputStream(f);
                    return new BufferedInputStream(fis);
                  }
                catch (FileNotFoundException fe)
                  {
                  }
              }
          }
        else
          {
            if (f.exists() && f.isFile())
              {
                try
                  {
                    ZipFile zip = new ZipFile(f);
                    ZipEntry entry = zip.getEntry(filename);
                    if (entry != null)
                      return zip.getInputStream(entry);
                  }
                catch (IllegalStateException ise)
                  {
                  }
                catch (ZipException ze)
                  {
                  }
                catch (IOException ioe)
                  {
                  }
              }
          }
      }
    return null;
  }

  /**
   * Attempt to get an InputStream for the specified filename.
   * 
   * @param filename the filename to get an InputStream for
   * @return <code>null</code> if the file cannot be found
   */
  private InputStream findFile(String filename)
  {
    File f = new File(filename);
    if (f.exists())
      {
        try
          {
            FileInputStream fis = new FileInputStream(f);
            return new BufferedInputStream(fis);
          }
        catch (FileNotFoundException fe)
          {
          }
      }
    return null;
  }

  /**
   * Class names of the form a.b.c, or simply c if the class has no package are
   * the most common. Also support the use of filenames here, if the file exists
   * relative to the current working directory or is absolute.
   * 
   * @param className
   * @return
   */
  private String getFileNameFromClassName(String className)
  {
    File f = new File(className);
    if (f.exists())
      return className;
    String filename = className.replace('.', '/');
    filename = filename + ".class";
    return filename;
  }

  /**
   * Used to set the user provided classpath.
   * 
   * @param path the user classpath
   */
  public void setClasspath(String path)
  {
    userclasspath = path;
  }
  
  /**
   * Returns the path used to find classes.  To make it possible to 
   * find classes from the user's provided classpath before finding
   * them in the system classpath the user's path will be first instead
   * of last as in Sun's implementation.
   * 
   * @return the entire classpath
   */
  public String getSearchPath()
  {
    StringBuffer buf = new StringBuffer();
    if (userclasspath != null)
      buf.append(userclasspath);
    else
    {
      if (bootclasspath != null)
      {
        buf.append(bootclasspath);
        buf.append(File.pathSeparator);
      }
      buf.append(classpath);
    }
    return buf.toString();    
  }
}
