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

import java.text.SimpleDateFormat;
import java.text.ParsePosition;


public class Date
  extends java.util.Date {

static private SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd");
    
public Date(int year, int month, int day) {
	super(year, month, day);
}

public Date(long date) {
	super(date);
}

public void setTime(long date) {
	super.setTime(date);
}

public static Date valueOf(String s)
	throws IllegalArgumentException
{
	java.util.Date date = sdf.parse(s, new ParsePosition(0));
	if (date == null) {
		throw new IllegalArgumentException(s);
	}
	return new Date(date.getTime());
}

public String toString() {
	return sdf.format(this);
}

public int getHours() {
	return (super.getHours());
}

public int getMinutes() {
	return (super.getMinutes());
}

public int getSeconds() {
	return (super.getSeconds());
}

public void setHours(int i) {
	super.setHours(i);
}

public void setMinutes(int i) {
	super.setMinutes(i);
}

public void setSeconds(int i) {
	super.setSeconds(i);
}

}
