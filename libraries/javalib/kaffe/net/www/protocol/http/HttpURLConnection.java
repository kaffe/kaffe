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
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.util.Vector;
import java.util.Iterator;
import java.net.URL;
import java.net.Socket;
import java.awt.Toolkit;

public class HttpURLConnection
  extends java.net.HttpURLConnection {

private static String proxyHost;
private static int proxyPort = -1;
private static boolean useProxy;

// store header fields as (key, value) pairs
private Vector headerFields = new Vector(0);

private Socket sock;
private InputStream in;
private DataOutputStream out;
private File tmpFile;
private OutputStream tmpOut;
private boolean redir = getFollowRedirects();

static {
	// How these properties are undocumented in the API doc.  We know
	// about them from www.icesoft.no's webpage
	proxyHost = System.getProperty("http.proxyHost");
	if (proxyHost != null && !proxyHost.equals("")) {
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
			try {
				proxyPort = Integer.parseInt(pp);
			} catch (NumberFormatException e) {
				proxyPort = -1; // Make sure.
			}
		}
	}
}


public HttpURLConnection(URL url) {
	super(url);
	connected = false;
}

public void connect() throws IOException {
	if(connected) return;
	connected = true;

	if (tmpOut != null) tmpOut.close();
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
			if (port == -1) {
				port = 80;
			}
			try {
				sock = new Socket(host, port);
			} catch (IOException e) {
				// Sun JDK just ignores proxy errors.
			}
		}

		if (sock == null) {
			port = url.getPort();
			host = url.getHost();
			if (port == -1) {
				port = 80;
			}
			sock = new Socket(host, port);
		}

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
		if (doOutput) method = "POST";
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
		if(doOutput) {
			long l = (tmpFile != null ? tmpFile.length() : 0L);
			out.writeBytes("Content-Length: " + l + "\r\n");
		}

		// emit all RequestHeaders
		for (Iterator iter = requestProperties.iterator(); iter.hasNext();) {
			String key = (String)iter.next();
			String val = (String)iter.next();
			out.writeBytes(key);
			if (val != null) 
				out.writeBytes( ": " + val );
			out.writeBytes("\r\n");
		}

        
		if (! useCaches) {
			if ( ! requestProperties.contains("Cache-Control") ) 
				out.writeBytes("Cache-Control: no-cache\r\n");
			if ( ! requestProperties.contains("Pragma") ) 
				out.writeBytes("Pragma: no-cache\r\n");
		}

		// header end
		out.writeBytes("\r\n");
		if(doOutput) {
			if (tmpFile != null) {
				InputStream tmpIn = new FileInputStream(
					tmpFile);
				byte[] buf = new byte[4096];
				while (true) {
					int l = tmpIn.read(buf, 0, buf.length);
					if (l < 0) break;
					out.write(buf,0,l);
				}
				tmpIn.close();
				tmpFile.delete();
			}
		}
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
		if (!redir || responseCode < HTTP_MULT_CHOICE || responseCode > HTTP_USE_PROXY || location == null) {
			break;
		}
		url = new URL(location);
	}
}

private void doConnect() throws IOException {
	if (!connected) connect();
}

private void tryConnect() {
	if (!connected) {
		try {
			connect();
		}
		catch (IOException e) {
			//  I don't know what to do
		}
	}
}

public InputStream getInputStream() throws IOException {
	doConnect();
	return (in);
}

public OutputStream getOutputStream() throws IOException {
	if (tmpOut != null) return tmpOut;
	tmpFile = File.createTempFile("POST",".tmp");
	tmpFile.deleteOnExit();
	tmpOut = new FileOutputStream(tmpFile);
	return tmpOut;
}

public String getHeaderField(String name) {
	tryConnect();
	// Ignore field 0, it's the Status-Line
	for (int i = headerFields.size() - 2; i > 0; i -= 2) {
		if (((String)headerFields.elementAt(i)).equalsIgnoreCase(name)) {
			return (String)headerFields.elementAt(i + 1);
		}
	}
	return (null);
}

public String getHeaderField(int pos) {
	tryConnect();
	if (pos < 0 || pos >= (headerFields.size() >> 1)) {
		return (null);
	}
	return (String)headerFields.elementAt((pos << 1) + 1);
}

public String getHeaderFieldKey(int pos) {
	tryConnect();
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
	tryConnect();
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
