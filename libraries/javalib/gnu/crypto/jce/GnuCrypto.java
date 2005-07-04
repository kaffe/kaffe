package gnu.crypto.jce;

// --------------------------------------------------------------------------
// $Id: GnuCrypto.java,v 1.3 2005/07/04 00:04:42 robilad Exp $
//
// Copyright (C) 2001, 2002, 2003, 2004 Free Software Foundation, Inc.
//
// This file is part of GNU Crypto.
//
// GNU Crypto is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2 of the License, or (at
// your option) any later version.
//
// GNU Crypto is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the
//
//    Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor,
//    Boston, MA  02110-1301
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
//
// --------------------------------------------------------------------------

import gnu.crypto.Registry;
import gnu.crypto.cipher.CipherFactory;
import gnu.crypto.mac.MacFactory;

import java.security.AccessController;
import java.security.PrivilegedAction;
import java.security.Provider;
import java.util.HashSet;
import java.util.Set;

/**
 * <p>The GNU Crypto implementation of the Java Cryptographic Extension (JCE)
 * Provider.</p>
 *
 * @version $Revision: 1.3 $
 * @see java.security.Provider
 */
public final class GnuCrypto extends Provider {

   // Constants and variables
   // -------------------------------------------------------------------------

   // Constructor(s)
   // -------------------------------------------------------------------------

   /**
    * <p>The <a href="http://www.gnu.org/software/gnu-crypto/">GNU Crypto</a>
    * Provider.</p>
    */
   public GnuCrypto() {
      super(Registry.GNU_CRYPTO, 2.1, "GNU Crypto JCE Provider");

      AccessController.doPrivileged(
         new PrivilegedAction() {
            public Object run() {
               // Cipher
               put("Cipher.ANUBIS", gnu.crypto.jce.cipher.AnubisSpi.class.getName());
               put("Cipher.ANUBIS ImplementedIn", "Software");
               put("Cipher.ARCFOUR", gnu.crypto.jce.cipher.ARCFourSpi.class.getName());
               put("Cipher.ARCFOUR ImplementedIn", "Software");
               put("Cipher.BLOWFISH", gnu.crypto.jce.cipher.BlowfishSpi.class.getName());
               put("Cipher.BLOWFISH ImplementedIn", "Software");
               put("Cipher.DES", gnu.crypto.jce.cipher.DESSpi.class.getName());
               put("Cipher.DES ImplementedIn", "Software");
               put("Cipher.KHAZAD", gnu.crypto.jce.cipher.KhazadSpi.class.getName());
               put("Cipher.KHAZAD ImplementedIn", "Software");
               put("Cipher.NULL", gnu.crypto.jce.cipher.NullCipherSpi.class.getName());
               put("Cipher.NULL ImplementedIn", "Software");
               put("Cipher.AES", gnu.crypto.jce.cipher.RijndaelSpi.class.getName());
               put("Cipher.AES ImplementedIn", "Software");
               put("Cipher.RIJNDAEL", gnu.crypto.jce.cipher.RijndaelSpi.class.getName());
               put("Cipher.RIJNDAEL ImplementedIn", "Software");
               put("Cipher.SERPENT", gnu.crypto.jce.cipher.SerpentSpi.class.getName());
               put("Cipher.SERPENT ImplementedIn", "Software");
               put("Cipher.SQUARE", gnu.crypto.jce.cipher.SquareSpi.class.getName());
               put("Cipher.SQUARE ImplementedIn", "Software");
               put("Cipher.TRIPLEDES", gnu.crypto.jce.cipher.TripleDESSpi.class.getName());
               put("Cipher.TRIPLEDES ImplementedIn", "Software");
               put("Cipher.TWOFISH", gnu.crypto.jce.cipher.TwofishSpi.class.getName());
               put("Cipher.TWOFISH ImplementedIn", "Software");
               put("Cipher.CAST5", gnu.crypto.jce.cipher.Cast5Spi.class.getName());
               put("Cipher.CAST5 ImplementedIn", "Software");

               // PBES2 ciphers.
               put("Cipher.PBEWithHMacHavalAndAES",
                   gnu.crypto.jce.cipher.PBES2.HMacHaval.AES.class.getName());
               put("Cipher.PBEWithHMacHavalAndAnubis",
                   gnu.crypto.jce.cipher.PBES2.HMacHaval.Anubis.class.getName());
               put("Cipher.PBEWithHMacHavalAndBlowfish",
                   gnu.crypto.jce.cipher.PBES2.HMacHaval.Blowfish.class.getName());
               put("Cipher.PBEWithHMacHavalAndCast5",
                   gnu.crypto.jce.cipher.PBES2.HMacHaval.Cast5.class.getName());
               put("Cipher.PBEWithHMacHavalAndDES",
                   gnu.crypto.jce.cipher.PBES2.HMacHaval.DES.class.getName());
               put("Cipher.PBEWithHMacHavalAndKhazad",
                   gnu.crypto.jce.cipher.PBES2.HMacHaval.Khazad.class.getName());
               put("Cipher.PBEWithHMacHavalAndSerpent",
                   gnu.crypto.jce.cipher.PBES2.HMacHaval.Serpent.class.getName());
               put("Cipher.PBEWithHMacHavalAndSquare",
                   gnu.crypto.jce.cipher.PBES2.HMacHaval.Square.class.getName());
               put("Cipher.PBEWithHMacHavalAndTripleDES",
                   gnu.crypto.jce.cipher.PBES2.HMacHaval.TripleDES.class.getName());
               put("Cipher.PBEWithHMacHavalAndTwofish",
                   gnu.crypto.jce.cipher.PBES2.HMacHaval.Twofish.class.getName());

               put("Cipher.PBEWithHMacMD2AndAES",
                   gnu.crypto.jce.cipher.PBES2.HMacMD2.AES.class.getName());
               put("Cipher.PBEWithHMacMD2AndAnubis",
                   gnu.crypto.jce.cipher.PBES2.HMacMD2.Anubis.class.getName());
               put("Cipher.PBEWithHMacMD2AndBlowfish",
                   gnu.crypto.jce.cipher.PBES2.HMacMD2.Blowfish.class.getName());
               put("Cipher.PBEWithHMacMD2AndCast5",
                   gnu.crypto.jce.cipher.PBES2.HMacMD2.Cast5.class.getName());
               put("Cipher.PBEWithHMacMD2AndDES",
                   gnu.crypto.jce.cipher.PBES2.HMacMD2.DES.class.getName());
               put("Cipher.PBEWithHMacMD2AndKhazad",
                   gnu.crypto.jce.cipher.PBES2.HMacMD2.Khazad.class.getName());
               put("Cipher.PBEWithHMacMD2AndSerpent",
                   gnu.crypto.jce.cipher.PBES2.HMacMD2.Serpent.class.getName());
               put("Cipher.PBEWithHMacMD2AndSquare",
                   gnu.crypto.jce.cipher.PBES2.HMacMD2.Square.class.getName());
               put("Cipher.PBEWithHMacMD2AndTripleDES",
                   gnu.crypto.jce.cipher.PBES2.HMacMD2.TripleDES.class.getName());
               put("Cipher.PBEWithHMacMD2AndTwofish",
                   gnu.crypto.jce.cipher.PBES2.HMacMD2.Twofish.class.getName());

               put("Cipher.PBEWithHMacMD4AndAES",
                   gnu.crypto.jce.cipher.PBES2.HMacMD4.AES.class.getName());
               put("Cipher.PBEWithHMacMD4AndAnubis",
                   gnu.crypto.jce.cipher.PBES2.HMacMD4.Anubis.class.getName());
               put("Cipher.PBEWithHMacMD4AndBlowfish",
                   gnu.crypto.jce.cipher.PBES2.HMacMD4.Blowfish.class.getName());
               put("Cipher.PBEWithHMacMD4AndCast5",
                   gnu.crypto.jce.cipher.PBES2.HMacMD4.Cast5.class.getName());
               put("Cipher.PBEWithHMacMD4AndDES",
                   gnu.crypto.jce.cipher.PBES2.HMacMD4.DES.class.getName());
               put("Cipher.PBEWithHMacMD4AndKhazad",
                   gnu.crypto.jce.cipher.PBES2.HMacMD4.Khazad.class.getName());
               put("Cipher.PBEWithHMacMD4AndSerpent",
                   gnu.crypto.jce.cipher.PBES2.HMacMD4.Serpent.class.getName());
               put("Cipher.PBEWithHMacMD4AndSquare",
                   gnu.crypto.jce.cipher.PBES2.HMacMD4.Square.class.getName());
               put("Cipher.PBEWithHMacMD4AndTripleDES",
                   gnu.crypto.jce.cipher.PBES2.HMacMD4.TripleDES.class.getName());
               put("Cipher.PBEWithHMacMD4AndTwofish",
                   gnu.crypto.jce.cipher.PBES2.HMacMD4.Twofish.class.getName());

               put("Cipher.PBEWithHMacMD5AndAES",
                   gnu.crypto.jce.cipher.PBES2.HMacMD5.AES.class.getName());
               put("Cipher.PBEWithHMacMD5AndAnubis",
                   gnu.crypto.jce.cipher.PBES2.HMacMD5.Anubis.class.getName());
               put("Cipher.PBEWithHMacMD5AndBlowfish",
                   gnu.crypto.jce.cipher.PBES2.HMacMD5.Blowfish.class.getName());
               put("Cipher.PBEWithHMacMD5AndCast5",
                   gnu.crypto.jce.cipher.PBES2.HMacMD5.Cast5.class.getName());
               put("Cipher.PBEWithHMacMD5AndDES",
                   gnu.crypto.jce.cipher.PBES2.HMacMD5.DES.class.getName());
               put("Cipher.PBEWithHMacMD5AndKhazad",
                   gnu.crypto.jce.cipher.PBES2.HMacMD5.Khazad.class.getName());
               put("Cipher.PBEWithHMacMD5AndSerpent",
                   gnu.crypto.jce.cipher.PBES2.HMacMD5.Serpent.class.getName());
               put("Cipher.PBEWithHMacMD5AndSquare",
                   gnu.crypto.jce.cipher.PBES2.HMacMD5.Square.class.getName());
               put("Cipher.PBEWithHMacMD5AndTripleDES",
                   gnu.crypto.jce.cipher.PBES2.HMacMD5.TripleDES.class.getName());
               put("Cipher.PBEWithHMacMD5AndTwofish",
                   gnu.crypto.jce.cipher.PBES2.HMacMD5.Twofish.class.getName());

               put("Cipher.PBEWithHMacSHA1AndAES",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA1.AES.class.getName());
               put("Cipher.PBEWithHMacSHA1AndAnubis",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA1.Anubis.class.getName());
               put("Cipher.PBEWithHMacSHA1AndBlowfish",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA1.Blowfish.class.getName());
               put("Cipher.PBEWithHMacSHA1AndCast5",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA1.Cast5.class.getName());
               put("Cipher.PBEWithHMacSHA1AndDES",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA1.DES.class.getName());
               put("Cipher.PBEWithHMacSHA1AndKhazad",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA1.Khazad.class.getName());
               put("Cipher.PBEWithHMacSHA1AndSerpent",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA1.Serpent.class.getName());
               put("Cipher.PBEWithHMacSHA1AndSquare",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA1.Square.class.getName());
               put("Cipher.PBEWithHMacSHA1AndTripleDES",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA1.TripleDES.class.getName());
               put("Cipher.PBEWithHMacSHA1AndTwofish",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA1.Twofish.class.getName());

               put("Cipher.PBEWithHMacSHA256AndAES",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA256.AES.class.getName());
               put("Cipher.PBEWithHMacSHA256AndAnubis",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA256.Anubis.class.getName());
               put("Cipher.PBEWithHMacSHA256AndBlowfish",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA256.Blowfish.class.getName());
               put("Cipher.PBEWithHMacSHA256AndCast5",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA256.Cast5.class.getName());
               put("Cipher.PBEWithHMacSHA256AndDES",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA256.DES.class.getName());
               put("Cipher.PBEWithHMacSHA256AndKhazad",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA256.Khazad.class.getName());
               put("Cipher.PBEWithHMacSHA256AndSerpent",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA256.Serpent.class.getName());
               put("Cipher.PBEWithHMacSHA256AndSquare",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA256.Square.class.getName());
               put("Cipher.PBEWithHMacSHA256AndTripleDES",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA256.TripleDES.class.getName());
               put("Cipher.PBEWithHMacSHA256AndTwofish",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA256.Twofish.class.getName());

               put("Cipher.PBEWithHMacSHA384AndAES",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA384.AES.class.getName());
               put("Cipher.PBEWithHMacSHA384AndAnubis",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA384.Anubis.class.getName());
               put("Cipher.PBEWithHMacSHA384AndBlowfish",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA384.Blowfish.class.getName());
               put("Cipher.PBEWithHMacSHA384AndCast5",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA384.Cast5.class.getName());
               put("Cipher.PBEWithHMacSHA384AndDES",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA384.DES.class.getName());
               put("Cipher.PBEWithHMacSHA384AndKhazad",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA384.Khazad.class.getName());
               put("Cipher.PBEWithHMacSHA384AndSerpent",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA384.Serpent.class.getName());
               put("Cipher.PBEWithHMacSHA384AndSquare",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA384.Square.class.getName());
               put("Cipher.PBEWithHMacSHA384AndTripleDES",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA384.TripleDES.class.getName());
               put("Cipher.PBEWithHMacSHA384AndTwofish",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA384.Twofish.class.getName());

               put("Cipher.PBEWithHMacSHA512AndAES",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA512.AES.class.getName());
               put("Cipher.PBEWithHMacSHA512AndAnubis",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA512.Anubis.class.getName());
               put("Cipher.PBEWithHMacSHA512AndBlowfish",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA512.Blowfish.class.getName());
               put("Cipher.PBEWithHMacSHA512AndCast5",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA512.Cast5.class.getName());
               put("Cipher.PBEWithHMacSHA512AndDES",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA512.DES.class.getName());
               put("Cipher.PBEWithHMacSHA512AndKhazad",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA512.Khazad.class.getName());
               put("Cipher.PBEWithHMacSHA512AndSerpent",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA512.Serpent.class.getName());
               put("Cipher.PBEWithHMacSHA512AndSquare",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA512.Square.class.getName());
               put("Cipher.PBEWithHMacSHA512AndTripleDES",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA512.TripleDES.class.getName());
               put("Cipher.PBEWithHMacSHA512AndTwofish",
                   gnu.crypto.jce.cipher.PBES2.HMacSHA512.Twofish.class.getName());

               put("Cipher.PBEWithHMacTigerAndAES",
                   gnu.crypto.jce.cipher.PBES2.HMacTiger.AES.class.getName());
               put("Cipher.PBEWithHMacTigerAndAnubis",
                   gnu.crypto.jce.cipher.PBES2.HMacTiger.Anubis.class.getName());
               put("Cipher.PBEWithHMacTigerAndBlowfish",
                   gnu.crypto.jce.cipher.PBES2.HMacTiger.Blowfish.class.getName());
               put("Cipher.PBEWithHMacTigerAndCast5",
                   gnu.crypto.jce.cipher.PBES2.HMacTiger.Cast5.class.getName());
               put("Cipher.PBEWithHMacTigerAndDES",
                   gnu.crypto.jce.cipher.PBES2.HMacTiger.DES.class.getName());
               put("Cipher.PBEWithHMacTigerAndKhazad",
                   gnu.crypto.jce.cipher.PBES2.HMacTiger.Khazad.class.getName());
               put("Cipher.PBEWithHMacTigerAndSerpent",
                   gnu.crypto.jce.cipher.PBES2.HMacTiger.Serpent.class.getName());
               put("Cipher.PBEWithHMacTigerAndSquare",
                   gnu.crypto.jce.cipher.PBES2.HMacTiger.Square.class.getName());
               put("Cipher.PBEWithHMacTigerAndTripleDES",
                   gnu.crypto.jce.cipher.PBES2.HMacTiger.TripleDES.class.getName());
               put("Cipher.PBEWithHMacTigerAndTwofish",
                   gnu.crypto.jce.cipher.PBES2.HMacTiger.Twofish.class.getName());

               put("Cipher.PBEWithHMacWhirlpoolAndAES",
                   gnu.crypto.jce.cipher.PBES2.HMacWhirlpool.AES.class.getName());
               put("Cipher.PBEWithHMacWhirlpoolAndAnubis",
                   gnu.crypto.jce.cipher.PBES2.HMacWhirlpool.Anubis.class.getName());
               put("Cipher.PBEWithHMacWhirlpoolAndBlowfish",
                   gnu.crypto.jce.cipher.PBES2.HMacWhirlpool.Blowfish.class.getName());
               put("Cipher.PBEWithHMacWhirlpoolAndCast5",
                   gnu.crypto.jce.cipher.PBES2.HMacWhirlpool.Cast5.class.getName());
               put("Cipher.PBEWithHMacWhirlpoolAndDES",
                   gnu.crypto.jce.cipher.PBES2.HMacWhirlpool.DES.class.getName());
               put("Cipher.PBEWithHMacWhirlpoolAndKhazad",
                   gnu.crypto.jce.cipher.PBES2.HMacWhirlpool.Khazad.class.getName());
               put("Cipher.PBEWithHMacWhirlpoolAndSerpent",
                   gnu.crypto.jce.cipher.PBES2.HMacWhirlpool.Serpent.class.getName());
               put("Cipher.PBEWithHMacWhirlpoolAndSquare",
                   gnu.crypto.jce.cipher.PBES2.HMacWhirlpool.Square.class.getName());
               put("Cipher.PBEWithHMacWhirlpoolAndTripleDES",
                   gnu.crypto.jce.cipher.PBES2.HMacWhirlpool.TripleDES.class.getName());
               put("Cipher.PBEWithHMacWhirlpoolAndTwofish",
                   gnu.crypto.jce.cipher.PBES2.HMacWhirlpool.Twofish.class.getName());

               // SecretKeyFactory interface to PBKDF2.
               put("SecretKeyFactory.PBKDF2WithHMacHaval",
                   gnu.crypto.jce.PBKDF2SecretKeyFactory.HMacHaval.class.getName());
               put("SecretKeyFactory.PBKDF2WithHMacMD2",
                   gnu.crypto.jce.PBKDF2SecretKeyFactory.HMacMD2.class.getName());
               put("SecretKeyFactory.PBKDF2WithHMacMD4",
                   gnu.crypto.jce.PBKDF2SecretKeyFactory.HMacMD4.class.getName());
               put("SecretKeyFactory.PBKDF2WithHMacMD5",
                   gnu.crypto.jce.PBKDF2SecretKeyFactory.HMacMD5.class.getName());
               put("SecretKeyFactory.PBKDF2WithHMacSHA1",
                   gnu.crypto.jce.PBKDF2SecretKeyFactory.HMacSHA1.class.getName());
               put("SecretKeyFactory.PBKDF2WithHMacSHA256",
                   gnu.crypto.jce.PBKDF2SecretKeyFactory.HMacSHA256.class.getName());
               put("SecretKeyFactory.PBKDF2WithHMacSHA384",
                   gnu.crypto.jce.PBKDF2SecretKeyFactory.HMacSHA384.class.getName());
               put("SecretKeyFactory.PBKDF2WithHMacSHA512",
                   gnu.crypto.jce.PBKDF2SecretKeyFactory.HMacSHA512.class.getName());
               put("SecretKeyFactory.PBKDF2WithHMacTiger",
                   gnu.crypto.jce.PBKDF2SecretKeyFactory.HMacTiger.class.getName());
               put("SecretKeyFactory.PBKDF2WithHMacWhirlpool",
                   gnu.crypto.jce.PBKDF2SecretKeyFactory.HMacWhirlpool.class.getName());

               // Simple SecretKeyFactory implementations.
               put ("SecretKeyFactory.Anubis", gnu.crypto.jce.key.AnubisSecretKeyFactoryImpl.class.getName());
               put ("SecretKeyFactory.Blowfish", gnu.crypto.jce.key.BlowfishSecretKeyFactoryImpl.class.getName());
               put ("SecretKeyFactory.Cast5", gnu.crypto.jce.key.Cast5SecretKeyFactoryImpl.class.getName());
               put ("SecretKeyFactory.DES", gnu.crypto.jce.key.DESSecretKeyFactoryImpl.class.getName());
               put ("SecretKeyFactory.Khazad", gnu.crypto.jce.key.KhazadSecretKeyFactoryImpl.class.getName());
               put ("SecretKeyFactory.Rijndael", gnu.crypto.jce.key.RijndaelSecretKeyFactoryImpl.class.getName());
               put ("SecretKeyFactory.Serpent", gnu.crypto.jce.key.SerpentSecretKeyFactoryImpl.class.getName());
               put ("SecretKeyFactory.Square", gnu.crypto.jce.key.SquareSecretKeyFactoryImpl.class.getName());
               put ("SecretKeyFactory.TripleDES", gnu.crypto.jce.key.DESedeSecretKeyFactoryImpl.class.getName());
               put ("Alg.Alias.SecretKeyFactory.AES", "Rijndael");
               put ("Alg.Alias.SecretKeyFactory.DESede", "TripleDES");
               put ("Alg.Alias.SecretKeyFactory.3-DES", "TripleDES");
               put ("Alg.Alias.SecretKeyFactory.3DES", "TripleDES");

               put("AlgorithmParameters.BlockCipherParameters",
                   gnu.crypto.jce.params.BlockCipherParameters.class.getName());

               // MAC
               put("Mac.HMAC-MD2", gnu.crypto.jce.mac.HMacMD2Spi.class.getName());
               put("Mac.HMAC-MD4", gnu.crypto.jce.mac.HMacMD4Spi.class.getName());
               put("Mac.HMAC-MD5", gnu.crypto.jce.mac.HMacMD5Spi.class.getName());
               put("Mac.HMAC-RIPEMD128", gnu.crypto.jce.mac.HMacRipeMD128Spi.class.getName());
               put("Mac.HMAC-RIPEMD160", gnu.crypto.jce.mac.HMacRipeMD160Spi.class.getName());
               put("Mac.HMAC-SHA160", gnu.crypto.jce.mac.HMacSHA160Spi.class.getName());
               put("Mac.HMAC-SHA256", gnu.crypto.jce.mac.HMacSHA256Spi.class.getName());
               put("Mac.HMAC-SHA384", gnu.crypto.jce.mac.HMacSHA384Spi.class.getName());
               put("Mac.HMAC-SHA512", gnu.crypto.jce.mac.HMacSHA512Spi.class.getName());
               put("Mac.HMAC-TIGER", gnu.crypto.jce.mac.HMacTigerSpi.class.getName());
               put("Mac.HMAC-HAVAL", gnu.crypto.jce.mac.HMacHavalSpi.class.getName());
               put("Mac.HMAC-WHIRLPOOL", gnu.crypto.jce.mac.HMacWhirlpoolSpi.class.getName());
               put("Mac.TMMH16", gnu.crypto.jce.mac.TMMH16Spi.class.getName());
               put("Mac.UHASH32", gnu.crypto.jce.mac.UHash32Spi.class.getName());
               put("Mac.UMAC32", gnu.crypto.jce.mac.UMac32Spi.class.getName());

               put("Mac.OMAC-ANUBIS", gnu.crypto.jce.mac.OMacAnubisImpl.class.getName());
               put("Mac.OMAC-BLOWFISH", gnu.crypto.jce.mac.OMacBlowfishImpl.class.getName());
               put("Mac.OMAC-CAST5", gnu.crypto.jce.mac.OMacCast5Impl.class.getName());
               put("Mac.OMAC-DES", gnu.crypto.jce.mac.OMacDESImpl.class.getName());
               put("Mac.OMAC-KHAZAD", gnu.crypto.jce.mac.OMacKhazadImpl.class.getName());
               put("Mac.OMAC-RIJNDAEL", gnu.crypto.jce.mac.OMacRijndaelImpl.class.getName());
               put("Mac.OMAC-SERPENT", gnu.crypto.jce.mac.OMacSerpentImpl.class.getName());
               put("Mac.OMAC-SQUARE", gnu.crypto.jce.mac.OMacSquareImpl.class.getName());
               put("Mac.OMAC-TRIPLEDES", gnu.crypto.jce.mac.OMacTripleDESImpl.class.getName());
               put("Mac.OMAC-TWOFISH", gnu.crypto.jce.mac.OMacTwofishImpl.class.getName());

               // Aliases
               put("Alg.Alias.AlgorithmParameters.AES", "BlockCipherParameters");
               put("Alg.Alias.AlgorithmParameters.BLOWFISH", "BlockCipherParameters");
               put("Alg.Alias.AlgorithmParameters.ANUBIS", "BlockCipherParameters");
               put("Alg.Alias.AlgorithmParameters.KHAZAD", "BlockCipherParameters");
               put("Alg.Alias.AlgorithmParameters.NULL", "BlockCipherParameters");
               put("Alg.Alias.AlgorithmParameters.RIJNDAEL", "BlockCipherParameters");
               put("Alg.Alias.AlgorithmParameters.SERPENT", "BlockCipherParameters");
               put("Alg.Alias.AlgorithmParameters.SQUARE", "BlockCipherParameters");
               put("Alg.Alias.AlgorithmParameters.TWOFISH", "BlockCipherParameters");
               put("Alg.Alias.Cipher.RC4", "ARCFOUR");
               put("Alg.Alias.Cipher.3-DES", "TRIPLEDES");
               put("Alg.Alias.Cipher.3DES", "TRIPLEDES");
               put("Alg.Alias.Cipher.DES-EDE", "TRIPLEDES");
               put("Alg.Alias.Cipher.DESede", "TRIPLEDES");
               put("Alg.Alias.Cipher.CAST128", "CAST5");
               put("Alg.Alias.Cipher.CAST-128", "CAST5");
               put("Alg.Alias.Mac.HMAC-SHS", "HMAC-SHA160");
               put("Alg.Alias.Mac.HMAC-SHA", "HMAC-SHA160");
               put("Alg.Alias.Mac.HMAC-SHA1", "HMAC-SHA160");
               put("Alg.Alias.Mac.HMAC-SHA-160", "HMAC-SHA160");
               put("Alg.Alias.Mac.HMAC-SHA-256", "HMAC-SHA256");
               put("Alg.Alias.Mac.HMAC-SHA-384", "HMAC-SHA384");
               put("Alg.Alias.Mac.HMAC-SHA-512", "HMAC-SHA512");
               put("Alg.Alias.Mac.HMAC-RIPEMD-160", "HMAC-RIPEMD160");
               put("Alg.Alias.Mac.HMAC-RIPEMD-128", "HMAC-RIPEMD128");
               put("Alg.Alias.Mac.OMAC-AES", "OMAC-RIJNDAEL");
               put("Alg.Alias.Mac.OMAC-3DES", "OMAC-3DES");
               put("Alg.Alias.Mac.HmacMD4", "HMAC-MD4");
               put("Alg.Alias.Mac.HmacMD5", "HMAC-MD5");
               put("Alg.Alias.Mac.HmacSHA-1", "HMAC-SHA-1");
               put("Alg.Alias.Mac.HmacSHA1", "HMAC-SHA1");
               put("Alg.Alias.Mac.HmacSHA-160", "HMAC-SHA-160");
               put("Alg.Alias.Mac.HmacSHA160", "HMAC-SHA-160");
               put("Alg.Alias.Mac.HmacSHA-256", "HMAC-SHA-256");
               put("Alg.Alias.Mac.HmacSHA256", "HMAC-SHA-256");
               put("Alg.Alias.Mac.HmacSHA-384", "HMAC-SHA-384");
               put("Alg.Alias.Mac.HmacSHA384", "HMAC-SHA-384");
               put("Alg.Alias.Mac.HmacSHA-512", "HMAC-SHA-512");
               put("Alg.Alias.Mac.HmacSHA512", "HMAC-SHA-512");
               put("Alg.Alias.Mac.HmacRIPEMD128", "HMAC-RIPEMD128");
               put("Alg.Alias.Mac.HmacRIPEMD-128", "HMAC-RIPEMD128");
               put("Alg.Alias.Mac.HmacRIPEMD160", "HMAC-RIPEMD160");
               put("Alg.Alias.Mac.HmacRIPEMD-160", "HMAC-RIPEMD160");
               put("Alg.Alias.Mac.HmacTiger", "HMAC-TIGER");
               put("Alg.Alias.Mac.HmacHaval", "HMAC-HAVAL");
               put("Alg.Alias.Mac.HmacWhirlpool", "HMAC-WHIRLPOOL");

               return null;
            }
         }
      );
   }

   // Class methods
   // -------------------------------------------------------------------------

   /**
    * <p>Returns a {@link Set} of names of symmetric key block cipher algorithms
    * available from this {@link Provider}.</p>
    *
    * @return a {@link Set} of cipher names (Strings).
    */
   public static final Set getCipherNames() {
      HashSet s = new HashSet();
      s.addAll(CipherFactory.getNames());
      s.add(Registry.ARCFOUR_PRNG);
      return s;
   }

   /**
    * <p>Returns a {@link Set} of names of MAC algorithms available from
    * this {@link Provider}.</p>
    *
    * @return a {@link Set} of MAC names (Strings).
    */
   public static final Set getMacNames() {
      return MacFactory.getNames();
   }

   // Instance methods
   // -------------------------------------------------------------------------
}
