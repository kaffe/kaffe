package kaffe.net.www.protocol.file;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.net.URLConnection;

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
public class FileURLConnection
  extends URLConnection
{
	final private static String[] headers = {
	"content-encoding",
	"content-length",
	"content-type",
	"date",
	"expiration",
	"If-Modified-Since",
	"lastModified",
};
	private String[] headersValue = new String[headers.length];
	final private static int ContentEncoding = 0;
	final private static int ContentLength = 1;
	final private static int ContentType = 2;
	final private static int Date = 3;
	final private static int Expiration = 4;
	final private static int IfModifiedSince = 5;
	final private static int LastModified = 6;

public FileURLConnection(URL url)
{
	super(url);
}

public void connect() throws IOException
{
	setHeaderField("content-type", guessContentTypeFromName(url.getFile()));
}

public String getHeaderField(String name)
{
	for (int i = 0; i < headers.length; i++) {
		if (headers[i].equals( name)) {
			return (getHeaderField(i));
		}
	}
	return (null);
}

public String getHeaderField(int pos)
{
	if (pos < 0 || pos >= headersValue.length) {
		return (null);
	}
	return (headersValue[pos]);
}

public String getHeaderFieldKey(int pos)
{
	if (pos < 0 || pos >= headers.length) {
		return (null);
	}
	return (headers[pos]);
}

public InputStream getInputStream() throws IOException
{
	return (new FileInputStream(url.getFile()));
}

void setHeaderField( String key, String value) {
	for ( int i=0; i<headers.length; i++ ) {
		if ( headers[i].equals( key) ) {
			headersValue[i] = value;
			break;
		}
	}
}
}
