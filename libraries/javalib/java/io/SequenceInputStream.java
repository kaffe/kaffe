package java.io;

import java.util.Enumeration;
import java.util.Vector;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class SequenceInputStream
  extends InputStream
{
	Vector streams = new Vector();
	int currentStreamIdx;
	InputStream currentStream;

public SequenceInputStream(Enumeration e) {
	while (e.hasMoreElements()) {
		streams.addElement(e.nextElement());
	}
	currentStream = (InputStream)streams.elementAt(0);
}

public SequenceInputStream(InputStream s1, InputStream s2) {
	streams.addElement(s1);
	streams.addElement(s2);
	currentStream = s1;
}

public int available() throws IOException {
	if (currentStreamIdx >= streams.size()) {
		return (0);
	}
	else {
		return (currentStream.available());
	}
}

public void close() throws IOException {
	for (int stream = currentStreamIdx; stream < streams.size(); stream++) {
		((InputStream)streams.elementAt(stream)).close();
	}
	currentStreamIdx = 0;
	currentStream = null;
	streams = new Vector();
}

public int read() throws IOException {
	if (currentStreamIdx >= streams.size()) {
		return -1;
	}
	for (;;) {
		int data = currentStream.read();
		if (data != -1) {
			return (data);
		}
		currentStream.close();
		currentStreamIdx++;
		if (currentStreamIdx >= streams.size()) {
			return -1;
		}
		currentStream = (InputStream)streams.elementAt(currentStreamIdx);
	}
}

public int read(byte buf[], int pos, int len) throws IOException {
	if (currentStreamIdx >= streams.size()) {
		return -1;
	}
	for (;;) {
		int nr = currentStream.read(buf, pos, len);
		if (nr != -1) {
			return (nr);
		}
		currentStream.close();
		currentStreamIdx++;
		if (currentStreamIdx >= streams.size()) {
			return -1;
		}
		currentStream = (InputStream)streams.elementAt(currentStreamIdx);
	}
}
}
