/*
 * Java core library component.
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.net.www.protocol.ftp;

import java.net.URLStreamHandler;
import java.net.URLConnection;
import java.net.URL;

public class Handler extends URLStreamHandler {

protected int getDefaultPort() {
	return 21;
}

protected URLConnection openConnection(URL u)
{
	return (new FtpURLConnection(u));
}

}
