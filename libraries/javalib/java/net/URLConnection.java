/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.net;

import java.lang.String;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Hashtable;
import kaffe.net.DefaultFileNameMap;
import kaffe.net.StreamMap;
import kaffe.net.DefaultStreamMap;

abstract public class URLConnection
{
	public static FileNameMap fileNameMap = new DefaultFileNameMap();
	public static StreamMap streamMap = new DefaultStreamMap();

	protected boolean allowUserInteraction = defaultAllowUserInteraction;
	protected boolean connected = false;
	protected boolean doInput = true;
	protected boolean doOutput = false;
	protected long ifModifiedSince = 0;
	protected URL url;
	protected boolean useCaches = defaultUseCaches;

	private static ContentHandlerFactory factory = null;
	private static boolean defaultAllowUserInteraction = false;
	private static boolean defaultUseCaches = false;

protected URLConnection (URL url) {
	this.url = url;
}

abstract public void connect() throws IOException;

public boolean getAllowUserInteraction() {
	return allowUserInteraction;
}

public Object getContent() throws IOException {
	return null;
}

public String getContentEncoding() {
	return getHeaderField("content-encoding");
}

public int getContentLength() {
	return getHeaderFieldInt("content-length", -1);
}

public String getContentType() {
	return getHeaderField("content-type");
}

public long getDate() {
	return getHeaderFieldDate("date", -1);
}

public static boolean getDefaultAllowUserInteraction() {
	return defaultAllowUserInteraction;
}

public static String getDefaultRequestProperty(String key) {
	return (null);
}

public boolean getDefaultUseCaches() {
	return defaultUseCaches;
}

public boolean getDoInput() {
	return doInput;
}

public boolean getDoOutput() {
	return doOutput;
}

public long getExpiration() {
	return getHeaderFieldDate("expiration", -1);
}

public String getHeaderField(String name) {
	return (null);
}

public String getHeaderField(int n) {
	return (null);
}

public String getHeaderFieldKey(int n) {
	return (null);
}


public long getHeaderFieldDate(String name, long def) {
	String date = getHeaderField(name);
	// Parse date goes here !!
	return (def);
}

public int getHeaderFieldInt(String name, int def) {
	String val = getHeaderField(name);
	try {
		return (Integer.parseInt(val));
	}
	catch (NumberFormatException _) {
	}
	return (def);
}

public long getIfModifiedSince() {
	if (ifModifiedSince == 0) {
		ifModifiedSince = getHeaderFieldInt("If-Modified-Since", 0);
	}
	return ifModifiedSince;
}

public InputStream getInputStream() throws IOException {
	throw new UnknownServiceException();
}

public long getLastModified() {
	return getHeaderFieldDate("lastModified", -1);
}

public OutputStream getOutputStream() throws IOException {
	throw new UnknownServiceException();
}

public String getRequestProperty(String key) {
	return (null);
}

public URL getURL() {
	return url;
}

public boolean getUseCaches() {
	return useCaches;
}

protected static String guessContentTypeFromName(String fname)
{
	if (fileNameMap != null) {
		return (fileNameMap.getContentTypeFor(fname));
	}
	return (null);
}

public static String guessContentTypeFromStream(InputStream is) throws IOException {
	if (streamMap != null) {
		return (streamMap.getContentTypeFor(is));
	}
	return (null);
}

public void setAllowUserInteraction(boolean allowuserinteraction) {
	allowUserInteraction = allowuserinteraction;
}

/* Factory and stream type stuff */
public static synchronized void setContentHandlerFactory(ContentHandlerFactory fac) {
	if (factory == null) {
		factory = fac;
	}
	else {
		throw new Error("factory already defined");
	}
}

public static void setDefaultAllowUserInteraction(boolean defaultallowuserinteraction) {
	defaultAllowUserInteraction = defaultallowuserinteraction;
}

public static void setDefaultRequestProperty(String key, String value) {
}

public void setDefaultUseCaches(boolean defaultusecaches) {
	defaultUseCaches = defaultusecaches;
}

public void setDoInput(boolean doinput) {
	doInput = doinput;
}

public void setDoOutput(boolean dooutput) {
	doOutput = dooutput;
}

public void setIfModifiedSince(long ifmodifiedsince) {
	ifModifiedSince = ifmodifiedsince;
}

public void setRequestProperty(String key, String value) {
}

public void setUseCaches(boolean usecaches) {
	useCaches = usecaches;
}

public String toString() {
	return (getClass().toString() + " " + url);
}

}
