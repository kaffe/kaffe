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

public final class FileDescriptor {

  public static final FileDescriptor in=initSystemFD(new FileDescriptor(), 0);
  public static final FileDescriptor out=initSystemFD(new FileDescriptor(), 1);
  public static final FileDescriptor err=initSystemFD(new FileDescriptor(), 2);

  /*
   * Valid file descriptors have values greater than or equal to 0 
   * therefore initialize fd to -1
   */
  private int fd = -1;

  static {
        System.loadLibrary("io");
  }

  private static native FileDescriptor initSystemFD(FileDescriptor fd, int stream);
  public native boolean valid();
  public native void sync() throws SyncFailedException;
}
