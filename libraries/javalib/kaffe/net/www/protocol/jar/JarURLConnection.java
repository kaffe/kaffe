
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

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLConnection;
import java.util.jar.JarFile;
import java.util.jar.JarEntry;
import java.util.zip.ZipEntry;

public class JarURLConnection extends java.net.JarURLConnection {
	private URL jarFileURL;
	private JarFile jarFile;
	private String jarEntryName;
	private ZipEntry jarEntry;

	public JarURLConnection(URL url) throws MalformedURLException {
		super(url);
		String file = url.getFile();
		int sep = file.lastIndexOf("!/");
		if (sep == -1) {
			throw new MalformedURLException("missing !/");
		}
		jarFileURL = new URL(file.substring(0, sep));
		jarEntryName = file.substring(sep + 2);
	}

	public JarFile getJarFile() throws IOException {
		connect();
		return jarFile;		// XXX should be read-only
	}

	public void connect() throws IOException {

		// Already connected?
		if (connected) {
			return;
		}

		// If inner URL is a file, just use it directly
		if (jarFileURL.getProtocol().equals("file")) {
			jarFile = new JarFile(jarFileURL.getFile());
		} else {

			// Get the JAR file input stream
			jarFileURLConnection = jarFileURL.openConnection();
			jarFileURLConnection.connect();
			InputStream in = jarFileURLConnection.getInputStream();

			// Save it to a temp file
			File tempFile = File.createTempFile("jar", null);
			OutputStream out = new FileOutputStream(tempFile);
			byte[] buf = new byte[1024];
			for (int r; (r = in.read(buf)) != -1; )
				out.write(buf, 0, r);
			out.close();
// XXX should use	tempFile.deleteOnExit();   but not implemented yet

			jarFile = new JarFile(tempFile);
		}

		// Get the entry in the file
		jarEntry = jarFile.getEntry(jarEntryName);
		if (jarEntry == null) {
			throw new IOException("JAR entry \""
			    + jarEntryName + "\" not found");
		}
		connected = true;
	}

	public InputStream getInputStream() throws IOException {
		if (!connected) {
			throw new IOException("not connected");
		}
		return jarFile.getInputStream(jarEntry);
	}
}

