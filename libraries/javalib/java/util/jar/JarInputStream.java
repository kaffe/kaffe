
/*
 * Java core library component.
 *
 * Copyright (c) 1999
 *	Archie L. Cobbs.  All rights reserved.
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Author: Archie L. Cobbs <archie@whistle.com>
 */

package java.util.jar;

import java.io.InputStream;
import java.io.IOException;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

public class JarInputStream extends ZipInputStream {
	private final Manifest manifest;
	private boolean gotNext;
	private ZipEntry next;

	public JarInputStream(InputStream in) throws IOException {
		this(in, true);
	}

	public JarInputStream(InputStream in, boolean vfy) throws IOException {
		super(in);

		// Try to read manifest, which must be first
		next = super.getNextEntry();
		if (next.getName().equals(JarFile.MANIFEST_NAME)) {
			// Note: it is important that Manifest(this) doesn't
			//   call the close() method of this.
			manifest = new Manifest(this);
			closeEntry();
			if (vfy) {
				verifyIfSigned();
			}
		} else {
			manifest = null;
			gotNext = true;
		}
	}

	private void verifyIfSigned() throws JarException {
		// XXX FIXME: verify the JAR file if it's signed
		//   this probably means storing it in a temp file
	}

	public Manifest getManifest() {
		return manifest;
	}

	public ZipEntry getNextEntry() throws IOException {
		return getNextJarEntry();
	}

	public JarEntry getNextJarEntry() throws IOException {
		if (gotNext) {
			gotNext = false;
		} else {
			next = super.getNextEntry();
		}
		if (next == null) {
			return null;
		}
		return new JarEntry(next, manifest == null ? null :
		    manifest.getAttributes(next.getName()));
	}

	public int read(byte[] b, int off, int len) throws IOException {
		return super.read(b, off, len);
	}

	protected ZipEntry createZipEntry(String name) {
		// XXX why is this overridden?
		return super.createZipEntry(name);
	}
}

