package java.net;

import java.io.IOException;
import java.io.InputStream;
import java.io.Serializable;
import java.lang.String;
import java.util.StringTokenizer;
import kaffe.net.DefaultURLStreamHandlerFactory;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
final public class URL
  implements Serializable
{
	final private static long serialVersionUID = -7627629688361524110L;
	private static URLStreamHandlerFactory defaultFactory = new DefaultURLStreamHandlerFactory();
	private static URLStreamHandlerFactory factory;
	private URLStreamHandler handler;
	private String protocol;
	private String host = "";
	private int port = -1;
	private String file = "/";
	private String ref;
	private URLConnection conn;

public URL(String spec) throws MalformedURLException {
	parseURL(spec);
}

public URL(String protocol, String host, String file) throws MalformedURLException {
	this(protocol, host, -1, file);
}

public URL(String protocol, String host, int port, String file) throws MalformedURLException {
	if (protocol == null || host == null || file == null) {
		throw new NullPointerException();
	}
	handler = getURLStreamHandler(protocol);
	handler.setURL(this, protocol, host, port, file, "");
}

public URL(URL context, String spec) throws MalformedURLException {
	// If no context of spec has a protocol - ignore context.
	if (context == null || spec.indexOf(':') != -1) {
		parseURL(spec);
	}
	// Else if spec is absolute just use context's protocol.
	else if ( spec.charAt(0) == '/') {
		protocol = context.protocol;
		host = context.host;
		port = context.port;
		handler = getURLStreamHandler(protocol);
		handler.parseURL(this, spec, 0, spec.length());
	}
	else {
		//merge path with relative spec
		String cs = context.toString();
		int lsi = cs.lastIndexOf( '/') + 1;
		parseURL(cs.substring(0, lsi) + spec);
	}

// System.out.println("merging " + context.toString() + " and " + spec + " into " + this);
}

public boolean equals(Object obj) {
	if (obj == null || !(obj instanceof URL)) {
		return (false);
	}
	URL that = (URL)obj;

	if (this.protocol.equals( that.protocol) &&
	    this.host.equals(that.host) &&
	    this.port == that.port &&
	    this.file.equals(that.file) &&
	    ((this.ref == null && that.ref == null)
	      || this.ref.equals(that.ref)) ) {
		return (true);
	}
	return (false);
}

final public Object getContent() throws IOException {
	openConnection();
	return (conn.getContent());
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

	throw new MalformedURLException("unknown protocol: " + protocol);
}

public int hashCode() {
	return (protocol.hashCode() ^ host.hashCode() ^ file.hashCode());
}

public URLConnection openConnection() throws IOException {
	// We *ALWAYS* open a new connection even if we already have
	// one open.
	conn = handler.openConnection(this);
	conn.connect();
	return (conn);
}

final public InputStream openStream() throws IOException {
	// We *ALWAYS* open a new connection even if we already have
	// one open.
	openConnection();
	return (conn.getInputStream());
}

private void parseURL(String spec) throws MalformedURLException {

	/* URL -> [<protocol>:][//<hostname>[:port]][/<file>] */

//System.out.println("Parsing URL " + spec);

	int pend = spec.indexOf(':', 0);
	if (pend == -1) {
		protocol = "file";
	}
	else {
		protocol = spec.substring(0, pend);
	}
	handler = getURLStreamHandler(protocol);
	handler.parseURL(this, spec, pend+1, spec.length());

//System.out.println("URL = " + this);
}

public boolean sameFile(URL that) {
	if (this.protocol.equals(that.protocol) &&
	    this.host.equals(that.host) &&
	    this.port == that.port &&
	    this.file.equals(that.file) ) {
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
	return (handler.toExternalForm(this));
}

public String toString() {
	return (toExternalForm());
}
}
