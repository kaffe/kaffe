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
import java.util.Hashtable;

public class ChoiceFormat extends NumberFormat {

private double[] limits;
private String[] strings;
private String pattern = null;
private Hashtable patternNames = null;
    
public ChoiceFormat(String patt) {
	applyPattern(patt);
}

public ChoiceFormat(double[] limits, String[] strings) {
	setChoices(limits, strings);
}

public void applyPattern(String patt) {
	int len = patt.length();
	int argcount = 0;

	for (int i = 0; i < len; i++) {
	        if (patt.charAt(i) == '|') {
			argcount++;
		}
	}
	limits = new double[argcount+1];
	strings = new String[argcount+1];

	int s = 0;
	int c = 0;
	StringBuffer str = new StringBuffer();

	for (int i = 0; i < len; i++) {
		char ch = patt.charAt(i);
		if (ch == '#') {
			limits[c] = Double.valueOf(patt.substring(s, i)).doubleValue();
			str.append(limits[c]+"#");
			s = i+1;
		}
		else if (ch == '<') {
			limits[c] = nextDouble(Double.valueOf(patt.substring(s, i)).doubleValue());
			str.append(Double.valueOf(patt.substring(s, i)).doubleValue()+"<");
			s = i+1;
		}
		else if (ch == '|') {
			strings[c] = patt.substring(s, i);
			str.append(strings[c]+"|");
			c++;
			s = i+1;
		}
	}
	strings[c] = patt.substring(s, len);
	str.append(strings[c]);
	pattern = new String(str);
}

public Object clone() {
	return (new ChoiceFormat(limits, strings));
}

public boolean equals(Object obj) {
	if (obj instanceof ChoiceFormat) {
		ChoiceFormat other = (ChoiceFormat)obj;
		for (int i = limits.length; i-- > 0; ) {
			if (limits[i] != other.limits[i] || !strings[i].equals(other.strings[i])) {
				return (false);
			}
		}
		return (true);
	}
	else {
		return (false);
	}
}

public StringBuffer format(long num, StringBuffer buf, FieldPosition ign) {
	return (format((double)num, buf, ign));
}

public StringBuffer format(double num, StringBuffer buf, FieldPosition ign) {
        if (num < limits[0] || Double.isNaN(num)) {
		buf.append(strings[0]);
		return (buf);
	}

	int len = limits.length - 1;
	for (int i = 0; i < len; i++) {
		if (limits[i] <= num && num < limits[i+1]) {
			buf.append(strings[i]);
			return (buf);
		}
	}

	buf.append(strings[len]);
	return (buf);
}

public Object[] getFormats() {
	return (strings);
}

public double[] getLimits() {
	return (limits);
}

public int hashCode() {
	return (super.hashCode());
}

public final static double nextDouble(double d) {
        if(Double.isNaN(d))
	        return (d);
	else if(Double.POSITIVE_INFINITY == d)
	        return (d);
	else if(-0.0d == d)
	        return (Double.longBitsToDouble(0x1L));

	long l = Double.doubleToLongBits(d);
	if(d < 0.0d)
	        return(Double.longBitsToDouble(l-1));
	else
	        return(Double.longBitsToDouble(l+1));
}

public final static double previousDouble(double d) {
        if(Double.isNaN(d))
	        return (d);
	else if(Double.NEGATIVE_INFINITY == d)
	        return (d);
	else if(0.0d == d)
	        return (Double.longBitsToDouble(0x8000000000000001L));

	long l = Double.doubleToLongBits(d);
	if(d > 0.0d)
	        return(Double.longBitsToDouble(l-1));
	else
	        return(Double.longBitsToDouble(l+1));
}

public static double nextDouble(double d, boolean next) {
	if (next == true) {
		return (nextDouble(d));
	}
	else {
		return (previousDouble(d));
	}
}

public Number parse(String str, ParsePosition pos) {
        int startIndex = pos.getIndex();
	int stopIndex = startIndex+1;
	
	if(patternNames == null){
	        patternNames = new Hashtable();
		for(int i=0;i<limits.length;i++)
		    patternNames.put(strings[i],new Double(limits[i]));
	}

	Double number;
	while(stopIndex <= str.length()){
	    number = (Double)patternNames.get(str.substring(startIndex,stopIndex));
	    if(number == null)
		stopIndex++;
	    else {
		pos.setIndex(stopIndex);
		return (number);
	    }
	}
	return new Double(Double.NaN);
}

public void setChoices(double[] limits, String[] strings) {
	this.limits = limits;
	this.strings = strings;
}

public String toPattern() {
	if(limits.length == 0)
	    return ("");

        if(pattern == null){
	    StringBuffer str = new StringBuffer();
	    
	    for(int i=0;i<limits.length;i++){
		str.append(limits[i]+"#"+strings[i]);
		if(i < limits.length-1)
		    str.append("|");
		pattern = new String(str);
	    }
	}
	return (pattern);
}

}
