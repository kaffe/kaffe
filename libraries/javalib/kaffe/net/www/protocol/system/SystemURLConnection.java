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

public class SystemURLConnection extends URLConnection {

final static private String headers[] = {
	"content-encoding",
	"content-length",
	"content-type",
	"date",
	"expiration",
	"If-Modified-Since",
	"lastModified",
};
private String headersValue[] = new String[headers.length];

final static private int ContentEncoding = 0;
final static private int ContentLength = 1;
final static private int ContentType = 2;
final static private int Date = 3;
final static private int Expiration = 4;
final static private int IfModifiedSince = 5;
final static private int LastModified = 6;

private byte[] data;

public SystemURLConnection(URL url)
{
	super(url);
}

public void connect() throws IOException
{
	String filename = url.getFile();
	data = ClassLoader.getSystemResourceAsBytes0(filename);
	headersValue[ContentEncoding] = fileNameMap.getContentTypeFor(filename);
}

public InputStream getInputStream() throws IOException
{
	if (data == null) {
		return (null);
	}
	else {
		return (new ByteArrayInputStream(data));
	}
}

public Object getContent() throws IOException {
	if (headersValue[ContentEncoding] == null) {
		return (null);
	}
	/*
	 * We only understand a limited number of things so far
	 */
	if (headersValue[ContentEncoding].equals("image/gif") ||
	    headersValue[ContentEncoding].equals("image/jpeg")) {
		return (Toolkit.getDefaultToolkit().createImage(data));
	}

	// Return null if we don't understand
	return (null);
}

public String getHeaderField(int pos)
{
	if (pos < 0 || pos >= headersValue.length) {
		return (null);
	}
	return (headersValue[pos]);
}

public String getHeaderField(String name)
{
	for (int i = 0; i < headers.length; i++) {
		if (name == headers[i]) {
			return (getHeaderField(i));
		}
	}
	return (null);
}

public String getHeaderFieldKey(int pos)
{
	if (pos < 0 || pos >= headers.length) {
		return (null);
	}
	return (headers[pos]);
}

}
