
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
import java.lang.String;
import java.util.HashMap;
import java.util.StringTokenizer;
import kaffe.net.DefaultURLStreamHandlerFactory;

public final class URL implements Serializable {
	private final static long serialVersionUID = -7627629688361524110L;
	private final static String HANDLER_PROP = "java.protocol.handler.pkgs";
	private static URLStreamHandlerFactory defaultFactory =
		new DefaultURLStreamHandlerFactory();
	private static URLStreamHandlerFactory factory;
	private static final HashMap defaultHandlers = new HashMap();
	private URLStreamHandler handler;
	private String protocol;
	private String host = "";
	private int port = -1;
	private String file = "";
	private String ref;
	private URLConnection conn;

public URL(String protocol, String host, String file)
		throws MalformedURLException {
	this(protocol, host, -1, file, null);
}

public URL(String protocol, String host, int port, String file)
		throws MalformedURLException {
	this(protocol, host, port, file, null);
}

// Note special handling for "file:" that mirrors what JDK seems to do
public URL(String protocol, String host, int port, String file,
		URLStreamHandler handler) throws MalformedURLException {
	if (protocol == null
	    || (host == null && !protocol.equals("file"))
	    || file == null) {
		throw new NullPointerException();
	}
	if (protocol.equals("file") && host != null) {
		setHandler("ftp", handler);
	} else {
		setHandler(protocol, handler);
	}
	this.handler.setURL(this, protocol, host, port, file, "");
}

public URL(String spec) throws MalformedURLException {
	this(null, spec);
}

public URL(URL context, String spec) throws MalformedURLException {
	this(context, spec, null);
}

//
// Create a new URL from an (optional) existing context and a String.
//
// Certain parts of the way an URL is parsed are universal for all protocols,
// and others are protocol specific. This is the way we parse an URL:
//
//	URL   ->   [<protocol>:]<protocol-specific-stuff>[#<reference>]
//
// Note that handler.parseURL() is responsible for merging the URL together
// with any inherited context. This is because different protocols have
// different rules for how a relative URL is interpreted with context.
//
public URL(URL context, String spec, URLStreamHandler handler)
		throws MalformedURLException {
	int firstColon = spec.indexOf(':');
	int firstSlash = spec.indexOf('/');
	int lastHash = spec.lastIndexOf('#');

	// See if "spec" specifies a protocol
	if (firstColon != -1
	    && (firstSlash == -1 || firstColon < firstSlash)) {

		// Save protocol
		protocol = spec.substring(0, firstColon);

		// If protocols are the same, inherit from context
		if (context != null && context.protocol.equals(protocol)) {
			host = context.host;
			port = context.port;
			file = context.file;
		}
	} else {	// Need to inherit the protocol from the context
		if (context == null) {
			throw new MalformedURLException("no protocol: " + spec);
		}
		protocol = context.protocol;
		host = context.host;
		port = context.port;
		file = context.file;
	}

	// Get anchor reference, if any
	if (lastHash != -1 && lastHash + 1 < spec.length()) {
		ref = spec.substring(lastHash + 1);
	}

	// Use handler to parse the rest, within
	//   the context defined for this URL so far
	setHandler(protocol, handler);
	try {
		this.handler.parseURL(this, spec, firstColon + 1,
		    lastHash != -1 ? lastHash : spec.length());
	}
	catch (Error e) {
		throw new MalformedURLException(e.getMessage());
	}
}

// Check the supplied handler if any, otherwise compute default handler
private void setHandler(String protocol, URLStreamHandler handler)
		throws MalformedURLException {

	// User-supplied handler?
	if (handler != null) {
		System.getSecurityManager().checkPermission(
		    new NetPermission("specifyStreamHandler"));
		this.handler = handler;
		return;
	}

	// Have we already determined the default handler for this protocol?
	handler = (URLStreamHandler) defaultHandlers.get(protocol);
	if (handler != null) {
		this.handler = handler;
		return;
	}

	// Try handler factory previously specified, if any
	if (factory != null) {
		handler = factory.createURLStreamHandler(protocol);
		if (handler != null) {
			defaultHandlers.put(protocol, handler);
			this.handler = handler;
			return;
		}
	}

	// Try factories specified in "java.protocol.handler.pkgs" property
	String faclist = System.getProperties().getProperty(HANDLER_PROP);
	if (faclist != null) {
		for (StringTokenizer s = new StringTokenizer(faclist, "|");
		    s.hasMoreTokens(); ) {
			try {
				handler = (URLStreamHandler) Class.forName(
				    s.nextToken() + "." + protocol + ".Handler"
				    ).newInstance();
				defaultHandlers.put(protocol, handler);
				this.handler = handler;
				return;
			} catch (ClassNotFoundException e) {
			} catch (IllegalAccessException e) {
			} catch (InstantiationException e) {
			} catch (ClassCastException e) {
			}
		}
	}

	// Try default kaffe implementation
	handler = defaultFactory.createURLStreamHandler(protocol);
	if (handler != null) {
		defaultHandlers.put(protocol, handler);
		this.handler = handler;
		return;
	}

	// None found
	throw new MalformedURLException("unknown protocol: " + protocol);
}

public boolean equals(Object obj) {
	if (!(obj instanceof URL)) {
		return (false);
	}
	URL that = (URL)obj;

	if (this.protocol.equals(that.protocol) &&
	    this.host.equals(that.host) &&
	    this.port == that.port &&
	    this.file.equals(that.file) &&
	    (this.ref == null ?
	      (that.ref == null) : this.ref.equals(that.ref)) ) {
		return (true);
	}
	return (false);
}

public final Object getContent() throws IOException {
	return openConnection().getContent();
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

public final InputStream openStream() throws IOException {
	// We *ALWAYS* open a new connection even if we already have
	// one open.
	return openConnection().getInputStream();
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
		System.getSecurityManager().checkSetFactory();
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
