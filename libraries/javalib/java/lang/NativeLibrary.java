
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

package java.lang;

import java.io.File;
import java.util.StringTokenizer;

/**
 * This class represents a linked in native library.
 */
class NativeLibrary {

	private int libIndex = -1;	// handle for native library

	public NativeLibrary(String path, ClassLoader loader) {
		if (loader == null)
			loader = ClassLoader.getSystemClassLoader();
		libIndex = linkLibrary(path);	// possible exception here
		loader.addNativeLibrary(this);
	}

	public static String[] getLibraryNames(String libname) {
		StringTokenizer t = new StringTokenizer(
		    System.getProperty("java.library.path"),
		    new String(new char[] { File.pathSeparatorChar }));
		String[] dirs = new String[t.countTokens()];
		for (int i = 0; i < dirs.length; i++) {
			dirs[i] = t.nextToken() + File.separator
			   + System.mapLibraryName(libname);
		}
		return dirs;
	}

	protected void finalize() throws Throwable {
		if (libIndex != -1)
			unlinkLibrary(libIndex);
		super.finalize();
	}

	private static native synchronized int linkLibrary(String path);
	private static native synchronized void unlinkLibrary(int index);

	public static native String getLibPrefix();
	public static native String getLibSuffix();
}

