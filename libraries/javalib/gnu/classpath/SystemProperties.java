/* SystemProperties.java -- Manage the System properties.
   Copyright (C) 2004, 2005 Free Software Foundation

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
	Properties dp = defaultProperties;
	
	//# UTF8
	dp.put("gnu.java.io.encoding_scheme_alias.utf8", "UTF8");
	dp.put("gnu.java.io.encoding_scheme_alias.utf-8", "UTF8");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-1208", "UTF8");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-1209", "UTF8");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-5304", "UTF8");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-5305", "UTF8");
	dp.put("gnu.java.io.encoding_scheme_alias.windows-65001", "UTF8");
	dp.put("gnu.java.io.encoding_scheme_alias.cp1208", "UTF8");

	//# UTF16 Little Endian
	dp.put("gnu.java.io.encoding_scheme_alias.utf16le", "UTF16LE");
	dp.put("gnu.java.io.encoding_scheme_alias.utf-16le", "UTF16LE");
	dp.put("gnu.java.io.encoding_scheme_alias.x-utf-16le", "UTF16LE");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-1202", "UTF16LE");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-13490", "UTF16LE");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-17586", "UTF16LE");
	dp.put("gnu.java.io.encoding_scheme_alias.utf16_littleendian",
	       "UTF16LE");
	dp.put("gnu.java.io.encoding_scheme_alias.windows-1200", "UTF16LE");
	dp.put("gnu.java.io.encoding_scheme_alias.unicodelittleunmarked", "UTF16LE");

	//# UTF16 Big Endian
	dp.put("gnu.java.io.encoding_scheme_alias.utf16be", "UTF16BE");
	dp.put("gnu.java.io.encoding_scheme_alias.utf-16be", "UTF16BE");
	dp.put("gnu.java.io.encoding_scheme_alias.x-utf-16be", "UTF16BE");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-1200", "UTF16BE");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-1201", "UTF16BE");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-5297", "UTF16BE");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-13488", "UTF16BE");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-17584", "UTF16BE");
	dp.put("gnu.java.io.encoding_scheme_alias.windows-1201", "UTF16BE");
	dp.put("gnu.java.io.encoding_scheme_alias.cp1200", "UTF16BE");
	dp.put("gnu.java.io.encoding_scheme_alias.cp1201", "UTF16BE");
	dp.put("gnu.java.io.encoding_scheme_alias.utf16_bigendian", "UTF16BE");
	dp.put("gnu.java.io.encoding_scheme_alias.unicodebigunmarked", "UTF16BE");

	//# UTF16
	dp.put("gnu.java.io.encoding_scheme_alias.utf16", "UTF16BE");
	dp.put("gnu.java.io.encoding_scheme_alias.utf-16", "UTF16BE");
	dp.put("gnu.java.io.encoding_scheme_alias.iso-10646-ucs-2", "UTF16BE");
	dp.put("gnu.java.io.encoding_scheme_alias.unicode", "UTF16BE");
	dp.put("gnu.java.io.encoding_scheme_alias.csunicode", "UTF16BE");
	dp.put("gnu.java.io.encoding_scheme_alias.ucs-2", "UTF16BE");
	//# UTF32 Little Endian
	dp.put("gnu.java.io.encoding_scheme_alias.utf32le", "UTF32LE");
	dp.put("gnu.java.io.encoding_scheme_alias.utf-32le", "UTF32LE");
	dp.put("gnu.java.io.encoding_scheme_alias.utf32_littleendian",
	       "UTF32LE");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-1234", "UTF32LE");

	//# UTF32 Big Endian
	dp.put("gnu.java.io.encoding_scheme_alias.utf32be", "UTF32BE");
	dp.put("gnu.java.io.encoding_scheme_alias.utf-32be", "UTF32BE");
	dp.put("gnu.java.io.encoding_scheme_alias.utf32_bigendian",
	       "UTF32BE");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-1232", "UTF32BE");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-1233", "UTF32BE");

	//# UTF32
	dp.put("gnu.java.io.encoding_scheme_alias.utf32", "UTF32BE");
	dp.put("gnu.java.io.encoding_scheme_alias.utf-32", "UTF32BE");
	dp.put("gnu.java.io.encoding_scheme_alias.iso-10646-ucs-4",
	       "UTF32BE");
	dp.put("gnu.java.io.encoding_scheme_alias.csucs4", "UTF32BE");
	dp.put("gnu.java.io.encoding_scheme_alias.ucs-4", "UTF32BE");

	//# US-ASCII
	dp.put("gnu.java.io.encoding_scheme_alias.ascii", "ASCII");
	dp.put("gnu.java.io.encoding_scheme_alias.us-ascii", "ASCII");
	dp.put("gnu.java.io.encoding_scheme_alias.ansi_x3.4-1968", "ASCII");
	dp.put("gnu.java.io.encoding_scheme_alias.ansi_x3.4-1986", "ASCII");
	dp.put("gnu.java.io.encoding_scheme_alias.iso_646.irv:1991", "ASCII");
	dp.put("gnu.java.io.encoding_scheme_alias.iso_646.irv:1983", "ASCII");
	dp.put("gnu.java.io.encoding_scheme_alias.iso646-us", "ASCII");
	dp.put("gnu.java.io.encoding_scheme_alias.us", "ASCII");
	dp.put("gnu.java.io.encoding_scheme_alias.csascii", "ASCII");
	dp.put("gnu.java.io.encoding_scheme_alias.iso-ir-6", "ASCII");
	dp.put("gnu.java.io.encoding_scheme_alias.cp367", "ASCII");
	dp.put("gnu.java.io.encoding_scheme_alias.ascii7", "ASCII");
	dp.put("gnu.java.io.encoding_scheme_alias.646", "ASCII");
	dp.put("gnu.java.io.encoding_scheme_alias.windows-20127", "ASCII");

	//# iso-8859-1 (latin 1)
	dp.put("gnu.java.io.encoding_scheme_alias.8859_1", "8859_1");
	dp.put("gnu.java.io.encoding_scheme_alias.iso8859_1", "8859_1");
	dp.put("gnu.java.io.encoding_scheme_alias.iso_8859_1", "8859_1");
	dp.put("gnu.java.io.encoding_scheme_alias.iso-8859-1", "8859_1");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-819", "8859_1");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm819", "8859_1");
	dp.put("gnu.java.io.encoding_scheme_alias.cp819", "8859_1");
	dp.put("gnu.java.io.encoding_scheme_alias.latin1", "8859_1");
	dp.put("gnu.java.io.encoding_scheme_alias.latin1", "8859_1");
	dp.put("gnu.java.io.encoding_scheme_alias.csisolatin1", "8859_1");
	dp.put("gnu.java.io.encoding_scheme_alias.iso-ir-100", "8859_1");
	dp.put("gnu.java.io.encoding_scheme_alias.iso_8859-1:1987", "8859_1");
	dp.put("gnu.java.io.encoding_scheme_alias.l1", "8859_1");
	dp.put("gnu.java.io.encoding_scheme_alias.819", "8859_1");

	//# iso-8859-2 (latin 2)
	dp.put("gnu.java.io.encoding_scheme_alias.8859_2", "8859_2");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-912_P100-1995",
	       "8859_2");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-912", "8859_2");
	dp.put("gnu.java.io.encoding_scheme_alias.iso_8859_2", "8859_2");
	dp.put("gnu.java.io.encoding_scheme_alias.iso8859_2", "8859_2");
	dp.put("gnu.java.io.encoding_scheme_alias.iso-8859-2", "8859_2");
	dp.put("gnu.java.io.encoding_scheme_alias.iso_8859-2:1987", "8859_2");
	dp.put("gnu.java.io.encoding_scheme_alias.latin2", "8859_2");
	dp.put("gnu.java.io.encoding_scheme_alias.csisolatin2", "8859_2");
	dp.put("gnu.java.io.encoding_scheme_alias.iso-ir-101", "8859_2");
	dp.put("gnu.java.io.encoding_scheme_alias.l2", "8859_2");
	dp.put("gnu.java.io.encoding_scheme_alias.8859_2", "8859_2");
	dp.put("gnu.java.io.encoding_scheme_alias.cp912", "8859_2");
	dp.put("gnu.java.io.encoding_scheme_alias.912", "8859_2");
	dp.put("gnu.java.io.encoding_scheme_alias.windows-28592", "8859_2");

	//# iso-8859-3 (latin 3)
	dp.put("gnu.java.io.encoding_scheme_alias.8859_3", "8859_3");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-913_P100-2000",
	       "8859_3");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-913", "8859_3");
	dp.put("gnu.java.io.encoding_scheme_alias.iso_8859_3", "8859_3");
	dp.put("gnu.java.io.encoding_scheme_alias.iso8859_3", "8859_3");
	dp.put("gnu.java.io.encoding_scheme_alias.iso-8859-3", "8859_3");
	dp.put("gnu.java.io.encoding_scheme_alias.iso_8859-3:1988", "8859_3");
	dp.put("gnu.java.io.encoding_scheme_alias.latin3", "8859_3");
	dp.put("gnu.java.io.encoding_scheme_alias.csisolatin3", "8859_3");
	dp.put("gnu.java.io.encoding_scheme_alias.iso-ir-109", "8859_3");
	dp.put("gnu.java.io.encoding_scheme_alias.l3", "8859_3");
	dp.put("gnu.java.io.encoding_scheme_alias.cp913", "8859_3");
	dp.put("gnu.java.io.encoding_scheme_alias.913", "8859_3");
	dp.put("gnu.java.io.encoding_scheme_alias.windows-28593", "8859_3");

	//# iso-8859-4 (latin 4)
	dp.put("gnu.java.io.encoding_scheme_alias.8859_4", "8859_4");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-914_P100-1995",
	       "8859_4");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-914", "8859_4");
	dp.put("gnu.java.io.encoding_scheme_alias.iso_8859_4", "8859_4");
	dp.put("gnu.java.io.encoding_scheme_alias.iso8859_4", "8859_4");
	dp.put("gnu.java.io.encoding_scheme_alias.iso-8859-4", "8859_4");
	dp.put("gnu.java.io.encoding_scheme_alias.latin4", "8859_4");
	dp.put("gnu.java.io.encoding_scheme_alias.csisolatin4", "8859_4");
	dp.put("gnu.java.io.encoding_scheme_alias.iso-ir-110", "8859_4");
	dp.put("gnu.java.io.encoding_scheme_alias.iso_8859-4:1988", "8859_4");
	dp.put("gnu.java.io.encoding_scheme_alias.l4", "8859_4");
	dp.put("gnu.java.io.encoding_scheme_alias.cp914", "8859_4");
	dp.put("gnu.java.io.encoding_scheme_alias.914", "8859_4");
	dp.put("gnu.java.io.encoding_scheme_alias.windows-28594", "8859_4");

	//# iso-8859-5 (cyrillic)
	dp.put("gnu.java.io.encoding_scheme_alias.8859_5", "8859_5");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-915_P100-1995",
	       "8859_5");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-915", "8859_5");
	dp.put("gnu.java.io.encoding_scheme_alias.iso_8859_5", "8859_5");
	dp.put("gnu.java.io.encoding_scheme_alias.iso8859_5", "8859_5");
	dp.put("gnu.java.io.encoding_scheme_alias.iso-8859-5", "8859_5");
	dp.put("gnu.java.io.encoding_scheme_alias.cyrillic", "8859_5");
	dp.put("gnu.java.io.encoding_scheme_alias.csisolatincyrillic",
	       "8859_5");
	dp.put("gnu.java.io.encoding_scheme_alias.iso-ir-144", "8859_5");
	dp.put("gnu.java.io.encoding_scheme_alias.iso_8859-5:1988", "8859_5");
	dp.put("gnu.java.io.encoding_scheme_alias.cp915", "8859_5");
	dp.put("gnu.java.io.encoding_scheme_alias.915", "8859_5");
	dp.put("gnu.java.io.encoding_scheme_alias.windows-28595", "8859_5");

	//# iso-8859-6 (arabic)
	dp.put("gnu.java.io.encoding_scheme_alias.8859_6", "8859_6");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-1089_P100-1995",
	       "8859_6");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-1089", "8859_6");
	dp.put("gnu.java.io.encoding_scheme_alias.iso_8859_6", "8859_6");
	dp.put("gnu.java.io.encoding_scheme_alias.iso8859_6", "8859_6");
	dp.put("gnu.java.io.encoding_scheme_alias.iso-8859-6", "8859_6");
	dp.put("gnu.java.io.encoding_scheme_alias.arabic", "8859_6");
	dp.put("gnu.java.io.encoding_scheme_alias.csisolatinarabic", "8859_6");
	dp.put("gnu.java.io.encoding_scheme_alias.iso-ir-127", "8859_6");
	dp.put("gnu.java.io.encoding_scheme_alias.iso_8859-6:1987", "8859_6");
	dp.put("gnu.java.io.encoding_scheme_alias.ecma-114", "8859_6");
	dp.put("gnu.java.io.encoding_scheme_alias.asmo-708", "8859_6");
	dp.put("gnu.java.io.encoding_scheme_alias.8859_6", "8859_6");
	dp.put("gnu.java.io.encoding_scheme_alias.cp1089", "8859_6");
	dp.put("gnu.java.io.encoding_scheme_alias.1089", "8859_6");
	dp.put("gnu.java.io.encoding_scheme_alias.windows-28596", "8859_6");
	dp.put("gnu.java.io.encoding_scheme_alias.iso-8859-6-i", "8859_6");
	dp.put("gnu.java.io.encoding_scheme_alias.iso-8859-6-e", "8859_6");

	//# iso-8859-7 (greek)
	dp.put("gnu.java.io.encoding_scheme_alias.8859_7", "8859_7");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-813_P100-1995",
	       "8859_7");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-813", "8859_7");
	dp.put("gnu.java.io.encoding_scheme_alias.iso_8859_7", "8859_7");
	dp.put("gnu.java.io.encoding_scheme_alias.iso8859_7", "8859_7");
	dp.put("gnu.java.io.encoding_scheme_alias.iso-8859-7", "8859_7");
	dp.put("gnu.java.io.encoding_scheme_alias.greek", "8859_7");
	dp.put("gnu.java.io.encoding_scheme_alias.greek8", "8859_7");
	dp.put("gnu.java.io.encoding_scheme_alias.elot_928", "8859_7");
	dp.put("gnu.java.io.encoding_scheme_alias.ecma-118", "8859_7");
	dp.put("gnu.java.io.encoding_scheme_alias.csisolatingreek", "8859_7");
	dp.put("gnu.java.io.encoding_scheme_alias.iso-ir-126", "8859_7");
	dp.put("gnu.java.io.encoding_scheme_alias.iso_8859-7:1987", "8859_7");
	dp.put("gnu.java.io.encoding_scheme_alias.cp813", "8859_7");
	dp.put("gnu.java.io.encoding_scheme_alias.813", "8859_7");
	dp.put("gnu.java.io.encoding_scheme_alias.windows-28597", "8859_7");

	//# iso-8859-8 (hebrew)
	dp.put("gnu.java.io.encoding_scheme_alias.8859_8", "8859_8");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-916_P100-1995",
	       "8859_8");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-916", "8859_8");
	dp.put("gnu.java.io.encoding_scheme_alias.iso_8859_8", "8859_8");
	dp.put("gnu.java.io.encoding_scheme_alias.iso8859_8", "8859_8");
	dp.put("gnu.java.io.encoding_scheme_alias.iso-8859-8", "8859_8");
	dp.put("gnu.java.io.encoding_scheme_alias.hebrew", "8859_8");
	dp.put("gnu.java.io.encoding_scheme_alias.csisolatinhebrew", "8859_8");
	dp.put("gnu.java.io.encoding_scheme_alias.iso-ir-138", "8859_8");
	dp.put("gnu.java.io.encoding_scheme_alias.iso_8859-8:1988", "8859_8");
	dp.put("gnu.java.io.encoding_scheme_alias.iso-8859-8-i", "8859_8");
	dp.put("gnu.java.io.encoding_scheme_alias.iso-8859-8-e", "8859_8");
	dp.put("gnu.java.io.encoding_scheme_alias.cp916", "8859_8");
	dp.put("gnu.java.io.encoding_scheme_alias.916", "8859_8");
	dp.put("gnu.java.io.encoding_scheme_alias.windows-28598", "8859_8");

	//# iso-8859-9 (latin-5)
	dp.put("gnu.java.io.encoding_scheme_alias.8859_9", "8859_9");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-920_P100-1995",
	       "8859_9");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-920", "8859_9");
	dp.put("gnu.java.io.encoding_scheme_alias.iso8859_9", "8859_9");
	dp.put("gnu.java.io.encoding_scheme_alias.iso-8859-9", "8859_9");
	dp.put("gnu.java.io.encoding_scheme_alias.iso_8859_9", "8859_9");
	dp.put("gnu.java.io.encoding_scheme_alias.latin5", "8859_9");
	dp.put("gnu.java.io.encoding_scheme_alias.csisolatin5", "8859_9");
	dp.put("gnu.java.io.encoding_scheme_alias.iso-ir-148", "8859_9");
	dp.put("gnu.java.io.encoding_scheme_alias.iso_8859-9:1989", "8859_9");
	dp.put("gnu.java.io.encoding_scheme_alias.l5", "8859_9");
	dp.put("gnu.java.io.encoding_scheme_alias.cp920", "8859_9");
	dp.put("gnu.java.io.encoding_scheme_alias.920", "8859_9");
	dp.put("gnu.java.io.encoding_scheme_alias.windows-28599", "8859_9");
	dp.put("gnu.java.io.encoding_scheme_alias.ecma-128", "8859_9");

	//# iso-8859-13 
	dp.put("gnu.java.io.encoding_scheme_alias.8859_13", "8859_13");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-921_P100-1995",
	       "8859_13");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-921", "8859_13");
	dp.put("gnu.java.io.encoding_scheme_alias.iso_8859_13", "8859_13");
	dp.put("gnu.java.io.encoding_scheme_alias.iso8859_13", "8859_13");
	dp.put("gnu.java.io.encoding_scheme_alias.iso-8859-13", "8859_13");
	dp.put("gnu.java.io.encoding_scheme_alias.8859_13", "8859_13");
	dp.put("gnu.java.io.encoding_scheme_alias.cp921", "8859_13");
	dp.put("gnu.java.io.encoding_scheme_alias.921", "8859_13");

	//# iso-8859-15 (latin-9)
	dp.put("gnu.java.io.encoding_scheme_alias.8859_15", "8859_15");
	dp.put("gnu.java.io.encoding_scheme_alias.iso8859_15", "8859_15");
	dp.put("gnu.java.io.encoding_scheme_alias.iso-8859-15", "8859_15");
	dp.put("gnu.java.io.encoding_scheme_alias.8859-15", "8859_15");
	dp.put("gnu.java.io.encoding_scheme_alias.latin9", "8859_15");
	dp.put("gnu.java.io.encoding_scheme_alias.iso_8859_15", "8859_15");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-923_P100-1998",
	       "8859_15");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-923", "8859_15");
	dp.put("gnu.java.io.encoding_scheme_alias.latin-9", "8859_15");
	dp.put("gnu.java.io.encoding_scheme_alias.l9", "8859_15");
	dp.put("gnu.java.io.encoding_scheme_alias.latin0", "8859_15");
	dp.put("gnu.java.io.encoding_scheme_alias.csisolatin0", "8859_15");
	dp.put("gnu.java.io.encoding_scheme_alias.csisolatin9", "8859_15");
	dp.put("gnu.java.io.encoding_scheme_alias.iso8859_15_fdis", "8859_15");
	dp.put("gnu.java.io.encoding_scheme_alias.cp923", "8859_15");
	dp.put("gnu.java.io.encoding_scheme_alias.923", "8859_15");
	dp.put("gnu.java.io.encoding_scheme_alias.windows-28605", "8859_15");

	//# Windows-1252 (cp-1252)
	dp.put("gnu.java.io.encoding_scheme_alias.windows1252", "Windows1252");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-5348_P100-1997",
	       "Windows1252");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-5348", "Windows1252");
	dp.put("gnu.java.io.encoding_scheme_alias.windows-1252",
	       "Windows1252");
	dp.put("gnu.java.io.encoding_scheme_alias.cp1252", "Windows1252");
	dp.put("gnu.java.io.encoding_scheme_alias.cp-1252", "Windows1252");

	//# Windows-1250 (cp-1250)
	dp.put("gnu.java.io.encoding_scheme_alias.windows1250", "Windows1250");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-5346_P100-1998",
	       "Windows1250");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-5346", "Windows1250");
	dp.put("gnu.java.io.encoding_scheme_alias.windows-1250",
	       "Windows1250");
	dp.put("gnu.java.io.encoding_scheme_alias.cp1250", "Windows1250");
	dp.put("gnu.java.io.encoding_scheme_alias.cp-1250", "Windows1250");

	//# ebcdic-xml-us
	dp.put("gnu.java.io.encoding_scheme_alias.ebcdic_xml_us",
	       "EBCDIC_XML_US");
	dp.put("gnu.java.io.encoding_scheme_alias.ebcdic", "EBCDIC_XML_US");
	dp.put("gnu.java.io.encoding_scheme_alias.ebcdic-xml-us",
	       "EBCDIC_XML_US");

	//# ebcdic latin 1 (cp1047)
	dp.put("gnu.java.io.encoding_scheme_alias.cp1047", "Cp1047");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-1047_P100-1995",
	       "Cp1047");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm-1047", "Cp1047");
	dp.put("gnu.java.io.encoding_scheme_alias.ibm1047", "Cp1047");
	dp.put("gnu.java.io.encoding_scheme_alias.cpibm1047", "Cp1047");
	dp.put("gnu.java.io.encoding_scheme_alias.cp-1047", "Cp1047");
	dp.put("gnu.java.io.encoding_scheme_alias.ebcdic1047", "Cp1047");
	dp.put("gnu.java.io.encoding_scheme_alias.ebcdic-1047", "Cp1047");
	
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
