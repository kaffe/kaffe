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

public class Country_it extends ListResourceBundle {
    public Object [] [] getContents() {
	return contents;
    }

    private Object [] [] contents = {
	{ "en_CA", "Canada" },
	{ "fr_CA", "Canada" },
	{ "zh_CN", "Cina" },
	{ "fr_FR", "Francia" },
	{ "de_DE", "Germania" },
	{ "it_IT", "Italia" },
	{ "ja_JP", "Giappone" },
	{ "ko_KR", "Corea del Sud" },
	{ "zh_TW", "Taiwan" },
	{ "en_GB", "Regno Unito" },
	{ "en_US", "Stati Uniti" }
    };
}
