/* gnu.classpath.tools.doclets.xmldoclet.FileSystemClassLoader
   Copyright (C) 2001 Free Software Foundation, Inc.

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
Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
02111-1307 USA. */

package gnu.classpath.tools.doclets.xmldoclet;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.InputStream;
import java.io.IOException;

import java.util.List;
import java.util.ArrayList;
import java.util.StringTokenizer;

import java.util.jar.JarEntry;
import java.util.jar.JarFile;

public class FileSystemClassLoader extends ClassLoader {

   private File[] pathComponents;

   public FileSystemClassLoader(String path)
   {
      List components = new ArrayList();
      for (StringTokenizer st = new StringTokenizer(path, File.pathSeparator); st.hasMoreTokens(); ) {
         File pathComponent = new File(st.nextToken());
         components.add(pathComponent);
      }
      File[] componentArray = new File[components.size()];
      this.pathComponents = (File[])components.toArray(componentArray);
   }

   public FileSystemClassLoader(File[] pathComponents)
   {
      this.pathComponents = pathComponents;
      for (int i = 0; i < pathComponents.length; ++i) {
         if (!pathComponents[i].exists()) {
            System.err.println("WARNING: Path component '" + pathComponents[i] + "' not found.");
         }
      }
   }

   public Class loadClass(String name)
      throws ClassNotFoundException {

      return super.loadClass(name);
   }

   public Class findClass(String name)
      throws ClassNotFoundException {

      byte[] b = loadClassData(name);
      return defineClass(name, b, 0, b.length);
   }

   private byte[] readFromStream(InputStream in, long size) 
      throws IOException
   {
      byte[] result = new byte[(int)size];
      int nread = 0;
      int offset = 0;
      while (offset < size && (nread = in.read(result, offset, (int)(size - offset))) >= 0) {
         offset += nread;
      }
      in.close();
      return result;
   }
   
   private byte[] loadClassData(String className) 
      throws ClassNotFoundException 
   {
      try {
         String classFileName = className.replace('.', File.separatorChar) + ".class";
         
         for (int i = 0; i < pathComponents.length; ++i) {
            try {
               File parent = pathComponents[i];
               if (parent.isDirectory()) {
                  File file = new File(parent, classFileName);
                  if (file.exists()) {
                     return readFromStream(new FileInputStream(file), file.length());
                  }
               }
               else {
                  JarFile jarFile = new JarFile(parent, false, JarFile.OPEN_READ);
                  JarEntry jarEntry = jarFile.getJarEntry(classFileName);
                  if (null != jarEntry) {
                     return readFromStream(jarFile.getInputStream(jarEntry), 
                                           jarEntry.getSize());
                  }
               }
            }
            catch (FileNotFoundException ignore) {
            }
         }
      }
      catch (IOException ignore_) {
      }
      throw new ClassNotFoundException(className);
   }      
}

