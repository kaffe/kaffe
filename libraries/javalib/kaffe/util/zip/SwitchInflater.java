/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.util.zip;

import java.util.zip.DataFormatException;
import java.util.zip.Inflater;

public class SwitchInflater extends Inflater {

  private int maxlen;
  private boolean stored;

  public SwitchInflater(boolean nowrap, boolean stored)
  {
    super(nowrap);
    this.stored = stored;
  }

  public SwitchInflater()
  {
    this(false, false);
  }

  public synchronized void setInput(byte b[], int o, int l)
  {
    super.setInput(b, o, l);
  }

  public synchronized boolean needsInput()
  {
    if (stored) {
      return (len > 0 ? false : true);
    }
    else {
      return super.needsInput();
    }
  }

  public synchronized boolean finished()
  {
    if (stored) {
      return (maxlen > 0 ? false : true);
    }
    else {
      return super.finished();
    }
  }

  public synchronized int inflate(byte b[], int o, int l) throws DataFormatException {
	if (stored) {
		/* Special case: len may be zero */
		if (len == 0)
		        return 0;

		if (l >= maxlen) {
			l = maxlen;
		}
		if (l >= len) {
			l = len;
		}
		System.arraycopy(buf, off, b, o, l);
		off += l;
		len -= l;
		maxlen -= l;
	}
	else {
		l = super.inflate(b, o, l);
	}
	return (l);
  }

  public synchronized void reset() {
    int olen = len;
    super.reset();
    len = olen;
  }

  public synchronized void setMode(boolean stored) {
    this.stored = stored;
  }

  public synchronized void setLength(int len) {
    this.maxlen = len;
  }
}
