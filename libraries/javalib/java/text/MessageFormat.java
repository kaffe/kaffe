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

private Locale loc = Locale.getDefault();
private String[] strs;
private Format[] forms;
private int[] argno;

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
	forms = new Format[argcount];
	strs = new String[argcount+1];
	argno = new int[argcount];

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

	argno[argcount] = anumber;
	if (aformat.equals("time")) {
		if (astyle.equals("")) {
			forms[argcount] = DateFormat.getTimeInstance(DateFormat.DEFAULT, loc);
		}
		else if (astyle.equals("short")) {
			forms[argcount] = DateFormat.getTimeInstance(DateFormat.SHORT, loc);
		}
		else if (astyle.equals("medium")) {
			forms[argcount] = DateFormat.getTimeInstance(DateFormat.MEDIUM, loc);
		}
		else if (astyle.equals("long")) {
			forms[argcount] = DateFormat.getTimeInstance(DateFormat.LONG, loc);
		}
		else if (astyle.equals("full")) {
			forms[argcount] = DateFormat.getTimeInstance(DateFormat.FULL, loc);
		}
		else {
			forms[argcount] = new SimpleDateFormat(astyle, loc);
		}
	}
	else if (aformat.equals("date")) {
		if (astyle.equals("")) {
			forms[argcount] = DateFormat.getDateInstance(DateFormat.DEFAULT, loc);
		}
		else if (astyle.equals("short")) {
			forms[argcount] = DateFormat.getDateInstance(DateFormat.SHORT, loc);
		}
		else if (astyle.equals("medium")) {
			forms[argcount] = DateFormat.getDateInstance(DateFormat.MEDIUM, loc);
		}
		else if (astyle.equals("long")) {
			forms[argcount] = DateFormat.getDateInstance(DateFormat.LONG, loc);
		}
		else if (astyle.equals("full")) {
			forms[argcount] = DateFormat.getDateInstance(DateFormat.FULL, loc);
		}
		else {
			forms[argcount] = new SimpleDateFormat(astyle, loc);
		}
	}
	else if (aformat.equals("number")) {
		if (astyle.equals("currency")) {
			forms[argcount] = NumberFormat.getCurrencyInstance(loc);
		}
		else if (astyle.equals("percent")) {
			forms[argcount] = NumberFormat.getPercentInstance(loc);
		}
		else if (astyle.equals("integer")) {
			forms[argcount] = NumberFormat.getNumberInstance(loc);
		}
		else {
			forms[argcount] = new DecimalFormat(astyle, loc);
		}
	}
	else if (aformat.equals("choice")) {
		forms[argcount] = new ChoiceFormat(astyle);
	}
	else {
		// Should be a string.
		forms[argcount] = null;
	}
}

public Object clone() {
	MessageFormat obj = new MessageFormat("");
	obj.loc = this.loc;
	obj.strs = this.strs;
	obj.forms = this.forms;
	obj.argno = this.argno;
	return (obj);
}

public boolean equals(Object obj) {
	try {
		MessageFormat other = (MessageFormat)obj;
		if (loc != other.loc) {
			return (false);
		}
		for (int i = 0; i < forms.length; i++) {
			if (!forms[i].equals(other.forms[i])) {
				return (false);
			}
			if (!strs[i].equals(other.strs[i])) {
				return (false);
			}
		}
		return (true);
	}
	catch (ClassCastException _) {
		return (false);
	}
}

public static String format(String patt, Object args[]) {
	return ((new MessageFormat(patt)).format(args, new StringBuffer(), null).toString());
}

public final StringBuffer format(Object args[], StringBuffer buf, FieldPosition ignore) {
	FieldPosition dummy = new FieldPosition(0);
	for (int i = 0; i < forms.length; i++) {
		buf.append(strs[i]);
		if (forms[i] == null) {
			buf.append(args[argno[i]].toString());
		}
		else {
			forms[i].format(args[argno[i]], buf, dummy);
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
	return (forms);
}

public Locale getLocale() {
	return (loc);
}

public int hashCode() {
	return (super.hashCode());
}

public Object[] parse(String str, ParsePosition pos) {
	throw new NotImplemented();
}

public Object[] parse(String str) throws ParseException {
	return (parse(str, new ParsePosition(0)));
}

public Object parseObject(String str, ParsePosition pos) {
	Object objs = parse(str, pos);
	return ((Object)objs);
}

public void setFormat(int num, Format newformat) {
	forms[num] = newformat;
}

public void setFormats(Format[] newformats) {
	forms = newformats;
}

public void setLocale(Locale loc) {
	this.loc = loc;
}

public String toPattern() {
	throw new NotImplemented();
}

}
