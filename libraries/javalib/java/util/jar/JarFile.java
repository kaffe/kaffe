
/*
 * Java core library component.
 *
 * Copyright (c) 1999
 *	Archie L. Cobbs.  All rights reserved.
 * Copyright (c) 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Author: Archie L. Cobbs <archie@whistle.com>
 */

package java.util.jar;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.util.Enumeration;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

public class JarFile extends ZipFile {
	public static final String MANIFEST_NAME = "META-INF/MANIFEST.MF";
	private Manifest manifest;

	public JarFile(String name) throws IOException {
		this(name, true);
	}

	public JarFile(String name, boolean verify) throws IOException {
		super(name);
		if (verify) {
			verifyIfSigned();
		}
	}

	public JarFile(File file) throws IOException {
		this(file, true);
	}

	public JarFile(File file, boolean verify) throws IOException {
		super(file);
		if (verify) {
			verifyIfSigned();
		}
	}

	private void verifyIfSigned() throws JarException {
		// XXX FIXME: verify the JAR file if it's signed
	}

	public Manifest getManifest() throws IOException {
		if (manifest == null) {
			ZipEntry me = getEntry(MANIFEST_NAME);
			if (me != null) {
				manifest = new Manifest(getInputStream(me));
			}
		}
		return manifest;
	}

	public JarEntry getJarEntry(String name) {
		ZipEntry ze = getEntry(name);
		if (ze == null) {
			return null;
		}
		Attributes attr = null;
		try {
			Manifest manifest = getManifest();
			if (manifest != null) {
				attr = getManifest().getAttributes(name);
			}
		} catch (IOException e) {
		}
		return new JarEntry(ze, attr);
	}

	public ZipEntry getEntry(String name) {
		// XXX not sure why this method is overridden
		return super.getEntry(name);
	}

	public Enumeration entries() {
		final Enumeration ents = super.entries();
		return new Enumeration() {
			public boolean hasMoreElements() {
				return ents.hasMoreElements();
			}
			public Object nextElement() {
				ZipEntry ze = (ZipEntry)ents.nextElement();
				return getJarEntry(ze.getName());
			}
		};
	}

	public InputStream getInputStream(ZipEntry ze) throws IOException {
		// XXX not sure why this method is overridden
		//     maybe so the hash can be verified?
		return super.getInputStream(ze);
	}
}


