/*
 * SystemURLConnection -
 *  URL connection to internal system resources.
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.net.www.protocol.system;

import java.net.URLConnection;
import java.net.URL;
import java.net.FileNameMap;
import java.awt.Image;
import java.io.IOException;
import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.awt.Toolkit;
import kaffe.net.www.protocol.BasicURLConnection;

public class SystemURLConnection extends BasicURLConnection {

private InputStream data;

public SystemURLConnection(URL url)
{
	super(url);
}

public void connect() throws IOException
{
	String filename = url.getFile();
	data = ClassLoader.getSystemResourceAsStream(filename);
	setContentTypeFromName();
}

public InputStream getInputStream() throws IOException
{
	return data;
}

}
