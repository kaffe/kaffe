package gnu.crypto.exp.ust;

// ----------------------------------------------------------------------------
// $Id: UST.java,v 1.2 2005/07/04 00:04:40 robilad Exp $
//
// Copyright (C) 2001, 2002, 2003, Free Software Foundation, Inc.
//
// This file is part of GNU Crypto.
//
// GNU Crypto is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// GNU Crypto is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to the
//
//    Free Software Foundation Inc.,
//    51 Franklin Street, Fifth Floor,
//    Boston, MA 02110-1301
//    USA
//
// Linking this library statically or dynamically with other modules is
// making a combined work based on this library.  Thus, the terms and
// conditions of the GNU General Public License cover the whole
// combination.
//
// As a special exception, the copyright holders of this library give
// you permission to link this library with independent modules to
// produce an executable, regardless of the license terms of these
// independent modules, and to copy and distribute the resulting
// executable under terms of your choice, provided that you also meet,
// for each linked independent module, the terms and conditions of the
// license of that module.  An independent module is a module which is
// not derived from or based on this library.  If you modify this
// library, you may extend this exception to your version of the
// library, but you are not obligated to do so.  If you do not wish to
// do so, delete this exception statement from your version.
// ----------------------------------------------------------------------------

import gnu.crypto.Registry;
import gnu.crypto.cipher.IBlockCipher;
import gnu.crypto.mac.TMMH16;
import gnu.crypto.prng.ICMGenerator;
import gnu.crypto.prng.IRandom;
import gnu.crypto.prng.PRNGFactory;
import gnu.crypto.prng.UMacGenerator;
import gnu.crypto.prng.LimitReachedException;

import java.math.BigInteger;
import java.security.InvalidKeyException;
import java.util.HashMap;
import java.util.Map;

/**
 * <p>The Universal Security Transform (UST) is a cryptographic transform for
 * providing confidentiality, message authentication, and replay protection.
 * This transform is sufficient for providing these services to network
 * protocols, though it does not specify a protocol itself.</p>
 *
 * <p>UST has the following parameters:</p>
 *
 * <ul>
 *    <li><b>INDEX_LENGTH</b>: The number of octets in an Index.</li>
 *
 *    <li><b>MAX_KEYSTREAM_LENGTH</b>: The maximum number of octets in a
 *    keystream segment.</li>
 *
 *    <li><b>PREFIX_LENGTH</b>: The number of octets in the keystream prefix.</li>
 *
 *    <li><b>TAG_LENGTH</b>: The number of octets in an Authentication Tag.</li>
 *
 *    <li><b>MAX_HASH_LENGTH</b>: The maximum number of octets that can be input
 *    to the hash function.</li>
 *
 *    <li><b>HASH_KEY_LENGTH</b>: The number of octets in a hash key.</li>
 * </ul>
 *
 * <p>All of these parameters <b>MUST</b> remain fixed for any given UST
 * context.</p>
 *
 * <p>The parameters <code>INDEX_LENGTH</code> and <code>MAX_KEYSTREAM_LENGTH</code>
 * are defined by the keystream generator. This is because the keystream
 * generators used in UST are families of <em>length-expanding pseudorandom
 * functions</em>. At the time of implementing UST, the only such generators
 * available are (a) {@link gnu.crypto.prng.UMacGenerator} and (b) {@link
 * gnu.crypto.prng.ICMGenerator}. Yet, both of these two algorithms require more
 * than just these two parameters to be fully qualified, and hence instantiated.
 * They both require an underlying <em>block cipher</em> as well as a <em>block
 * size</em> and <em>key material</em>. While default values are used for the
 * former two of these parameters -if they are not included in their
 * initialisation {@link java.util.Map}- the latter parameter; i.e. <em>key
 * material</em>, should be specified.</p>
 *
 * <p>The parameters <code>TAG_LENGTH</code>, <code>MAX_HASH_LENGTH</code>, and
 * <code>HASH_KEY_LENGTH</code> are defined by the hash function. Please note
 * that when the UST document refers to a <em>hash function</em> it actually
 * means a <em>Universal Hash Function</em>. At the time of implementing UST,
 * the only such functions available are {@link gnu.crypto.mac.UHash32} and
 * {@link gnu.crypto.mac.TMMH16}, with the latter more suited for UST purposes
 * because it is independent from any other algorithm --the same is not true for
 * (our implementation of) <code>UHash32</code> because of its current
 * dependencies on {@link gnu.crypto.mac.UMac32}; although this may change in
 * the future. For this reason we shall only allow {@link gnu.crypto.mac.TMMH16}
 * as the UST's underlying <em>Universal Hash Function</em>.</p>
 *
 * <p>The length of any <em>Plaintext</em> protected by UST <b>MUST NOT</b>
 * exceed the smaller of <code>(MAX_KEYSTREAM_LENGTH - TAG_LENGTH)</code> and
 * <code>MAX_HASH_LEN</code>.</p>
 *
 * <p>The value of <code>HASH_KEY_LENGTH</code> <b>MUST</b> be no greater than
 * <code>MAX_KEYSTREAM_LENGTH</code>. The value of <code>TAG_LENGTH</code>
 * <b>MUST</b> be no greater than <code>HASH_KEY_LENGTH</code>.</p>
 *
 * <p>Once a UST instance is initialised, one can (a) authenticate designated
 * bytes -an operation that does not yield any output- and/or (b) encipher
 * designated bytes -an operation that does yield an output equal in length to
 * the input.</p>
 *
 * <p>The understanding of the author of this implementation is that an
 * initialised UST can be used to process different messages while relying on
 * that UST to generate and use a different index value for each message without
 * the need for specifying eveytime that value; i.e. the implementation shall
 * keep track of the index to ensure no previously used value, generated and
 * used with the same key material, is ever re-used. Furthermore, because we
 * want to <em>maximise</em> the possible length of messages processed by a UST
 * with a given <em>key material</em>, each new/updated index value should yield
 * two keystream generators: one for use with the <em>Integrity Protection
 * service</em> (referred to as the Authentication Function in the UST draft),
 * and the other for use to encipher the message (<em>Confidentiality Protection
 * service</em>) --see Figures 1 and 2 in the UST draft. This way the maximum
 * plaintext protected by such a UST is <code>(MAX_KEYSTREAM_LENGTH - TAG_LENGTH)
 * </code> only, and the number of different messages protected by one set of
 * <em>key material</em> is <code>(2^(8*INDEX_LENGTH) - 1)</code> --the index
 * value <code>0</code> being reserved to generate all other keying material! To
 * achieve this objective this implementation does the following:</p>
 *
 * <ol>
 *    <li>We define <code>MAX_INDEX</code> to be <code>(2^(8*INDEX_LENGTH) - 1)
 *    </code> --all length quantities are expressed in bytes.</li>
 *    <li>With every initialisation of a UST, a <em>keying</em> index of value
 *    <code>0</code> and the user-supplied key material are used to initialise
 *    an internal keystream generator. A <em>message</em> index is initialised
 *    to <code>-1</code>.</li>
 *    <li>With every start-of-message, (a) increment the <em>message</em> index,
 *    and (b) check if it is greater than <code>MAX_INDEX</code>. If it is we
 *    throw an exception. Otherwise, we generate from the <em>internal</em>
 *    keystream enough key material to initialise two keystream generators, both
 *    with that same index, one for the <em>Integrity Protection Function</em>
 *    and the other for the <em>Confidentiality Protection Function</em>. Please
 *    note that strictly speaking, the first of this pair of keystreams is NOT
 *    the Confidentiality Protection Function keystream generator, since it is
 *    also used to generate the bytes of the Integrity Protection Function's
 *    <em>Prefix</em>. In other words, as soon as we detect the requirement for
 *    Integrity Protection service, we instantiate the keystream generator
 *    pair, even if the Confidentiality Protection service is not required.</li>
 *    <li>If on the other hand, the start-of-message includes a designated index
 *    value, we use that index as is, in combination with enough key material
 *    generated by our internal keystream.</li>
 *    <li>With every process-message (be it autheticate or encipher) we process
 *    the message through one or both functions as desired (integrity protection
 *    only, or integrity protection as well as confidentiality protection).</li>
 *    <li>With every end-of-message, we terminate the integrity protection and
 *    compute the authentication tag.</li>
 *    <li>It is easy to show that it is possible to re-construct the state of
 *    such a UST, given the user-supplied key material and a designated index
 *    value.</li>
 *    <li>The new constraint that would limit the number of messages, and their
 *    length, with this scheme becomes:
 *    <pre>
 *    MAX_INDEX * 2 * (BLOCK_SIZE - INDEX_LENGTH) &lt;= MAX_KEYSTREAM_LENGTH
 *    </pre>
 *    where <code>BLOCK_SIZE</code> is the underlying cipher's block size in
 *    bytes.</li>
 * </ol>
 *
 * <p>Another alternative to using two keystream generators, would have been to
 * (a) use only one, but (b) in-line the code of the Authentication Function
 * inside the UST. This way, (a) enough output bytes from that keystream would
 * be used to setup the Authentication Function, and (b) the same output bytes
 * from that keystream would be used to compute the authentication context, as
 * well as encipher the message. But this weakens the UST, since an attacker,
 * with a known plaintext, can reconstruct the keystream generator output (used
 * for both the authentication and encipherement functions) by simply xoring the
 * message with the ciphertext.</p>
 *
 * <p>The next figure depicts a block diagram of our implementation of the UST:
 * </p>
 *
 * <pre>
   +----------------------------+               +-----------+
   | user-supplied key material +------+  +-----+ Index = 0 |
   +----------------------------+      |  |     +-----------+
                                       V  V
   +-----------+          +------------------------------+
   |   Index   |          | Internal Keystream Generator |
   +-----+-----+          +-------------+----------------+
         |                              |
    +----|------------------------------+
    |    |    +-----------------+
    |    +--->| Confidentiality |
    +----|--->|   Protection    +--+-----------------+
    |    |    |    Generator    |  |                 |
    |    |    +-----------------+  V                 V
    |    |                 +------------+---------------------------+
    |    |    +------------+   Prefix   |         Suffix            +---+
    |    |    |            +------------+---------------------------+   |
    |    |    |                 <------------- message ------------->   |
    |    |    |                 +-------+---------------------------+   |
    |    |    |         +-------+ Clear |          Opaque           +--(+)
    |    |    |         |       +-------+---------------------------+   |
    |    |    |         |               +---------------------------+   |
    |    |    |         |      +--------+         Ciphertext        |<--+
    |    |    |         |      |        +---------------------------+
    |    |    |         V      V
    |    |    |    +-----------------+
    |    |    +--->|    Integrity    |    +-----------+
    |    +-------->|   Protection    +--->| Auth. Tag |
    +------------->|    Generator    |    +-----------+
                   +-----------------+
 * </pre>
 *
 * <p>References:</p>
 *
 * <ol>
 *    <li><a
 *    href="http://www.ietf.org/internet-drafts/draft-mcgrew-saag-ust-00.txt">
 *    The Universal Security Transform</a>, David A. McGrew.</li>
 * </ol>
 *
 * @version $Revision: 1.2 $
 */
public class UST {

   // Constants and variables
   // -------------------------------------------------------------------------

   /** Property name of a UST index_length. */
   public static final String INDEX_LENGTH = "gnu.crypto.ust.index.length";
   /** Property name of the keystream generator type to use. */
   public static final String KEYSTREAM = "gnu.crypto.ust.keystream.name";
   /** Property name of the keystream underlying cipher. */
   public static final String CIPHER = "gnu.crypto.ust.cipher.name";
   /** Property name of the UST user-supplied key material. */
   public static final String KEY_MATERIAL = "gnu.crypto.ust.key";
   /** Property name of the authentication tag length in bytes. */
   public static final String TAG_LENGTH = "gnu.crypto.ust.tag.length";
   /** Property name of the confidentiality protection flag. */
   public static final String CONFIDENTIALITY = "gnu.crypto.ust.confidentiality";
   /** Property name of the integrity protection flag. */
   public static final String INTEGRITY = "gnu.crypto.ust.integrity";

   /** caches the result of the correctness test, once executed. */
   private static Boolean valid;

   /** Internal keystream generator. */
   private IRandom keystream = null;
   /** Confidentiality protection function keystream generator. */
   private IRandom cpStream = null;
   /** Integrity protection function keystream generator. */
   private IRandom ipStream = null;
   /** Integrity protection function algorithm. */
   private TMMH16 mac = null;
   /** Partially filled attributes of the internal keystream generator. */
   private HashMap kAttributes = new HashMap(5);
   /** Partially filled attributes of the underlying confidentiality KSG. */
   private HashMap cpAttributes = new HashMap(5);
   /** Partially filled attributes of the underlying integrity KSG. */
   private HashMap ipAttributes = new HashMap(5);
   /** Partially filled attributes of the underlying Authentication Function. */
   private HashMap macAttributes = new HashMap(2);
   /** Whether this UST should provide, or not, integrity protection. */
   private boolean wantIntegrity = true;
   /** Whether this UST should provide, or not, confidentiality protection. */
   private boolean wantConfidentiality = false;
   /** Size of user-supplied key material; same as internal keying material. */
   private int keysize;
   /** The message index. */
   private BigInteger index;
   /** The maximum value allowed for the index. */
   private BigInteger maxIndex;
   /** Integrity protection function output length. */
   private int macLength;
   /** Have we started? */
   private boolean ready = false;
   /** The instance lock. */
   private Object lock = new Object();

   // Constructor(s)
   // -------------------------------------------------------------------------

   // Class methods
   // -------------------------------------------------------------------------

   // Instance methods
   // -------------------------------------------------------------------------

   /**
    * <p>Initialise this instance with the designated set of attributes.</p>
    *
    * <p>The possible attributes for a <code>UST</code> are:</p>
    *
    * <ul>
    *    <li>{@link #CONFIDENTIALITY}: a {@link java.lang.Boolean} that
    *    indicates if Confidentiality Protection service is to be activated for
    *    messages processed with this instance.</li>
    *    <li>{@link #INTEGRITY}: a {@link java.lang.Boolean} that indicates if
    *    Integrity Protection service is to be activated for messages processed
    *    with this instance.</li>
    *    <li>{@link #KEYSTREAM}: a {@link java.lang.String} that indicates the
    *    algorithm name of the underlying keystream generators used with this
    *    instance. Currently the only allowed values are:
    *    {@link Registry#UMAC_PRNG} and {@link Registry#ICM_PRNG}.</li>
    *    <li>{@link #INDEX_LENGTH}: a {@link java.lang.Integer} that is only
    *    needed if the {@link Registry#ICM_PRNG} is chosen as the keystream
    *    generator algorithm. This value is the count in bytes of the segment
    *    index portion of an <code>ICMGenerator</code>.
    *    <li>{@link #CIPHER}: a {@link java.lang.String} that indicates the
    *    algorithm name of the underlying symmetric key block cipher to use with
    *    the designated keystream generators. If this value is undefined, then
    *    the default cipher algorithm for the selected keystream generator
    *    algorithm shall be used (which is {@link Registry#RIJNDAEL_CIPHER} for
    *    both keystream generator algorithms).</li>
    *    <li>{@link IBlockCipher#CIPHER_BLOCK_SIZE}: a {@link java.lang.Integer}
    *    that indicates the block-size to use with the designated symmetric key
    *    block cipher algorithm. If this value is undefined, then the default
    *    block size for the chosen cipher is used.</li>
    *    <li>{@link #TAG_LENGTH}: a {@link java.lang.Integer} that indicates the
    *    length of the resulting authentication tag, if/when the Integrity
    *    Protection service is activated.</li>
    *    <li>{@link #KEY_MATERIAL}: a byte array containing the user-supplied
    *    key material needed to seed the internal keystream generator.</li>
    * </ul>
    *
    * @param attributes the map of attributes to use for this instance.
    */
   public void init(Map attributes) {
      synchronized(lock) {
         String keystreamName = (String) attributes.get(KEYSTREAM);
         if (keystreamName == null) {
            throw new IllegalArgumentException(KEYSTREAM);
         }

         keystream = PRNGFactory.getInstance(keystreamName);
         kAttributes.clear(); // prepare for new values
         cpAttributes.clear();
         ipAttributes.clear();

         // find out which cipher algorithm to use
         String underlyingCipher = (String) attributes.get(CIPHER);
         if (underlyingCipher != null) {
            cpAttributes.put(ICMGenerator.CIPHER, underlyingCipher);
            ipAttributes.put(ICMGenerator.CIPHER, underlyingCipher);
            if (keystream instanceof ICMGenerator) {
               kAttributes.put(ICMGenerator.CIPHER, underlyingCipher);
            } else if (keystream instanceof UMacGenerator) {
               kAttributes.put(UMacGenerator.CIPHER, underlyingCipher);
            } else {
               throw new IllegalArgumentException(KEYSTREAM);
            }
         }

         // did she specify which block size to use it in?
         Integer blockSize = (Integer) attributes.get(IBlockCipher.CIPHER_BLOCK_SIZE);
         if (blockSize != null) {
            kAttributes.put(IBlockCipher.CIPHER_BLOCK_SIZE, blockSize);
            cpAttributes.put(IBlockCipher.CIPHER_BLOCK_SIZE, blockSize);
            ipAttributes.put(IBlockCipher.CIPHER_BLOCK_SIZE, blockSize);
         }

         // get the key material.
         byte[] key = (byte[]) attributes.get(KEY_MATERIAL);
         if (key == null) {
            throw new IllegalArgumentException(KEY_MATERIAL);
         }

         keysize = key.length;
         if (keystream instanceof ICMGenerator) {
            // for an ICMGenerator-based UST, the key material is effectively
            // twice the underlying cipher's desired/needed key size: half of
            // these bytes is the key material per se for the cipher, and the
            // other half shall be used as the "offset" for the ICMGenerator.

            // ensure length is > 0 and is even
            int limit = key.length;
            if (limit < 2) {
               throw new IllegalArgumentException(KEY_MATERIAL);
            } else if ((limit & 0x01) != 0) {
               throw new IllegalArgumentException(KEY_MATERIAL);
            }
            limit /= 2;
            byte[] cipherKey = new byte[limit];
            byte[] offset = new byte[limit];
            System.arraycopy(key, 0,     cipherKey, 0, limit);
            System.arraycopy(key, limit, offset,    0, limit);
            kAttributes.put(IBlockCipher.KEY_MATERIAL, cipherKey);
            kAttributes.put(ICMGenerator.OFFSET, offset);
         } else {
            // if we're here then it's a UMacGenerator and the key is used as is
            kAttributes.put(IBlockCipher.KEY_MATERIAL, key);
         }

         // get the index length. it only makes sense for the ICMGenerator
         // for the UMacGenerator it's always 1
         Integer ndxLength = (Integer) attributes.get(INDEX_LENGTH);
         if (ndxLength != null) {
            if (keystream instanceof ICMGenerator) {
               kAttributes.put(ICMGenerator.SEGMENT_INDEX_LENGTH, ndxLength);
               // max index length is 2 ^ segment-length-in-bits - 1
               maxIndex = BigInteger.valueOf(2L).pow(8*ndxLength.intValue())
                     .subtract(BigInteger.ONE);
            } else if (ndxLength.intValue() != 1) {
               throw new IllegalArgumentException(INDEX_LENGTH);
            } else {
               maxIndex = BigInteger.valueOf(255L);
            }
         } else if (keystream instanceof ICMGenerator) { // we need this value
            throw new IllegalArgumentException(INDEX_LENGTH);
         } else {
            maxIndex = BigInteger.valueOf(255L);
         }

         // the keystream with index 0 is our source for keying material
         // in this implementation we shall use index 0 to compute the key
         // material for the hash function
         if (keystream instanceof ICMGenerator) {
            kAttributes.put(ICMGenerator.SEGMENT_INDEX, BigInteger.ZERO);
         } else {
            kAttributes.put(UMacGenerator.INDEX, new Integer(0));
         }

         // we have everything we need. init the internal keystream generator
         keystream.init(kAttributes);

         index = BigInteger.valueOf(-1L);

         // find out what security services to provide
         Boolean confidentiality = (Boolean) attributes.get(CONFIDENTIALITY);
         if (confidentiality == null) { // by default we dont provide it
            wantConfidentiality = false;
         } else {
            wantConfidentiality = confidentiality.booleanValue();
         }

         if (wantConfidentiality) {
            cpStream = PRNGFactory.getInstance(keystreamName);
         }

         Boolean integrity = (Boolean) attributes.get(INTEGRITY);
         if (integrity == null) { // by default we do provide it
            wantIntegrity = true;
         } else {
            wantIntegrity = integrity.booleanValue();
         }

         if (wantIntegrity) {
            // make sure we have the other generator to provide our prefix
            if (cpStream == null) {
               cpStream = PRNGFactory.getInstance(keystreamName);
            }

            ipStream = PRNGFactory.getInstance(keystreamName);
            // only when integrity protection service is desired do we look for
            // a tag length property
            Integer tagLength = (Integer) attributes.get(TAG_LENGTH);
            if (tagLength == null) {
               throw new IllegalArgumentException(TAG_LENGTH);
            }

            macAttributes.put(TMMH16.TAG_LENGTH, tagLength);
            macLength = tagLength.intValue();
         }

         ready = false;
      }
   }

   /**
    * <p>Signals the start of a new message to process with this <code>UST</code>.
    * </p>
    *
    * @return a byte array containing the representation of the <em>Index</em>
    * used for this message.
    * @throws LimitReachedException if the value of the <em>Index</em> has
    * reached its allowed upper bound. To use this <code>UST</code> instance
    * a new initialisation (with a new user-supplied key material) should occur.
    * @throws InvalidKeyException if the underlying cipher, used in either or
    * both the Integrity Protection and Confidentiality Protection functions
    * has detected an exception.
    */
   public byte[] beginMessage()
   throws LimitReachedException, InvalidKeyException {
      beginMessageWithIndex(index.add(BigInteger.ONE));
      return index.toByteArray();
   }

   /**
    * <p>Signals the start of a new message to process with this <code>UST</code>
    * with a designated <emIndex</em> value.</p>
    *
    * @param ndx the <em>Index</em> to use with the new message.
    * @throws LimitReachedException if the value of the <em>Index</em> has
    * reached its allowed upper bound. To use this <code>UST</code> instance
    * a new initialisation (with a new user-supplied key material) should occur.
    * @throws InvalidKeyException if the underlying cipher, used in either or
    * both the Integrity Protection and Confidentiality Protection functions
    * has detected an exception.
    */
   public void beginMessageWithIndex(int ndx)
   throws LimitReachedException, InvalidKeyException {
      beginMessageWithIndex(ndx);
   }

   /**
    * <p>Signals the start of a new message to process with this <code>UST</code>
    * with a designated <emIndex</em> value.</p>
    *
    * @param ndx the <em>Index</em> to use with the new message.
    * @throws LimitReachedException if the value of the <em>Index</em> has
    * reached its allowed upper bound. To use this <code>UST</code> instance
    * a new initialisation (with a new user-supplied key material) should occur.
    * @throws InvalidKeyException if the underlying cipher, used in either or
    * both the Integrity Protection and Confidentiality Protection functions
    * has detected an exception.
    */
   public void beginMessageWithIndex(BigInteger ndx)
   throws LimitReachedException, InvalidKeyException {
      if (ndx.compareTo(maxIndex) > 0) {
         throw new LimitReachedException();
      }

      index = ndx;

      // depending on the desired services, get keying material from the
      // internal keystream generator and complete the relevant attributes.
      // the key size shall be the same size as the user-supplied key material.
      // remember we need the 1st generator if integrity is required even when
      // confidentiality is not
      if (wantConfidentiality || wantIntegrity) {
         byte[] cpKey = new byte[keysize];
         keystream.nextBytes(cpKey, 0, keysize);
         cpAttributes.put(IBlockCipher.KEY_MATERIAL, cpKey);
         if (cpStream instanceof ICMGenerator) {
            cpAttributes.put(ICMGenerator.SEGMENT_INDEX, index);
         } else {
            cpAttributes.put(UMacGenerator.INDEX, new Integer(index.intValue()));
         }

         cpStream.init(cpAttributes);
      }

      if (wantIntegrity) {
         byte[] ipKey = new byte[keysize];
         keystream.nextBytes(ipKey, 0, keysize);
         ipAttributes.put(IBlockCipher.KEY_MATERIAL, ipKey);
         if (ipStream instanceof ICMGenerator) {
            ipAttributes.put(ICMGenerator.SEGMENT_INDEX, index);
         } else {
            ipAttributes.put(UMacGenerator.INDEX, new Integer(index.intValue()));
         }

         ipStream.init(ipAttributes);

         // get prefix bytes
         byte[] prefix = new byte[macLength];
         cpStream.nextBytes(prefix, 0, macLength);
         macAttributes.put(TMMH16.PREFIX, prefix);

         mac = new TMMH16();
         macAttributes.put(TMMH16.KEYSTREAM, ipStream);
         mac.init(macAttributes);
      }

      ready = true;
   }

   /**
    * <p>Process the <em>Clear</em> part of the message.</p>
    *
    * @param in a byte array containing the <em>Clear</em> part of the message.
    * @param offset the starting index in <code>in</code> where the <em>Clear</em>
    * message bytes should be considered.
    * @param length the count of bytes in <code>in</code>, starting from
    * <code>offset</code> to consider.
    * @throws IllegalStateException if no start-of-message method has been
    * invoked earlier or the Integrity Protection service has not been activated.
    */
   public void doClear(byte[] in, int offset, int length) {
      if (!ready) {
         throw new IllegalStateException();
      }
      if (!wantIntegrity) {
         throw new IllegalStateException();
      }

      mac.update(in, offset, length);
   }

   /**
    * <p>Process the <em>Opaque</em> part of the message.</p>
    *
    * @param in a byte array containing the <em>Clear</em> part of the message.
    * @param inOffset the starting index in <code>in</code> where the
    * <em>Opaque</em> message bytes should be considered.
    * @param length the count of bytes in <code>in</code>, starting from
    * <code>inOffset</code> to consider.
    * @param out the byte array where the enciphered opaque message should be
    * stored.
    * @param outOffset the starting offset in <code>out</code> where the
    * enciphered bytes should be stored.
    * @throws IllegalStateException if no start-of-message method has been
    * invoked earlier.
    * @throws LimitReachedException if one or both of the underlying keystream
    * generators have reached their limit.
    */
   public void
   doOpaque(byte[] in, int inOffset, int length, byte[] out, int outOffset)
   throws LimitReachedException {
      if (!ready) {
         throw new IllegalStateException();
      }
      if (wantIntegrity) {
         mac.update(in, inOffset, length);
      }

      if (wantConfidentiality) {
         byte[] suffix = new byte[length];
         cpStream.nextBytes(suffix, 0, length);
         for (int i = 0; i < length; ) {
            out[outOffset++] = (byte)(in[inOffset++] ^ suffix[i++]);
         }
      } else {
         System.arraycopy(in, inOffset, out, outOffset, length);
      }
   }

   /**
    * <p>Signals the end of the message transformation.</p>
    *
    * @return the authentication tag bytes, if and when the Integrity Protection
    * service was specified; otherwise a 0-long byte array is returned.
    * @throws IllegalStateException if no start-of-message method has been
    * invoked earlier.
    */
   public byte[] endMessage() {
      if (!ready) {
         throw new IllegalStateException();
      }
      if (!wantIntegrity) {
         return new byte[0];
      }

      byte[] result = mac.digest();
      reset();
      return result;
   }

   /**
    * <p>Reset this instance and prepare for processing a new message.</p>
    */
   public void reset() {
      ready = false;
      if (wantIntegrity) {
         mac.reset();
      }
   }

   public boolean selfTest() {
      if (valid == null) {
         try {
            // TODO: compute and test equality with one known vector
            UST ust = new UST();
            Map attributes = new HashMap();
            attributes.put(KEYSTREAM, Registry.UMAC_PRNG);
            attributes.put(TAG_LENGTH, new Integer(4));
            attributes.put(KEY_MATERIAL, "abcdefghijklmnop".getBytes("ASCII"));
            attributes.put(CONFIDENTIALITY, Boolean.TRUE);
            attributes.put(INTEGRITY, Boolean.TRUE);
            ust.init(attributes);

            ust.beginMessage();
            ust.doClear("Giambattista Bodoni".getBytes("ASCII"), 0, 19);
            byte[] out = new byte[17];
            ust.doOpaque("Que du magnifique".getBytes("ASCII"), 0, 17, out, 0);
            // System.out.println("ust.opaque=0x"+Util.toString(out));
            // 0x45BED162FED5591B74ACB0E5CA742295FF
            byte[] tag = ust.endMessage();
            // System.out.println("ust.tag=0x"+Util.toString(tag));
            // 0x3489BA84

            valid = Boolean.TRUE;
         } catch (Exception x) {
            x.printStackTrace(System.err);
            valid = Boolean.FALSE;
         }
      }
      return valid.booleanValue();
   }
}

