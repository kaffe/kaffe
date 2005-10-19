package gnu.crypto.jce.sig;

// ----------------------------------------------------------------------------
// $Id: RSAKeyPairGeneratorSpi.java,v 1.1 2005/10/19 20:15:38 guilhem Exp $
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
import gnu.crypto.key.rsa.RSAKeyPairGenerator;

import java.security.InvalidAlgorithmParameterException;
import java.security.SecureRandom;
import java.security.spec.AlgorithmParameterSpec;
import java.security.spec.RSAKeyGenParameterSpec;
import java.util.HashMap;

/**
 * The implementation of a {@link java.security.KeyPairGenerator} adapter class
 * to wrap gnu.crypto RSA keypair generator instances.<p>
 *
 * In case the client does not explicitly initialize the KeyPairGenerator (via
 * a call to an <code>initialize()</code> method), the GNU Crypto provider
 * uses a default <i>modulus</i> size (keysize) of 1024 bits.<p>
 *
 * @version $Revision: 1.1 $
 */
public class RSAKeyPairGeneratorSpi extends KeyPairGeneratorAdapter {

   // Constants and variables
   // -------------------------------------------------------------------------

   // Constructor(s)
   // -------------------------------------------------------------------------

   public RSAKeyPairGeneratorSpi() {
      super(Registry.RSA_KPG);
   }

   // Class methods
   // -------------------------------------------------------------------------

   // Instance methods
   // -------------------------------------------------------------------------

   public void initialize(int keysize, SecureRandom random) {
      HashMap attributes = new HashMap();
      attributes.put(RSAKeyPairGenerator.MODULUS_LENGTH, new Integer(keysize));
      if (random != null) {
         attributes.put(RSAKeyPairGenerator.SOURCE_OF_RANDOMNESS, random);
      }

      adaptee.setup(attributes);
   }

   public void initialize(AlgorithmParameterSpec params, SecureRandom random)
   throws InvalidAlgorithmParameterException {
      HashMap attributes = new HashMap();
      if (params != null) {
         if (!(params instanceof RSAKeyGenParameterSpec)) {
            throw new InvalidAlgorithmParameterException("params");
         }

         attributes.put(RSAKeyPairGenerator.RSA_PARAMETERS, params);
      }

      if (random != null) {
         attributes.put(RSAKeyPairGenerator.SOURCE_OF_RANDOMNESS, random);
      }

      adaptee.setup(attributes);
   }
}
