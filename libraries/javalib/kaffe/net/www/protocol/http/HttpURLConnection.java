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
import java.io.BufferedInputStream;
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
	"content-location",
	"date",
	"expiration",
	"If-Modified-Since",
	"lastModified",
	"location",
	"server"
};
final private static int ContentEncoding = 0;
final private static int ContentLength = 1;
final private static int ContentType = 2;
final private static int Date = 3;
final private static int Expiration = 4;
final private static int IfModifiedSince = 5;
final private static int LastModified = 6;

private static String proxyHost;
private static int proxyPort = -1;
private static boolean useProxy = false;

private String[] headersValue = new String[headers.length];

private Socket sock;
private InputStream in;
private DataOutputStream out;
private boolean redir = getFollowRedirects();

static {
	// How these properties are undocumented in the API doc.  We know
	// about them from www.icesoft.no's webpage
	//
	proxyHost = System.getProperty("http.proxyHost");
	if (proxyHost != null) {
		// Sun also supports a http.nonProxyHosts property to
		// avoid proxy use for local sites.  It's a regular expression
		// like so "*.pa.dec.com|*.compaq.com"
		// For now, we always use a proxy if these properties are set.
		// 
		// In JDK 1.2, the properties are read when an connection
		// is established, allowing the use of different proxies
		// during the run-time of a program.  FIXME
		useProxy = true;
		String pp = System.getProperty("http.proxyPort");
		if (pp != null) {
			proxyPort = Integer.parseInt(pp);
		}
	}
}


public HttpURLConnection(URL url) {
	super(url);
}

public void connect() throws IOException {
	for (;;) {
		int port;
		String host;
		if (useProxy) {
			port = proxyPort;
			host = proxyHost;
		}
		else {
			port = url.getPort();
			host = url.getHost();
		}
		if (port == -1) {
			port = 80;
		}
		sock = new Socket(host, port);

		in = new BufferedInputStream(sock.getInputStream());
		out = new DataOutputStream(sock.getOutputStream());

		// Make the http request.
		String file;
		if (useProxy) {
			file = url.toString();
		}
		else {
			file = url.getFile();
			if (file.equals("")) {
				file = "/";
			}
		}
		out.writeBytes(method + " " + file + " HTTP/1.0\r\n\r\n");
		out.flush();

		DataInputStream inp = new DataInputStream(in);
		for (;;) {
			String line = inp.readLine();
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
				int pos = line.indexOf(':');
				if (pos > 0) {
					String key = line.substring(0, pos);
					for (pos++; Character.isWhitespace(line.charAt(pos)); pos++)
						;
					String val = line.substring(pos);
					setHeaderField(key, val);
				}
			}
		}

		// Handle redirection
		String location = getHeaderField("location");
		if (redir == false || responseCode < HTTP_MULT_CHOICE || responseCode > HTTP_USE_PROXY || location == null) {
			break;
		}
		url = new URL(location);
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
		return (in);
	}
	/*
	 * We only understand a limited number of things so far
	 */
	if (headersValue[ContentEncoding].startsWith("image/")) {
		return (Toolkit.getDefaultToolkit().getImage(url).getSource());
	}

	// Return the input stream if we don't understand
	return (in);
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
	sock = null;
}

public boolean usingProxy() {
	return (useProxy);
}

/**
 * Used by the ICE browser.
 */
public void setInstanceFollowRedirects(boolean redir) {
	this.redir = redir;
}

}
