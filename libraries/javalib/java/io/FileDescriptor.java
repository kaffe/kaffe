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

  public static FileDescriptor in=initSystemFD(new FileDescriptor(), 0);
  public static FileDescriptor out=initSystemFD(new FileDescriptor(), 1);
  public static FileDescriptor err=initSystemFD(new FileDescriptor(), 2);

  private int fd;

  static {
        System.loadLibrary("io");
  }

  private static native FileDescriptor initSystemFD(FileDescriptor fd, int stream);
  public native boolean valid();
  public native void sync() throws SyncFailedException;
}
