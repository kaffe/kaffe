/**
 * TmpFile - temporary file
 *
 * This class is used to provide a type for temporary files, which are created
 * in a specific directory (that can be set via a system property
 * "file.tmpDir" and otherwise defaults to "/tmp"). This class is mainly
 * used to enable controlled file system access in a restricted SecurityManager
 * context).
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.io;

import java.io.File;

public class TmpFile
  extends File
{
	static String tmpDir;

static {
	tmpDir = System.getProperty( "file.tmpdir", File.separator + "tmp") +
	            File.separatorChar;
}

public TmpFile ( String name ) {
	super( getTmpPath( name, null));
}

public TmpFile ( String name, String extension ) {
	super( getTmpPath( name, extension));
}

public static String getTmpPath ( String name, String extension) {
	String path;

	if ( name == null )
		path = tmpDir + Long.toHexString( System.currentTimeMillis());
	else {
		int n;
		if ( (n = name.lastIndexOf( File.separatorChar)) >= 0 ) { // skip path
			name = name.substring( n+1);
		}
		path = tmpDir + name;
	}

	if ( extension != null )
		path += extension;
		
	return path;
}
}
