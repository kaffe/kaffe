/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package javax.servlet.http;

import java.util.Hashtable;
import javax.servlet.ServletInputStream;

public class HttpUtils {

public HttpUtils() {
}

public static Hashtable parseQueryString(String s) {
	throw new kaffe.util.NotImplemented(HttpUtils.class.getName() + ".parseQueryString()");
}

public static Hashtable parsePostData(int len, ServletInputStream in) {
	throw new kaffe.util.NotImplemented(HttpUtils.class.getName() + ".parsePostData()");
}

public static StringBuffer getRequestURL(HttpServletRequest req) {
	throw new kaffe.util.NotImplemented(HttpUtils.class.getName() + ".getRequestURL");
}

}
