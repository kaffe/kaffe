/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.net;

import java.io.IOException;
import java.io.InputStream;
import java.io.Serializable;
import java.util.StringTokenizer;
import kaffe.net.DefaultURLStreamHandlerFactory;

final public class URL
  implements Serializable
{
	private static URLStreamHandlerFactory defaultFactory = new DefaultURLStreamHandlerFactory();
	private static URLStreamHandlerFactory factory;
	private URLStreamHandler handler;
	private String protocol;
	private String host;
	private int port;
	private String file;
	private String ref;
	private URLConnection conn;

public URL(String spec) throws MalformedURLException {
	int fstart;

	/* URL -> <protocol>:[//<hostname>[:port]]/<file> */

	int pend = spec.indexOf(':', 0);
	if (pend == -1) {
		throw new MalformedURLException("no protocol");
	}
	protocol = spec.substring(0, pend);

	int hstart = pend+3;
	if (spec.substring(pend+1, hstart).equals("//")) {
		int hend = spec.indexOf(':', hstart);
		if (hend == -1) {
			hend = spec.indexOf('/', hstart);
			if (hend == -1) {
				throw new MalformedURLException("no host");
			}
			host = spec.substring(hstart, hend);
			port = getDefaultPort(protocol);
		}
		else {
			host = spec.substring(hstart, hend);
			int postart = hend+1;
			int poend = spec.indexOf('/', postart);
			port = Integer.parseInt(spec.substring(postart, poend));
		}
		fstart = spec.indexOf( '/', hstart);
	}
	else {
		host = "";
		port = getDefaultPort(protocol);
		fstart = pend;
	}

	if (fstart != -1) {
		file = spec.substring(fstart+1);
	}
	else {
		file = "";
	}
}

public URL(String protocol, String host, String file) throws MalformedURLException {
	this(protocol, host, -1, file);
}

public URL(String protocol, String host, int port, String file) throws MalformedURLException {
	this.protocol = protocol;
	this.host = host;
	this.file = file;
	if (port == -1) {
		port = getDefaultPort(protocol);
	}
	this.port = port;
}

public URL(URL context, String spec) throws MalformedURLException {
	this (context.toString() + spec);
}

public boolean equals(Object obj) {
	if (obj == null || !(obj instanceof URL)) {
		return (false);
	}
	URL that = (URL)obj;

	if (this.protocol == that.protocol &&
	    this.host == that.host &&
	    this.port == that.port &&
	    this.file == that.file &&
	    this.ref == that.ref) {
		return (true);
	}
	return (false);
}

final public Object getContent() throws IOException {
	openConnection();
	conn.connect();
	return (conn.getContent());
}

private static int getDefaultPort(String protocol)
{
	int port;

	if (protocol == "ftp") {
		port = 21;
	}
	else if (protocol == "telnet") {
		port = 23;
	}
	else if (protocol == "gopher") {
		port = 70;
	}
	else if (protocol == "http") {
		port = 80;
	}
	else if (protocol == "news") {
		port = 119;
	}
	else {
		port = 0;
	}
	return (port);
}

public String getFile() {
	return (file);
}

public String getHost() {
	return (host);
}

public int getPort() {
	return (port);
}

public String getProtocol() {
	return (protocol);
}

public String getRef() {
	return (ref);
}

private static URLStreamHandler getURLStreamHandler(String protocol) throws MalformedURLException {
	URLStreamHandler handler = null;

	if (factory != null) {
		handler = factory.createURLStreamHandler(protocol);
		if (handler != null) {
			return (handler);
		}
	}
	handler = defaultFactory.createURLStreamHandler(protocol);
	if (handler != null) {
		return (handler);
	}

	throw new MalformedURLException("failed to find handler");
}

public int hashCode() {
	return (protocol.hashCode() ^ host.hashCode() ^ file.hashCode());
}

public URLConnection openConnection() throws IOException {
	if (conn == null) {
		if (handler == null) {
			handler = getURLStreamHandler(protocol);
		}
		conn = handler.openConnection(this);
	}
	return (conn);
}

public InputStream openStream() throws IOException {
	if (conn == null) {
		openConnection();
		conn.connect();
	}
	return (conn.getInputStream());
}

public boolean sameFile(URL that) {
	if (this.protocol == that.protocol &&
	    this.host == that.host &&
	    this.port == that.port &&
	    this.file == that.file) {
		return (true);
	}
	return (false);
}

protected void set(String protocol, String host, int port, String file, String ref) {
	this.protocol = protocol;
	this.host = host;
	this.port = port;
	this.file = file;
	this.ref = ref;
}

public static synchronized void setURLStreamHandlerFactory(URLStreamHandlerFactory fac) {
	if (factory == null) {
		factory = fac;
	}
	else {
		throw new Error("factory already set");
	}
}

public String toExternalForm() {
	return (toString());
}

public String toString() {
	return (protocol + "://" + host + ":" + port + "/" + file);
}
}
