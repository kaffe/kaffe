
/*
 * Java core library component.
 *
 * Copyright (c) 1999
 *      Archie L. Cobbs.  All rights reserved.
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Author: Archie L. Cobbs <archie@whistle.com>
 */

package kaffe.net.www.protocol.jar;

import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLConnection;
import java.net.URLStreamHandler;

public class Handler extends URLStreamHandler {

	protected URLConnection openConnection(URL u) throws IOException {
		return new JarURLConnection(u);
	}

	protected void parseURL(URL u, String spec0, int start, int limit)
			throws MalformedURLException {
		String spec = spec0.substring(start, limit);
		String file = u.getFile();
		String innerUrl, specFile;
		int posn;

		// Separate inner URL and entry pathname
		if ((posn = file.lastIndexOf("!/")) == -1) {
			innerUrl = "";
		} else {
			innerUrl = file.substring(0, posn);
			file = file.substring(posn + 1);
		}

		// Is the inner URL specified in the new URL?
		if ((posn = spec.lastIndexOf("!/")) != -1) {
			innerUrl = spec.substring(0, posn);
			try {
				new URL(innerUrl);
			} catch (MalformedURLException e) {
				throw new MalformedURLException(
				    "invalid inner URL: " + e.getMessage());
			}
			specFile = spec.substring(posn + 1);
		} else if (innerUrl.equals("")) {
			throw new MalformedURLException("no !/ in spec");
		} else {	// inherit inner URL from context
			specFile = spec;
		}

		// Merge together "file" (from context) with "specFile"
                if (specFile.charAt(0) == '/') {
                        file = specFile;
                } else {
                        int lastSlash = file.lastIndexOf('/');
                        if (lastSlash == -1) {
                                file = specFile;
                        } else {
                                file = file.substring(0, lastSlash + 1)
                                    + specFile;
                        }
                }

		setURL(u, u.getProtocol(), "", -1,
		    innerUrl + "!" + file, u.getRef());
	}

	// This does some rudimentary sanity checking on the URL contents
	//   before forwarding the method call up to the superclass.
	protected void setURL(URL u, String protocol, String host, int port,
			String file, String ref) throws MalformedURLException {
		int sep = file.indexOf("!/");
		if (!host.equals("") || port != -1 || sep == -1) {
			throw new MalformedURLException("invalid JAR URL");
		}
		new URL(file.substring(0, sep));
		super.setURL(u, protocol, host, port, file, ref);
	}

	protected String toExternalForm(URL u) {
		StringBuffer buf = new StringBuffer();
		buf.append(u.getProtocol());
		buf.append(':');
		buf.append(u.getFile());
		if (u.getRef() != null && !u.getRef().equals("")) {
			buf.append('#');
			buf.append(u.getRef());
		}
		return buf.toString();
	}
}

