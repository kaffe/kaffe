package kaffe.net.www.protocol.ftp;

import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.net.URLConnection;
import kaffe.net.www.protocol.BasicURLConnection;

/*
 * FtpURLConnection -
 *  Simple FTP URL handler.
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

public class FtpURLConnection extends BasicURLConnection
{
	public FtpURLConnection(URL url) {
		super(url);
	}

	public void connect() throws IOException {
		setContentTypeFromName();
	}

	public InputStream getInputStream() throws IOException {
		throw new kaffe.util.NotImplemented();
	}
}

