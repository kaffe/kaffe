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
import java.io.DataOutputStream;
import java.io.DataInputStream;
import java.io.BufferedInputStream;
import java.util.Vector;
import java.net.URL;
import java.net.Socket;
import java.awt.Toolkit;

public class HttpURLConnection
  extends java.net.HttpURLConnection {

private static String proxyHost;
private static int proxyPort = -1;
private static boolean useProxy = false;

// store header fields as (key, value) pairs
private Vector headerFields = new Vector(0);

private Socket sock;
private InputStream in;
private DataOutputStream out;
private boolean redir = getFollowRedirects();

static {
	// How these properties are undocumented in the API doc.  We know
	// about them from www.icesoft.no's webpage
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
	    	// reset response data
		responseCode = -1;
		responseMessage = null;
		headerFields = new Vector();
		
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
		// HTTP/1.0 request line
		out.writeBytes(method + " " + file + " HTTP/1.0\r\n");
		// HTTP/1.1 Host header field, required for virtual server name
		port = url.getPort();
		if ((port == -1) || (port == 80)) {
			out.writeBytes("Host: " + url.getHost() + "\r\n");
		}
		else {
			out.writeBytes("Host: " + url.getHost() + ":" + port + "\r\n");
		}
		// TODO: emit all RequestHeaders see setRequestProperty
		// header end
		out.writeBytes("\r\n");
		out.flush();

		DataInputStream inp = new DataInputStream(in);
		String line;

		// there *must* be a Status-Line
		// Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
		line = inp.readLine();
		if ((line == null) || !line.startsWith("HTTP/")) {
			// It will be better to throw an error,
			// but Sun API does not
			break;
		}
		int versionPos = line.indexOf('/');
		int responsePos = line.indexOf(' ');
		// for the length of the version num we could use 3 but
		// computing the value is probably safer; W3C might introduce
		// HTTP/1.12 for example
		String httpResponseVersion = line.substring(versionPos + 1, responsePos);
		int messagePos = line.indexOf(' ', ++responsePos);
		try {
			// get the Status-Code
			responseCode = Integer.parseInt(line.substring(responsePos, messagePos));
		}
		catch (NumberFormatException _) {
			// It will be better to throw an error,
			// but Sun API does not
			break;
		}
		// the last thing to do is to save the Reason-Phrase
		responseMessage = line.substring(messagePos + 1);

		// save the Status-Line as field 0 as Sun API does
		headerFields.addElement(null);
		headerFields.addElement(line);
		    
		// now read header fields
		for (;;) {
			line = inp.readLine();
			if ((line == null) || line.equals("")) {
				break;
			}
			int pos = line.indexOf(':');
			if (pos > 0) {
				String key = line.substring(0, pos);
				for (pos++; Character.isWhitespace(line.charAt(pos)); pos++)
					;
				String val = line.substring(pos);
				setHeaderField(key, val);
			}
		}
		// when we get here the header has been read, what's on the
		// stream now is an entity or multiple entities

		// IF WE WANT HTTP/1.1 NOW IS THE TIME TO HANDLE CHUNKED
		// ENCODING!

		// Handle redirection
		String location = getHeaderField("Location");
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
	// Ignore field 0, it's the Status-Line
	for (int i = headerFields.size() - 2; i > 0; i -= 2) {
		if (((String)headerFields.elementAt(i)).equalsIgnoreCase(name)) {
			return (String)headerFields.elementAt(i + 1);
		}
	}
	return (null);
}

public String getHeaderField(int pos) {
	if (pos < 0 || pos >= (headerFields.size() >> 1)) {
		return (null);
	}
	return (String)headerFields.elementAt((pos << 1) + 1);
}

public String getHeaderFieldKey(int pos) {
	if (pos < 0 || pos >= (headerFields.size() >> 1)) {
		return (null);
	}
	return (String)headerFields.elementAt(pos << 1);
}

protected void setHeaderField(String key, String value) {
	// Ignore field 0, it's the Status-Line
	for (int i = headerFields.size() - 2; i > 0; i -= 2) {
		if (((String)headerFields.elementAt(i)).equalsIgnoreCase(key)) {
			headerFields.setElementAt(value, i + 1);
			return;
		}
	}
	// if the key did not exist in the header then add it now
	headerFields.addElement(key);
	headerFields.addElement(value);
}

public Object getContent() throws IOException {
    	String ct = getContentType();
	if (ct == null) {
		return (in);
	}
	
	// We only understand a limited number of things so far
	if (ct.startsWith("image/")) {
		return (Toolkit.getDefaultToolkit().getImage(url).getSource());
	}

	// Return the input stream if we don't understand
	return (in);
}

protected void setContentTypeFromName() {
	String ct = getFileNameMap().getContentTypeFor(url.getFile());
	setHeaderField ("Content-Type", ct);
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
