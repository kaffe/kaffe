/*
 * Java core library component.
 *
 * Copyright (c) 2002
 *      Dalibor Topic <robilad@yahoo.com>.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.util.locale;

import java.util.ListResourceBundle;

public class Country_en extends ListResourceBundle {
    public Object [] [] getContents() {
	return contents;
    }

    private Object [] [] contents = {
	{ "en_CA", "Canada" },
	{ "fr_CA", "Canada" },
	{ "zh_CN", "China" },
	{ "fr_FR", "France" },
	{ "de_DE", "Germany" },
	{ "it_IT", "Italy" },
	{ "ja_JP", "Japan" },
	{ "ko_KR", "South Korea" },
	{ "zh_TW", "Taiwan" },
	{ "en_GB", "United Kingdom" },
	{ "en_US", "United States" }
    };
}
