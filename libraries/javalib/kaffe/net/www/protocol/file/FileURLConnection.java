/*
 * FileURLConnection -
 *  Simple file URL handler.
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.net.www.protocol.file;

import java.net.URLConnection;
import java.net.URL;
import java.io.IOException;
import java.io.FileInputStream;
import java.io.InputStream;

public class FileURLConnection extends URLConnection {

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

private FileInputStream in;

public FileURLConnection(URL url)
{
	super(url);
}

public void connect() throws IOException
{
	in = new FileInputStream(url.getFile());
}

public InputStream getInputStream() throws IOException
{
	return (in);
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
