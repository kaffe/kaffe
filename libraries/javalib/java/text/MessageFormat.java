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
import java.util.Locale;
import kaffe.util.NotImplemented;

public class MessageFormat extends Format {

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
	int len = patt.length();
	int argcount = 0;
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
	formats = new Format[argcount];
	strs = new String[argcount+1];
	argumentNumber = new int[argcount];

	argcount = 0;
	int start = 0;
	for (int curr = 0; curr < len; curr++) {
		if (patt.charAt(curr) == '{') {
			strs[argcount] = patt.substring(start, curr);
			curr++;
			start = curr;
			while (patt.charAt(curr) != '}' && curr < len) {
				curr++;
			}
			parseFormat(patt.substring(start, curr), argcount);
			argcount++;
			start = curr+1;
		}
	}
	strs[argcount] = patt.substring(start, len);
}

private void parseFormat(String argument, int argcount) {
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

	argumentNumber[argcount] = anumber;
	if (aformat.equals("time")) {
		if (astyle.equals("")) {
			formats[argcount] = DateFormat.getTimeInstance(DateFormat.DEFAULT, locale);
		}
		else if (astyle.equals("short")) {
			formats[argcount] = DateFormat.getTimeInstance(DateFormat.SHORT, locale);
		}
		else if (astyle.equals("medium")) {
			formats[argcount] = DateFormat.getTimeInstance(DateFormat.MEDIUM, locale);
		}
		else if (astyle.equals("long")) {
			formats[argcount] = DateFormat.getTimeInstance(DateFormat.LONG, locale);
		}
		else if (astyle.equals("full")) {
			formats[argcount] = DateFormat.getTimeInstance(DateFormat.FULL, locale);
		}
		else {
			formats[argcount] = new SimpleDateFormat(astyle, locale);
		}
	}
	else if (aformat.equals("date")) {
		if (astyle.equals("")) {
			formats[argcount] = DateFormat.getDateInstance(DateFormat.DEFAULT, locale);
		}
		else if (astyle.equals("short")) {
			formats[argcount] = DateFormat.getDateInstance(DateFormat.SHORT, locale);
		}
		else if (astyle.equals("medium")) {
			formats[argcount] = DateFormat.getDateInstance(DateFormat.MEDIUM, locale);
		}
		else if (astyle.equals("long")) {
			formats[argcount] = DateFormat.getDateInstance(DateFormat.LONG, locale);
		}
		else if (astyle.equals("full")) {
			formats[argcount] = DateFormat.getDateInstance(DateFormat.FULL, locale);
		}
		else {
			formats[argcount] = new SimpleDateFormat(astyle, locale);
		}
	}
	else if (aformat.equals("number")) {
		if (astyle.equals("currency")) {
			formats[argcount] = NumberFormat.getCurrencyInstance(locale);
		}
		else if (astyle.equals("percent")) {
			formats[argcount] = NumberFormat.getPercentInstance(locale);
		}
		else if (astyle.equals("integer")) {
			formats[argcount] = NumberFormat.getNumberInstance(locale);
		}
		else {
			formats[argcount] = new DecimalFormat(astyle, locale);
		}
	}
	else if (aformat.equals("choice")) {
		formats[argcount] = new ChoiceFormat(astyle);
	}
	else {
		// Should be a string.
		formats[argcount] = null;
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
	for (int i = 0; i < formats.length; i++) {
		buf.append(strs[i]);
		if (formats[i] == null) {
			buf.append(args[argumentNumber[i]].toString());
		}
		else {
			formats[i].format(args[argumentNumber[i]], buf, dummy);
		}
	}
	buf.append(strs[strs.length - 1]);
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
	throw new NotImplemented(MessageFormat.class.getName() + ".parse(String,ParsePosition)");
}

public Object[] parse(String str) throws ParseException {
	return (parse(str, new ParsePosition(0)));
}

public Object parseObject(String str, ParsePosition pos) {
	Object objs = parse(str, pos);
	return ((Object)objs);
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
	throw new NotImplemented(MessageFormat.class.getName() + ".toPattern()");
}

}
