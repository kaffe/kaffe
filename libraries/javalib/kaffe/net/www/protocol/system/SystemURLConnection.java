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

import java.awt.Image;
import java.awt.Toolkit;
import java.io.ByteArrayInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.net.FileNameMap;
import java.net.URL;
import java.net.URLConnection;
import kaffe.net.www.protocol.BasicURLConnection;
import kaffe.lang.Application;

public class SystemURLConnection
  extends BasicURLConnection
{
	private InputStream data;

public SystemURLConnection(URL url)
{
	super(url);
}

public void connect() throws IOException
{
	String filename = url.getFile();
	Application currentApp = Application.getApplication();
	if (currentApp != null) {
		data = currentApp.getResourceAsStream(filename);
	} else {
		data = ClassLoader.getSystemResourceAsStream(filename);
	}
	setContentTypeFromName();
}

public InputStream getInputStream() throws IOException
{
	if (data == null) {
		throw new FileNotFoundException(url.getFile());
	}
	return data;
}
}
