/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.lang;

public class Object {
  public final native Class  getClass();
  public       native int    hashCode();
  protected    native Object clone() throws CloneNotSupportedException;
  public final native void   notify();
  public final native void   notifyAll();
  
  public boolean equals(Object obj) {
    return (this==obj);  /* Well either its equal or it aint! */
  }
  
  public String toString() {
    /* According to the 1.0.2 docs this uses the hashCode, but:

       "the hex. string printed is based on the hash code, but may not 
        be the actual hash code"

       Nice one, Sun! */
    
    return "@"+((new Integer(hashCode())).toString());
  }
  
  public final void wait(long timeout, int nanos) throws InterruptedException {
    /* Ignore nanos, except avoid clipping a non-zero quantity to zero */
    if (timeout == 0 && nanos > 0)
       timeout++;

    wait(timeout);    
  }
  
  public final void wait() throws InterruptedException {
    /* Just wait forever */
    wait(0);
  }

  public final void wait(long timeout) throws InterruptedException {
    wait0(timeout);
    if (Thread.interrupted()) {
      throw new InterruptedException();
    }
  }
  
  protected void finalize() throws Throwable {
    /* Does nothing for Object class */
  }

  private final native void wait0(long timeout);
}
