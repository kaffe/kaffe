/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.text;

import java.lang.String;
import java.io.Serializable;
import java.util.ResourceBundle;
import java.util.Locale;

public class DateFormatSymbols implements Serializable, Cloneable {

private static final long serialVersionUID = -5987973545549424702L;

String[] amPmStrings;
String[] eras;
String localPatternChars;
String[] months;
String[] shortMonths;
String[] shortWeekdays;
String[] weekdays;
String[][] zoneStrings;

public DateFormatSymbols() {
	this(Locale.getDefault());
}

public DateFormatSymbols(Locale loc) {
	ResourceBundle bundle = Format.getResources("dateformat", loc);

	amPmStrings = (String[])bundle.getObject("amPmStrings");
	eras = (String[])bundle.getObject("eras");
	localPatternChars = bundle.getString("localPatternChars");
	months = (String[])bundle.getObject("months");
	shortMonths = (String[])bundle.getObject("shortMonths");
	shortWeekdays = (String[])bundle.getObject("shortWeekdays");
	weekdays = (String[])bundle.getObject("weekdays");
	zoneStrings = (String[][])bundle.getObject("zoneStrings");
}

public Object clone() {
	try {
		return (super.clone());
	}
	catch (CloneNotSupportedException _) {
		return (null);
	}
}

public boolean equals(Object obj) {
	if (obj instanceof DateFormatSymbols) {
		DateFormatSymbols other = (DateFormatSymbols)obj;
		if (!localPatternChars.equals(other.localPatternChars)) {
			return (false);
		}
		for (int i = eras.length; i-- > 0; ) {
			if (!eras[i].equals(other.eras[i])) {
				return (false);
			}
		}
		for (int i = amPmStrings.length; i-- > 0; ) {
			if (!amPmStrings[i].equals(other.amPmStrings[i])) {
				return (false);
			}
		}
		for (int i = months.length; i-- > 0; ) {
			if (!months[i].equals(other.months[i])) {
				return (false);
			}
		}
		for (int i = shortMonths.length; i-- > 0; ) {
			if (!shortMonths[i].equals(other.shortMonths[i])) {
				return (false);
			}
		}
		for (int i = weekdays.length; i-- > 0; ) {
			if (!weekdays[i].equals(other.weekdays[i])) {
				return (false);
			}
		}
		for (int i = shortWeekdays.length; i-- > 0; ) {
			if (!shortWeekdays[i].equals(other.shortWeekdays[i])) {
				return (false);
			}
		}
		for (int i = zoneStrings.length; i-- > 0; ) {
			for (int j = zoneStrings[i].length; j-- > 0; ) {
				if (!zoneStrings[i][j].equals(other.zoneStrings[i][j])) {
					return (false);
				}
			}
		}
		return (true);
	}
	else {
		return (false);
	}
}

public String[] getAmPmStrings() {
	return (amPmStrings);
}

public String[] getEras() {
	return (eras);
}

public String getLocalPatternChars() {
	return (localPatternChars);
}

public String[] getMonths() {
	return (months);
}

public String[] getShortMonths() {
	return (shortMonths);
}

public String[] getShortWeekdays() {
	return (shortWeekdays);
}

public String[] getWeekdays() {
	return (weekdays);
}

public String[][] getZoneStrings() {
	return (zoneStrings);
}

public int hashCode() {
	return (super.hashCode());
}

public void setAmPmStrings(String[] data) {
	amPmStrings = data;
}

public void setEras(String[] data) {
	eras = data;
}

public void setLocalPatternChars(String data) {
	localPatternChars = data;
}

public void setMonths(String[] data) {
	months = data;
}

public void setShortMonths(String[] data) {
	shortMonths = data;
}

public void setShortWeekdays(String[] data) {
	shortWeekdays = data;
}

public void setWeekdays(String[] data) {
	weekdays = data;
}

public void setZoneStrings(String[][] data) {
	zoneStrings = data;
}

}
