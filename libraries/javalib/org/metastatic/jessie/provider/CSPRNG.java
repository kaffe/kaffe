/* CSPRNG.java -- continuously seeded PRNG.
   Copyright (C) 2004 Free Software Foundation, Inc.

A version of this class is also a part of GNU Crypto.

This file is a part of Jessie.

Jessie is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

Jessie is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with Jessie; if not, write to the

   Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor,
   Boston, MA  02110-1301
   USA

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under terms
of your choice, provided that you also meet, for each linked independent
module, the terms and conditions of the license of that module.  An
independent module is a module which is not derived from or based on
this library.  If you modify this library, you may extend this exception
to your version of the library, but you are not obligated to do so.  If
you do not wish to do so, delete this exception statement from your
version.  */

/* This class is based on the CSPRNG from cryptlib, Copyright (C)
 * 1995-2003 Peter Gutmann. The notice from `device/dev_sys.c' followns:
 *
 * The random pool handling code in this module and the misc/rnd*.c modules
 * represent the cryptlib continuously seeded pseudorandom number generator
 * (CSPRNG) as described in my 1998 Usenix Security Symposium paper "The
 * generation of practically strong random numbers".
 *
 * The CSPRNG code is copyright Peter Gutmann (and various others) 1995-2003
 * all rights reserved.  Redistribution of the CSPRNG modules and use in
 * source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice
 *    and this permission notice in its entirety.
 *
 * 2. Redistributions in binary form must reproduce the copyright notice in
 *    the documentation and/or other materials provided with the distribution.
 *
 * 3. A copy of any bugfixes or enhancements made must be provided to the
 *    author, <pgut001@cs.auckland.ac.nz> to allow them to be added to the
 *    baseline version of the code.
 *
 * ALTERNATIVELY, the code may be distributed under the terms of the GNU
 * General Public License, version 2 or any later version published by the
 * Free Software Foundation, in which case the provisions of the GNU GPL are
 * required INSTEAD OF the above restrictions.
 *
 * Although not required under the terms of the GPL, it would still be nice
 * if you could make any changes available to the author to allow a
 * consistent code base to be maintained
 *
 * ---
 *
 * Notice that here we elect the latter option, and this code is licensed
 * under the GPL.
 */

package org.metastatic.jessie.provider;

import gnu.crypto.Properties;
import gnu.crypto.Registry;
import gnu.crypto.cipher.CipherFactory;
import gnu.crypto.cipher.IBlockCipher;
import gnu.crypto.hash.HashFactory;
import gnu.crypto.hash.IMessageDigest;
import gnu.crypto.prng.BasePRNG;
import gnu.crypto.prng.IRandom;
import gnu.crypto.prng.LimitReachedException;

import java.io.ByteArrayOutputStream;
import java.io.FileInputStream;
import java.io.InputStream;
import java.io.PrintStream;

import java.net.MalformedURLException;
import java.net.URL;

import java.security.AccessController;
import java.security.InvalidKeyException;
import java.security.PrivilegedAction;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.StringTokenizer;

import org.metastatic.jessie.EntropySource;

/**
 * <p>An entropy pool-based pseudo-random number generator based on the PRNG
 * in Peter Gutmann's cryptlib (<a
 * href="http://www.cs.auckland.ac.nz/~pgut001/cryptlib/">http://www.cs.auckland.ac.nz/~pgut001/cryptlib/</a>).</p>
 *
 * <p>The basic properties of this generator are:</p>
 *
 * <ol>
 * <li>The internal state cannot be determined by knowledge of the input.</li>
 * <li>It is resistant to bias introduced by specific inputs.</li>
 * <li>The output does not reveal the state of the generator.</li>
 * </ol>
 */
public class CSPRNG extends BasePRNG
{

  // Constants and fields.
  // -------------------------------------------------------------------------

  private static final boolean DEBUG = false;
  private static void debug(String msg)
  {
    System.err.print(">>> CSPRNG: ");
    System.err.println(msg);
  }

  /**
   * The system instance, used throughout Jessie when secure random numbers
   * are needed.
   */
  static final CSPRNG SYSTEM_RANDOM;

  /**
   * Property name for the list of files to read for random values. The
   * mapped value is a list with the following values:
   *
   * <ol>
   * <li>A {@link Double}, indicating the suggested <i>quality</i> of this
   * source. This value must be between 0 and 100.</li>
   * <li>An {@link Integer}, indicating the number of bytes to skip in the file
   * before reading bytes. This can be any nonnegative value.</li>
   * <li>An {@link Integer}, indicating the number of bytes to read.</li>
   * <li>A {@link String}, indicating the path to the file.</li>
   * </ol>
   *
   * @see gnu.crypto.util.SimpleList
   */
  public static final String FILE_SOURCES = "jessie.prng.pool.files";

  /**
   * Property name for the list of URLs to poll for random values. The
   * mapped value is a list formatted similarly as in {@link #FILE_SOURCES},
   * but the fourth member is a {@link URL}.
   */
  public static final String URL_SOURCES = "jessie.prng.pool.urls";

  /**
   * Property name for the list of programs to execute, and use the output
   * as new random bytes. The mapped property is formatted similarly an in
   * {@link #FILE_SOURCES} and {@link #URL_SOURCES}, except the fourth
   * member is a {@link String} of the program to execute.
   */
  public static final String PROGRAM_SOURCES = "jessie.prng.pool.programs";

  /**
   * Property name for a list of other sources of entropy. The mapped
   * value must be a list of {@link EntropySource} objects.
   */
  public static final String OTHER_SOURCES = "jessie.prng.pool.other";

  /**
   * Property name for whether or not to wait for the slow poll to
   * complete, passed as a {@link Boolean}. The default value is true.
   */
  public static final String BLOCKING = "jessie.prng.pool.blocking";

  private static final String FILES = "jessie.csprng.file.";
  private static final String URLS = "jessie.csprng.url.";
  private static final String PROGS = "jessie.csprng.program.";
  private static final String OTHER = "jessie.csprng.other.";
  private static final String BLOCK = "jessie.csprng.blocking";

  private static final int POOL_SIZE = 256;
  private static final int ALLOC_SIZE = 260;
  private static final int OUTPUT_SIZE = POOL_SIZE / 2;
  private static final int X917_POOL_SIZE = 16;
  private static final String HASH_FUNCTION = Registry.SHA160_HASH;
  private static final String CIPHER = Registry.AES_CIPHER;
  private static final int MIX_COUNT = 10;
  private static final int X917_LIFETIME = 8192;

  /**
   * The spinner group singleton. We use this to add a small amount of
   * randomness (in addition to the current time and the amount of
   * free memory) based on the randomness (if any) present due to
   * system load and thread scheduling.
   */
  private static final Spinner[] SPINNERS = new Spinner[8];
  static
  {
    SPINNERS[0] = new Spinner(); SPINNERS[0].start();
    SPINNERS[1] = new Spinner(); SPINNERS[1].start();
    SPINNERS[2] = new Spinner(); SPINNERS[2].start();
    SPINNERS[3] = new Spinner(); SPINNERS[3].start();
    SPINNERS[4] = new Spinner(); SPINNERS[4].start();
    SPINNERS[5] = new Spinner(); SPINNERS[5].start();
    SPINNERS[6] = new Spinner(); SPINNERS[6].start();
    SPINNERS[7] = new Spinner(); SPINNERS[7].start();

    try
      {
        SYSTEM_RANDOM = getSystemInstance ();
      }
    catch (Exception x)
      {
        throw new Error (x);
      }
  }

  /**
   * The message digest (SHA-1) used in the mixing function.
   */
  private final IMessageDigest hash;

  /**
   * The cipher (AES) used in the output masking function.
   */
  private final IBlockCipher cipher;

  /**
   * The number of times the pool has been mixed.
   */
  private int mixCount;

  /**
   * The entropy pool.
   */
  private final byte[] pool;

  /**
   * The quality of the random pool (percentage).
   */
  private double quality;

  /**
   * The index of the next byte in the entropy pool.
   */
  private int index;

  /**
   * The pool for the X9.17-like generator.
   */
  private byte[] x917pool;

  /**
   * The number of iterations of the X9.17-like generators.
   */
  private int x917count;

  /**
   * Whether or not the X9.17-like generator is initialized.
   */
  private boolean x917init;

  /**
   * The list of file soures.
   */
  private final List files;

  /**
   * The list of URL sources.
   */
  private final List urls;

  /**
   * The list of program sources.
   */
  private final List progs;

  /**
   * The list of other sources.
   */
  private final List other;

  /**
   * Whether or not to wait for the slow poll to complete.
   */
  private boolean blocking;

  /**
   * The thread that polls for random data.
   */
  private Poller poller;
  private Thread pollerThread;

  // Constructor.
  // -------------------------------------------------------------------------

  public CSPRNG()
  {
    super("CSPRNG");
    pool = new byte[ALLOC_SIZE];
    x917pool = new byte[X917_POOL_SIZE];
    x917count = 0;
    x917init = false;
    quality = 0.0;
    hash = HashFactory.getInstance(HASH_FUNCTION);
    cipher = CipherFactory.getInstance(CIPHER);
    buffer = new byte[OUTPUT_SIZE];
    ndx = 0;
    initialised = false;
    files = new LinkedList();
    urls = new LinkedList();
    progs = new LinkedList();
    other = new LinkedList();
  }

  // Class methods.
  // -------------------------------------------------------------------------

  /**
   * <p>Create and initialize a CSPRNG instance with the "system" parameters;
   * the files, URLs, programs, and {@link EntropySource} sources used by
   * the instance are derived from properties set in the system {@link
   * Properties}.</p>
   *
   * <p>All properties are of the from <i>name</i>.</i>N</i>, where <i>name</i>
   * is the name of the source, and <i>N</i> is an integer (staring at 1) that
   * indicates the preference number for that source.</p>
   *
   * <p>The following vales for <i>name</i> are used here:</p>
   *
   * <dl>
   * <dt>jessie.csprng.file</dt>
   * <dd><p>These properties are file sources, passed as the {@link #FILE_SOURCES}
   * parameter of the instance. The property value is a 4-tuple formatted as:</p>
   *
   * <blockquote><i>quality</i> ; <i>offset</i> ; <i>count</i> ; <i>path</i></blockquote>
   *
   * <p>The parameters are mapped to the parameters defined for {@link
   * #FILE_SOURCES}. Leading or trailing spaces on any item are trimmed
   * off.</p></dd>
   *
   * <dt>jessie.csprng.url</dt>
   * <dd><p>These properties are URL sources, passed as the {@link #URL_SOURCES}
   * parameter of the instance. The property is formatted the same way as file
   * sources, but the <i>path</i> argument must be a valid URL.</p></dd>
   *
   * <dt>jessie.csprng.program</dt>
   * <dd><p>These properties are program sources, passed as the {@link
   * #PROGRAM_SOURCES} parameter of the instance. This property is formatted
   * the same way as file and URL sources, but the last argument is a program
   * and its arguments.</p></dd>
   *
   * <dt>jessie.cspring.other</dt>
   * <dd><p>These properties are other sources, passed as the {@link OTHER_SOURCES}
   * parameter of the instance. The property value must be the full name
   * of a class that implements the {@link EntropySource} interface and has a
   * public no-argument constructor.</p></dd>
   * </dl>
   *
   * <p>Finally, a boolean property "jessie.csprng.blocking" can be set to
   * the desired value of {@link #BLOCKING}.</p>
   *
   * <p>An example of valid properties would be:</p>
   *
   * <pre>
   * jessie.csprng.blocking=true
   *
   * jessie.csprng.file.1=75.0;0;256;/dev/random
   * jessie.csprng.file.2=10.0;0;100;/home/user/file
   *
   * jessie.csprng.url.1=5.0;0;256;http://www.random.org/cgi-bin/randbyte?nbytes=256
   * jessie.csprng.url.2=0;256;256;http://slashdot.org/
   *
   * jessie.csprng.program.1=0.5;0;10;last -n 50
   * jessie.csprng.program.2=0.5;0;10;tcpdump -c 5
   *
   * jessie.csprng.other.1=foo.bar.MyEntropySource
   * jessie.csprng.other.2=com.company.OtherEntropySource
   * </pre>
   */
  public static CSPRNG getSystemInstance()
    throws ClassNotFoundException, MalformedURLException, NumberFormatException
  {
    CSPRNG instance = new CSPRNG();
    HashMap attrib = new HashMap();
    attrib.put(BLOCKING, new Boolean(Util.getSecurityProperty(BLOCK)));
    String s = null;

    // Get each file source "jessie.csprng.file.N".
    List l = new LinkedList();
    for (int i = 0; (s = Util.getSecurityProperty(FILES+i)) != null; i++)
      {
        try
          {
            l.add(parseString(s.trim()));
          }
        catch (NumberFormatException nfe)
          {
          }
      }
    attrib.put(FILE_SOURCES, l);

    l = new LinkedList();
    for (int i = 0; (s = Util.getSecurityProperty(URLS+i)) != null; i++)
      {
        try
          {
            l.add(parseURL(s.trim()));
          }
        catch (NumberFormatException nfe)
          {
          }
        catch (MalformedURLException mue)
          {
          }
      }
    attrib.put(URL_SOURCES, l);

    l = new LinkedList();
    for (int i = 0; (s = Util.getSecurityProperty(PROGS+i)) != null; i++)
      {
        try
          {
            l.add(parseString(s.trim()));
          }
        catch (NumberFormatException nfe)
          {
          }
      }
    attrib.put(PROGRAM_SOURCES, l);

    l = new LinkedList();
    for (int i = 0; (s = Util.getSecurityProperty(OTHER+i)) != null; i++)
      {
        try
          {
            Class c = Class.forName(s.trim());
            l.add(c.newInstance());
          }
        catch (ClassNotFoundException cnfe)
          {
          }
        catch (InstantiationException ie)
          {
          }
        catch (IllegalAccessException iae)
          {
          }
      }
    attrib.put(OTHER_SOURCES, l);

    instance.init(attrib);
    return instance;
  }

  private static List parseString(String s) throws NumberFormatException
  {
    StringTokenizer tok = new StringTokenizer(s, ";");
    if (tok.countTokens() != 4)
      {
        throw new IllegalArgumentException("malformed property");
      }
    Double quality = new Double(tok.nextToken());
    Integer offset = new Integer(tok.nextToken());
    Integer length = new Integer(tok.nextToken());
    String str = tok.nextToken();
    return new SimpleList(quality, offset, length, str);
  }

  private static List parseURL(String s)
    throws MalformedURLException, NumberFormatException
  {
    StringTokenizer tok = new StringTokenizer(s, ";");
    if (tok.countTokens() != 4)
      {
        throw new IllegalArgumentException("malformed property");
      }
    Double quality = new Double(tok.nextToken());
    Integer offset = new Integer(tok.nextToken());
    Integer length = new Integer(tok.nextToken());
    URL url = new URL(tok.nextToken());
    return new SimpleList(quality, offset, length, url);
  }

  // Instance methods.
  // -------------------------------------------------------------------------

  public Object clone()
  {
    return new CSPRNG();
  }

  public void setup(Map attrib)
  {
    List list = null;

    if (DEBUG)
      {
        debug(String.valueOf(attrib));
      }
    try
      {
        list = (List) attrib.get(FILE_SOURCES);
        if (DEBUG)
          {
            debug(String.valueOf(list));
          }
        if (list != null)
          {
            files.clear();
            for (Iterator it = list.iterator(); it.hasNext(); )
              {
                List l = (List) it.next();
                if (DEBUG)
                  {
                    debug("l=" + l);
                  }
                if (l.size() != 4)
                  {
                    if (DEBUG)
                      {
                        debug("file list too small: " + l.size());
                      }
                    throw new IllegalArgumentException("invalid file list");
                  }
                Double quality = (Double) l.get(0);
                Integer offset = (Integer) l.get(1);
                Integer length = (Integer) l.get(2);
                String source = (String) l.get(3);
                files.add(new SimpleList(quality, offset, length, source));
              }
          }
      }
    catch (ClassCastException cce)
      {
        if (DEBUG)
          {
            debug("bad file list: " + cce.getMessage());
            cce.printStackTrace();
          }
        throw new IllegalArgumentException("invalid file list");
      }

    try
      {
        list = (List) attrib.get(URL_SOURCES);
        if (DEBUG)
          {
            debug(String.valueOf(list));
          }
        if (list != null)
          {
            urls.clear();
            for (Iterator it = list.iterator(); it.hasNext(); )
              {
                List l = (List) it.next();
                if (DEBUG)
                  {
                    debug("l=" + l);
                  }
                if (l.size() != 4)
                  {
                    if (DEBUG)
                      {
                        debug("URL list too small: " + l.size());
                      }
                    throw new IllegalArgumentException("invalid URL list");
                  }
                Double quality = (Double) l.get(0);
                Integer offset = (Integer) l.get(1);
                Integer length = (Integer) l.get(2);
                URL source = (URL) l.get(3);
                urls.add(new SimpleList(quality, offset, length, source));
              }
          }
      }
    catch (ClassCastException cce)
      {
        if (DEBUG)
          {
            debug("bad URL list: " + cce.getMessage());
            cce.printStackTrace();
          }
        throw new IllegalArgumentException("invalid URL list");
      }

    try
      {
        list = (List) attrib.get(PROGRAM_SOURCES);
        if (DEBUG)
          {
            debug(String.valueOf(list));
          }
        if (list != null)
          {
            progs.clear();
            for (Iterator it = list.iterator(); it.hasNext(); )
              {
                List l = (List) it.next();
                if (DEBUG)
                  {
                    debug("l=" + l);
                  }
                if (l.size() != 4)
                  {
                    if (DEBUG)
                      {
                        debug("program list too small: " + l.size());
                      }
                    throw new IllegalArgumentException("invalid program list");
                  }
                Double quality = (Double) l.get(0);
                Integer offset = (Integer) l.get(1);
                Integer length = (Integer) l.get(2);
                String source = (String) l.get(3);
                progs.add(new SimpleList(quality, offset, length, source));
              }
          }
      }
    catch (ClassCastException cce)
      {
        if (DEBUG)
          {
            debug("bad program list: " + cce.getMessage());
            cce.printStackTrace();
          }
        throw new IllegalArgumentException("invalid program list");
      }

    try
      {
        list = (List) attrib.get(OTHER_SOURCES);
        if (DEBUG)
          {
            debug(String.valueOf(list));
          }
        if (list != null)
          {
            other.clear();
            for (Iterator it = list.iterator(); it.hasNext(); )
              {
                EntropySource src = (EntropySource) it.next();
                if (DEBUG)
                  {
                    debug("src=" + src);
                  }
                if (src == null)
                  {
                    throw new NullPointerException("null source in source list");
                  }
                other.add(src);
              }
          }
      }
    catch (ClassCastException cce)
      {
        throw new IllegalArgumentException("invalid source list");
      }

    try
      {
        Boolean block = (Boolean) attrib.get(BLOCKING);
        if (block != null)
          {
            blocking = block.booleanValue();
          }
        else
          {
            blocking = true;
          }
      }
    catch (ClassCastException cce)
      {
        throw new IllegalArgumentException("invalid blocking parameter");
      }

    poller = new Poller(files, urls, progs, other, this);
    try
      {
        fillBlock();
      }
    catch (LimitReachedException lre)
      {
        throw new RuntimeException("bootstrapping CSPRNG failed");
      }
  }

  public synchronized byte nextByte () throws LimitReachedException
  {
    return super.nextByte();
  }

  public synchronized void nextBytes (byte[] out, int offset, int length)
    throws LimitReachedException
  {
    super.nextBytes (out, offset, length);
  }

  public synchronized void fillBlock() throws LimitReachedException
  {
    if (DEBUG)
      {
        debug("fillBlock");
      }
    if (getQuality() < 100.0)
      {
        if (DEBUG)
          {
            debug("doing slow poll");
          }
        slowPoll();
      }

    do
      {
        fastPoll();
        mixRandomPool();
      }
    while (mixCount < MIX_COUNT);

    if (!x917init || x917count >= X917_LIFETIME)
      {
        mixRandomPool(pool);
        Map attr = new HashMap();
        byte[] key = new byte[32];
        System.arraycopy(pool, 0, key, 0, 32);
        cipher.reset();
        attr.put(IBlockCipher.KEY_MATERIAL, key);
        try
          {
            cipher.init(attr);
          }
        catch (InvalidKeyException ike)
          {
            throw new Error(ike.toString());
          }

        mixRandomPool(pool);
        generateX917(pool);
        mixRandomPool(pool);
        generateX917(pool);

        if (x917init)
          {
            quality = 0.0;
          }
        x917init = true;
        x917count = 0;
      }

    byte[] export = new byte[ALLOC_SIZE];
    for (int i = 0; i < ALLOC_SIZE; i++)
      {
        export[i] = (byte) (pool[i] ^ 0xFF);
      }

    mixRandomPool();
    mixRandomPool(export);

    generateX917(export);

    for (int i = 0; i < OUTPUT_SIZE; i++)
      {
        buffer[i] = (byte) (export[i] ^ export[i + OUTPUT_SIZE]);
      }
    Arrays.fill(export, (byte) 0);
  }

  /**
   * Add an array of bytes into the randomness pool. Note that this method
   * will <i>not</i> increment the pool's quality counter (this can only be
   * done via a source provided to the setup method).
   *
   * @param buf The byte array.
   * @param off The offset from whence to start reading bytes.
   * @param len The number of bytes to add.
   * @throws ArrayIndexOutOfBoundsException If <i>off</i> or <i>len</i> are
   * out of the range of <i>buf</i>.
   */
  public synchronized void addRandomBytes(byte[] buf, int off, int len)
  {
    if (off < 0 || len < 0 || off+len > buf.length)
      {
        throw new ArrayIndexOutOfBoundsException();
      }
    if (DEBUG)
      {
        debug("adding random bytes:");
        debug(Util.toHexString(buf, off, len));
      }
    final int count = off+len;
    for (int i = off; i < count; i++)
      {
        pool[index++] ^= buf[i];
        if (index == pool.length)
          {
            mixRandomPool();
            index = 0;
          }
      }
  }

  /**
   * Add a single random byte to the randomness pool. Note that this method
   * will <i>not</i> increment the pool's quality counter (this can only be
   * done via a source provided to the setup method).
   *
   * @param b The byte to add.
   */
  public synchronized void addRandomByte(byte b)
  {
    if (DEBUG)
      {
        debug("adding byte " + Integer.toHexString(b));
      }
    pool[index++] ^= b;
    if (index >= pool.length)
      {
        mixRandomPool();
        index = 0;
      }
  }

  // Package methods.
  // -------------------------------------------------------------------------

  synchronized void addQuality(double quality)
  {
    if (DEBUG)
      {
        debug("adding quality " + quality);
      }
    if (this.quality < 100)
      {
        this.quality += quality;
      }
    if (DEBUG)
      {
        debug("quality now " + this.quality);
      }
  }

  synchronized double getQuality()
  {
    return quality;
  }

  // Own methods.
  // -------------------------------------------------------------------------

  /**
   * The mix operation. This method will, for every 20-byte block in the
   * random pool, hash that block, the previous 20 bytes, and the next
   * 44 bytes with SHA-1, writing the result back into that block.
   */
  private void mixRandomPool(byte[] buf)
  {
    int hashSize = hash.hashSize();
    for (int i = 0; i < buf.length; i += hashSize)
      {

        // First update the bytes [p-19..p-1].
        if (i == 0)
          {
            hash.update(buf, buf.length - hashSize, hashSize);
          }
        else
          {
            hash.update(buf, i - hashSize, hashSize);
          }

        // Now the next 64 bytes.
        if (i + 64 < buf.length)
          {
            hash.update(buf, i, 64);
          }
        else
          {
            hash.update(buf, i, buf.length - i);
            hash.update(buf, 0, 64 - (buf.length - i));
          }

        byte[] digest = hash.digest();
        System.arraycopy(digest, 0, buf, i, hashSize);
      }
  }

  private void mixRandomPool()
  {
    mixRandomPool(pool);
    mixCount++;
  }

  private void generateX917(byte[] buf)
  {
    int off = 0;
    for (int i = 0; i < buf.length; i += X917_POOL_SIZE)
      {
        int copy = Math.min(buf.length - i, X917_POOL_SIZE);
        for (int j = 0; j < copy; j++)
          {
            x917pool[j] ^= pool[off + j];
          }

        cipher.encryptBlock(x917pool, 0, x917pool, 0);
        System.arraycopy(x917pool, 0, buf, off, copy);
        cipher.encryptBlock(x917pool, 0, x917pool, 0);

        off += copy;
        x917count++;
      }
  }

  /**
   * Add random data always immediately available into the random pool, such
   * as the values of the eight asynchronous counters, the current time, the
   * current memory usage, the calling thread name, and the current stack
   * trace.
   *
   * <p>This method does not alter the quality counter, and is provided more
   * to maintain randomness, not to seriously improve the current random
   * state.
   */
  private void fastPoll()
  {
    byte b = SPINNERS[0].getCount();
    b ^= SPINNERS[1].getCount();
    b ^= SPINNERS[2].getCount();
    b ^= SPINNERS[3].getCount();
    b ^= SPINNERS[4].getCount();
    b ^= SPINNERS[5].getCount();
    b ^= SPINNERS[6].getCount();
    b ^= SPINNERS[7].getCount();
    addRandomByte(b);
    addRandomByte((byte) System.currentTimeMillis());
    addRandomByte((byte) Runtime.getRuntime().freeMemory());

    String s = Thread.currentThread().getName();
    if (s != null)
      {
        byte[] buf = s.getBytes();
        addRandomBytes(buf, 0, buf.length);
      }

    ByteArrayOutputStream bout = new ByteArrayOutputStream(1024);
    PrintStream pout = new PrintStream(bout);
    Throwable t = new Throwable();
    t.printStackTrace(pout);
    pout.flush();
    byte[] buf = bout.toByteArray();
    addRandomBytes(buf, 0, buf.length);
  }

  private void slowPoll() throws LimitReachedException
  {
    if (DEBUG)
      {
        boolean alive = pollerThread != null && pollerThread.isAlive();
        debug("poller is alive? " + alive);
      }
    if (pollerThread == null || !pollerThread.isAlive())
      {
        pollerThread = new Thread(poller);
        pollerThread.setDaemon(true);
        pollerThread.start();
        if (blocking)
          {
            try
              {
                pollerThread.join();
              }
            catch (InterruptedException ie)
              {
              }
          }

        // If the full slow poll has completed after we waited for it,
        // and there in insufficient randomness, throw an exception.
        if (/*!Thread.interrupted() &&*/ blocking && quality < 100.0)
          {
            if (DEBUG)
              {
                debug("insufficient quality: " + quality);
              }
            throw new LimitReachedException();
          }
      }
  }

  protected void finalize() throws Throwable
  {
    if (pollerThread != null && pollerThread.isAlive())
      {
//         pollerThread.interrupt();
        poller.stopUpdating();
//         pollerThread.interrupt();
      }
    Arrays.fill(pool, (byte) 0);
    Arrays.fill(x917pool, (byte) 0);
    Arrays.fill(buffer, (byte) 0);
  }

  // Inner classes.
  // -------------------------------------------------------------------------

  /**
   * A simple thread that constantly updates a byte counter. This class is
   * used in a group of lowest-priority threads and the values of their
   * counters (updated in competition with all other threads) is used as a
   * source of entropy bits.
   */
  private static class Spinner extends Thread
  {

    // Field.
    // -----------------------------------------------------------------------

    private byte counter;

    // Constructor.
    // -----------------------------------------------------------------------

    Spinner()
    {
      setPriority(MIN_PRIORITY);
      setDaemon(true);
    }

    // Instance methods.
    // -----------------------------------------------------------------------

    public void run()
    {
      while (true)
        {
          counter++;
          try { sleep (100); }
          catch (InterruptedException ie) { }
        }
    }

    byte getCount()
    {
      return counter;
    }
  }

  private final class Poller implements Runnable
  {

    // Fields.
    // -----------------------------------------------------------------------

    private final List files;
    private final List urls;
    private final List progs;
    private final List other;
    private final CSPRNG pool;
    private boolean running;

    // Constructor.
    // -----------------------------------------------------------------------

    Poller(List files, List urls, List progs, List other, CSPRNG pool)
    {
      super();
      this.files = Collections.unmodifiableList(files);
      this.urls = Collections.unmodifiableList(urls);
      this.progs = Collections.unmodifiableList(progs);
      this.other = Collections.unmodifiableList(other);
      this.pool = pool;
    }

    // Instance methods.
    // -----------------------------------------------------------------------

    public void run()
    {
      running = true;
      if (DEBUG)
        {
          debug("files: " + files);
          debug("URLs: " + urls);
          debug("progs: " + progs);
        }
      Iterator files_it = files.iterator();
      Iterator urls_it = urls.iterator();
      Iterator prog_it = progs.iterator();
      Iterator other_it = other.iterator();

      while (files_it.hasNext() || urls_it.hasNext() || prog_it.hasNext() ||
             other_it.hasNext())
        {

          // There is enough random data. Go away.
          if (pool.getQuality() >= 100.0 || !running)
            {
              return;
            }

          if (files_it.hasNext())
            {
              try
                {
                  List l = (List) files_it.next();
                  if (DEBUG)
                    {
                      debug(l.toString());
                    }
                  double qual = ((Double) l.get(0)).doubleValue();
                  int offset = ((Integer) l.get(1)).intValue();
                  int count = ((Integer) l.get(2)).intValue();
                  String src = (String) l.get(3);
                  InputStream in = new FileInputStream(src);
                  byte[] buf = new byte[count];
                  if (offset > 0)
                    {
                      in.skip(offset);
                    }
                  int len = in.read(buf);
                  if (len >= 0)
                    {
                      pool.addRandomBytes(buf, 0, len);
                      pool.addQuality(qual * ((double) len / (double) count));
                    }
                  if (DEBUG)
                    {
                      debug("got " + len + " bytes from " + src);
                    }
                }
              catch (Exception x)
                {
                  if (DEBUG)
                    {
                      debug(x.toString());
                      x.printStackTrace();
                    }
                }
            }

          if (pool.getQuality() >= 100.0 || !running)
            {
              return;
            }

          if (urls_it.hasNext())
            {
              try
                {
                  List l = (List) urls_it.next();
                  if (DEBUG)
                    {
                      debug(l.toString());
                    }
                  double qual = ((Double) l.get(0)).doubleValue();
                  int offset = ((Integer) l.get(1)).intValue();
                  int count = ((Integer) l.get(2)).intValue();
                  URL src = (URL) l.get(3);
                  InputStream in = src.openStream();
                  byte[] buf = new byte[count];
                  if (offset > 0)
                    {
                      in.skip(offset);
                    }
                  int len = in.read(buf);
                  if (len >= 0)
                    {
                      pool.addRandomBytes(buf, 0, len);
                      pool.addQuality(qual * ((double) len / (double) count));
                    }
                  if (DEBUG)
                    {
                      debug("got " + len + " bytes from " + src);
                    }
                }
              catch (Exception x)
                {
                  if (DEBUG)
                    {
                      debug(x.toString());
                      x.printStackTrace();
                    }
                }
            }

          if (pool.getQuality() >= 100.0 || !running)
            {
              return;
            }

          if (prog_it.hasNext())
            {
              try
                {
                  List l = (List) prog_it.next();
                  if (DEBUG)
                    {
                      debug(l.toString());
                    }
                  double qual = ((Double) l.get(0)).doubleValue();
                  int offset = ((Integer) l.get(1)).intValue();
                  int count = ((Integer) l.get(2)).intValue();
                  String src = (String) l.get(3);
                  Process proc = Runtime.getRuntime().exec(src);
                  InputStream in = proc.getInputStream();
                  byte[] buf = new byte[count];
                  if (offset > 0)
                    {
                      in.skip(offset);
                    }
                  int len = in.read(buf);
                  if (len >= 0)
                    {
                      pool.addRandomBytes(buf, 0, len);
                      pool.addQuality(qual * ((double) len / (double) count));
                    }
                  if (DEBUG)
                    {
                      debug("got " + len + " bytes from " + src);
                    }
                }
              catch (Exception x)
                {
                  if (DEBUG)
                    {
                      debug(x.toString());
                      x.printStackTrace();
                    }
                }
            }

          if (pool.getQuality() >= 100.0 || !running)
            {
              return;
            }

          if (other_it.hasNext())
            {
              try
                {
                  EntropySource src = (EntropySource) other_it.next();
                  byte[] buf = src.nextBytes();
                  if (pool == null)
                    {
                      return;
                    }
                  pool.addRandomBytes(buf, 0, buf.length);
                  pool.addQuality(src.quality());
                  if (DEBUG)
                    {
                      debug("got " + buf.length + " bytes from " + src);
                    }
                }
              catch (Exception x)
                {
                  if (DEBUG)
                    {
                      debug(x.toString());
                      x.printStackTrace();
                    }
                }
            }
        }
    }

    public void stopUpdating()
    {
      running = false;
    }
  }
}
