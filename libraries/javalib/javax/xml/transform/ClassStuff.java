/*
 * ClassStuff.java
 * Copyright (C) 2001 The Free Software Foundation
 * 
 * This file is part of GNU JAXP, a library.
 *
 * GNU JAXP is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * GNU JAXP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
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

package javax.xml.transform;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.lang.reflect.Method;
import java.util.Properties;


/**
 * Package-private utility methods for sharing
 * magic related to class loading.
 * NOTE:  This is cloned in javax.xml.parsers,
 * where a different exception is thrown (bleech).
 * Keep changes to the two copies in sync.
 *
 * @author David Brownell
 * @version	1.2
 */
final class ClassStuff
{
  
  private ClassStuff()
  {
  }

  /**
   * Get the default factory using the four-stage defaulting
   * mechanism defined by JAXP.
   */
  static Object createFactory(String label, String defaultClass)
    throws TransformerFactoryConfigurationError
  {
    String		name = null;
    ClassLoader	loader = null;

    // figure out which class loader to use.
    // source compiles on jdk 1.1, but works with jdk 1.2+ security
    try
      {
        Method 	m = null;

        // Can we use JDK 1.2 APIs?  Preferred: security policies apply.
        m = Thread.class.getMethod("getContextClassLoader", null);
        loader = (ClassLoader) m.invoke(Thread.currentThread(), null);
      }
    catch (NoSuchMethodException e)
      {
        // Assume that we are running JDK 1.1; use current ClassLoader
        loader = ClassStuff.class.getClassLoader();
      }
    catch (Exception e)
      {
        // "should not happen"
        throw new UnknownError(e.getMessage());
      }

    // 1. Check System Property
    // ... normally fails in applet environments
    try
      {
        name = System.getProperty (label);
      }
    catch (SecurityException e)
      {
        /* IGNORE */
      }

    // 2. Check in $JAVA_HOME/lib/jaxp.properties
    try
      {
        if (name == null)
          {
            String	javaHome;
            File	file;

            javaHome = System.getProperty("java.home");
            file = new File(new File(javaHome, "lib"), "jaxp.properties");
            if (file.exists())
              {
                FileInputStream	in = new FileInputStream(file);
                Properties		props = new Properties();

                props.load(in);
                name  = props.getProperty(label);
                in.close();
              }
          }
      }
    catch (Exception e)
      {
        /* IGNORE */
      }

    // 3. Check Services API
    if (name == null)
      {
        try
          {
            String		service = "META-INF/services/" + label;
            InputStream	in;
            BufferedReader	reader;

            if (loader == null)
              {
                in = ClassLoader.getSystemResourceAsStream(service);
              }
            else
              {
                in = loader.getResourceAsStream(service);
              }
            if (in != null)
              {
                reader = new BufferedReader(new InputStreamReader(in, "UTF8"));
                name = reader.readLine();
                in.close();
              }
          }
        catch (Exception e2)
          {
            /* IGNORE */
          }
      }

    // 4. Distro-specific fallback
    if (name == null)
      {
        name = defaultClass;
      }

    // Instantiate!
    try
      {
        Class	klass;

        if (loader == null)
          {
            klass = Class.forName(name);
          }
        else
          {
            klass = loader.loadClass(name);
          }
        return klass.newInstance();

      }
    catch (ClassNotFoundException e)
      {
        throw new TransformerFactoryConfigurationError (e,
                                                        "Factory class " + name
                                                        + " not found");
      }
    catch (IllegalAccessException e)
      {
        throw new TransformerFactoryConfigurationError (e,
                                                        "Factory class " + name
                                                        + " found but cannot be loaded");
      }
    catch (InstantiationException e)
      {
        throw new TransformerFactoryConfigurationError (e,
                                                        "Factory class " + name
                                                        + " loaded but cannot be instantiated"
                                                        + " ((no default constructor?)");
      }
  }

}
