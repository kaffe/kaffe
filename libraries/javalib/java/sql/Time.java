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

public class Time
  extends Date {

public Time(int hour, int minute, int second) {
	super();
	setHours(hour);
	setMinutes(minute);
	setSeconds(second);
}

public Time(long time) {
	super(time);
}

public void setTime(long time) {
	super.setTime(time);
}

public static Time valueOf(String s) {
	return (new Time(Date.parse(s)));
}

public String toString() {
	return (super.toString());
}

public int getYear() {
	return (super.getYear());
}

public int getMonth() {
	return (super.getMonth());
}

public int getDay() {
	return (super.getDay());
}

public int getDate() {
	return (super.getDate());
}

public void setYear(int i) {
	super.setYear(i);
}

public void setMonth(int i) {
	super.setMonth(i);
}

public void setDate(int i) {
	super.setDate(i);
}

}
