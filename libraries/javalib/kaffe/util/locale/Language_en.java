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

public class Language_en extends ListResourceBundle {
    public Object [] [] getContents() {
	return contents;
    }

    private Object [] [] contents = {
	{ "en_CA", "English" },
	{ "fr_CA", "French" },
	{ "zh_CN", "Chinese" },
	{ "zh", "Chinese" },
	{ "en", "English" },
	{ "fr_FR", "French" },
	{ "fr", "French" },
	{ "de", "German" },
	{ "de_DE", "German" },
	{ "it", "Italian" },
	{ "it_IT", "Italian" },
	{ "ja_JP", "Japanese" },
	{ "ja", "Japanese" },
	{ "ko_KR", "Korean" },
	{ "ko", "Korean" },
	{ "zh_TW", "Chinese" },
	{ "en_GB", "English" },
	{ "en_US", "English" }
    };
}
