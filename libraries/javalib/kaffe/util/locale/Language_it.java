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
	{ "zh", "cinese" },
	{ "en", "inglese" },
	{ "fr", "francese" },
	{ "de", "tedesco" },
	{ "it", "italiano" },
	{ "ja", "giapponese" },
	{ "ko", "coreano" },
    };
}
