/**
 * NullOutputStream - an OutputStream without any side effects (the most
 *                    clean thing)
 *
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 */

package kaffe.io;

import java.io.OutputStream;

public class NullOutputStream
  extends OutputStream
{
	public static NullOutputStream singleton;

static {
	singleton = new NullOutputStream();
}

public NullOutputStream () {
}

public void write ( int b ) {
	// the big black hole..
}
}
