package java.io;

import java.io.Serializable;
import java.lang.String;
import java.util.Random;
import java.util.Vector;
import java.lang.Comparable;
import java.util.StringTokenizer;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

public class File implements Serializable, Comparable {

	private static final long serialVersionUID = 301077366599181567L;
	public final static String separator = System.getProperty("file.separator");
	public final static char separatorChar = separator.charAt(0);
	public final static String pathSeparator = System.getProperty("path.separator");
	public final static char pathSeparatorChar = pathSeparator.charAt(0);

	private final static Random random = new Random();
	private String path;

static {
	System.loadLibrary("io");
}

public File(File dir, String name) {
        this((dir == null) ? null : dir.getPath(), name);
}

public File(String path, String name) {
	if (name == null) {
	    throw new NullPointerException();
	}
	if (path == null) {
	   this.path = name;
	}
	else if (path.length() == 0) {
	   this.path = separatorChar + name;
	}
	else {
		char last = path.charAt(path.length() - 1);

		if (last != separatorChar) {
			this.path = path + separatorChar + name;
		}
		else {
			this.path = path + name;
		}
	}
}

public File(String path) {
	if (path == null) {
		throw new NullPointerException();
	}
	this.path = path;
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

public static File createTempFile(String prefix, String suffix)
		throws IOException {
	return createTempFile(prefix, suffix, null);
}

public static File createTempFile(String prefix, String suffix, File dir)
		throws IOException {
	if (prefix.length() < 3) {
		throw new IllegalArgumentException(prefix);
	}
	if (suffix == null) {
		suffix = ".tmp";
	}
	if (dir == null) {
		dir = new File(System.getProperties().getProperty(
			"java.io.tmpdir"));
	}
	while (true) {
		File f = new File(dir, prefix
		    + Integer.toHexString(
			random.nextInt(0x100000)).toUpperCase() + suffix);
		if (!f.exists()) {
			return f;
		}
	}
}

public boolean delete() {
	System.getSecurityManager().checkDelete(getPath());

	return isDirectory0() ? rmdir0() : delete0();
}

native private boolean delete0();
native private boolean rmdir0();

// XXX FIXME implement me .. using Godmar's Application stuff ..
public void deleteOnExit() {
}

public int compareTo(Object that) {
	return compareTo((File)that);
}

// XXX some filesystems are case-insensitive
public int compareTo(File that) {
	return this.path.compareTo(that.path);
}

// XXX some filesystems are case-insensitive
public boolean equals(Object that) {
	return (that instanceof File) ? path.equals(((File)that).path) : false;
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
        StringTokenizer tok = new StringTokenizer(getAbsolutePath(), separator);
        int len = tok.countTokens();
        String[] array = new String[len+2];
        int j = 0;
        for (int i = 0; i < len; i++) {
                String str = tok.nextToken();
                if (str.equals("..")) {
                        j--;
                }
                else {
                        array[j] = str;
                        j++;
                }
        }

	// If the length is 0 then just return the base.
	if (j == 0) {
		return (separator);
	}

        // Build a string of the remaining elements.
        StringBuffer buf = new StringBuffer();
        for (int i = 0; i < j; i++) {
		buf.append(separatorChar);
                buf.append(array[i]);
        }
        return (buf.toString());
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

native public boolean isAbsolute();

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
	String all[] = list();

	if ((all == null) || (filter == null) ) {
		return (all);
	}

	Vector filtered = new Vector();
	
	for (int idx = 0; idx < all.length; idx++) {
		String fn = all[idx];
		if (filter.accept(this, fn)) {
			filtered.addElement(fn);
		}
	}

	String result[] = new String[filtered.size()];
	filtered.copyInto((Object [])result);

	return (result);
}

public File[] listFiles() {
	return listFiles(null);
}

public File[] listFiles(FilenameFilter filter) {
	String[] names = list(filter);
	if (names == null) {
		return null;
	}
	File[] files = new File[names.length];
	for (int idx = 0; idx < names.length; idx++) {
		files[idx] = new File(this, names[idx]);
	}
	return files;
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
