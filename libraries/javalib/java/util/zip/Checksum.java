/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.util.zip;

public interface Checksum {

  public abstract void update(int b);
  public abstract void update(byte b[], int off, int len);
  public abstract long getValue();
  public abstract void reset();

}
