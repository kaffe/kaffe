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

public class FileReader extends InputStreamReader {

  public FileReader(String fileName) throws FileNotFoundException
  {
    super(new FileInputStream(fileName));
  }

  public FileReader(File file) throws FileNotFoundException
  {
    super(new FileInputStream(file));
  }

  public FileReader(FileDescriptor fd)
  {
    super(new FileInputStream(fd));
  }

}
