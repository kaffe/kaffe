/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998, 2000
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
package java.text;

import java.util.Locale;
import kaffe.util.NotImplemented;

public class DecimalFormat
  extends NumberFormat
{
	private static final long serialVersionUID = 864413376551465018L;
	private DecimalFormatSymbols syms;
	private int groupsize;
	private int multiplier;
	private String negativeprefix;
	private String negativesuffix;
	private String positiveprefix;
	private String positivesuffix;
	private boolean decsepshown;

public DecimalFormat() {
	this("#,##0.###;-#,##0.###", Locale.getDefault());
}

public DecimalFormat(String pattern) {
	this(pattern, Locale.getDefault());
}

public DecimalFormat(String pattern, DecimalFormatSymbols syms) {
	this.syms = syms;
	applyPattern(pattern);
}

DecimalFormat(String pattern, Locale loc) {
	this(pattern, new DecimalFormatSymbols(loc));
}

public void applyLocalizedPattern(String pattern) {
	char[] patt = pattern.toCharArray();

	for (int i = 0; i < patt.length; i++) {
		if (patt[i] == syms.digit) {
			patt[i] = '#';
		}
		else if (patt[i] == syms.patternSeparator) {
			patt[i] = ';';
		}
		else if (patt[i] == syms.zeroDigit) {
			patt[i] = '0';
		}
		else if (patt[i] == syms.groupSeparator) {
			patt[i] = ',';
		}
		else if (patt[i] == syms.decimalSeparator) {
			patt[i] = '.';
		}
		else if (patt[i] == syms.percentSign) {
			patt[i] = '%';
		}
		else if (patt[i] == syms.permillSign) {
			patt[i] = '\u2030';
		}
		else if (patt[i] == syms.currencySign) {
			patt[i] = '\u00a4';
		}
		else if (patt[i] == syms.minusSign) {
			patt[i] = '-';
		}
		else if (patt[i] == '\'') {
			for (i++; i < patt.length && patt[i] != '\''; i++);
		}
		else {
			// Just leave the character alone
		}
	}

	applyPattern(new String(patt));
}

public void applyPattern(String pattern) {

	multiplier = 100;
	negativeprefix = "";
	negativesuffix = "";
	positiveprefix = "";
	positivesuffix = "";
	intonly = true;
	minint = 1;
	maxint = Integer.MAX_VALUE;
	minfrac = 0;
	maxfrac = 0;
	decsepshown = false;
	groupsize = Integer.MAX_VALUE;
	grouping = false;

	char[] patt = pattern.toCharArray();

	int formatstart = 0;
	int formatend = patt.length;

	int want = 0;
	int si = 0;
	for (int i = 0; i < patt.length; i++) {
		switch (patt[i]) {
		case '0':
		case '#':
		case '.':
		case ',':
			switch (want) {
			case 0:
				positiveprefix = new String(patt, si, i-si);
				want = 1;
				formatstart = i;
				break;
			case 2:
				positivesuffix = new String(patt, si, i-si);
				want = 3;
				break;
			case 4:
				negativeprefix = new String(patt, si, i-si);
				want = 5;
				break;
			case 6:
				negativesuffix = new String(patt, si, i-si);
				want = 7;
				break;
			}
			break;

		case ';':
			if ( want == 2 ) {
				positivesuffix = new String(patt, si, i-si);
			}
			si = i+1;
			formatend = i;
			want = 4;
			break;

		default:
			switch (want) {
			case 1:
				si = i;
				want = 2;
				break;
			case 5:
				si = i;
				want = 6;
				break;
			}
			break;
		}
	}
	if (want == 6) {
		negativesuffix = new String(patt, si, patt.length-si);
	}

	// If we don't distiguish between positive and negative then
	// add some default.
	if (positiveprefix.equals(negativeprefix) && positivesuffix.equals(negativesuffix)) {
		char[] minus = new char[1];
		minus[0] = syms.minusSign;
		negativeprefix = new String(minus);
	}

	boolean dec = false;
	int group = -1;
	int zerocount = 0;
	int hashcount = 0;

	for (int i = formatstart; i < formatend; i++) {
		switch (patt[i]) {
		case '0':
			if (hashcount > 0 && dec)
				throw new IllegalArgumentException("illegal pattern \"" + pattern + "\"");
			zerocount++;
			break;
		case '#':
			if (zerocount > 0 && !dec)
				throw new IllegalArgumentException("illegal pattern \"" + pattern + "\"");
			hashcount++;
			break;
		case '.':
			if (dec)
				throw new IllegalArgumentException("illegal pattern \"" + pattern + "\"");
			dec = true;
			minint = zerocount;
			maxint = Integer.MAX_VALUE;
			if (group == -1 || i - group < 2) {
				groupsize = 0;
				grouping = false;
			}
			else {
				groupsize = i - group - 1;
				grouping = true;
			}
			hashcount = 0;
			zerocount = 0;
			break;
		case ',':
			if (group >= 0)
				throw new IllegalArgumentException("illegal pattern \"" + pattern + "\"");
			group = i;
			break;

		default:
			break;
		}
	}

	if (dec) {
		intonly = false;
		minfrac = zerocount;
		maxfrac = minfrac + hashcount;
		if (zerocount > 0) {
			decsepshown = true;
		}
	}
	else {
		minint = zerocount;
	}
}

public Object clone() {
	return (super.clone());
}

public boolean equals(Object obj) {
	return (super.equals(obj));
}

private StringBuffer format(String num, StringBuffer app, FieldPosition pos) {
	StringBuffer buf = new StringBuffer();
	char[] val = num.toCharArray();

	int decpos = num.indexOf('.');

	int endpos = decpos;
	if (endpos == -1) {
		endpos = val.length;
	} else {
		/*
	 	 * prepare num so the stuff below works:
	 	 * 	a) if we're formatting a floating point value but don't have any
	 	 * 	   fractional digits, we may have to round the last integer digit
	 	 *	b) if we're formatting a floating point value that has more fractional
	 	 * 	   digits than our pattern, round the last digit that fits into the pattern 
	 	 */
		if (maxfrac==0) {
			if (val[decpos+1] >= '5') {
				val[decpos-1] = (char)(val[decpos-1] + 1 - '0' + syms.zeroDigit);	
			}
		} else if ((maxfrac < val.length-decpos-1) && (val[decpos+maxfrac+1] >= '5')) {
			val[decpos+maxfrac] = (char)(val[decpos+maxfrac] + 1 - '0' + syms.zeroDigit); 
		}
	}

	int startpos = 0;
	if (val[startpos] == '-') {
		startpos++;
	}

	int count = 0;
	for (int i = endpos - 1; i >= startpos && count < maxint; i--, count++) {
		if (grouping && count % groupsize == 0 && count > 0) {
			buf.append(syms.groupSeparator);
		}

		buf.append((char)(val[i] - '0' + syms.zeroDigit));
	}
	for (; count < minint; count++) {
		if (grouping && count % groupsize == 0) {
			buf.append(syms.groupSeparator);
		}
		buf.append(syms.zeroDigit);
	}

	buf.reverse();

	if (val[0] == '-') {
		buf.insert(0, negativeprefix);
	}
	else {
		buf.insert(0, positiveprefix);
	}


	if (pos.getField() == INTEGER_FIELD) {
		pos.setBeginIndex(app.length());
		app.append(buf);
		pos.setEndIndex(app.length());
	}
	else {
		app.append(buf);
	}
	buf.setLength(0);

	if (!intonly) {

		count = 0;

		if (decpos != -1) {
			startpos = decpos + 1;
			endpos = val.length;

			/* First we remove the extra zero generated
			 * by Double.toString()
			 */
			if (val[endpos-1] == '0')
			    endpos--;

			for (int i = startpos; i < endpos && count < maxfrac; i++, count++) {
				buf.append((char)(val[i] - '0' + syms.zeroDigit));
			}
		}

		for (; count < minfrac; count++) {
			buf.append(syms.zeroDigit);
		}

		if (count == 0)
			decpos = -1;
	}

	if (val[0] == '-') {
		buf.append(negativesuffix);
	}
	else {
		buf.append(positivesuffix);
	}

	if (decsepshown || (!intonly && (decpos != -1 || minfrac > 0))) {
		app.append(syms.decimalSeparator);
	}

	if (pos.getField() == FRACTION_FIELD) {
		pos.setBeginIndex(app.length());
		app.append(buf);
		pos.setEndIndex(app.length());
	}
	else {
		app.append(buf);
	}

	return (app);
}

public StringBuffer format(double num, StringBuffer buf, FieldPosition pos) {
	return (format(Double.toString(num), buf, pos));

}

public StringBuffer format(long num, StringBuffer buf, FieldPosition pos) {
	return (format(Long.toString(num), buf, pos));
}

public DecimalFormatSymbols getDecimalFormatSymbols() {
	return (syms);
}

public int getGroupingSize() {
	return (groupsize);
}

public int getMultiplier() {
	return (multiplier);
}

public String getNegativePrefix() {
	return (negativeprefix);
}

public String getNegativeSuffix() {
	return (negativesuffix);
}

public String getPositivePrefix() {
	return (positiveprefix);
}

public String getPositiveSuffix() {
	return (positivesuffix);
}

public int hashCode() {
	return (super.hashCode());
}

public boolean isDecimalSeparatorAlwaysShown() {
	return (decsepshown);
}

	
public Number parse(String source, ParsePosition pos) {
	StringBuffer sb = new StringBuffer();
	int startIndex = pos.getIndex();
	int index = parse0 (source, startIndex, source.length(), sb, false);

	if (index < 0) {
		pos.setErrorIndex (-index);
		return null;
	}

	pos.setIndex (index);
	
	// try Long first.
	try {
		return Long.valueOf (sb.toString());
	}
	catch (NumberFormatException nfe) {
	}
	try {
		return Double.valueOf (sb.toString());
	}
	catch (NumberFormatException nfe) {
		pos.setIndex (startIndex);
		pos.setErrorIndex (index);
		return null;
	}
}

private int parse0 (String source, int index, int endIndex,
		    StringBuffer sb, boolean isExponent) 
{
	if (!isExponent && !isParseIntegerOnly() &&
	    source.regionMatches (false, index, syms.nan, 0, syms.nan.length())) {
	    sb.append("NaN");
	    return index + syms.nan.length();
	}
	
	if ((positiveprefix.length() > 0) &&
	    source.regionMatches (false, index, positiveprefix, 0, positiveprefix.length())) {
		index += positiveprefix.length();
	}
	else if ((negativeprefix.length() > 0) && 
		 source.regionMatches (false, index, negativeprefix, 0, negativeprefix.length())) {
		sb.append('-');
		index += negativeprefix.length();
	}

	if (!isExponent && !isParseIntegerOnly() &&
	    source.regionMatches (false, index, syms.infinity, 0, syms.infinity.length())) {
	    sb.append("Inf");
	    index += syms.infinity.length();
	}
	
	boolean allowDot = !isExponent && !isParseIntegerOnly();
	char zeroDigit = syms.zeroDigit;
	char nineDigit = (char)(zeroDigit + 9);
	while (index < endIndex) {
		char c = source.charAt(index);
		if ((zeroDigit <= c) && (c <= nineDigit)) {
			sb.append ((char)(c - zeroDigit + '0'));
			index++;
		}
		else if (c == syms.decimalSeparator && allowDot) {
			allowDot = false;
			sb.append ('.');
			index++;
		}
		else if (c == syms.groupSeparator && isGroupingUsed() && !isExponent) {
			index++;
		}
		else if (c == 'E' && !isExponent) {
			sb.append ('E');
			index = parse0 (source, index + 1, endIndex, sb, true);
			if (index < 0)
				return index;
		}
		else break;
	}
	
	return index;
}

public void setDecimalFormatSymbols(DecimalFormatSymbols syms) {
	this.syms = syms;
}

public void setDecimalSeparatorAlwaysShown(boolean val) {
	decsepshown = val;
}

public void setGroupingSize(int val) {
	groupsize = val;
}

public void setMultiplier(int val) {
	multiplier = val;
}

public void setNegativePrefix(String val) {
	negativeprefix = val;
}

public void setNegativeSuffix(String val) {
	negativesuffix = val;
}

public void setPositivePrefix(String val) {
	positiveprefix = val;
}

public void setPositiveSuffix(String val) {
	positivesuffix = val;
}

public String toLocalizedPattern() {
	throw new NotImplemented();
}

public String toPattern() {
	throw new NotImplemented();
}
}
