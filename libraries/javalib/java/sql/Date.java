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

public class Date
  extends java.util.Date {

public Date(int year, int month, int day) {
	super(year, month, day);
}

public Date(long date) {
	super(date);
}

public void setTime(long date) {
	super.setTime(date);
}

public static Date valueOf(String s) {
	return (new Date(java.util.Date.parse(s)));
}

public String toString() {
	return (super.toString());
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
