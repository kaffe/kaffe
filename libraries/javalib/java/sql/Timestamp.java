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

import java.text.DecimalFormat;
import java.text.FieldPosition;
import java.text.ParsePosition;
import java.text.SimpleDateFormat;
import java.util.Date;


public class Timestamp
  extends Date {

private static final long serialVersionUID = 2745179027874758501L;
private int nanos;
private static SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
private static DecimalFormat nanosFormat = new DecimalFormat("000000000");


public Timestamp(int year, int month, int date, int hour, int minute, int second, int nano) {
	super(year, month, date, hour, minute, second);
	nanos = nano;
}

public Timestamp(long time) {
	super(time);
}

public static Timestamp valueOf(String s)
	throws IllegalArgumentException
{
	ParsePosition pos = new ParsePosition (0);
	
	Date date = dateFormat.parse (s, pos);
	if (date == null) {
		throw new IllegalArgumentException(s);
	}

	Timestamp ts = new Timestamp(date.getTime());
	
	int index = pos.getIndex() + 1;
	if ((index < s.length()) && 
	    (s.charAt(index) == '.')) {
		pos.setIndex(index);
		Number nanos = nanosFormat.parse(s, pos);
		if (nanos == null) {
			throw new IllegalArgumentException(s);
		}
		ts.setNanos(nanos.intValue());
	}

	return ts;
}

public String toString() {
	StringBuffer sb = new StringBuffer();
	dateFormat.format(this, sb, new FieldPosition(0));
	if (getNanos() != 0) {
		sb.append('.');
		nanosFormat.format ((long)getNanos(), sb, new FieldPosition(0));
	}
	
	return sb.toString();
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
