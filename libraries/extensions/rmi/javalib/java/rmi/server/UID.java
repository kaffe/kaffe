/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.rmi.server;

import java.io.Serializable;
import java.io.DataOutput;
import java.io.DataInput;
import java.io.IOException;
import java.util.Random;
import java.lang.Thread;
import java.lang.InterruptedException;

public final class UID
	implements Serializable {

public static final long serialVersionUID = 1086053664494604050L;

private static final Object lock = UID.class;
private static long baseTime = System.currentTimeMillis();
private static short nextCount = Short.MIN_VALUE;
// This is sun's algorithm - don't ask me why ...
private static final int uniqueNr = (new Object()).hashCode();

private int unique;
private long time;
private short count;

/**
 * This is sun's algorithm - don't ask me why ...
 */
public UID() {
	synchronized (lock) {
		if (count == Short.MAX_VALUE) {
			long newtime;
			for (;;) {
				newtime = System.currentTimeMillis();
				if (newtime - baseTime > 1000) {
					break;
				}
				try {
					Thread.sleep(1000);
				}
				catch (InterruptedException _) {
				}
			}
			baseTime = newtime;
			nextCount = Short.MIN_VALUE;
		}
		count = nextCount++;
		unique = uniqueNr;
		time = baseTime;
	}
}

public UID(short num) {
	unique = (int)num;
	time = 0;
	count = 0;
}

public int hashCode() {
	return (unique);
}

public boolean equals(Object obj) {
	if (obj instanceof UID) {
		UID uid = (UID)obj;
		if (this.unique == uid.unique &&
		    this.time == uid.time &&
		    this.count == uid.count) {
			return (true);
		}
	}
	return (false);
}

public String toString() {
	return ("[UID: " + unique + "," + time + "," + count + "]");
}

public void write(DataOutput out) throws IOException {
	out.writeInt(unique);
	out.writeLong(time);
	out.writeShort(count);
}

public static UID read(DataInput in) throws IOException {
	UID id = new UID();
	id.unique = in.readInt();
	id.time = in.readLong();
	id.count = in.readShort();
	return (id);
}

}
