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

public class Language_fr extends ListResourceBundle {
    public Object [] [] getContents() {
	return contents;
    }

    private Object [] [] contents = {
	{ "en_CA", "anglais" },
	{ "fr_CA", "français" },
	{ "zh_CN", "chinois" },
	{ "zh", "chinois" },
	{ "en", "anglais" },
	{ "fr_FR", "français" },
	{ "fr", "français" },
	{ "de", "allemand" },
	{ "de_DE", "allemand" },
	{ "it", "italien" },
	{ "it_IT", "italien" },
	{ "ja_JP", "japonais" },
	{ "ja", "japonais" },
	{ "ko_KR", "coréen" },
	{ "ko", "coréen" },
	{ "zh_TW", "chinois" },
	{ "en_GB", "anglais" },
	{ "en_US", "anglais" }
    };
}
