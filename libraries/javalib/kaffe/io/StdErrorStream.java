/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.io;

import java.io.IOException;
import java.io.OutputStream;

public class StdErrorStream
  extends OutputStream {

static {
        System.loadLibrary("io");
}

public StdErrorStream() {
}

native public void write(int b) throws IOException;

}
