/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.net;

import java.net.FileNameMap;
import java.lang.String;

public class DefaultFileNameMap implements FileNameMap {

public String getContentTypeFor(String filename) {
	if (filename.endsWith(".gif")) {
		return ("image/gif");
	}
	if (filename.endsWith(".jpeg") || filename.endsWith(".jpg") || filename.endsWith(".jpe")) {
		return ("image/jpeg");
	}
	if (filename.endsWith(".png")) {
		return ("image/png");
	}
	if (filename.endsWith(".html") || filename.endsWith(".htm")) {
		return ("text/html");
	}
	if (filename.endsWith(".txt")) {
		return ("text/plain");
	}
	return (null);
}

}
