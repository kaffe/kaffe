/*
 * BasicURLConnection -
 *  Simple abstract URL handler.
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.net.www.protocol;

import java.io.IOException;
import java.net.URL;
import java.net.URLConnection;
import java.awt.Toolkit;

public abstract class BasicURLConnection
  extends URLConnection
{
	final protected static String[] headers = {
	"content-encoding",
	"content-length",
	"content-type",
	"date",
	"expiration",
	"If-Modified-Since",
	"lastModified"
};
	protected String[] headersValue = new String[headers.length];
	final protected static int ContentEncoding = 0;
	final protected static int ContentLength = 1;
	final protected static int ContentType = 2;
	final protected static int Date = 3;
	final protected static int Expiration = 4;
	final protected static int IfModifiedSince = 5;
	final protected static int LastModified = 6;

public BasicURLConnection(URL url) {
	super(url);
}

public abstract void connect() throws IOException;

public String getHeaderField(String name) {
	for (int i = 0; i < headers.length; i++) {
		if (headers[i].equals( name)) {
			return (getHeaderField(i));
		}
	}
	return (null);
}

public String getHeaderField(int pos) {
	if (pos < 0 || pos >= headersValue.length) {
		return (null);
	}
	return (headersValue[pos]);
}

public String getHeaderFieldKey(int pos) {
	if (pos < 0 || pos >= headers.length) {
		return (null);
	}
	return (headers[pos]);
}

protected void setHeaderField( String key, String value) {
	for ( int i=0; i<headers.length; i++ ) {
		if ( headers[i].equals( key) ) {
			headersValue[i] = value;
			break;
		}
	}
}

public Object getContent() throws IOException {
	if (headersValue[ContentEncoding] == null) {
		return (null);
	}
	/*
	 * We only understand a limited number of things so far
	 */
	if (headersValue[ContentEncoding].startsWith("image/")) {
		return (Toolkit.getDefaultToolkit().getImage(url).getSource());
	}

	// Return null if we don't understand
	return (null);
}

protected void setContentTypeFromName() {
	String ct = getFileNameMap().getContentTypeFor( url.getFile());
	headersValue[ContentType] = ct;
	headersValue[ContentEncoding] = ct;
}

}
