
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

import java.io.IOException;
import java.util.zip.ZipEntry;
import java.security.cert.Certificate;

public class JarEntry extends ZipEntry {
	private final Attributes attr;

	public JarEntry(String name) {
		super(name);
		attr = null;
	}

	public JarEntry(ZipEntry ze) {
		super(ze);
		attr = null;
	}

	public JarEntry(JarEntry je) {
		super(je);
		attr = je.attr;		// XXX	(Attributes)je.attr.clone();
	}

	JarEntry(ZipEntry ze, Attributes attr) {
		super(ze);
		this.attr = attr;	// XXX  (Attributes)attr.clone();
	}

	public Attributes getAttributes() throws IOException {
		return attr;
	}

	public Certificate[] getCertificates() {
		return null;
	}
}
