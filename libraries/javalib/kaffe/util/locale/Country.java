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

public class Country extends ListResourceBundle {
    public Object [] [] getContents() {
	return contents;
    }

    private Object [] [] contents = {
	{ "en_CA", "" },
	{ "fr_CA", "" },
	{ "zh_CN", "" },
	{ "zh", "" },
	{ "en", "" },
	{ "fr_FR", "" },
	{ "fr", "" },
	{ "de", "" },
	{ "de_DE", "" },
	{ "it", "" },
	{ "it_IT", "" },
	{ "ja_JP", "" },
	{ "ja", "" },
	{ "ko_KR", "" },
	{ "ko", "" },
	{ "zh_TW", "" },
	{ "en_GB", "" },
	{ "en_US", "" }
    };
}
