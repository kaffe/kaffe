/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 * Some more Copyright(c) 2003 Guilhem Lavaux
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.text;

import java.util.Locale;
import kaffe.util.NotImplemented;

public class MessageFormat extends Format {
	private class MessagePatternDescription {
		String[] strs;
		Format[] formats;
		int[] argumentNumber;
	}

/**
 * @serial The locale to use for formatting numbers and dates.
 */
private Locale locale = Locale.getDefault();

/**
 * @serial An array of ten formatters, which are used to format 
 * the first ten arguments.
 */
private Format[] formats;

/**
 * @serial The argument numbers corresponding to each formatter. 
 *  (The formatters are stored in the order they occur in the pattern, not
 *   in the order in which the arguments are specified.)
 */
private int[] argumentNumber;

/*
 * XXX: this class does not conform to Sun's serial form.
 * We list the members that need to initialized for proper serialization
 * below.
 */
transient private String[] strs;

/**
 * @serial
 *  One less than the number of entries in offsets. Can also be
 *  thought of as the index of the highest-numbered element in
 *  offsets that is being used. All of these arrays should have the
 *  same number of elements being used as offsets does, and so this
 *  variable suffices to tell us how many entries are in all of them.
 */
private int maxOffset;

/**
 * @serial
 *  The positions where the results of formatting each argument are
 *  to be inserted into the pattern.
 */
private int[] offsets;

/**
 * @serial
 *  The string that the formatted values are to be plugged into. In
 *  other words, this is the pattern supplied on construction with
 *  all of the {} expressions taken out.
 */
private String pattern;

public MessageFormat(String patt) {
	applyPattern(patt);
}

public void applyPattern(String patt) {
	MessagePatternDescription desc;

	desc = tryToApplyPattern(patt);

	formats = desc.formats;
	strs = desc.strs;
	argumentNumber = desc.argumentNumber;
	pattern = patt;
}

private MessagePatternDescription tryToApplyPattern(String patt) {
	int len = patt.length();
	int argcount = 0;
	MessagePatternDescription desc = new MessagePatternDescription();
	
	for (int i = 0; i < len; i++) {
		if (patt.charAt(i) == '{') {
			argcount++;
			while (patt.charAt(i) != '}' && i < len) {
				i++;
			}
		}
	}

	// Allocate the required number of formatting arguments and enough
	// strings to go around them.
	desc.formats = new Format[argcount];
	desc.strs = new String[argcount+1];
	desc.argumentNumber = new int[argcount];

	argcount = 0;
	int start = 0;
	for (int curr = 0; curr < len; curr++) {
		if (patt.charAt(curr) == '{') {
			desc.strs[argcount] = patt.substring(start, curr);
			curr++;
			start = curr;
			while (patt.charAt(curr) != '}' && curr < len) {
				curr++;
			}
			parseFormat(patt.substring(start, curr), desc, argcount);
			argcount++;
			start = curr+1;
		}
	}
	desc.strs[argcount] = patt.substring(start, len);

	return desc;
}

private void parseFormat(String argument, MessagePatternDescription desc,
			 int argcount) {
	int anumber;
	String aformat;
	String astyle;
	int nend = argument.indexOf(',');
	int fstart = nend + 1;
	if (nend == -1) {
		nend = argument.length();
		fstart = nend;
	}
	int fend = argument.indexOf(',', nend+1);
	int sstart = fend + 1;
	if (fend == -1) {
		fend = argument.length();
		sstart = fend;
	}

	try {
		anumber = Integer.parseInt(argument.substring(0, nend));
	}
	catch (NumberFormatException _) {
		anumber = -1;
	}
	aformat = argument.substring(fstart, fend);
	astyle = argument.substring(sstart, argument.length());

	desc.argumentNumber[argcount] = anumber;
	if (aformat.equals("time")) {
		if (astyle.equals("")) {
			desc.formats[argcount] = DateFormat.getTimeInstance(DateFormat.DEFAULT, locale);
		}
		else if (astyle.equals("short")) {
			desc.formats[argcount] = DateFormat.getTimeInstance(DateFormat.SHORT, locale);
		}
		else if (astyle.equals("medium")) {
			desc.formats[argcount] = DateFormat.getTimeInstance(DateFormat.MEDIUM, locale);
		}
		else if (astyle.equals("long")) {
			desc.formats[argcount] = DateFormat.getTimeInstance(DateFormat.LONG, locale);
		}
		else if (astyle.equals("full")) {
			desc.formats[argcount] = DateFormat.getTimeInstance(DateFormat.FULL, locale);
		}
		else {
			desc.formats[argcount] = new SimpleDateFormat(astyle, locale);
		}
	}
	else if (aformat.equals("date")) {
		if (astyle.equals("")) {
			desc.formats[argcount] = DateFormat.getDateInstance(DateFormat.DEFAULT, locale);
		}
		else if (astyle.equals("short")) {
			desc.formats[argcount] = DateFormat.getDateInstance(DateFormat.SHORT, locale);
		}
		else if (astyle.equals("medium")) {
			desc.formats[argcount] = DateFormat.getDateInstance(DateFormat.MEDIUM, locale);
		}
		else if (astyle.equals("long")) {
			desc.formats[argcount] = DateFormat.getDateInstance(DateFormat.LONG, locale);
		}
		else if (astyle.equals("full")) {
			desc.formats[argcount] = DateFormat.getDateInstance(DateFormat.FULL, locale);
		}
		else {
			desc.formats[argcount] = new SimpleDateFormat(astyle, locale);
		}
	}
	else if (aformat.equals("number")) {
		if (astyle.equals("")) {
			desc.formats[argcount] = NumberFormat.getInstance(locale);
		}
		else if (astyle.equals("currency")) {
			desc.formats[argcount] = NumberFormat.getCurrencyInstance(locale);
		}
		else if (astyle.equals("percent")) {
			desc.formats[argcount] = NumberFormat.getPercentInstance(locale);
		}
		else if (astyle.equals("integer")) {
			desc.formats[argcount] = NumberFormat.getNumberInstance(locale);
		}
		else {
			desc.formats[argcount] = new DecimalFormat(astyle, locale);
		}
	}
	else if (aformat.equals("choice")) {
		desc.formats[argcount] = new ChoiceFormat(astyle);
	}
	else {
		// Should be a string.
		desc.formats[argcount] = null;
	}
}

public Object clone() {
	MessageFormat obj = new MessageFormat("");
	obj.locale = this.locale;
	obj.strs = this.strs;
	obj.formats = this.formats;
	obj.argumentNumber = this.argumentNumber;
	return (obj);
}

public boolean equals(Object obj) {
	if (obj instanceof MessageFormat) {
		MessageFormat other = (MessageFormat)obj;
		if (locale != other.locale) {
			return (false);
		}
		for (int i = formats.length; i-- > 0; ) {
			if (!formats[i].equals(other.formats[i])) {
				return (false);
			}
			if (!strs[i].equals(other.strs[i])) {
				return (false);
			}
		}
		return (true);
	}
	else {
		return (false);
	}
}

public static String format(String patt, Object args[]) {
	return ((new MessageFormat(patt)).format(args, new StringBuffer(), null).toString());
}

public final StringBuffer format(Object args[], StringBuffer buf, FieldPosition ignore) {
	FieldPosition dummy = new FieldPosition(0);
	MessagePatternDescription desc = new MessagePatternDescription();
	boolean hasChanged = true;
	StringBuffer temp_buf;

	desc.formats = formats;
	desc.strs = strs;
	desc.argumentNumber = argumentNumber;

	do {
		temp_buf = new StringBuffer();

		for (int i = 0; i < desc.formats.length; i++) {
			temp_buf.append(desc.strs[i]);
			if (desc.formats[i] == null) {
				temp_buf.append(args[desc.argumentNumber[i]].toString());
			}
			else {
				desc.formats[i].format(args[desc.argumentNumber[i]], temp_buf, dummy);
			}
		}
	        temp_buf.append(desc.strs[desc.strs.length - 1]);
		desc = tryToApplyPattern(temp_buf.toString());
		if (desc.argumentNumber.length == 0)
	            hasChanged = false;
	} while(hasChanged);
	buf.append(temp_buf.toString());
	return (buf);
}

public final StringBuffer format(Object arg, StringBuffer append, FieldPosition ignore) {
	if (arg instanceof Object[]) {
		return (format((Object[])arg, append, ignore));
	}
	else {
		return (format(new Object[]{ arg }, append, ignore));
	}
}

public Format[] getFormats() {
	return (formats);
}

public Locale getLocale() {
	return (locale);
}

public int hashCode() {
	return (super.hashCode());
}

public Object[] parse(String str, ParsePosition pos) {
	Object[] data = new Object[argumentNumber.length];
	int count, startPos, endPos;
	int parsingPosition;
	ParsePosition subParsePos = new ParsePosition(0);

	parsingPosition = pos.getIndex();
	str = str.substring(pos.getIndex());
	for (parsingPosition=0,count=0;count<strs.length;count++) {
		// Check the inter-argument string.
		endPos = strs[count].length();
		if (!strs[count].equals(str.substring(0, endPos))) {
			pos.setErrorIndex(parsingPosition);
			return null;
		}
		str = str.substring(endPos);

		parsingPosition += endPos;
		if (count == strs.length-1)
			break;

		// There is predefined format: great, let it decide if it
		// is parsable.
		if (formats[count] != null) {
			subParsePos.setIndex(0);
			data[count] = formats[count].parseObject(str, subParsePos);
			if (data[count] == null) {
				pos.setErrorIndex(subParsePos.getErrorIndex()+parsingPosition);
				return null;
			}
			str = str.substring(subParsePos.getIndex());
			parsingPosition += subParsePos.getIndex();
		} else {
			int boundary;
			// Aie ! We need to guess. Try to mimic JDK.
			// First find the boundary of the argument.
			if (count == strs.length-2 && strs[strs.length-1].equals(""))
				boundary = str.length();
			else {
				boundary = str.indexOf(strs[count+1]);
				if (boundary == -1) {
					pos.setErrorIndex(parsingPosition);
					return null;
				}
			}

			// Just extract the string.
			data[count] = str.substring(0, boundary);
			str = str.substring(boundary);
			parsingPosition += boundary;
		}
	}
	pos.setIndex(parsingPosition);

	return data;
}

public Object[] parse(String str) throws ParseException {
	ParsePosition pos = new ParsePosition(0);
	Object[] data;
	
	data = parse(str, pos);
	if (data == null)
		throw new ParseException("Message parsing error", pos.getErrorIndex());

	return data;
}

public Object parseObject(String str, ParsePosition pos) {
	return (parse(str, pos));
}

public void setFormat(int num, Format newformat) {
	formats[num] = newformat;
}

public void setFormats(Format[] newformats) {
	formats = newformats;
}

public void setLocale(Locale loc) {
	this.locale = loc;
}

public String toPattern() {
	return pattern;
}

}
