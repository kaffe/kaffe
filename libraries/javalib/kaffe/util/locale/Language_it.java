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

public class Language_it extends ListResourceBundle {
    public Object [] [] getContents() {
	return contents;
    }

    private Object [] [] contents = {
	{ "en_CA", "inglese" },
	{ "fr_CA", "francese" },
	{ "zh_CN", "cinese" },
	{ "zh", "cinese" },
	{ "en", "inglese" },
	{ "fr_FR", "francese" },
	{ "fr", "francese" },
	{ "de", "tedesco" },
	{ "de_DE", "tedesco" },
	{ "it", "italiano" },
	{ "it_IT", "italiano" },
	{ "ja_JP", "giapponese" },
	{ "ja", "giapponese" },
	{ "ko_KR", "coreano" },
	{ "ko", "coreano" },
	{ "zh_TW", "cinese" },
	{ "en_GB", "inglese" },
	{ "en_US", "inglese" }
    };
}
