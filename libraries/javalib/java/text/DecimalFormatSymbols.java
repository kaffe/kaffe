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

import java.io.Serializable;
import java.util.ResourceBundle;
import java.util.Locale;

public class DecimalFormatSymbols implements Serializable, Cloneable {

char digit;
char patternSeperator;
char zeroDigit;
char groupSeperator;
char decimalSeperator;
char percentSign;
char permillSign;
char minusSign;
String infinity;
String nan;
char currencySign;

public DecimalFormatSymbols() {
	this(Locale.getDefault());
}

public DecimalFormatSymbols(Locale loc) {
	ResourceBundle bundle = Format.getResources("numberformat", loc);

	digit = bundle.getString("#").charAt(0);
	patternSeperator = bundle.getString(";").charAt(0);
	zeroDigit = bundle.getString("0").charAt(0);
	groupSeperator = bundle.getString(",").charAt(0);
	decimalSeperator = bundle.getString(".").charAt(0);
	percentSign = bundle.getString("%").charAt(0);
	permillSign = bundle.getString("\u2030").charAt(0);
	minusSign = bundle.getString("-").charAt(0);
	infinity = bundle.getString("\u221e");
	nan = bundle.getString("\ufffd");
	currencySign = bundle.getString("\u00a4").charAt(0);
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
	try {
		DecimalFormatSymbols other = (DecimalFormatSymbols)obj;
		if (digit == other.digit &&
		    patternSeperator == other.patternSeperator &&
		    zeroDigit == other.zeroDigit &&
		    groupSeperator == other.groupSeperator &&
		    decimalSeperator == other.decimalSeperator &&
		    percentSign == other.percentSign &&
		    permillSign == other.permillSign &&
		    minusSign == other.minusSign &&
		    infinity.equals(other.infinity) &&
		    nan.equals(other.nan)) {
			return (true);
		}
	}
	catch (ClassCastException _) {
	}
	return (false);
}

public char getDecimalSeparator() {
	return (decimalSeperator);
}

public char getDigit() {
	return (digit);
}

public char getGroupingSeperator() {
	return (groupSeperator);
}

public String getInfinity() {
	return (infinity);
}

public char getMinusSign() {
	return (minusSign);
}

public String getNan() {
	return (nan);
}

public char getPatternSeperator() {
	return (patternSeperator);
}

public char getPercent() {
	return (percentSign);
}

public char getPerMill() {
	return (permillSign);
}

public char getZeroDigit() {
	return (zeroDigit);
}

public int hashCode() {
	return (super.hashCode());
}

public void setDecimalSeparator(char val) {
	decimalSeperator = val;
}

public void setDigit(char val) {
	digit = val;
}

public void setGroupingSeperator(char val) {
	groupSeperator = val;
}

public void setInfinity(String val) {
	infinity = val;
}

public void setMinusSign(char val) {
	minusSign = val;
}

public void setNan(String val) {
	nan = val;
}

public void setPatternSeperator(char val) {
	patternSeperator = val;
}

public void setPercent(char val) {
	percentSign = val;
}

public void setPerMill(char val) {
	permillSign = val;
}

public void setZeroDigit(char val) {
	zeroDigit = val;
}

}
