/* VMSystemProperties.java -- Allow the VM to set System properties.
   Copyright (C) 2004 Free Software Foundation

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

package gnu.classpath;

import java.net.URL;
import java.util.Properties;

class VMSystemProperties
{
    /**
     * Get the system properties. This is done here, instead of in System,
     * because of the bootstrap sequence. Note that the native code should
     * not try to use the Java I/O classes yet, as they rely on the properties
     * already existing. The only safe method to use to insert these default
     * system properties is {@link Properties#setProperty(String, String)}.
     *
     * <p>These properties MUST include:
     * <dl>
     * <dt>java.version         <dd>Java version number
     * <dt>java.vendor          <dd>Java vendor specific string
     * <dt>java.vendor.url      <dd>Java vendor URL
     * <dt>java.home            <dd>Java installation directory
     * <dt>java.vm.specification.version <dd>VM Spec version
     * <dt>java.vm.specification.vendor  <dd>VM Spec vendor
     * <dt>java.vm.specification.name    <dd>VM Spec name
     * <dt>java.vm.version      <dd>VM implementation version
     * <dt>java.vm.vendor       <dd>VM implementation vendor
     * <dt>java.vm.name         <dd>VM implementation name
     * <dt>java.specification.version    <dd>Java Runtime Environment version
     * <dt>java.specification.vendor     <dd>Java Runtime Environment vendor
     * <dt>java.specification.name       <dd>Java Runtime Environment name
     * <dt>java.class.version   <dd>Java class version number
     * <dt>java.class.path      <dd>Java classpath
     * <dt>java.library.path    <dd>Path for finding Java libraries
     * <dt>java.io.tmpdir       <dd>Default temp file path
     * <dt>java.compiler        <dd>Name of JIT to use
     * <dt>java.ext.dirs        <dd>Java extension path
     * <dt>os.name              <dd>Operating System Name
     * <dt>os.arch              <dd>Operating System Architecture
     * <dt>os.version           <dd>Operating System Version
     * <dt>file.separator       <dd>File separator ("/" on Unix)
     * <dt>path.separator       <dd>Path separator (":" on Unix)
     * <dt>line.separator       <dd>Line separator ("\n" on Unix)
     * <dt>user.name            <dd>User account name
     * <dt>user.home            <dd>User home directory
     * <dt>user.dir             <dd>User's current working directory
     * <dt>gnu.cpu.endian       <dd>"big" or "little"
     * </dl>
     *
     * @param p the Properties object to insert the system properties into
     */
    static void preInit(Properties properties) 
    {

      // Set the static, os-independant properties first.

      properties.setProperty("java.version", "1.4.2");
      properties.setProperty("java.vendor", "Kaffe.org project");
      properties.setProperty("java.vendor.url", "http://www.kaffe.org");
      properties.setProperty("java.vendor.url.bug", "http://www.kaffe.org");
      properties.setProperty("java.vm.specification.version", "1.0");
      properties.setProperty("java.vm.specification.vendor", "Sun Microsystems Inc.");
      properties.setProperty("java.vm.specification.name", "Java Virtual Machine Specification");
      properties.setProperty("java.vm.version", "1.1.6");
      properties.setProperty("java.vm.vendor", "Kaffe.org project");
      properties.setProperty("java.vm.name", "Kaffe");
      properties.setProperty("java.specification.version", "1.4");
      properties.setProperty("java.specification.vendor", "Sun Microsystems Inc.");
      properties.setProperty("java.specification.name", "Java Platform API Specification");
      properties.setProperty("java.class.version", "48.0");
      properties.setProperty("gnu.classpath.vm.shortname", "Kaffe");
      properties.setProperty("java.library.path", "");
      properties.setProperty("java.ext.dirs", "");

      // Undocumented properties used by some applications
      properties.setProperty("java.runtime.name", properties.getProperty("java.vm.name"));
      properties.setProperty("java.runtime.version", properties.getProperty("java.vm.version"));

      /* Define the default java compiler - this should be configured
       * somehow.
       */
      properties.setProperty("kaffe.compiler", "jikes");
      properties.setProperty("build.compiler", "jikes");
      properties.setProperty("build.rmic", "kaffe");

      // Invoke native methods to get the values of the other properties

      final String JAVA_HOME = getJavaHome();
      properties.setProperty("java.home", JAVA_HOME);

      final String JAVA_CLASS_PATH = getJavaClassPath();
      properties.setProperty("java.class.path", JAVA_CLASS_PATH);

      final String JAVA_IO_TMPDIR = getJavaIoTmpdir();
      properties.setProperty("java.io.tmpdir", JAVA_IO_TMPDIR);

      final String JAVA_COMPILER = getJavaCompiler();
      properties.setProperty("java.compiler", JAVA_COMPILER);

      final String OS_NAME = getOsName();
      properties.setProperty("os.name", OS_NAME);

      String OS_ARCH = getOsArch();

      // Unify x86 OS_ARCH for jogl
      if (OS_ARCH.startsWith("i") && OS_ARCH.endsWith("86"))
        OS_ARCH = "i386";

      properties.setProperty("os.arch", OS_ARCH);

      final String OS_VERSION = getOsVersion();
      properties.setProperty("os.version", OS_VERSION);

      final String FILE_SEPARATOR = getFileSeparator();
      properties.setProperty("file.separator", FILE_SEPARATOR);

      final String PATH_SEPARATOR = getPathSeparator();
      properties.setProperty("path.separator", PATH_SEPARATOR);

      final String LINE_SEPARATOR = getLineSeparator();
      properties.setProperty("line.separator", LINE_SEPARATOR);

      final String USER_NAME = getUserName();
      properties.setProperty("user.name", USER_NAME);

      final String USER_HOME = getUserHome();
      properties.setProperty("user.home", USER_HOME);

      final String USER_DIR = getUserDir();
      properties.setProperty("user.dir", USER_DIR);

      final String GNU_CPU_ENDIAN = getGnuCpuEndian();
      properties.setProperty("gnu.cpu.endian", GNU_CPU_ENDIAN);

      final String KAFFE_LIBRARY_PATH = getKaffeLibraryPath();
      properties.setProperty("gnu.classpath.boot.library.path", KAFFE_LIBRARY_PATH);

      final String SUN_BOOT_CLASS_PATH = getSunBootClassPath();
      properties.setProperty("sun.boot.class.path", SUN_BOOT_CLASS_PATH);

      final String LOCALE = getLocale();
      if (LOCALE != null && LOCALE.length() > 2)
	{
	  properties.setProperty("user.language", LOCALE.substring(0, 2));
	  if (LOCALE.charAt(2) == '_')
	    {
	      properties.setProperty("user.region", LOCALE.substring(3, 5));
	    }
	}
      else 
	{
	  // if no locale set, use en_US
	  properties.setProperty("user.language", "en");
	  properties.setProperty("user.region", "US");
	}

      // Add GNU Classpath specific properties
      properties.put("gnu.classpath.home",
		     properties.get("java.home"));
      properties.put("gnu.classpath.version",
		     Configuration.CLASSPATH_VERSION);

      // Set base URL if not already set.
      if (properties.get("gnu.classpath.home.url") == null)
	properties.put("gnu.classpath.home.url",
		       "file://" + properties.get("gnu.classpath.home") + "/jre/lib");
    }

    /**
     * Here you get a chance to overwrite some of the properties set by
     * the common SystemProperties code. For example, it might be
     * a good idea to process the properties specified on the command
     * line here.
     */
    static native void postInit(Properties properties);


  static native String getLocale();
  static native String getKaffeLibraryPath();
  static native String getSunBootClassPath();
  static native String getJavaClassPath();
  static native String getJavaHome();
  static native String getJavaIoTmpdir();
  static native String getJavaCompiler();
  static native String getOsName();
  static native String getOsArch();
  static native String getOsVersion();
  static native String getFileSeparator();
  static native String getPathSeparator();
  static native String getLineSeparator();
  static native String getUserName();
  static native String getUserHome();
  static native String getUserDir();
  static native String getGnuCpuEndian();
}
