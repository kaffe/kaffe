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
	private static final long serialVersionUID = -7627629688361524110L;
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

	/* URL -> [<protocol>:][//<hostname>[:port]][/<file>] */

//System.out.println("Parsing URL " + spec);

	int pend = spec.indexOf(':', 0);
	if (pend == -1) {
		// Although it doesn't say in the spec, it appears that
		// the protocol defaults to 'file' if it's missing.
		protocol = "file";
		host = "";
		port = -1;
		file = spec;
	}
	else {
		protocol = spec.substring(0, pend);

		int hstart = pend+3;
		if (spec.length() >= hstart && spec.substring(pend+1, hstart).equals("//")) {
			int hend = spec.indexOf(':', hstart);
			if (hend != -1) {
				host = spec.substring(hstart, hend);
				int postart = hend+1;
				int poend = spec.indexOf('/', postart);
				if (poend == -1) {
					poend = spec.length();
				}
// XXX Should we do this?  	try {
					port = Integer.parseInt(
					    spec.substring(postart, poend));
			//	} catch (NumberFormatException e) {
			//		throw new MalformedURLException(
			//		    "bad port \""
			//		    + spec.substring(postart, poend)
			//		    + "\"");
			//	}
				fstart = spec.indexOf( '/', hstart);
			}
			else {
				hend = spec.indexOf('/', hstart);
				if (hend != -1) {
					host = spec.substring(hstart, hend);
					port = -1;
					fstart = spec.indexOf( '/', hstart);
				}
				else {
					host = spec.substring(hstart);
					port = -1;
					fstart = -1;
				}
			}
		}
		else {
			host = "";
			port = -1;
			fstart = pend + 1;
		}

		if (fstart != -1) {
			file = spec.substring(fstart);
			int rIdx = file.lastIndexOf( '#');
			if ( rIdx > -1 ) {
				ref = file.substring( rIdx+1);
				file = file.substring( 0, rIdx);
			}
		}
		else {
			file = "";
		}
	}
	handler = getURLStreamHandler(protocol);
//System.out.println("URL = " + this);
}

public URL(String protocol, String host, String file) throws MalformedURLException {
	this(protocol, host, -1, file);
}

public URL(String protocol, String host, int port, String file) throws MalformedURLException {
	if (protocol == null || host == null || file == null) {
		throw new NullPointerException();
	}
	this.protocol = protocol;
	this.host = host;
	this.file = file;
	this.port = port;
	handler = getURLStreamHandler(protocol);
}

public URL(URL context, String spec) throws MalformedURLException {
//if (context != null) System.out.println("context = " + context);
//if (spec != null) System.out.println("spec = " + spec);
	this( merge( context, spec));
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

private static String merge(URL context, String spec) {
	if ( context == null )
		return spec;

	String cs = context.toString();
	
	if ( spec == null )
		return (cs);
		
	//spec is a ref ( keep file )
	if ( spec.indexOf( '#') == 0 )
		return (cs+spec);
		
	//merge files
	int ls = cs.lastIndexOf( '/');
	if ( ls > -1 ) {
		int ld = cs.lastIndexOf( '.');
		if ( ld > ls ) {
			return (cs.substring( 0, ls+1) + spec);
		}
	}
	
	return (cs + spec);
}

public URLConnection openConnection() throws IOException {
	if (conn == null) {
		conn = handler.openConnection(this);
		conn.connect();
	}
	return (conn);
}

public final InputStream openStream() throws IOException {
	if (conn == null) {
		openConnection();
	}
	return (conn.getInputStream());
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
	return (toString());
}

public String toString() {
	StringBuffer buf = new StringBuffer();
	buf.append(protocol);
	buf.append(":");
	if (!host.equals("")) {
		buf.append("//");
		buf.append(host);
		if (port != -1) {
			buf.append(":");
			buf.append(Integer.toString(port));
		}
	}
	buf.append(file);
	return (buf.toString());
}
}
