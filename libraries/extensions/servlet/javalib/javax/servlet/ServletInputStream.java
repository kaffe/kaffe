/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package javax.servlet;

import java.io.InputStream;
import java.io.IOException;

public abstract class ServletInputStream
  extends InputStream {

protected ServletInputStream() {
}

public int readLine(byte b[], int off, int len) throws IOException {
	int count = 0;
	for (; len > 0; count++, len--) {
                int data = read();
                if (data == -1) {
			if (count == 0) {
				return (-1);
			}
                        break;
                }
		b[off] = (byte)data;
		off++;
                if ((char)data == '\n') {
                        break;
                }
        }
	return (count);
}

}
