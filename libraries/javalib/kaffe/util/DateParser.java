package kaffe.util;

import java.text.DateFormatSymbols;
import java.text.ParseException;
import java.util.Calendar;
import java.util.Date;
import java.util.TimeZone;

/*
 * DateParser -
 *
 * Copyright (c) 1998
 *    Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 */
public class DateParser
{
public static int getDayIdx( String day, DateFormatSymbols syms) {
	String[] arr;
	int idx;
	
	arr = syms.getWeekdays();
	for ( idx=0; idx<arr.length; idx++) {
		if ( arr[idx].equals( day) )
			return idx;
	}

	arr = syms.getShortWeekdays();
	for ( idx=0; idx<arr.length; idx++) {
		if ( arr[idx].equals( day) )
			return idx;
	}
	
	return -1;
}

public static int getMonthIdx( String day, DateFormatSymbols syms) {
	String[] arr;
	int idx;
	
	arr = syms.getMonths();
	for ( idx=0; idx<arr.length; idx++) {
		if ( arr[idx].equals( day) )
			return idx;
	}

	arr = syms.getShortMonths();
	for ( idx=0; idx<arr.length; idx++) {
		if ( arr[idx].equals( day) )
			return idx;
	}
	
	return -1;
}

public static void main( String[] args) {
	try {
		Date d = DateParser.parse( args[0] );
		System.out.println( d);
	}
	catch ( ParseException _x) {
		System.out.println( _x + " near: " + _x.getErrorOffset() );
	}
}

public static Date parse( String dateStr) throws ParseException {
	return parse( dateStr, new DateFormatSymbols() );
}

public static Date parse( String dateStr, DateFormatSymbols syms) throws ParseException {
	Calendar cal = Calendar.getInstance();
	DateScanner ds = new DateScanner( dateStr, syms);
	int tok;
	
	while ( (tok = ds.nextToken()) != DateScanner.EOF ) {
		switch ( tok) {
			case DateScanner.UNKNOWN:
				throw new ParseException( dateStr, ds.si1);
			case DateScanner.DAY:
			case DateScanner.WDDAY:
				int wdday = Integer.parseInt( ds.stringToken());
				cal.set( Calendar.DAY_OF_MONTH, wdday );
				break;
			case DateScanner.MONTH:
				int month = Integer.parseInt( ds.stringToken() ) -1;
				cal.set( Calendar.MONTH, month );
				break;
			case DateScanner.YEAR:
				int year = Integer.parseInt( ds.stringToken());
				if ( year < 100 )
					year += 1900;
				cal.set( Calendar.YEAR, year  );
				break;
			case DateScanner.HOUR:
				int hour = Integer.parseInt( ds.stringToken());
				cal.set( Calendar.HOUR_OF_DAY, hour );
				break;
			case DateScanner.MINUTE:
				int minute = Integer.parseInt( ds.stringToken());
				cal.set( Calendar.MINUTE, minute );
				break;
			case DateScanner.SECOND:
				int second = Integer.parseInt( ds.stringToken());
				cal.set( Calendar.SECOND, second );
				break;
			case DateScanner.WEEKDAY:
				int day = getDayIdx( ds.stringToken(), syms );
				cal.set( Calendar.DAY_OF_WEEK, day );
				break;
			case DateScanner.ZONE:
				String zone = ds.stringToken();
				cal.setTimeZone( TimeZone.getTimeZone( zone ));
				break;
			case DateScanner.OFFSET:
				int offs = Integer.parseInt( ds.stringToken());
				int val = (offs % 100) * 60000 + (offs / 100) * 3600000;
				if ( ds.src[ds.si0-1] == '-' )
					val *= -1;
				cal.set( Calendar.ZONE_OFFSET, val );
				break;
			case DateScanner.WDMONTH:
				int wdmonth = getMonthIdx( ds.stringToken(), syms );
				cal.set( Calendar.MONTH, wdmonth );
				break;
			case DateScanner.AMPM:
				String s = ds.stringToken();
				if ( s.equals( "PM") ) {
					int h = cal.get( Calendar.HOUR_OF_DAY);
					if ( h < 12 )
						h += 12;
					cal.set( Calendar.HOUR_OF_DAY, h);
				}
				break;
		}
	}
	
	return cal.getTime();
}
}
