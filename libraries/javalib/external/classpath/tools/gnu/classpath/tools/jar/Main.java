/* Main.java - jar program main()
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
 02110-1301 USA.

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


package gnu.classpath.tools.jar;

import gnu.classpath.tools.getopt.ClasspathToolParser;
import gnu.classpath.tools.getopt.FileArgumentCallback;
import gnu.classpath.tools.getopt.Option;
import gnu.classpath.tools.getopt.OptionException;
import gnu.classpath.tools.getopt.OptionGroup;
import gnu.classpath.tools.getopt.Parser;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.zip.ZipOutputStream;

public class Main
{
  /** The mode of operation. This is the class representing
   * the action; we make a new instance before using it. It
   * must be a subclass of Action. 'null' means the mode
   * has not yet been set.  */
  Class operationMode;

  /** The archive file name.  */
  File archiveFile;

  /** The zip storage mode.  */
  int storageMode = ZipOutputStream.DEFLATED;

  /** True if we should read file names from stdin.  */
  boolean readNamesFromStdin = false;

  /** True for verbose mode.  */
  boolean verbose = false;

  /** True if we want a manifest file.  */
  boolean wantManifest = true;

  /** Name of manifest file to use.  */
  File manifestFile;

  /** A list of Entry objects, each describing a file to write.  */
  ArrayList entries = new ArrayList();

  /** Used only while parsing, holds the first argument for -C.  */
  String changedDirectory;

  class HandleFile
      extends FileArgumentCallback
  {
    public void notifyFile(String fileArgument)
    {
      Entry entry;
      if (changedDirectory != null)
        {
          entry = new Entry(new File(changedDirectory, fileArgument),
                            fileArgument);
          changedDirectory = null;
        }
      else
        entry = new Entry(new File(fileArgument));
      entries.add(entry);
    }
  }

  // An option that knows how to set the operation mode.
  private class ModeOption
      extends Option
  {
    private Class mode;

    public ModeOption(char shortName, String description, Class mode)
    {
      super(shortName, description);
      this.mode = mode;
    }

    public void parsed(String argument) throws OptionException
    {
      if (operationMode != null)
        throw new OptionException("operation mode already specified");
      operationMode = mode;
    }
  }

  private Parser initializeParser()
  {
    Parser p = new ClasspathToolParser("jar");
    p.setHeader("Usage: jar -ctxu [OPTIONS] jar-file [-C DIR FILE] FILE...");

    OptionGroup grp = new OptionGroup("Operation mode");
    grp.add(new ModeOption('c', "create a new archive", Creator.class));
    grp.add(new ModeOption('x', "extract from archive", Extractor.class));
    grp.add(new ModeOption('t', "list archive contents", Lister.class));
    grp.add(new ModeOption('u', "update archive", Updater.class));
    p.add(grp);

    grp = new OptionGroup("Operation modifiers");
    grp.add(new Option('f', "specify archive file name", "FILE")
    {
      public void parsed(String argument) throws OptionException
      {
        // FIXME: error if already set.
        archiveFile = new File(argument);
      }
    });
    grp.add(new Option('0', "store only; no ZIP compression")
    {
      public void parsed(String argument) throws OptionException
      {
        storageMode = ZipOutputStream.STORED;
      }
    });
    grp.add(new Option('v', "verbose operation")
    {
      public void parsed(String argument) throws OptionException
      {
        verbose = true;
      }
    });
    grp.add(new Option('M', "do not create a manifest file")
    {
      public void parsed(String argument) throws OptionException
      {
        wantManifest = false;
      }
    });
    grp.add(new Option('m', "specify manifest file", "FILE")
    {
      public void parsed(String argument) throws OptionException
      {
        manifestFile = new File(argument);
      }
    });
    // -@
    p.add(grp);

    grp = new OptionGroup("File name selection");
    grp.add(new Option('C', "change to directory before the next file",
                       "DIR FILE")
    {
      public void parsed(String argument) throws OptionException
      {
        changedDirectory = argument;
      }
    });
    p.add(grp);
    // -i - need to parse classes

    return p;
  }

  private void run(String[] args) throws OptionException,
      InstantiationException, IllegalAccessException, IOException
  {
    Parser p = initializeParser();
    // Special hack to emulate old tar-style commands.
    if (args[0].charAt(0) != '-')
      args[0] = '-' + args[0];
    p.parse(args, new HandleFile());
    if (operationMode == null)
      throw new OptionException("must specify one of -t, -c, -u, or -x");
    if (changedDirectory != null)
      throw new OptionException("-C argument requires both directory and filename");
    Action t = (Action) operationMode.newInstance();
    t.run(this);
  }

  public static void main(String[] args)
  {
    Main jarprogram = new Main();
    try
      {
        jarprogram.run(args);
      }
    catch (OptionException arg)
      {
        System.err.println("jar: " + arg.getMessage());
        System.exit(1);
      }
    catch (Exception e)
      {
        System.err.println("jar: internal error:");
        e.printStackTrace(System.err);
        System.exit(1);
      }
  }
}
