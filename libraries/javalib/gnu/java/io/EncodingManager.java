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

import java.lang.reflect.Constructor;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.util.Hashtable;
import java.util.StringTokenizer;
import gnu.java.io.decode.Decoder;
import gnu.java.io.encode.Encoder;


import java.io.FileOutputStream;

/**
  * This class is used to create new instances of Decoders for a specified
  * encoding scheme.  These instances are cache for fast subsequent retrieval 
  * if necessary.
  *
  * @version 0.0
  *
  * @author Aaron M. Renn (arenn@urbanophile.com)
  */
public class EncodingManager
{

/*************************************************************************/

/*
 * Class Variables
 */

/**
  * This is the encoding class search path
  */
private static String encoding_path;

/**
  * This is the system default character encoding
  */
private static String default_encoding;

/**
  * This is the <code>Constructor</code> for the default <code>Decoder</code>
  */
private static Constructor default_decoder_cons;

/**
  * This is the <code>Constructor</code> for the default <code>Encoder</code>
  */
private static Constructor default_encoder_cons;

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
  * This is our hash table of previously loaded <code>Decoder</code> classes
  */
private static Hashtable decoder_cons;

/**
  * This is hash table of cached instances of <code>Decoder</code> objects
  */
private static Hashtable decoder_instances;

/**
  * This is our hash table of previously loaded <code>Encoder</code> classes
  */
private static Hashtable encoder_cons;

/**
  * This is hash table of cached instances of <code>Encoder</code> objects
  */
private static Hashtable encoder_instances;


static 
{
  // Initialize hashtables
  decoder_cons = new Hashtable();
  encoder_cons = new Hashtable();
  decoder_instances = new Hashtable();
  encoder_instances = new Hashtable();

  // Find the system default decoder search path
  encoding_path = System.getProperty("file.encoding.pkg");
  if (encoding_path == null)
    encoding_path = "gnu.java.io";
  else
    encoding_path = encoding_path + ":gnu.java.io";

  // Find the system default encoding name
  String default_encoding = System.getProperty("file.encoding","8859_1");

  // Load the class
  try
    {
      // First the Decoder side
      default_decoder_cons = findDecoderConstructor(default_encoding, true);

      Object[] objs = new Object[1];
      objs[0] = null;

      default_decoder_instance = 
            (Decoder)default_decoder_cons.newInstance(objs);
        
      // Now the Encoder side
      default_encoder_cons = findEncoderConstructor(default_encoding, true);

      objs = new Object[1];
      objs[0] = null;

      default_encoder_instance = 
            (Encoder)default_encoder_cons.newInstance(objs);
        
      // Add items to the hashtable;
      decoder_cons.put(default_encoding, default_decoder_cons);
      encoder_cons.put(default_encoding, default_encoder_cons);
      decoder_instances.put(default_encoding, default_decoder_instance);
      encoder_instances.put(default_encoding, default_encoder_instance);
    }
  catch(Exception e)
    {
      throw new Error("Cannot load system default encoding '" + 
                     default_encoding + "': " + e.getMessage());
    }
}  

/*************************************************************************/

/*
 * Class Methods
 */

/**
  * This method loads a <code>Decoder</code> class for the given
  * encoding name.
  *
  * @exception UnsupportedEncodingException If a <code>Decoder</code> for this encoding cannot be found.
  */
private static Constructor
findDecoderConstructor(String encoding, boolean cache) 
                             throws UnsupportedEncodingException
{
  // First check for an aliased encoding name
  String alias = System.getProperty("gnu.java.io.encoding_scheme_alias." + 
                                    encoding);
  if (alias != null)
    encoding = alias;

  StringTokenizer st = new StringTokenizer(encoding_path, ":");

  while (st.hasMoreTokens())
    {
      String classname = st.nextToken() + ".decode.Decoder" + encoding;
      try
        {
          Class cls = Class.forName(classname);

          Class[] params = new Class[1];
          params[0] = InputStream.class;

          Constructor cons = cls.getConstructor(params);

          if (cache)
            decoder_cons.put(encoding, cons);            

          return(cons);
        }
      catch(Exception e) { ; }
    }

  throw new UnsupportedEncodingException(encoding);
}
       
/*************************************************************************/

/**
  * This method loads an <code>Encoder</code> class for the given
  * encoding name.
  *
  * @exception UnsupportedEncodingException If a <code>Encoder</code> for this encoding cannot be found.
  */
private static Constructor
findEncoderConstructor(String encoding, boolean cache) 
                             throws UnsupportedEncodingException
{
  // First check for an aliased encoding name
  String alias = System.getProperty("gnu.java.io.encoding_scheme_alias." + 
                                    encoding);
  if (alias != null)
    encoding = alias;

  StringTokenizer st = new StringTokenizer(encoding_path, ":");

  while (st.hasMoreTokens())
    {
      String classname = st.nextToken() + ".encode.Encoder" + encoding;
      try
        {
          Class cls = Class.forName(classname);

          Class[] params = new Class[1];
          params[0] = OutputStream.class;

          Constructor cons = cls.getConstructor(params);

          if (cache)
            encoder_cons.put(encoding, cons);            

          return(cons);
        }
      catch(Exception e) { ; }
    }

  throw new UnsupportedEncodingException(encoding);
}
       
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
getDecoder()
{
  return(default_decoder_instance);
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
  Object[] params = new Object[1];
  params[0] = in;

  Decoder dec = null;
  try
    {
      dec = (Decoder)default_decoder_cons.newInstance(params);
    }
  catch(Exception e)
    {
      throw new Error("Unexpected problems with default decoder");
    }

  return(dec);
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
  * @param cache <code>true</code> to cache the returned <code>Decoder</code>, <code>false</code> otherwise.
  *
  * @exception UnsupportedEncodingException If a <code>Decoder</code> for this encoding cannot be found
  */
public static Decoder
getDecoder(InputStream in, String encoding, boolean cache) 
                              throws UnsupportedEncodingException
{
  Constructor cons = findDecoderConstructor(encoding, cache);
  Object[] params = new Object[1];
  params[0] = in;

  Decoder dec = null;
  try
    {
      dec = (Decoder)cons.newInstance(params); 
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
  return(default_encoder_instance);
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
  Object[] params = new Object[1];
  params[0] = out;

  Encoder enc = null;
  try
    {
      enc = (Encoder)default_encoder_cons.newInstance(params);
    }
  catch(Exception e)
    {
      throw new Error("Unexpected problems with default decoder");
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
  * @param cache <code>true</code> to cache the returned <code>Encoder</code>, <code>false</code> otherwise.
  *
  * @exception UnsupportedEncodingException If a <code>Decoder</code> for this encoding cannot be found
  */
public static Encoder
getEncoder(OutputStream out, String encoding, boolean cache) 
                              throws UnsupportedEncodingException
{
  Constructor cons = findEncoderConstructor(encoding, cache);
  Object[] params = new Object[1];
  params[0] = out;

  Encoder enc = null;
  try
    {
      enc = (Encoder)cons.newInstance(params); 
    }
  catch(Exception e)
    {
      throw new UnsupportedEncodingException(encoding + ": " + e.getMessage());
    }

  return(enc);
}

} // class EncodingManager

