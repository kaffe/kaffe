/* VMClassLoader.java -- Reference implementation of native interface
   required by ClassLoader
   Copyright (C) 1998, 2001, 2002, 2004 Free Software Foundation

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

package java.lang;

import java.security.AllPermission;
import java.security.CodeSource;
import java.security.Permissions;
import java.security.ProtectionDomain;
import java.net.URL;
import java.io.IOException;
import java.io.File;
import java.util.Enumeration;
import java.util.Map;
import java.util.HashMap;
import java.util.StringTokenizer;
import java.util.Vector;
import java.util.zip.ZipFile;
import java.util.zip.ZipEntry;
import java.lang.reflect.Constructor;

import gnu.java.util.EmptyEnumeration;
import gnu.classpath.SystemProperties;


/**
 * java.lang.VMClassLoader is a package-private helper for VMs to implement
 * on behalf of java.lang.ClassLoader.
 *
 * @author John Keiser
 * @author Mark Wielaard <mark@klomp.org>
 * @author Eric Blake <ebb9@email.byu.edu>
 */
final class VMClassLoader
{
   private static final Map bootjars = new HashMap();

  /**
   * Helper to define a class using a string of bytes. This assumes that
   * the security checks have already been performed, if necessary.
   *
   * <strong>For backward compatibility, this just ignores the protection
   * domain; that is the wrong behavior, and you should directly implement
   * this method natively if you can.</strong>
   *
   * Implementations of this method are advised to consider the
   * situation where user code modifies the byte array after it has
   * been passed to defineClass.  This can be handled by making a
   * private copy of the array, or arranging to only read any given
   * byte a single time.
   *
   * @param name the name to give the class, or null if unknown
   * @param data the data representing the classfile, in classfile format
   * @param offset the offset into the data where the classfile starts
   * @param len the length of the classfile data in the array
   * @param pd the protection domain
   * @return the class that was defined
   * @throws ClassFormatError if data is not in proper classfile format
   */
  static final native Class defineClass(ClassLoader cl, String name,
					byte[] data, int offset, int len,
					ProtectionDomain pd)
    throws ClassFormatError;

  /**
   * Helper to resolve all references to other classes from this class.
   *
   * @param c the class to resolve
   */
  static final native void resolveClass(Class c);

  /**
   * Helper to load a class from the bootstrap class loader.
   *
   * @param name the class name to load
   * @param resolve whether to resolve it
   * @return the class, loaded by the bootstrap classloader or null
   * if the class wasn't found. Returning null is equivalent to throwing
   * a ClassNotFoundException (but a possible performance optimization).
   */
   static native Class loadClass(String name, boolean resolve)
      throws ClassNotFoundException;


   private static URL nextResource (StringTokenizer path, String name)
   {
      while (path.hasMoreTokens())
      {
         File file = new File(path.nextToken());

         if (!file.exists()) {
            continue;
         }
         else if (file.isDirectory()) {
            file = new File(file, name);
            if (file.isFile()) {
               try {
                  return new URL("file", "", file.getCanonicalPath().replace(File.separatorChar, '/'));
               } catch (IOException e) {
               }
            }
         }
         else if (file.isFile()) {
            ZipFile zip = (ZipFile) bootjars.get(file.getName());
            try {
               if (zip == null) {
                  zip = new ZipFile(file);
                  bootjars.put(file.getName(), zip);
               }
               ZipEntry entry = zip.getEntry(name);
               if (entry != null && !entry.isDirectory()) {
                  return new URL("jar:file:"
                                 + file.getCanonicalPath().replace(File.separatorChar, '/') + "!/" + entry.getName());
               }
            } catch (IOException e) {
            }
         }
      }

      return null;
   }

  /**
   * Helper to load a resource from the bootstrap class loader.
   *
   * @param name the resource to find
   * @return the URL to the resource
   */
  static URL getResource(String name)
  {
     String classpath = gnu.classpath.SystemProperties.getProperties().getProperty("sun.boot.class.path");

     StringTokenizer path = new StringTokenizer(classpath, File.pathSeparator);

     if (name.startsWith("/")) {
        name = name.substring(1);
     }

     return nextResource (path, name);
  }

  /**
   * Helper to get a list of resources from the bootstrap class loader.
   *
   * @param name the resource to find
   * @return an enumeration of resources
   * @throws IOException if one occurs
   */
  static Enumeration getResources(String name) throws IOException
  {
     String classpath = gnu.classpath.SystemProperties.getProperties().getProperty("sun.boot.class.path");

     StringTokenizer path = new StringTokenizer(classpath, File.pathSeparator);

     if (name.startsWith("/")) {
        name = name.substring(1);
     }

     Vector ret = new Vector ();

     for (;;) {
        URL url = nextResource (path, name);

        if (url == null)
           break;

        ret.add (url);
     }

     return ret.elements ();
  }

  /**
   * Helper to get a package from the bootstrap class loader.  The default
   * implementation of returning null may be adequate, or you may decide
   * that this needs some native help.
   *
   * @param name the name to find
   * @return the named package, if it exists
   */
  static Package getPackage(String name)
  {
     return null;
  }

  /**
   * Helper to get all packages from the bootstrap class loader.  The default
   * implementation of returning an empty array may be adequate, or you may
   * decide that this needs some native help.
   *
   * @return all named packages, if any exist
   */
  static Package[] getPackages()
  {
     return new Package[0];
  }

  /**
   * Helper for java.lang.Integer, Byte, etc to get the TYPE class
   * at initialization time. The type code is one of the chars that
   * represents the primitive type as in JNI.
   *
   * <ul>
   * <li>'Z' - boolean</li>
   * <li>'B' - byte</li>
   * <li>'C' - char</li>
   * <li>'D' - double</li>
   * <li>'F' - float</li>
   * <li>'I' - int</li>
   * <li>'J' - long</li>
   * <li>'S' - short</li>
   * <li>'V' - void</li>
   * </ul>
   *
   * Note that this is currently a java version that converts the type code
   * to a string and calls the native <code>getPrimitiveClass(String)</code>
   * method for backwards compatibility with VMs that used old versions of
   * GNU Classpath. Please replace this method with a native method
   * <code>final static native Class getPrimitiveClass(char type);</code>
   * if your VM supports it. <strong>The java version of this method and
   * the String version of this method will disappear in a future version
   * of GNU Classpath</strong>.
   *
   * @param type the primitive type
   * @return a "bogus" class representing the primitive type
   */
  static final Class getPrimitiveClass(char type)
  {
    Class primitive = getPrimitiveClass0(type);
    if (primitive == null)
	throw new NoClassDefFoundError("Invalid type specifier: " + type);
    return primitive;
  }


  /**
   * Helper for java.lang.Integer, Byte, etc to get the TYPE class
   * at initialization time. The type code is one of the chars that
   * represents the primitive type as in JNI.
   *
   * <ul>
   * <li>'Z' - boolean</li>
   * <li>'B' - byte</li>
   * <li>'C' - char</li>
   * <li>'D' - double</li>
   * <li>'F' - float</li>
   * <li>'I' - int</li>
   * <li>'J' - long</li>
   * <li>'S' - short</li>
   * <li>'V' - void</li>
   * </ul>
   *
   * @param type the primitive type
   * @return a "bogus" class representing the primitive type, or null
   *
   */
  native static final Class getPrimitiveClass0(char type);

  /**
   * The system default for assertion status. This is used for all system
   * classes (those with a null ClassLoader), as well as the initial value for
   * every ClassLoader's default assertion status.
   *
   * XXX - Not implemented yet; this requires native help.
   *
   * @return the system-wide default assertion status
   */
  static final boolean defaultAssertionStatus()
  {
    return true;
  }

  /**
   * The system default for package assertion status. This is used for all
   * ClassLoader's packageAssertionStatus defaults. It must be a map of
   * package names to Boolean.TRUE or Boolean.FALSE, with the unnamed package
   * represented as a null key.
   *
   * XXX - Not implemented yet; this requires native help.
   *
   * @return a (read-only) map for the default packageAssertionStatus
   */
  static final Map packageAssertionStatus()
  {
    return new HashMap();
  }

  /**
   * The system default for class assertion status. This is used for all
   * ClassLoader's classAssertionStatus defaults. It must be a map of
   * class names to Boolean.TRUE or Boolean.FALSE
   *
   * XXX - Not implemented yet; this requires native help.
   *
   * @return a (read-only) map for the default classAssertionStatus
   */
  static final Map classAssertionStatus()
  {
    return new HashMap();
  }

  static ClassLoader getSystemClassLoader ()
  {
    return ClassLoader.defaultGetSystemClassLoader ();
  }

  /**
   * Set this field to true if the VM wants to keep its own cache.
   */
  static final boolean USE_VM_CACHE = false;

  /**
   * If the VM wants to keep its own cache, this method can be replaced.
   */
  static Class findLoadedClass(ClassLoader cl, String name)
  {
    return (Class) cl.loadedClasses.get(name);
  }
}
