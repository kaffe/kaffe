/*
 * Java core library component.
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.sql;

import java.util.Date;

public class Timestamp
  extends Date {

private static final long serialVersionUID = 2745179027874758501L;
private int nanos;

public Timestamp(int year, int month, int date, int hour, int minute, int second, int nano) {
	super(year, month, date, hour, minute, second);
	nanos = nano;
}

public Timestamp(long time) {
	super(time);
}

public static Timestamp valueOf(String s) {
	return (new Timestamp(Date.parse(s)));
}

public String toString() {
	return (super.toString());
}

public int getNanos() {
	return (nanos);
}

public void setNanos(int n) {
	nanos = n;
}

public boolean equals(Timestamp ts) {
	return (super.equals(ts) && nanos == ts.nanos);
}

public boolean before(Timestamp ts) {
	long mytime = getTime();
	long yourtime = ts.getTime();
	if (mytime < yourtime) {
		return (true);
	}
	else if (mytime > yourtime) {
		return (false);
	}
	else if (nanos < ts.nanos) {
		return (true);
	}
	else {
		return (false);
	}
}

public boolean after(Timestamp ts) {
	long mytime = getTime();
	long yourtime = ts.getTime();
	if (mytime > yourtime) {
		return (true);
	}
	else if (mytime < yourtime) {
		return (false);
	}
	else if (nanos > ts.nanos) {
		return (true);
	}
	else {
		return (false);
	}
}

}
