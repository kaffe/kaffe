
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

package java.net;

import java.io.IOException;
import java.util.jar.*;

/*
 * A "jar" URL is like a normal URL, where the //hostname part is replaced
 * with an inner URL (that points at the JAR file), and the inner URL
 * is separated from the JAR file entry path by "!/"
 */

public abstract class JarURLConnection extends URLConnection {
	protected URLConnection jarFileURLConnection;

	protected JarURLConnection(URL url) throws MalformedURLException {
		super(url);
	}

	public URL getJarFileURL() {
		try {
			return new URL(getURL().getHost());
		} catch (MalformedURLException e) {
			// this supposedly can't happen
		}
		return null;
	}

	public String getEntryName() {
		String file = getURL().getFile();

		if (file.equals("")) {
			return null;
		}
		return file;
	}

	public abstract JarFile getJarFile() throws IOException;

	public Manifest getManifest() throws IOException {
		return getJarFile().getManifest();
	}

	public JarEntry getJarEntry() throws IOException {
		String file = getURL().getFile();

		if (file.equals("")) {
			return null;
		}
		JarEntry je = getJarFile().getJarEntry(file);
		if (je == null) {
			return null;
		}
		return je;	// XXX FIXME should be unmodifiable
	}

	public Attributes getAttributes() throws IOException {
		String file = getURL().getFile();

		if (file.equals("")) {
			return null;
		}
		return getJarEntry().getAttributes();
	}

	public Attributes getMainAttributes() throws IOException {
		Manifest m = getManifest();
		if (m == null) {
			return null;
		}
		return m.getMainAttributes();
	}

/**** XXX FIXME implement Certificates
	public Certificate[] getCertificates() throws IOException {
		String file = getURL().getFile();

		if (file.equals("")) {
			return null;
		}
		return getJarEntry().getCertificates();
	}
****/

}

