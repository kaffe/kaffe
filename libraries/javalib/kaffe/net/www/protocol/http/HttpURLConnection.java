/*
 * HttpURLConnection -
 *  Simple HTTP URL handler.
 *
 * Copyright (c) 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.net.www.protocol.http;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.DataOutputStream;
import java.io.DataInputStream;
import java.util.StringTokenizer;
import java.net.URL;
import java.net.URLConnection;
import java.net.Socket;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.net.URLConnection;
import java.awt.Toolkit;

public class HttpURLConnection
  extends java.net.HttpURLConnection {

final private static String[] headers = {
	"content-encoding",
	"content-length",
	"content-type",
	"date",
	"expiration",
	"If-Modified-Since",
	"lastModified",
};
final private static int ContentEncoding = 0;
final private static int ContentLength = 1;
final private static int ContentType = 2;
final private static int Date = 3;
final private static int Expiration = 4;
final private static int IfModifiedSince = 5;
final private static int LastModified = 6;
private String[] headersValue = new String[headers.length];

private Socket sock;
private DataInputStream in;
private DataOutputStream out;

public HttpURLConnection(URL url) {
	super(url);
}

public void connect() throws IOException {
	int port = url.getPort();
	if (port == -1) {
		port = 80;
	}
	sock = new Socket(url.getHost(), port);

	in = new DataInputStream(sock.getInputStream());
	out = new DataOutputStream(sock.getOutputStream());

	// Make the http request.
	String file = url.getFile();
	if (file.equals("")) {
		file = "/";
	}
	out.writeBytes(method + " " + file + " HTTP/1.0\n\n");

	for (;;) {
		String line = in.readLine();
		if (line == null || line.equals("")) {
			break;
		}
		// This is the page response 
		else if (line.startsWith("HTTP")) {
			responseMessage = line;
			StringTokenizer tok = new StringTokenizer(line);
			if (tok.countTokens() >= 3) {
				tok.nextToken();
				try {
					responseCode = Integer.parseInt(tok.nextToken());
				}
				catch (NumberFormatException _) {
					responseCode = HTTP_SERVER_ERROR;
				}
			}
		}
		// Everything else is a colon seperated header and value.
		else {
			StringTokenizer tok = new StringTokenizer(line, " \t:");
			if (tok.countTokens() == 2) {
				String key = tok.nextToken();
				String val = tok.nextToken();
				setHeaderField(key, val);
			}
		}
	}
}

public InputStream getInputStream() throws IOException {
	return (in);
}

public String getHeaderField(String name) {
	for (int i = 0; i < headers.length; i++) {
		if (headers[i].equalsIgnoreCase( name)) {
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
		if ( headers[i].equalsIgnoreCase( key) ) {
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

public void disconnect() {
	try {
		sock.close();
	}
	catch (IOException _) {
	}
}

public boolean usingProxy() {
	return (false);
}

}
