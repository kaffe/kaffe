package kaffe.text.dateformat;

import java.util.ListResourceBundle;

public class locale_en_US extends ListResourceBundle {

private Object[][] contents = {

	/*
	 * Time/Date formats
	 */

	// Full, Long, Medium, Short
	{ "date", new String[]{ "EEEE, MMMM d, yyyy", "MMMM d, yyyy", "d-MMM-yy", "M/d/yy" } },
	{ "time", new String[]{ "h:mm:ss;SS 'o''''clock' a z", "h:mm:ss a z", "h:mm:ss a", "h:mm a" } },

	/*
	 * DateFormatSymbols information
	 */

	{ "amPmStrings",	new String[]{ "AM", "PM" } },
	{ "eras",		new String[]{ "BC", "AD" } },
	{ "localPatternChars",	"GyMdkHmsSEDFwWahKz" },
	{ "months",		new String[]{ "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December", "" } },
	{ "shortMonths",	new String[]{ "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", "" } },
	{ "weekdays",		new String[]{ "", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" } },
	{ "shortWeekdays",	new String[]{ "", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" } },
	{ "zoneStrings", new String[][]{
		new String[]{ "PST", "Pacific Standard Time", "PST", "Pacific Daylight Time", "PDT", "San Francisco" },
		new String[]{ "MST", "Mountain Standard Time", "MST", "Mountain Daylight Time", "MDT", "Denver" },
		new String[]{ "PNT", "Mountain Standard Time", "MST", "Mountain Daylight Time", "MDT", "Phoenix" },
		new String[]{ "CST", "Central Standard Time", "CST", "Central Daylight Time", "CDT", "Chicago" },
		new String[]{ "EST", "Eastern Standard Time", "EST", "Eastern Daylight Time", "EDT", "New York" },
		new String[]{ "IST", "Eastern Standard Time", "EST", "Eastern Daylight Time", "EDT", "Indianapolis" },
		new String[]{ "PRT", "Atlantic Standard Time", "AST", "Atlantic Daylight Time", "ADT", "Halifax" },
		new String[]{ "HST", "Hawaii Standard Time", "HST", "Hawaii Daylight Time", "HDT", "Honolulu" },
		new String[]{ "AST", "Alaska Standard Time", "AST", "Alaska Daylight Time", "ADT", "Anchorage" } } }
};

public Object[][] getContents() {
	return (contents);
}

}
