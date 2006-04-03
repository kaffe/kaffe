/* Main.java -- JAR signing and verification tool not unlike jarsigner
   Copyright (C) 2006 Free Software Foundation, Inc.

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
Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301 USA. */


package gnu.classpath.tools.jarsigner;

import gnu.classpath.SystemProperties;
import gnu.classpath.tools.HelpPrinter;
import gnu.java.security.OID;
import gnu.java.security.Registry;
import gnu.javax.security.auth.callback.ConsoleCallbackHandler;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.security.AccessController;
import java.security.Key;
import java.security.KeyStore;
import java.security.KeyStoreException;
import java.security.NoSuchAlgorithmException;
import java.security.PrivateKey;
import java.security.PrivilegedAction;
import java.security.Provider;
import java.security.Security;
import java.security.UnrecoverableKeyException;
import java.security.cert.Certificate;
import java.security.cert.CertificateException;
import java.util.Locale;
import java.util.jar.Attributes.Name;
import java.util.logging.Logger;

import javax.security.auth.callback.Callback;
import javax.security.auth.callback.CallbackHandler;
import javax.security.auth.callback.PasswordCallback;
import javax.security.auth.callback.UnsupportedCallbackException;

/**
 * The GNU Classpath implementation of the <i>jarsigner</i> tool.
 * <p>
 * The <i>jarsigner</i> tool is used to sign and verify JAR (Java ARchive)
 * files.
 * <p>
 * This implementation is intended to be compatible with the behaviour
 * described in the public documentation of the same tool included in JDK 1.4.
 */
public class Main
{
  private static final Logger log = Logger.getLogger(Main.class.getName());
  private static final String HELP_PATH = "jarsigner/jarsigner.txt";
  private static final Locale EN_US_LOCALE = new Locale("en", "US");
  static final String DIGEST = "SHA1-Digest";
  static final String DIGEST_MANIFEST = "SHA1-Digest-Manifest";
  static final Name DIGEST_ATTR = new Name(DIGEST);
  static final Name DIGEST_MANIFEST_ATTR = new Name(DIGEST_MANIFEST);
  static final OID DSA_SIGNATURE_OID = new OID(Registry.DSA_OID_STRING);
  static final OID RSA_SIGNATURE_OID = new OID(Registry.RSA_OID_STRING);

  private boolean verify;
  private String ksURL;
  private String ksType;
  private String password;
  private String ksPassword;
  private String sigFileName;
  private String signedJarFileName;
  private boolean verbose;
  private boolean certs;
  private boolean internalSF;
  private boolean sectionsOnly;
  private String providerClassName;
  private String jarFileName;
  private String alias;

  protected Provider provider;
  private boolean providerInstalled;
  private char[] ksPasswordChars;
  private KeyStore store;
  private char[] passwordChars;
  private PrivateKey signerPrivateKey;
  private Certificate[] signerCertificateChain;

  private Main()
  {
    super();
  }

  public static final void main(String[] args)
  {
    log.entering("Main", "main", args);

    Main tool = new Main();
    try
      {
        tool.processArgs(args);
        tool.start();
      }
    catch (SecurityException x)
      {
        log.throwing("Main", "main", x);
        System.err.println("jarsigner: " + x.getMessage());
      }
    catch (Exception x)
      {
        log.throwing("Main", "main", x);
        System.err.println("jarsigner error: " + x);
      }

    tool.teardown();

    log.exiting("Main", "main");
    // System.exit(0);
  }

  // helper methods -----------------------------------------------------------

  /**
   * Read the command line arguments setting the tool's parameters in
   * preparation for the user desired action.
   * 
   * @param args an array of options (strings).
   * @throws Exception if an exceptio occurs during the process.
   */
  private void processArgs(String[] args) throws Exception
  {
    log.entering("Main", "processArgs", args);

    HelpPrinter.checkHelpKey(args, HELP_PATH);
    if (args == null || args.length == 0)
      HelpPrinter.printHelpAndExit(HELP_PATH);

    int limit = args.length;
    log.finest("args.length=" + limit);
    int i = 0;
    String opt;
    while (i < limit)
      {
        opt = args[i++];
        log.finest("args[" + (i - 1) + "]=" + opt);
        if (opt == null || opt.length() == 0)
          continue;

        if ("-verify".equals(opt)) // -verify
          verify = true;
        else if ("-keystore".equals(opt)) // -keystore URL
          ksURL = args[i++];
        else if ("-storetype".equals(opt)) // -storetype STORE_TYPE
          ksType = args[i++];
        else if ("-storepass".equals(opt)) // -storepass PASSWORD
          ksPassword = args[i++];
        else if ("-keypass".equals(opt)) // -keypass PASSWORD
          password = args[i++];
        else if ("-sigfile".equals(opt)) // -sigfile NAME
          sigFileName = args[i++];
        else if ("-signedjar".equals(opt)) // -signedjar FILE_NAME
          signedJarFileName = args[i++];
        else if ("-verbose".equals(opt)) // -verbose
          verbose = true;
        else if ("-certs".equals(opt)) // -certs
          certs = true;
        else if ("-internalsf".equals(opt)) // -internalsf
          internalSF = true;
        else if ("-sectionsonly".equals(opt)) // -sectionsonly
          sectionsOnly = true;
        else if ("-provider".equals(opt)) // -provider PROVIDER_CLASS_NAME
          providerClassName = args[i++];
        else
          {
            jarFileName = opt;
            if (! verify)
              alias = args[i++];

            break;
          }
      }

    if (i < limit) // more options than needed
      log.warning("Last argument is assumed at index #" + (i - 1)
                  + ". Remaining arguments (" + args[i]
                  + "...) will be ignored");

    setupCommonParams();
    if (verify)
      {
        log.info("Will verify with the following parameters:");
        log.info("     jar-file = '" + jarFileName + "'");
        log.info("Options:");
        log.info("     provider = '" + providerClassName + "'");
        log.info("      verbose ? " + verbose);
        log.info("        certs ? " + certs);
        log.info("   internalsf ? " + internalSF);
        log.info(" sectionsonly ? " + sectionsOnly);
      }
    else // sign
      {
        setupSigningParams();

        log.info("Will sign with the following parameters:");
        log.info("     jar-file = '" + jarFileName + "'");
        log.info("        alias = '" + alias + "'");
        log.info("Options:");
        log.info("     keystore = '" + ksURL + "'");
        log.info("    storetype = '" + ksType + "'");
        log.info("    storepass = '" + ksPassword + "'");
        log.info("      keypass = '" + password + "'");
        log.info("      sigfile = '" + sigFileName + "'");
        log.info("    signedjar = '" + signedJarFileName + "'");
        log.info("     provider = '" + providerClassName + "'");
        log.info("      verbose ? " + verbose);
        log.info("   internalsf ? " + internalSF);
        log.info(" sectionsonly ? " + sectionsOnly);
      }

    log.exiting("Main", "processArgs");
  }

  /**
   * Invokes the <code>start()</code> method of the concrete handler.
   * <p>
   * Depending on the result of processing the command line arguments, this
   * handler may be one for signing the jar, or verifying it.
   * 
   * @throws Exception if an exception occurs during the process.
   */
  private void start() throws Exception
  {
    log.entering("Main", "start");

    if (verify)
      {
        JarVerifier jv = new JarVerifier(this);
        jv.start();
      }
    else
      {
        JarSigner js = new JarSigner(this);
        js.start();
      }

    log.exiting("Main", "start");
  }

  /**
   * Ensures that the underlying JVM is left in the same state as we found it
   * when we first launched the tool. Specifically, if we have installed a new
   * security provider then now is the time to remove it.
   * <p>
   * Note (rsn): this may not be necessary if we terminate the JVM; i.e. call
   * {@link System#exit(int)} at the end of the tool's invocation. Nevertheless
   * it's good practive to return the JVM to its initial state.
   */
  private void teardown()
  {
    log.entering("Main", "teardown");

    if (providerInstalled)
      {
        final String providerName = provider.getName();
        log.info("About to remove provider: " + providerName);
        // remove it. again we need to override security checks
        AccessController.doPrivileged(new PrivilegedAction()
        {
          public Object run()
          {
            Security.removeProvider(providerName);
            return null;
          }
        });
      }

    log.exiting("Main", "teardown");
  }

  /**
   * After processing the command line arguments, this method is invoked to
   * process the common parameters which may have been encountered among the
   * actual arguments.
   * <p>
   * Common parameters are those which are allowed in both signing and
   * verification modes.
   * 
   * @throws InstantiationException if a security provider class name is
   *           specified but that class name is that of either an interface or
   *           an abstract class.
   * @throws IllegalAccessException if a security provider class name is
   *           specified but no 0-arguments constructor is defined for that
   *           class.
   * @throws ClassNotFoundException if a security provider class name is
   *           specified but no such class was found in the classpath.
   * @throws IOException if the JAR file name for signing, or verifying, does
   *           not exist, exists but denotes a directory, or is not readable.
   */
  private void setupCommonParams() throws InstantiationException,
      IllegalAccessException, ClassNotFoundException, IOException
  {
    log.entering("Main", "setupCommonParams");

    if (jarFileName == null)
      HelpPrinter.printHelpAndExit(HELP_PATH);

    File jar = new File(jarFileName);
    if (! jar.exists())
      throw new FileNotFoundException(jarFileName);

    if (jar.isDirectory())
      throw new IOException("JAR file [" + jarFileName
                            + "] is NOT a file object");
    if (! jar.canRead())
      throw new IOException("JAR file [" + jarFileName + "] is NOT readable");

    if (providerClassName != null && providerClassName.length() > 0)
      {
        provider = (Provider) Class.forName(providerClassName).newInstance();
        // is it already installed?
        String providerName = provider.getName();
        Provider installedProvider = Security.getProvider(providerName);
        if (installedProvider != null)
          log.info("Provider " + providerName + " is already installed");
        else // install it
          installNewProvider();
      }

    if (! verbose && certs)
      {
        log.warning("Option <certs> is set but <verbose> is not. Ignored");
        certs = false;
      }

    log.exiting("Main", "setupCommonParams");
  }

  /**
   * Install the user defined security provider in the underlying JVM.
   * <p>
   * Also record this fact so we can remove it when we exit the tool.
   */
  private void installNewProvider()
  {
    log.entering("Main", "installNewProvider");

    String providerName = provider.getName();
    log.info("About to install new provider: " + providerName);
    // we need to override security checks
    Boolean result = (Boolean) AccessController.doPrivileged(new PrivilegedAction()
    {
      public Object run()
      {
        int actualPosition = Security.insertProviderAt(provider, 1);
        return new Boolean(actualPosition != - 1);
      }
    });
    log.info("Provider " + providerName + " installed successfully? " + result);
    providerInstalled = result.booleanValue();

    log.exiting("Main", "installNewProvider");
  }

  /**
   * After processing the command line arguments, this method is invoked to
   * process the parameters which may have been encountered among the actual
   * arguments, and which are specific to the signing action of the tool.
   * 
   * @throws KeyStoreException if no implementation of the designated (or
   *           default type) of a key store is availabe.
   * @throws IOException if an I/O related exception occurs during the process.
   * @throws NoSuchAlgorithmException if an implementation of an algorithm used
   *           by the key store is not available.
   * @throws CertificateException if an exception occurs while reading a
   *           certificate from the key store.
   * @throws UnsupportedCallbackException if no implementation of a password
   *           callback is available.
   * @throws UnrecoverableKeyException if the wrong password was used to unlock
   *           the key store.
   * @throws SecurityException if the designated alias is not known to the key
   *           store or is not an Alias of a Key Entry.
   */
  private void setupSigningParams() throws KeyStoreException, IOException,
      NoSuchAlgorithmException, CertificateException,
      UnsupportedCallbackException, UnrecoverableKeyException
  {
    log.entering("Main", "setupSigningParams");

    if (ksURL == null || ksURL.trim().length() == 0)
      {
        String userHome = SystemProperties.getProperty("user.home");
        if (userHome == null || userHome.trim().length() == 0)
          throw new SecurityException("Option '-keystore' is not defined or"
                                      + " is an empty string, and 'user.home'"
                                      + " is unknown");
        ksURL = "file:" + userHome.trim() + "/.keystore";
      }
    else
      {
        ksURL = ksURL.trim();
        if (ksURL.indexOf(":") == -1)
          ksURL = "file:" + ksURL;
      }

    if (ksType == null || ksType.trim().length() == 0)
      ksType = KeyStore.getDefaultType();
    else
      ksType = ksType.trim();

    store = KeyStore.getInstance(ksType);

    if (ksPassword == null)
      {
        // ask the user to provide one
        CallbackHandler handler = new ConsoleCallbackHandler();
        PasswordCallback pcb = new PasswordCallback("Enter keystore password: ",
                                                    false);
        handler.handle(new Callback[] { pcb });
        ksPasswordChars = pcb.getPassword();
      }
    else
      ksPasswordChars = ksPassword.toCharArray();

    URL url = new URL(ksURL);
    InputStream stream = url.openStream();
    store.load(stream, ksPasswordChars);

    if (alias == null)
      HelpPrinter.printHelpAndExit(HELP_PATH);

    if (! store.containsAlias(alias))
      throw new SecurityException("Designated alias [" + alias
                                  + "] MUST be known to the key store in use");
    if (! store.isKeyEntry(alias))
      throw new SecurityException("Designated alias [" + alias
                                  + "] MUST be an Alias of a Key Entry");
    Key key;
    if (password == null)
      {
        passwordChars = ksPasswordChars;
        try
          {
            key = store.getKey(alias, passwordChars);
          }
        catch (UnrecoverableKeyException x)
          {
            // ask the user to provide one
            CallbackHandler handler = new ConsoleCallbackHandler();
            PasswordCallback pcb = new PasswordCallback("Enter key password for "
                                                        + alias + ": ", false);
            handler.handle(new Callback[] { pcb });
            passwordChars = pcb.getPassword();
            // take 2
            key = store.getKey(alias, passwordChars);
          }
      }
    else
      {
        passwordChars = password.toCharArray();
        key = store.getKey(alias, passwordChars);
      }

    if (! (key instanceof PrivateKey))
      throw new SecurityException("Key associated with " + alias
                                  + " MUST be a private key");
    signerPrivateKey = (PrivateKey) key;
    signerCertificateChain = store.getCertificateChain(alias);
    log.finest(String.valueOf(signerCertificateChain));

    if (sigFileName == null)
      sigFileName = alias;

    sigFileName = sigFileName.toUpperCase(EN_US_LOCALE);
    if (sigFileName.length() > 8)
      sigFileName = sigFileName.substring(0, 8);

    char[] chars = sigFileName.toCharArray();
    for (int i = 0; i < chars.length; i++)
      {
        char c = chars[i];
        if (! (Character.isLetter(c)
            || Character.isDigit(c)
            || c == '_'
            || c == '-'))
          chars[i] = '_';
      }

    sigFileName = new String(chars);

    if (signedJarFileName == null)
      signedJarFileName = jarFileName;

    log.exiting("Main", "setupSigningParams");
  }

  boolean isVerbose()
  {
    return verbose;
  }

  boolean isCerts()
  {
    return certs;
  }

  String getSigFileName()
  {
    return this.sigFileName;
  }

  String getJarFileName()
  {
    return this.jarFileName;
  }

  boolean isSectionsOnly()
  {
    return this.sectionsOnly;
  }

  boolean isInternalSF()
  {
    return this.internalSF;
  }

  PrivateKey getSignerPrivateKey()
  {
    return this.signerPrivateKey;
  }

  Certificate[] getSignerCertificateChain()
  {
    return signerCertificateChain;
  }

  String getSignedJarFileName()
  {
    return this.signedJarFileName;
  }
}
