/*
 * Java core library component.
 *
 * Copyright (c) 2005
 *	Ito Kazumitsu <kaz@maczuka.gcd.org>
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.io;

import java.nio.charset.spi.CharsetProvider;
import java.nio.charset.Charset;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;

/**
 * Charset provider wrapping kaffe.io.{ByteToChar|CharToByte}Converter.
 * In order that this class is used, a file named
 * java.nio.charset.spi.CharsetProvider
 * must exist in the resource directory META-INF/services, and
 * it must contain a line that reads "kaffe.io.KaffeCharsetProvider".
 *
 * @author Ito Kazumitsu
 * @see Charset
 */
public final class KaffeCharsetProvider extends CharsetProvider
{

  /**
    * Map from canonical name (in upper case) to objects whose contents are:
    * [0]: cached instance of the charset,
    * [1]: name used to get kaffe.io.{ByteToChar|CharToByte}Converter
    * [2]: an array of aliases and
    * [3]: an array of float values used for creating a new Charset instance,
    * averageCharsPerByte, maxCharsPerByte, averageBytesPerChar and
    * maxBytesPerChar.
    */
  private HashMap charsets;
  private HashMap canonicalNames;

  private static final int CSOBJ_CACHE = 0;
  private static final int CSOBJ_KAFFEIONAME = 1;
  private static final int CSOBJ_ALIASES = 2;
  private static final int CSOBJ_FACTORS = 3;

  public KaffeCharsetProvider ()
  {
    charsets = new HashMap();

    // Name supported charsets here.

    charsets.put("EUC-JP",
      new Object[] {null, "EUC-JP",
        new String[] {"EUC_JP", "EUCJP"}, // Sun's JDK supports these names.
        new float[] {0.5f, 1.0f, 2.0f, 2.0f}});

    charsets.put("ISO-2022-JP",
      new Object[] {null, "ISO-2022-JP",
        null,
        new float[] {0.5f, 1.0f, 2.0f, 8.0f}});

    charsets.put("WINDOWS-31J",
      new Object[] {null, "MS932",
        new String[] {"MS932"},
        new float[] {0.5f, 1.0f, 2.0f, 2.0f}});

    charsets.put("SHIFT_JIS",
      new Object[] {null, "SHIFT_JIS",
        new String[] {"SJIS", "SHIFT-JIS"}, // Sun's JDK supports these names.
        new float[] {0.5f, 1.0f, 2.0f, 2.0f}});

    canonicalNames = new HashMap();
    for (Iterator i = charsets.keySet().iterator (); i.hasNext (); )
      {
        String name = (String)(i.next ());
        Object[] objs = (Object[])(charsets.get(name));
        if (objs[CSOBJ_ALIASES] != null)
          {
            String[] aliases = (String[])(objs[CSOBJ_ALIASES]);
            for (int j = 0; j < aliases.length; j++)
              {
                canonicalNames.put (aliases[j].toUpperCase(), name);
              }
          }
       }
  }

  public Iterator charsets ()
  {
    HashMap map = new HashMap();
    for (Iterator i = charsets.keySet().iterator (); i.hasNext (); )
      {
        String name = (String)(i.next ());
        map.put(name, charsetForName(name));
      }
    return Collections.unmodifiableCollection (map.values ())
                      .iterator ();
  }

  public Charset charsetForName (String charsetName)
  {
    charsetName = canonicalize (charsetName);
    Object[] objs = (Object[])(charsets.get(charsetName));
    if (objs == null) return null;
    if (objs[CSOBJ_CACHE] == null)
      {
        String kaffeIOName = (String)(objs[CSOBJ_KAFFEIONAME]);
        String[] aliases = (String[])(objs[CSOBJ_ALIASES]);
        float[] factors = (float[])(objs[CSOBJ_FACTORS]);
        objs[CSOBJ_CACHE] = new KaffeCharset(charsetName, kaffeIOName,
            aliases, factors);
      }
    return (Charset)(objs[CSOBJ_CACHE]);
  }

  private String canonicalize (String charsetName)
  {
    charsetName = charsetName.toUpperCase();
    Object o = canonicalNames.get (charsetName);
    return o == null ? charsetName : (String)o;
  }

}
