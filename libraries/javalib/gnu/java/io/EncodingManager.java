/* EncodingManager.java -- Manages character encoding translators
   Copyright (C) 1998, 1999 Free Software Foundation, Inc.

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


package gnu.java.io;

import gnu.java.io.decode.Decoder;
import gnu.java.io.decode.KaffeDecoder;
import gnu.java.io.encode.Encoder;
import gnu.java.io.encode.KaffeEncoder;

import java.io.InputStream;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.util.Hashtable;

// gnu.java.io.EncodingManager that works with kaffe

/**
  * This class is used to create new instances of Decoders for a specified
  * encoding scheme.  These instances are cache for fast subsequent retrieval 
  * if necessary.
  *
  * @version 0.0
  *
  * @author Aaron M. Renn (arenn@urbanophile.com),
  * Ito Kazumitsu (kaz@maczuka.gcd.org)
  */
public class EncodingManager
{

/*************************************************************************/

/*
 * Class Variables
 */

/**
  * This is the system default character encoding
  */
private static String default_encoding = "Default";

/**
  * This is the default instance of the default <code>Decoder</code>, put
  * here to make access even faster than through the Hashtable
  */
private static Decoder default_decoder_instance;

/**
  * This is the default instance of the default <code>Encoder</code>, put
  * here to make access even faster than through the Hashtable
  */
private static Encoder default_encoder_instance;

/**
  * This is hash table of cached instances of <code>Decoder</code> objects
  */
private static Hashtable decoder_instances;

/**
  * This is hash table of cached instances of <code>Encoder</code> objects
  */
private static Hashtable encoder_instances;


/*
static 
{
  // Initialize hashtables
  decoder_instances = new Hashtable();
  encoder_instances = new Hashtable();

  // Find the system default encoding name
  // String default_encoding = System.getProperty("file.encoding","8859_1");
  // I am not sure whether System has already been initialized.
  String default_encoding = "Default";

  // Load the class
  try
    {
      // First the Decoder side

      default_decoder_instance = 
            new KaffeDecoder(null, default_encoding);
        
      // Now the Encoder side

      default_encoder_instance = 
            new KaffeEncoder(null, default_encoding);
        
      // Add items to the hashtable;
      decoder_instances.put(default_encoding, default_decoder_instance);
      encoder_instances.put(default_encoding, default_encoder_instance);
    }
  catch(Exception e)
    {
      throw new Error("Cannot load system default encoding '" + 
                     default_encoding + "': " + e.getMessage());
    }
}
*/  

/*************************************************************************/

/*
 * Class Methods
 */

/*************************************************************************/

/**
  * This method returns the default instance of the default <code>Decoder</code> 
  * which must be used only for calling the static byte array conversion methods.
  * Calling any instance methods on this object will result in a 
  * <code>NullPointerException</code>.
  *
  * @return An instance of the default <code>Decoder</code>.
  */
public static Decoder
getDecoder() throws UnsupportedEncodingException
{
  // return(default_decoder_instance);
  // return(new KaffeDecoder(null, default_encoding));
  return(new KaffeDecoder(null, "Default"));
}

/*************************************************************************/

/**
  * This method returns the default instance of the <code>Decoder</code>
  * for the named encoding.  This must be used only for calling the static
  * byte array conversion methods.  Calling any instance methods on this
  * object will result in a <code>NullPointerException</code>
  *
  * This form of <code>getDecoder</code> caches the instance that is returned.  If
  * this decoder is for a complex character encoding that may use lots of
  * memory and is only needed once or infrequently, consider using the form 
  * of the <code>getDecoder</code> method that does not cache the results 
  * to save resources.
  *
  * @param encoding The name of the encoding to retrieve a <code>Decoder</code> for.
  *
  * @return An instance of the <code>Decoder</code> for the named encoding.
  *
  * @exception UnsupportedEncodingException If a <code>Decoder</code> for the named encoding cannot be found
  */
public static Decoder
getDecoder(String encoding) throws UnsupportedEncodingException
{
  return(getDecoder(encoding, true));
}

/*************************************************************************/

/**
  * This method returns the default instance of the <code>Decoder</code>
  * for the named encoding.  This must be used only for calling the static
  * byte array conversion methods.  Calling any instance methods on this
  * object will result in a <code>NullPointerException</code>
  *
  * @param encoding The name of the encoding to retrieve a <code>Decoder</code> for.
  * @param cache <code>true</code> to cache this encoding, <code>false</code> otherwise
  *
  * @return An instance of the <code>Decoder</code> for the named encoding.
  *
  * @exception UnsupportedEncodingException If a <code>Decoder</code> for the named encoding cannot be found
  */
public static Decoder
getDecoder(String encoding, boolean cache) throws UnsupportedEncodingException
{
  if (decoder_instances == null) decoder_instances = new Hashtable();
  Decoder dec = (Decoder)decoder_instances.get(encoding);
  if (dec != null)
    return(dec);
 
  dec = getDecoder(null, encoding, cache);

  if (cache)
    decoder_instances.put(encoding, dec);

  return(dec);
}

/*************************************************************************/

/**
  * This method returns a <code>Decoder</code> object that can read
  * from the specified <code>InputStream</code> using the default 
  * encoding.
  *
  * @param in The <code>InputStream</code> to read from
  */
public static Decoder
getDecoder(InputStream in)
{
  try
    {
      // return(getDecoder(in, default_encoding, false));
      return(getDecoder(in, "Default", false));
    }
  catch(Exception e)
    {
      throw new Error("Unexpected problems with default decoder");
    }
}

/*************************************************************************/

/**
  * This method returns a <code>Decoder</code> object that can read from
  * the specified <code>InputStream</code> using the named encoding
  *
  * This form of <code>getDecoder</code> caches the instance that is returned.  If
  * this decoder is for a complex character encoding that may use lots of
  * memory and is only needed once or infrequently, consider using the form 
  * of the <code>getDecoder</code> method that does not cache the results 
  * to save resources.
  *
  * @param in The <code>InputStream</code> to read from
  * @param encoding The name of the character encoding scheme to use
  *
  * @exception UnsupportedEncodingException If a <code>Decoder</code> for this encoding cannot be found
  */
public static Decoder
getDecoder(InputStream in, String encoding) throws UnsupportedEncodingException
{
  return(getDecoder(in, encoding, true));
}

/*************************************************************************/

/**
  * This method returns a <code>Decoder</code> object that can read from
  * the specified <code>InputStream</code> using the named encoding
  *
  * @param in The <code>InputStream</code> to read from
  * @param encoding The name of the character encoding scheme to use
  * @param cache <code>true</code> to cache the returned <code>Decoder</code>, <code>false</code> otherwise.  Actually, not used.
  *
  * @exception UnsupportedEncodingException If a <code>Decoder</code> for this encoding cannot be found
  */
public static Decoder
getDecoder(InputStream in, String encoding, boolean cache) 
                              throws UnsupportedEncodingException
{
  Decoder dec = null;
  try
    {
      dec = new KaffeDecoder(in, encoding);
    }
  catch(Exception e)
    {
      throw new UnsupportedEncodingException(encoding + ": " + e.getMessage());
    }

  return(dec);
}

/*************************************************************************/

/**
  * This method returns the default instance of the default <code>Encoder</code> 
  * which must be used only for calling the static byte array conversion methods.
  * Calling any instance methods on this object will result in a 
  * <code>NullPointerException</code>.
  *
  * @return An instance of the default <code>Encoder</code>.
  */
public static Encoder
getEncoder()
{
  // return(default_encoder_instance);
  return(new KaffeEncoder(null, "Default"));
}

/*************************************************************************/

/**
  * This method returns the default instance of the <code>Encoder</code>
  * for the named encoding.  This must be used only for calling the static
  * byte array conversion methods.  Calling any instance methods on this
  * object will result in a <code>NullPointerException</code>
  *
  * This form of <code>getEncoder</code> caches the instance that is returned.  If
  * this decoder is for a complex character encoding that may use lots of
  * memory and is only needed once or infrequently, consider using the form 
  * of the <code>getEncoder</code> method that does not cache the results 
  * to save resources.
  *
  * @param encoding The name of the encoding to retrieve a <code>Encoder</code> for.
  *
  * @return An instance of the <code>Encoder</code> for the named encoding.
  *
  * @exception UnsupportedEncodingException If a <code>Encoder</code> for the named encoding cannot be found
  */
public static Encoder
getEncoder(String encoding) throws UnsupportedEncodingException
{
  return(getEncoder(encoding, true));
}

/*************************************************************************/

/**
  * This method returns the default instance of the <code>Encoder</code>
  * for the named encoding.  This must be used only for calling the static
  * byte array conversion methods.  Calling any instance methods on this
  * object will result in a <code>NullPointerException</code>
  *
  * @param encoding The name of the encoding to retrieve a <code>Encoder</code> for.
  * @param cache <code>true</code> to cache this encoding, <code>false</code> otherwise
  *
  * @return An instance of the <code>Encoder</code> for the named encoding.
  *
  * @exception UnsupportedEncodingException If a <code>Encoder</code> for the named encoding cannot be found
  */
public static Encoder
getEncoder(String encoding, boolean cache) throws UnsupportedEncodingException
{
  if (encoder_instances == null) encoder_instances = new Hashtable();
  Encoder enc = (Encoder)encoder_instances.get(encoding);
  if (enc != null)
    return(enc);
 
  enc = getEncoder(null, encoding, cache);

  if (cache)
    encoder_instances.put(encoding, enc);

  return(enc);
}

/*************************************************************************/

/**
  * This method returns an <code>Encoder</code> object that can write
  * to the specified <code>OutputStream</code> using the default 
  * encoding.
  *
  * @param out The <code>OutputStream</code> to read from
  */
public static Encoder
getEncoder(OutputStream out)
{
  Encoder enc = null;
  try
    {
      // enc = getEncoder(out, default_encoding, false);
      enc = getEncoder(out, "Default", false);
    }
  catch(Exception e)
    {
      Error err = new Error("Unexpected problems with default encoder: an exception was received");
      err.initCause(e);

      throw err;
    }

  return(enc);
}

/*************************************************************************/

/**
  * This method returns an <code>Encoder</code> object that can write to
  * the specified <code>OutputStream</code> using the named encoding
  *
  * This form of <code>getencoder</code> caches the instance that is returned.  If
  * this encoder is for a complex character encoding that may use lots of
  * memory and is only needed once or infrequently, consider using the form 
  * of the <code>getEncoder</code> method that does not cache the results 
  * to save resources.
  *
  * @param in The <code>OutputStream</code> to read from
  * @param encoding The name of the character encoding scheme to use
  *
  * @exception UnsupportedEncodingException If an <code>Encoder</code> for this encoding cannot be found
  */
public static Encoder
getEncoder(OutputStream in, String encoding) throws UnsupportedEncodingException
{
  return(getEncoder(in, encoding, true));
}

/*************************************************************************/

/**
  * This method returns an <code>Encoder</code> object that can write to
  * the specified <code>OutputStream</code> using the named encoding
  *
  * @param in The <code>OutputStream</code> to read from
  * @param encoding The name of the character encoding scheme to use
  * @param cache <code>true</code> to cache the returned <code>Encoder</code>, <code>false</code> otherwise. Actually, not used.
  *
  * @exception UnsupportedEncodingException If a <code>Decoder</code> for this encoding cannot be found
  */
public static Encoder
getEncoder(OutputStream out, String encoding, boolean cache) 
                              throws UnsupportedEncodingException
{
  Encoder enc = null;
  try
    {
      enc = new KaffeEncoder(out, encoding); 
    }
  catch(Exception e)
    {
      UnsupportedEncodingException e2 = new UnsupportedEncodingException(encoding + ": " + e.getMessage());

      e2.initCause(e);
      throw e2;
    }

  return(enc);
}

} // class EncodingManager

