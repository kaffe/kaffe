/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.io;


public class FileWriter extends OutputStreamWriter {

  public FileWriter(String fileName) throws IOException
  {
    super(new FileOutputStream(fileName));
  }

  public FileWriter(String fileName, boolean app) throws IOException
  {
    super(new FileOutputStream(fileName, app));
  }

  public FileWriter(File file) throws IOException
  {
    super(new FileOutputStream(file));
  }

  public FileWriter(FileDescriptor fd)
  {
    super(new FileOutputStream(fd));
  }
}
