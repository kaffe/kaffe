package kaffe.util;

import java.text.DateFormatSymbols;
import java.util.TimeZone;

/*
 * DateScanner -
 *
 * Copyright (c) 1998
 *    Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 */
public class DateScanner
{
	int si0;
	int si1;
	private int sLen;
	char[] src;
	private int tok;
	final static int DAY = 0;
	final static int MONTH = 1;
	final static int YEAR = 2;
	final static int HOUR = 3;
	final static int MINUTE = 4;
	final static int SECOND = 5;
	final static int WEEKDAY = 6;
	final static int ZONE = 7;
	final static int OFFSET = 8;
	final static int EOF = -1;
	final static int WDDAY = 9;
	final static int WDMONTH = 10;
	final static int AMPM = 11;
	final static int UNKNOWN = -2;
	private DateFormatSymbols syms;

public DateScanner( String date) {
	this( date, new DateFormatSymbols() );
}

public DateScanner( String date, DateFormatSymbols syms) {
	this.syms = syms;
	src = date.toCharArray();
	sLen = src.length;
	si0 = 0;
	si1 = -1;
	tok = EOF;
}

int closeToken() {
	if ( (src[si0] >= 'A') && (src[si0] <= 'Z') ) {
		String s = stringToken();
		if ( DateParser.getDayIdx( s, syms) > -1 )
			return ( tok = WEEKDAY );
		if ( DateParser.getMonthIdx( s, syms) > -1 )
			return ( tok = WDMONTH );
		if ( TimeZone.getTimeZone( s) != null )
			return ( tok = ZONE);
		if ( s.equals( "AM") || s.equals( "PM") )
			return ( tok = AMPM);
		return UNKNOWN;
	}
	
	switch ( tok) {
		case HOUR:	
			return ( tok = MINUTE);
		case MINUTE:
			return ( tok = SECOND);
		case MONTH:
			return ( tok = YEAR);
		case DAY:
			return ( tok = YEAR);
		case WEEKDAY:
			return ( tok = WDDAY);
		case WDDAY:
			return ( tok = WDMONTH);
		case WDMONTH:
			return ( tok = YEAR);
		case ZONE:
			return ( tok = OFFSET);
	}
	
	return (tok = EOF);
}

public static void main( String[] args) {
	DateScanner ds = new DateScanner( args[0]);
	int tok;
	while ( (tok = ds.nextToken() ) != EOF ) {
		System.out.println( tok + " " + ds.stringToken());
	}
}

public int nextToken() {
	char c;
	
	si1++;
	
	if ( si1 >= sLen )
		return ( tok = EOF);
		
	for ( ; si1<sLen; si1++) {
		if ( src[si1] != ' ' )
			break;
	}
	
	for ( si0 = si1; si1 < sLen; si1++) {
		c = src[si1];
		switch( c) {
			case '/':
				if ( tok == MONTH )
					return (tok = DAY);
				return (tok = MONTH);
			case ' ':
				return closeToken();
			case '.':
				if ( tok == DAY )
					return (tok = MONTH);
				return (tok = DAY);
			case ':':
				if ( tok == HOUR )
					return (tok = MINUTE);
				return (tok = HOUR);
			case ',':
				if ( tok == WDMONTH )
					return (tok = DAY);
				return (tok = WEEKDAY);
			case '+':
			case '-':
				return (tok = ZONE);
		}
	}

	return closeToken();
}

public String stringToken() {
	return new String( src, si0, si1-si0);
}
}
