/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.io;

import java.util.Vector;
import java.lang.String;

public class File {

	final public static String separator = System.getProperty("file.separator");
	final public static char separatorChar = separator.charAt(0);
	final public static String pathSeparator = System.getProperty("path.separator");
	final public static char pathSeparatorChar = pathSeparator.charAt(0);
	private String path;

static {
	System.loadLibrary("io");
}

public File(File dir, String name) {
	if (dir == null) {
		this.path = name;
	}
	else {
		this.path = dir.getPath() + separatorChar + name;
	}
}

public File(String path) {
	this.path = path;
}

public File(String path, String name) {
	if (path == null) {
		path = ".";
	}
	this.path = path + separatorChar + name;
}

public boolean canRead() {
	checkReadAccess();

	return (exists() && canRead0());
}

native private boolean canRead0();

public boolean canWrite() {
	checkWriteAccess();

	return canWrite0();
}

native private boolean canWrite0();

private void checkReadAccess() {
	System.getSecurityManager().checkRead(getPath());
}

private void checkWriteAccess() {
	System.getSecurityManager().checkWrite(getPath());
}

public boolean delete() {
	System.getSecurityManager().checkDelete(getPath());

	return delete0();
}

native private boolean delete0();

public boolean equals(Object obj) {
	if (obj != null && obj instanceof File) {
		return (((File)obj).getPath().equals( getPath() ) );
	}
	else {
		return (false);
	}
}

public boolean exists() {
	checkReadAccess();

	return exists0();			
}

native private boolean exists0();

public String getAbsolutePath() {
	if (isAbsolute()) {
		return getPath();
	}
	else {
		return System.getProperty("user.dir") + separatorChar + getPath();
	}
}

public String getCanonicalPath() throws IOException {
	String str = getAbsolutePath();
	// The only processing at the moment is to remove any appended file
	// seperator.
	int len = str.length();
	if (len > 1 && str.charAt(len-1) == separatorChar) {
		str = str.substring(0, len-1);
	}
	return (str);
}

public String getName() {
	return path.substring(path.lastIndexOf(separatorChar)+1);
}

public String getParent() {
	int slashIndex = path.lastIndexOf(separatorChar);
	if (slashIndex > 0) {
		return (path.substring(0, slashIndex));
	}
	if (slashIndex == 0 && path.length() > 1) {
		return (separator);
	}
	return (null);
}

public String getPath() {
	return path;
}

public int hashCode() {
	return path.hashCode();
}

public native boolean isAbsolute();

public boolean isDirectory() {
	checkReadAccess();

	return isDirectory0();
}

native private boolean isDirectory0();

public boolean isFile() {
	checkReadAccess();

	return isFile0();
}

native private boolean isFile0();

public long lastModified() {
	checkReadAccess();

	return lastModified0();
}

native private long lastModified0();

public long length() {
	checkReadAccess();

	return length0();
}

native private long length0();

public String[] list() {
	checkReadAccess();

	return list0();
}

public String[] list(FilenameFilter filter) {
	Vector filtered = new Vector();
	String all[] = list();

	if (all == null) {
		return (null);
	}

	for (int idx = 0; idx < all.length; idx++) {
		String fn = all[idx];
		if (filter != null && filter.accept(this, fn)) {
			filtered.addElement(fn);
		}
	}

	String result[] = new String[filtered.size()];
	filtered.copyInto((Object [])result);

	return (result);
}

native private String[] list0();

public boolean mkdir() {
	checkWriteAccess();

	if (isDirectory()) {
		return (true);
	}
	else {
		return (mkdir0());
	}
}

native private boolean mkdir0();

public boolean mkdirs() {
	if (!toString().equals(separator)) {
		String parentString = getParent();
		if (parentString != null) {
			File parent = new File(parentString);
			if (parent.mkdirs() == false) {
				return (false);
			}
		}
	}
	return (mkdir());
}

public boolean renameTo(File dest) {
	System.getSecurityManager().checkWrite(getPath());
	System.getSecurityManager().checkWrite(dest.getPath());

	return renameTo0(dest);
}

native private boolean renameTo0(File that);

public String toString() {
	return path;
}

}
