/* SystemProperties.java -- Manage the System properties.
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
Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
02111-1307 USA.

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

import java.util.Properties;

/**
 * The class manages the System properties. This class is only available to
 * privileged code (i.e. code loaded by the bootstrap class loader) and
 * therefore doesn't do any security checks.
 * This class is separated out from java.lang.System to simplify bootstrap
 * dependencies and to allow trusted code a simple and efficient mechanism
 * to access the system properties.
 */
public class SystemProperties
{
    /**
     * Stores the current system properties. This can be modified by
     * {@link #setProperties(Properties)}, but will never be null, because
     * setProperties(null) sucks in the default properties.
     */
    private static Properties properties;

    /**
     * The default properties. Once the default is stabilized,
     * it should not be modified;
     * instead it is cloned when calling <code>setProperties(null)</code>.
     */
    private static final Properties defaultProperties = new Properties();

    static
    {
        VMSystemProperties.preInit(defaultProperties);

        defaultProperties.put("gnu.classpath.home",
            Configuration.CLASSPATH_HOME);
        defaultProperties.put("gnu.classpath.version",
            Configuration.CLASSPATH_VERSION);

        // Set base URL if not already set.
        if (defaultProperties.get("gnu.classpath.home.url") == null)
            defaultProperties.put("gnu.classpath.home.url",
                "file://" + Configuration.CLASSPATH_HOME + "/lib");

        // Set short name if not already set.
        if (defaultProperties.get("gnu.classpath.vm.shortname") == null)
        {
            String value = defaultProperties.getProperty("java.vm.name");
            int index = value.lastIndexOf(' ');
            if (index != -1)
                value = value.substring(index + 1);
            defaultProperties.put("gnu.classpath.vm.shortname", value);
        }

        // Network properties
        if (defaultProperties.get("http.agent") == null)
        {
            String userAgent
                = ("gnu-classpath/"
                + defaultProperties.getProperty("gnu.classpath.version")
                + " ("
                + defaultProperties.getProperty("gnu.classpath.vm.shortname")
                + "/"
                + defaultProperties.getProperty("java.vm.version")
                + ")");
            defaultProperties.put("http.agent", userAgent);
        }

        // Common encoding aliases. See gnu.java.io.EncodingManager.
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.ISO8859_1",
            "8859_1");

        defaultProperties.put("gnu.java.io.encoding_scheme_alias.ISO-8859-1",
            "8859_1");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.ISO-8859-2",
            "8859_2");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.ISO-8859-3",
            "8859_3");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.ISO-8859-4",
            "8859_4");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.ISO-8859-5",
            "8859_5");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.ISO-8859-6",
            "8859_6");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.ISO-8859-7",
            "8859_7");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.ISO-8859-8",
            "8859_8");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.ISO-8859-9",
            "8859_9");

        defaultProperties.put("gnu.java.io.encoding_scheme_alias.iso-8859-1",
            "8859_1");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.iso-8859-2",
            "8859_2");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.iso-8859-3",
            "8859_3");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.iso-8859-4",
            "8859_4");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.iso-8859-5",
            "8859_5");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.iso-8859-6",
            "8859_6");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.iso-8859-7",
            "8859_7");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.iso-8859-8",
            "8859_8");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.iso-8859-9",
            "8859_9");

        defaultProperties.put("gnu.java.io.encoding_scheme_alias.iso8859_1",
            "8859_1");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.iso8859_2",
            "8859_2");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.iso8859_3",
            "8859_3");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.iso8859_4",
            "8859_4");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.iso8859_5",
            "8859_5");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.iso8859_6",
            "8859_6");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.iso8859_7",
            "8859_7");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.iso8859_8",
            "8859_8");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.iso8859_9",
            "8859_9");

        defaultProperties.put("gnu.java.io.encoding_scheme_alias.iso-latin-1",
            "8859_1");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.iso-latin-2",
            "8859_2");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.iso-latin-3",
            "8859_3");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.iso-latin-4",
            "8859_4");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.iso-latin-5",
            "8859_5");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.iso-latin-6",
            "8859_6");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.iso-latin-7",
            "8859_7");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.iso-latin-8",
            "8859_8");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.iso-latin-9",
            "8859_9");

        defaultProperties.put("gnu.java.io.encoding_scheme_alias.latin1",
            "8859_1");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.latin2",
            "8859_2");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.latin3",
            "8859_3");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.latin4",
            "8859_4");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.latin5",
            "8859_5");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.latin6",
            "8859_6");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.latin7",
            "8859_7");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.latin8",
            "8859_8");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.latin9",
            "8859_9");

        defaultProperties.put("gnu.java.io.encoding_scheme_alias.UTF-8", "UTF8");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.utf-8", "UTF8");

        // XXX FIXME - Cheat a little for ASCII.
        // Remove when we get a real "ASCII En/Decoder"
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.ASCII", "8859_1");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.ascii", "8859_1");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.US-ASCII",
            "8859_1");
        defaultProperties.put("gnu.java.io.encoding_scheme_alias.us-ascii",
            "8859_1");

        // 8859_1 is a safe default encoding to use when not explicitly set
        if (defaultProperties.get("file.encoding") == null)
            defaultProperties.put("file.encoding", "8859_1");

        // XXX FIXME - Temp hack for old systems that set the wrong property
        if (defaultProperties.get("java.io.tmpdir") == null)
            defaultProperties.put("java.io.tmpdir",
                defaultProperties.get("java.tmpdir"));

        VMSystemProperties.postInit(defaultProperties);

        // Note that we use clone here and not new.  Some programs assume
        // that the system properties do not have a parent.
        properties = (Properties)defaultProperties.clone();
    }

    public static String getProperty(String name)
    {
        return properties.getProperty(name);
    }

    public static String getProperty(String name, String defaultValue)
    {
        return properties.getProperty(name, defaultValue);
    }

    public static String setProperty(String name, String value)
    {
        return (String)properties.setProperty(name, value);
    }

    public static Properties getProperties()
    {
        return properties;
    }

    public static void setProperties(Properties properties)
    {
        if (properties == null)
          {
            // Note that we use clone here and not new.  Some programs
            // assume that the system properties do not have a parent.
            properties = (Properties)defaultProperties.clone();
          }

        SystemProperties.properties = properties;
    }
}
