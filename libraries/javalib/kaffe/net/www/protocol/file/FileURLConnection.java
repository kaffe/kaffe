package kaffe.net.www.protocol.file;

import java.io.ByteArrayInputStream;
import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import kaffe.net.www.protocol.BasicURLConnection;
import java.io.FilePermission;
import java.security.Permission;

/*
 * FileURLConnection -
 *  Simple file URL handler.
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class FileURLConnection
  extends BasicURLConnection
{
	private InputStream file;
	private FilePermission permission;

public FileURLConnection(URL url) {
	super(url);
}

public void connect() throws IOException {
	String fn = url.getFile();

	setContentTypeFromName();

	if ( (File.separatorChar == '\\') && (fn.indexOf('/') >= 0) ){
		// we are on a DOS-like filesystem, replace URL slashes
		fn = fn.replace( '/', File.separatorChar);
	}

	File fl = new File(fn);

	permission = new FilePermission (fl.getPath(), "read");

	if( fl.isDirectory() )
	{
		int lpc, flatIndex = 0, len = 0;
		String files[] = fl.list();
		byte fileBits[][];
		byte flatBits[];

		fileBits = new byte[files.length][];
		for( lpc = 0; lpc < files.length; lpc++ )
		{
			fileBits[lpc] = files[lpc].getBytes();
			len += fileBits[lpc].length
				+ 1 /* for line-feed */;
		}
		flatBits = new byte[len];
		for( lpc = 0; lpc < files.length; lpc++ )
		{
			System.arraycopy(fileBits[lpc], 0,
					 flatBits, flatIndex,
					 fileBits[lpc].length);
			flatIndex += fileBits[lpc].length;
			flatBits[flatIndex] = (byte)'\n';
			flatIndex += 1;
		}
		setHeaderField("content-length", Integer.toString(len));
		this.file = new ByteArrayInputStream(flatBits);
	}
	else
	{
		setHeaderField("content-length", Long.toString(fl.length()));
		file = new BufferedInputStream(new FileInputStream( fl));
	}
}

public Permission getPermission () throws IOException {
	return permission;
}

public InputStream getInputStream() throws IOException {
	return (file);
}
}
