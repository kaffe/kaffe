/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.beans;

public interface Visibility {

  public abstract boolean needsGui();
  public abstract void dontUseGui();
  public abstract void okToUseGui();
  public abstract boolean avoidingGui();

}
