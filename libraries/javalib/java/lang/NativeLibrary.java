
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
import java.io.FileNotFoundException;
import java.util.StringTokenizer;

/**
 * This class represents a linked in native library.
 */
class NativeLibrary {

	private int libIndex = -1;	// handle for native library

	NativeLibrary(String path, ClassLoader loader)
		throws FileNotFoundException {
		if (loader == null)
		{
			// possible exception here
			/*
			 * No need to save the index since the root class
			 * loader isn't going away so we won't ever unlink
			 * the library.
			 */
			linkLibrary(path);
		}
		else
		{
			// possible exception here
			libIndex = linkLibrary(path);
			loader.addNativeLibrary(this);
		}
	}

	static String[] getLibraryNames(String libname) {
		final String libPath = 
			System.getProperty("kaffe.library.path")
			+ File.pathSeparatorChar
			+ System.getProperty("java.library.path");

		StringTokenizer t = new StringTokenizer(libPath,
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

	private static native int linkLibrary(String path)
		throws FileNotFoundException;
	
	private static native void unlinkLibrary(int index);

	/**
	 * @return The configured native library prefix (e.g. lib)
	 */
	static native String getLibPrefix();

	/**
	 * @return The configured native library suffix (e.g. .so)
	 */
	static native String getLibSuffix();
}

