package gnu.crypto.mac;

// ----------------------------------------------------------------------------
// $Id: BaseMac.java,v 1.2 2004/10/17 19:06:55 robilad Exp $
//
// Copyright (C) 2001, 2002, Free Software Foundation, Inc.
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
//    59 Temple Place - Suite 330,
//    Boston, MA 02111-1307
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

import gnu.crypto.hash.IMessageDigest;

import java.util.Map;
import java.security.InvalidKeyException;

/**
 * <p>A base abstract class to facilitate <i>MAC</i> (Message Authentication
 * Code) implementations.</p>
 *
 * @version $Revision: 1.2 $
 */
public abstract class BaseMac implements IMac {

   // Constants and variables
   // -------------------------------------------------------------------------

   /** The canonical name prefix of the <i>MAC</i>. */
   protected String name;

   /** Reference to the underlying hash algorithm instance. */
   protected IMessageDigest underlyingHash;

   /** The length of the truncated output in bytes. */
   protected int truncatedSize;

   /** The authentication key for this instance. */
//   protected transient byte[] K;

   // Constructor(s)
   // -------------------------------------------------------------------------

   /**
    * <p>Trivial constructor for use by concrete subclasses.</p>
    *
    * @param name the canonical name of this instance.
    */
   protected BaseMac(String name) {
      super();

      this.name = name;
   }

   /**
    * <p>Trivial constructor for use by concrete subclasses.</p>
    *
    * @param name the canonical name of this instance.
    * @param underlyingHash the underlying message digest algorithm instance.
    */
   protected BaseMac(String name, IMessageDigest underlyingHash) {
      this(name);

      if (underlyingHash != null) {
         truncatedSize = underlyingHash.hashSize();
      }
      this.underlyingHash = underlyingHash;
   }

   // Class methods
   // -------------------------------------------------------------------------

   // Instance methods
   // -------------------------------------------------------------------------

   // gnu.crypto.mac.IMac interface implementation ----------------------------

   public String name() {
      return name;
   }

   public int macSize() {
      return truncatedSize;
   }

   public void update(byte b) {
      underlyingHash.update(b);
   }

   public void update(byte[] b, int offset, int len) {
      underlyingHash.update(b, offset, len);
   }

   public void reset() {
      underlyingHash.reset();
   }

   public Object clone() throws CloneNotSupportedException
   {
      return super.clone();
   }

   // methods to be implemented by concrete subclasses ------------------------

   public abstract void init(Map attributes)
   throws InvalidKeyException, IllegalStateException;

   public abstract byte[] digest();

   public abstract boolean selfTest();
}
