/*
 * Java comm library component.
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package javax.comm;

import java.util.TooManyListenersException;

public abstract class ParallelPort
  extends CommPort {

public static final int LPT_MODE_ANY = 1;
public static final int LPT_MODE_SPP = 2;
public static final int LPT_MODE_PS2 = 3;
public static final int LPT_MODE_EPP = 4;
public static final int LPT_MODE_ECP = 5;
public static final int LPT_MODE_NIBBLE = 6;

public ParallelPort() {
}

public abstract void addEventListener(ParallelPortEventListener lsnr) throws TooManyListenersException;
public abstract void removeEventListener();
public abstract void notifyOnError(boolean notify);
public abstract void notifyOnBuffer(boolean notify);
public abstract int getOutputBufferFree();
public abstract boolean isPaperOut();
public abstract boolean isPrinterBusy();
public abstract boolean isPrinterSelected();
public abstract boolean isPrinterTimedOut();
public abstract boolean isPrinterError();
public abstract void restart();
public abstract void suspend();
public abstract int getMode();
public abstract int setMode(int mode) throws UnsupportedCommOperationException;

}
