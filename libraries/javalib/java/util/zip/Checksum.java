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

  void update(int b);
  void update(byte b[], int off, int len);
  long getValue();
  void reset();

}
