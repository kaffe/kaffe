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

import java.net.URLConnection;
import java.net.ProtocolException;
import java.lang.String;
import java.io.IOException;

abstract public class HttpURLConnection extends URLConnection {

public static final int HTTP_OK = 200;
public static final int HTTP_CREATED = 201;
public static final int HTTP_ACCEPTED = 202;
public static final int HTTP_NOT_AUTHORITATIVE = 203;
public static final int HTTP_NO_CONTENT = 204;
public static final int HTTP_RESET = 205;
public static final int HTTP_PARTIAL = 206;

public static final int HTTP_MULT_CHOICE = 300;
public static final int HTTP_MOVED_PERM = 301;
public static final int HTTP_MOVED_TEMP = 302;
public static final int HTTP_SEE_OTHER = 303;
public static final int HTTP_NOT_MODIFIED = 304;
public static final int HTTP_USE_PROXY = 305;

public static final int HTTP_BAD_REQUEST = 400;
public static final int HTTP_UNAUTHORIZED = 401;
public static final int HTTP_PAYMENT_REQUIRED = 402;
public static final int HTTP_FORBIDDEN = 403;
public static final int HTTP_NOT_FOUND = 404;
public static final int HTTP_BAD_METHOD = 405;
public static final int HTTP_NOT_ACCEPTABLE = 406;
public static final int HTTP_PROXY_AUTH = 407;
public static final int HTTP_CLIENT_TIMEOUT = 408;
public static final int HTTP_CONFLICT = 409;
public static final int HTTP_GONE = 410;
public static final int HTTP_LENGTH_REQUIRED = 411;
public static final int HTTP_PRECON_FAILED = 412;
public static final int HTTP_ENTITY_TOO_LARGE = 413;
public static final int HTTP_REQ_TOO_LONG = 414;
public static final int HTTP_UNSUPPORTED_TYPE = 415;

public static final int HTTP_SERVER_ERROR = 500;
public static final int HTTP_INTERNAL_ERROR = 501;
public static final int HTTP_BAD_GATEWAY = 502;
public static final int HTTP_UNAVAILABLE = 503;
public static final int HTTP_GATEWAY_TIMEOUT = 504;
public static final int HTTP_VERSION = 505;

static private boolean followRedirects;

protected String method = "GET";
protected int responseCode;
protected String responseMessage;

protected HttpURLConnection(URL url) {
	super(url);
}

public static boolean getFollowRedirects () {
        return followRedirects;
}

public static void setFollowRedirects (boolean follow) {
       followRedirects = follow;
}

public String getRequestMethod () {
        return method;
}

public int getResponseCode () throws IOException {
        return responseCode;
}

public String getResponseMessage () throws IOException {
        return responseMessage;
}

public void setRequestMessage (String message) throws ProtocolException {
	if (!message.equals("DELETE") &&
	    !message.equals("GET") &&
	    !message.equals("HEAD") &&
	    !message.equals("OPTIONS") &&
	    !message.equals("POST") &&
	    !message.equals("PUT") &&
	    !message.equals("TRACE")) {
		throw new ProtocolException("bad request message: " + message);
	}
	method = message;
}

public abstract void disconnect ();
public abstract boolean usingProxy();

}
