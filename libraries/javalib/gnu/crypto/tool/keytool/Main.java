/* Main.java -- keytool utility.
   Copyright (C) 2004  Free Software Foundation, Inc.

This file is part of GNU Crypto.

GNU Crypto is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

GNU Crypto is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to the
Free Software Foundation Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301  USA  */


package gnu.crypto.tool.keytool;

import gnu.crypto.Properties;
import gnu.crypto.Registry;
import gnu.crypto.auth.callback.AbstractCallbackHandler;
import gnu.crypto.auth.callback.GnuCallbacks;

import gnu.getopt.Getopt;
import gnu.getopt.LongOpt;

import java.io.PrintStream;
import java.security.Provider;
import java.security.Security;

import java.text.DateFormat;
import java.text.MessageFormat;

import java.util.Iterator;
import java.util.LinkedList;
import java.util.MissingResourceException;
import java.util.PropertyResourceBundle;
import java.util.ResourceBundle;
import java.util.StringTokenizer;

import javax.security.auth.callback.CallbackHandler;

public final class Main
{

  // Constants and fields.
  // -------------------------------------------------------------------------

  public static final String PROGNAME = "keytool";

  private static final int OPT_ARMOR       = 1024;
  private static final int OPT_CALLBACK    = 1025;
  private static final int OPT_CERT_TYPE   = 1026;
  private static final int OPT_DATE        = 1027;
  private static final int OPT_DEBUG       = 1028;
  private static final int OPT_NO_KEYSTORE = 1029;
  private static final int OPT_PCLASS      = 1030;
  private static final int OPT_PNAME       = 1031;
  private static final int OPT_STORE_TYPE  = 1032;
  private static final int OPT_VALIDITY    = 1033;
  private static final int OPT_VERSION     = 1034;

  static ResourceBundle messages;

// Main.
  // -------------------------------------------------------------------------

  public static void main (final String[] argv)
  {
    try
      {
        messages = PropertyResourceBundle.getBundle ("gnu/crypto/tool/keytool/MessagesBundle");
      }
    catch (MissingResourceException mre)
      {
        System.err.println (PROGNAME + ": can't load messages bundle!");
        System.err.println (PROGNAME + ": something is seriously wrong with your installation.");
        System.exit (1);
      }
    final LinkedList commands = new LinkedList();
    Command cmd = new Command ();
    final String optstring = "thegirlsdpa:A:D:f:k:K:no:v";
    final LongOpt[] longopts = {
      // Commands.
      new LongOpt ("delete",   LongOpt.NO_ARGUMENT, null, 'd'),
      new LongOpt ("export",   LongOpt.NO_ARGUMENT, null, 'e'),
      new LongOpt ("generate", LongOpt.NO_ARGUMENT, null, 'g'),
      new LongOpt ("help",     LongOpt.NO_ARGUMENT, null, 'h'),
      new LongOpt ("import",   LongOpt.NO_ARGUMENT, null, 'i'),
      new LongOpt ("list",     LongOpt.NO_ARGUMENT, null, 'l'),
      new LongOpt ("path",     LongOpt.NO_ARGUMENT, null, 'p'),
      new LongOpt ("revoke",   LongOpt.NO_ARGUMENT, null, 'r'),
      new LongOpt ("sign",     LongOpt.NO_ARGUMENT, null, 's'),
      new LongOpt ("trust",    LongOpt.NO_ARGUMENT, null, 't'),
      new LongOpt ("version",  LongOpt.NO_ARGUMENT, null, OPT_VERSION),

      // Options.
      new LongOpt ("alias",          LongOpt.REQUIRED_ARGUMENT, null, 'a'),
      new LongOpt ("armor",          LongOpt.NO_ARGUMENT,       null, OPT_ARMOR),
      new LongOpt ("callback",       LongOpt.REQUIRED_ARGUMENT, null, OPT_CALLBACK),
      new LongOpt ("ca-alias",       LongOpt.REQUIRED_ARGUMENT, null, 'A'),
      new LongOpt ("cert-type",      LongOpt.REQUIRED_ARGUMENT, null, OPT_CERT_TYPE),
      new LongOpt ("date",           LongOpt.REQUIRED_ARGUMENT, null, OPT_DATE),
      new LongOpt ("debug",          LongOpt.NO_ARGUMENT,       null, OPT_DEBUG),
      new LongOpt ("file",           LongOpt.REQUIRED_ARGUMENT, null, 'f'),
      new LongOpt ("keep-going",     LongOpt.NO_ARGUMENT,       null, 'n'),
      new LongOpt ("keystore",       LongOpt.REQUIRED_ARGUMENT, null, 'k'),
      new LongOpt ("no-keystore",    LongOpt.NO_ARGUMENT,       null, OPT_NO_KEYSTORE),
      new LongOpt ("outfile",        LongOpt.REQUIRED_ARGUMENT, null, 'o'),
      new LongOpt ("provider",       LongOpt.REQUIRED_ARGUMENT, null, OPT_PNAME),
      new LongOpt ("provider-class", LongOpt.REQUIRED_ARGUMENT, null, OPT_PCLASS),
      new LongOpt ("secret-keystore",LongOpt.REQUIRED_ARGUMENT, null, 'K'),
      new LongOpt ("store-type",     LongOpt.REQUIRED_ARGUMENT, null, OPT_STORE_TYPE),
      new LongOpt ("validity",       LongOpt.REQUIRED_ARGUMENT, null, OPT_VALIDITY),
      new LongOpt ("verbose",        LongOpt.NO_ARGUMENT,       null, 'v')
    };
    Getopt g = new Getopt (PROGNAME, argv, optstring, longopts);
    int c;
    boolean keep_going = false;
    boolean gnuCryptoProperty = false;
    boolean securityProperty = false;
    boolean debug = false;

    while ((c = g.getopt()) != -1) switch (c)
      {
      case 'd':
        commands.add (cmd = new Delete (cmd));
        break;

      case 'e':
        commands.add (cmd = new Export (cmd));
        break;

      case 'g':
        commands.add (cmd = new GenKey (cmd));
        break;

      case 'i':
        commands.add (cmd = new Import (cmd));
        break;

      case 'l':
        commands.add (cmd = new List (cmd));
        break;

      case 'p':
        commands.add (cmd = new Path (cmd));
        break;

      case 'r':
        commands.add (cmd = new Revoke (cmd));
        break;

      case 's':
        commands.add (cmd = new Sign (cmd));
        break;

      case 't':
        commands.add (cmd = new Trust (cmd));
        break;

      case 'h':
        help (System.out);
        System.exit (0);

      case OPT_VERSION:
        version (System.out);
        if (keep_going)
          break;
        System.exit (0);

      case 'a':
        cmd.alias = g.getOptarg();
        break;

      case 'A':
        cmd.caAlias = g.getOptarg();
        break;

      case 'D':
        {
          String property = g.getOptarg();
          String value = "true";
          int i;
          if ((i = property.indexOf ('=')) >= 0)
            {
              value = property.substring (i + 1);
              property = property.substring (0, i);
            }
          try
            {
              if (securityProperty)
                Security.setProperty (property, value);
              else if (gnuCryptoProperty)
                Properties.setProperty (property, value);
              else
                {
                  if (i < 0 && property.equals ("security"))
                    securityProperty = true;
                  else if (i < 0 && property.equals ("crypto"))
                    gnuCryptoProperty = true;
                  else
                    {
                      securityProperty = false;
                      gnuCryptoProperty = false;
                      System.setProperty (property, value);
                    }
                }
            }
          catch (SecurityException se)
            {
              error ("security policy does not allow setting properties");
            }
        }
        break;

      case 'f':
        cmd.file = g.getOptarg();
        break;

      case 'k':
        cmd.storeFile = g.getOptarg();
        break;

      case 'K':
        cmd.secretStore = g.getOptarg();
        break;

      case 'n':
        keep_going = !keep_going;
        break;

      case 'o':
        cmd.outFile = g.getOptarg();
        break;

      case 'v':
        cmd.verbose++;
        break;

      case OPT_ARMOR:
        cmd.armor = !cmd.armor;
        break;

      case OPT_CALLBACK:
        try
          {
            cmd.handler = AbstractCallbackHandler.getInstance (g.getOptarg(),
                                                               new GnuCallbacks());
          }
        catch (Exception e)
          {
            error ("can't load callback handler " + g.getOptarg()
                   + ": " + e.getMessage());
          }
        break;

      case OPT_DATE:
        try
          {
            cmd.date = DateFormat.getDateInstance().parse (g.getOptarg());
          }
        catch (Exception x)
          {
            error ("can't format date: " + x.getMessage());
          }
        break;

      case OPT_DEBUG:
        debug = true;
        break;

      case OPT_NO_KEYSTORE:
        cmd.noKeystore = !cmd.noKeystore;
        break;

      case OPT_PCLASS:
        try
          {
            cmd.provider = (Provider)
              Class.forName (g.getOptarg()).newInstance();
          }
        catch (Exception e)
          {
            error ("can't load class " + g.getOptarg()
                   + ": " + e.getMessage());
          }
        break;

      case OPT_PNAME:
        cmd.provider = Security.getProvider (g.getOptarg().intern());
        if (cmd.provider == null)
          {
            error ("provider " + g.getOptarg() + " is not available");
          }
        break;

      case OPT_STORE_TYPE:
        cmd.storeType = g.getOptarg();
        break;

      case OPT_VALIDITY:
        try
          {
            cmd.validity = Integer.parseInt (g.getOptarg());
            if (cmd.validity <= 0)
              throw new NumberFormatException ("must be positive");
          }
        catch (NumberFormatException nfe)
          {
            error ("invalid validity: " + nfe.getMessage());
          }

      case '?':
      case ':':
        System.err.println ("Try `" + PROGNAME + " --help' for more info.");
        System.exit (1);
      }

    if (commands.size() == 0)
      {
        System.err.println (PROGNAME + ": no command given");
        System.err.println ("Try `" + PROGNAME + " --help' for more info.");
        System.exit (1);
      }

    for (Iterator it = commands.iterator(); it.hasNext(); )
      {
        try
          {
            cmd = (Command) it.next();
            cmd.run();
          }
        catch (Throwable t)
          {
            System.err.println (PROGNAME + ": " + cmd + ": " +
                                t.getMessage());
            if (debug)
              t.printStackTrace();
            if (!keep_going)
              System.exit (1);
          }
      }
  }

  private static void help (final PrintStream out)
  {
    String msg = null;
    try
      {
        for (int i = 1; ; i++)
          {
            out.println (MessageFormat.format (messages.getString ("usage."+i),
                                               new Object[] { PROGNAME }));
          }
      }
    catch (MissingResourceException mre)
      {
      }
    out.println();
    fill (messages.getString ("commands"), out);
    out.println();

    int hc = 18;
    out.print ("  -d, --delete    ");
    fill (messages.getString ("command.delete"), out, hc);
    out.print ("  -e, --export    ");
    fill (messages.getString ("command.export"), out, hc);
    out.print ("  -g, --generate  ");
    fill (messages.getString ("command.generate"), out, hc);
    out.print ("  -i, --import    ");
    fill (messages.getString ("command.import"), out, hc);
    out.print ("  -l, --list      ");
    fill (messages.getString ("command.list"), out, hc);
    out.print ("  -p, --path      ");
    fill (messages.getString ("command.path"), out, hc);
    out.print ("  -r, --revoke    ");
    fill (messages.getString ("command.revoke"), out, hc);
    out.print ("  -s, --sign      ");
    fill (messages.getString ("command.sign"), out, hc);
    out.print ("  -t, --trust     ");
    fill (messages.getString ("command.trust"), out, hc);
    out.print ("  -h, --help      ");
    fill (messages.getString ("command.help"), out, hc);
    out.print ("      --version   ");
    fill (messages.getString ("command.version"), out, hc);

    out.println();
    fill (messages.getString ("options"), out);
    out.println();

    hc = 30;
    out.print ("  -a, --alias=NAME            ");
    fill (messages.getString ("opt.alias"), out, hc);
    out.print ("  -A, --ca-alias=NAME         ");
    fill (messages.getString ("opt.ca-alias"), out, hc);
    out.print ("      --armor                 ");
    fill (messages.getString ("opt.armor"), out, hc);
    out.print ("      --callback=TYPE         ");
    fill (messages.getString ("opt.callback"), out, hc);
    out.print ("      --cert-type=TYPE        ");
    fill (messages.getString ("opt.cert-type"), out, hc);
    out.print ("      --date=DATE             ");
    fill (messages.getString ("opt.date"), out, hc);
    out.print ("  -f, --file=FILE             ");
    fill (messages.getString ("opt.file"), out, hc);
    out.print ("  -k, --keystore=FILE         ");
    fill (messages.getString ("opt.keystore"), out, hc);
    out.print ("  -K, --secret-keystore=FILE  ");
    fill (messages.getString ("opt.secret-keystore"), out, hc);
    out.print ("      --truststore=FILE       ");
    fill (messages.getString ("opt.truststore"), out, hc);
    out.print ("  -n, --keep-going            ");
    fill (messages.getString ("opt.keep-going"), out, hc);
    out.print ("      --no-keystore           ");
    fill (messages.getString ("opt.no-keystore"), out, hc);
    out.print ("  -o, --outfile=FILE          ");
    fill (messages.getString ("opt.outfile"), out, hc);
    out.print ("      --provider=NAME         ");
    fill (messages.getString ("opt.provider"), out, hc);
    out.print ("      --provider-class=CLASS  ");
    fill (messages.getString ("opt.provider-class"), out, hc);
    out.print ("      --store-type=TYPE       ");
    fill (messages.getString ("opt.store-type"), out, hc);
    out.print ("      --validity=DAYS         ");
    fill (messages.getString ("opt.validity"), out, hc);
    out.print ("  -v, --verbose               ");
    fill (messages.getString ("opt.verbose"), out, hc);
    out.print ("      -Dname=value            ");
    fill (messages.getString ("opt.D"), out, hc);
  }

  private static int FILL_COLUMN = 78;

  private static void fill (String message, final PrintStream out)
  {
    fill (message, out, 0);
  }

  private static void fill (String message, final PrintStream out,
                            final int hangColumn)
  {
    int col = hangColumn;
    StringTokenizer tok = new StringTokenizer (message);
    String token = null;
    while (tok.hasMoreTokens() || token != null)
      {
        while (col < hangColumn)
          {
            out.print (' ');
            col++;
          }
        if (token == null)
          token = tok.nextToken();
        if (col + token.length() + (col > hangColumn ? 1 : 0) > FILL_COLUMN
            && col != hangColumn)
          {
            out.println();
            col = 0;
            continue;
          }
        if (col > hangColumn)
          {
            col++;
            out.print (' ');
          }
        out.print (token);
        col += token.length();
        token = null;
      }
    out.println();
  }

  private static void version (final PrintStream out)
  {
    out.print (PROGNAME);
    out.print (" (GNU Crypto version ");
    out.print (Registry.VERSION_STRING);
    out.println (")");
    out.println ("Copyright (C) 2004  Free Software Foundation, Inc.");
    out.println();
    fill (messages.getString ("boilerplate"), out);
  }

  private static void error (final String message)
  {
    System.err.print (PROGNAME);
    System.err.print (": ");
    System.err.println (message);
    System.exit (1);
  }
}
