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
	int currentStream = 0;

public SequenceInputStream(Enumeration e)
	{
	while (e.hasMoreElements()) {
		streams.addElement(e.nextElement());
	}
}

public SequenceInputStream(InputStream s1, InputStream s2)
	{
	streams.addElement(s1);
	streams.addElement(s2);
}

public int available() throws IOException
{
	if (currentStream >= streams.size()) {
		return (0);
	}
	else {
		return (((InputStream)streams.elementAt(currentStream)).available());
	}
}

public void close() throws IOException
{
	for (int stream = currentStream; stream < streams.size(); stream++) {
		((InputStream)streams.elementAt(stream)).close();
	}
}

public int read() throws IOException
{
	if (currentStream >= streams.size()) {
		return -1;
	}

	int data = ((InputStream)streams.elementAt(currentStream)).read();
	if (data == -1) {
		currentStream++;
		return (read());
	}
	return (data);
}

public int read(byte buf[], int pos, int len) throws IOException
{
	if (currentStream >= streams.size()) {
		return -1;
	}

	int data = ((InputStream)streams.elementAt(currentStream)).read(buf, pos, len);
	if (data == -1) {
		currentStream++;
		return (read(buf, pos, len));
	}
	return (data);
}
}
