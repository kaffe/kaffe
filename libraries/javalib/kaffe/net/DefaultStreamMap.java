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

import java.io.InputStream;
import java.io.IOException;

public class DefaultStreamMap implements StreamMap {

public String getContentTypeFor(InputStream in) {
	int b0;
	int b1;
	int b2;
	int b3;
	try {
		if (!in.markSupported()) {
			return (null);
		}
		in.mark(4);
		b0 = in.read();
		b1 = in.read();
		b2 = in.read();
		b3 = in.read();
		in.reset();
	}
	catch (IOException _) {
		return (null);
	}

	/* Is it a class file ? */
	if (b0 == 0xCA && b1 == 0xFE && b2 == 0xBA && b3 == 0xBE) {
		return ("application/java-vm");
	}
	/* Is is a GIF */
	else if (b0 == 'G' && b1 == 'I' && b2 == 'F') {
		return ("image/gif");
	}
	/* Is is a JPEG */
	else if (b0 == 0xFF && b1 == 0xD8 && b2 == 0xFF && b3 == 0xE0) {
		return ("image/jpeg");
	}
	/* Is is a PNG */
	else if (b0 == 0x89 && b1 == 'P' && b2 == 'N' && b3 == 'G') {
		return ("image/png");
	}

	return (null);
}

}
