package kaffe.net.www.protocol.file;

import java.awt.Toolkit;
import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStream;
import java.net.URL;
import java.net.URLConnection;
import kaffe.net.www.protocol.BasicURLConnection;

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
	InputStream file;

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
	
	file = new BufferedInputStream(new FileInputStream( fn));
}

public InputStream getInputStream() throws IOException {
	return (file);
}
}
