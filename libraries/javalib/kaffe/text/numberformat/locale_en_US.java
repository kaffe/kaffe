/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */


package kaffe.text.numberformat;

import java.util.ListResourceBundle;

public class locale_en_US extends ListResourceBundle {

private Object[][] contents = {

	{ "#",				"#" },
	{ ";",				";" },
	{ "0",				"0" },
	{ ",",				"," },
	{ ".",				"." },
	{ "%",				"%" },
	{ "\u2030",			"%" },
	{ "-",				"-" },
	{ "\u221e",			"Infinity" },
	{ "\ufffd",			"NaN" },
	{ "\u00a4",			"$" },
};

public Object[][] getContents() {
	return (contents);
}

}
