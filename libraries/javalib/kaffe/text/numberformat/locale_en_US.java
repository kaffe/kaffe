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
