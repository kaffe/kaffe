package java.text;

import java.util.Locale;
import kaffe.util.NotImplemented;

public class DecimalFormat extends NumberFormat {

private DecimalFormatSymbols syms;

private int groupsize;
private int multiplier;
private String negativeprefix;
private String negativesuffix;
private String positiveprefix;
private String positivesuffix;
private boolean decsepshown;

public DecimalFormat() {
	this("0.#", Locale.getDefault());
}

public DecimalFormat(String pattern) {
	this(pattern, Locale.getDefault());
}

public DecimalFormat(String pattern, Locale loc) {
	this(pattern, new DecimalFormatSymbols(loc));
}

public DecimalFormat(String pattern, DecimalFormatSymbols syms) {
	this.syms = syms;
	applyPattern(pattern);
}

public void applyLocalizedPattern(String pattern) {
	char[] patt = pattern.toCharArray();

	for (int i = 0; i < patt.length; i++) {
		if (patt[i] == syms.digit) {
			patt[i] = '#';
		}
		else if (patt[i] == syms.patternSeperator) {
			patt[i] = ';';
		}
		else if (patt[i] == syms.zeroDigit) {
			patt[i] = '0';
		}
		else if (patt[i] == syms.groupSeperator) {
			patt[i] = ',';
		}
		else if (patt[i] == syms.decimalSeperator) {
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
	maxfrac = Integer.MAX_VALUE;
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
			if (hashcount > 0 && dec == true) {
				// throw new ParseException("", i);
				return;
			}
			zerocount++;
			break;
		case '#':
			if (zerocount > 0 && dec == false) {
				// throw new ParseException("", i);
				return;
			}
			hashcount++;
			break;
		case '.':
			if (dec == true) {
				// throw new ParseException("", i);
				return;
			}
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
			if (group >= 0) {
				// throw new ParseException("", i);
				return;
			}
			group = i;
			break;

		default:
			break;
		}
	}

	if (dec == true) {
		intonly = false;
		minfrac = zerocount;
		maxfrac = Integer.MAX_VALUE;
		if (zerocount == 0 && hashcount == 0) {
			decsepshown = true;
		}
	}
}

public Object clone() {
	return (super.clone());
}

public boolean equals(Object obj) {
	return (super.equals(obj));
}

public StringBuffer format(double num, StringBuffer buf, FieldPosition pos) {
	return (format(Double.toString(num), buf, pos));

}

public StringBuffer format(long num, StringBuffer buf, FieldPosition pos) {
	return (format(Long.toString(num), buf, pos));
}

private StringBuffer format(String num, StringBuffer app, FieldPosition pos) {
	StringBuffer buf = new StringBuffer();
	char[] val = num.toCharArray();

	int decpos = num.indexOf('.');

	int endpos = decpos;
	if (endpos == -1) {
		endpos = val.length;
	}
	int startpos = 0;
	if (val[startpos] == '-') {
		startpos++;
	}

	int count = 0;
	for (int i = endpos - 1; i >= startpos && count < maxint; i--, count++) {
		if (grouping == true && count % groupsize == 0 && count > 0) {
			buf.append(syms.groupSeperator);
		}
		buf.append((char)(val[i] - '0' + syms.zeroDigit));
	}
	for (; count < minint; count++) {
		if (grouping == true && count % groupsize == 0) {
			buf.append(syms.groupSeperator);
		}
		buf.append(syms.zeroDigit);
	}

	if (val[0] == '-') {
		buf.append(negativeprefix);
	}
	else {
		buf.append(positiveprefix);
	}

	buf.reverse();

	if (pos.field == INTEGER_FIELD) {
		pos.begin = app.length();
		app.append(buf);
		pos.end = app.length();
	}
	else {
		app.append(buf);
	}
	buf.setLength(0);

	if (intonly == false && minfrac > 0) {

		count = 0;

		if (decpos != -1) {
			startpos = decpos + 1;
			endpos = val.length;

			for (int i = startpos; i < endpos && count < maxfrac; i++, count++) {
				buf.append((char)(val[i] - '0' + syms.zeroDigit));
			}
		}

		for (; count < minfrac; count++) {
			buf.append(syms.zeroDigit);
		}

	}

	if (val[0] == '-') {
		buf.append(negativesuffix);
	}
	else {
		buf.append(positivesuffix);
	}

	if (decsepshown == true || (intonly == false && (decpos != -1 || minfrac > 0))) {
		app.append(syms.decimalSeperator);
	}

	if (pos.field == FRACTION_FIELD) {
		pos.begin = app.length();
		app.append(buf);
		pos.end = app.length();
	}
	else {
		app.append(buf);
	}

	return (app);
}

public DecimalFormatSymbols getDecimalFormatSymbols() {
	return (syms);
}

public int getGroupSize() {
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

public boolean isDecimalSeperatorAlwaysShown() {
	return (decsepshown);
}

public Number parse(String source, ParsePosition pos) {
	throw new NotImplemented();
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

public String toLocalizePattern() {
	throw new NotImplemented();
}

public String toPattern() {
	throw new NotImplemented();
}

}
