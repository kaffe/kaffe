/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.net;

import java.lang.String;
import java.io.IOException;

abstract public class URLStreamHandler {

public URLStreamHandler() {}

abstract protected URLConnection openConnection(URL u) throws IOException;

protected void parseURL(URL u, String spec, int start, int limit) {
}

protected void setURL(URL u, String protocol, String host, int port, String file, String ref) {
	u.set(protocol, host, port, file, ref);
}

protected String toExternalForm(URL u) {
	return (u.toExternalForm());
}
}
