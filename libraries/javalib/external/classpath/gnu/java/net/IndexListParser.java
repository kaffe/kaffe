/* IndexListParser.java -- 
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


package gnu.java.net;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.URL;
import java.util.ArrayList;
import java.util.jar.JarFile;

/**
 * The INDEX.LIST file contains sections each separated by a blank line. 
 * Each section defines the content of a jar, with a
 * header defining the jar file path name, followed by a list of paths.
 * The jar file paths are relative to the codebase of the root jar.
 * 
    Specification
    index file :            version-info blankline section*
    version-info :          JarIndex-Version: version-number
    version-number :        digit+{.digit+}*
    section :               body blankline
    body :                  header name*
    header :                char+.jar newline
    name :                  char+ newline
    
 * @author langel at redhat dot com
 */
public class IndexListParser
{
  String filePath = "META-INF/INDEX.LIST";
  String versInfo = "JarIndex-Version: ";
  double versionNumber;
  ArrayList headers = new ArrayList();
  
  /**
   * Parses the given jarfile's INDEX.LIST file if it exists.
   * 
   * @param jarfile - the given jar file
   * @param baseJarURL - the codebase of the jar file
   * @param baseURL - the base url for the headers
   */
  public IndexListParser(JarFile jarfile, URL baseJarURL, URL baseURL)
  {
    try
    {
    // Parse INDEX.LIST if it exists
    if (jarfile.getEntry(filePath) != null)
      {
        BufferedReader br = new BufferedReader(new InputStreamReader(new URL(baseJarURL,
                                                                             filePath).openStream()));
        
        // Must start with version info
        String line = br.readLine();
        if (!line.startsWith(versInfo))
          return;
        versionNumber = Double.parseDouble(line.substring(versInfo.length()).trim());
        
        // Blank line must be next
        line = br.readLine();
        if (!line.equals(""))
          {
            clearAll();
            return;
          }
        
        // May contain sections
        line = br.readLine();
        while (line != null)
          {
            headers.add(new URL(baseURL, line));
            
            // Skip all names in the section
            while (! (line = br.readLine()).equals(""));
            line = br.readLine();
          }
      }
    // else INDEX.LIST does not exist
    }
    catch (Exception ex)
    {
      clearAll();
    }
  }
  
  /**
   * Clears all the variables. This is called when parsing fails.
   */
  void clearAll()
  {
    versionNumber = 0;
    headers.clear();
  }
  
  /**
   * Gets the version info for the file.
   * 
   * @return the version info.
   */
  public String getVersionInfo()
  {
    return versInfo + getVersionNumber();
  }
  
  /**
   * Gets the version number of the file.
   * 
   * @return the version number.
   */
  public double getVersionNumber()
  {
    return versionNumber;
  }
  
  /**
   * Gets the array list of all the headers found in the file.
   * 
   * @return an array list of all the headers.
   */
  public ArrayList getHeaders()
  {
    return headers;
  }
}
