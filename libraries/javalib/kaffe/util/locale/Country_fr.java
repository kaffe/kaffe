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

public class Country_fr extends ListResourceBundle {
    public Object [] [] getContents() {
	return contents;
    }

    private Object [] [] contents = {
	{ "en_CA", "Canada" },
	{ "fr_CA", "Canada" },
	{ "zh_CN", "Chine" },
	{ "fr_FR", "France" },
	{ "de_DE", "Allemagne" },
	{ "it_IT", "Italie" },
	{ "ja_JP", "Japon" },
	{ "ko_KR", "Corée du Sud" },
	{ "zh_TW", "Taiwan" },
	{ "en_GB", "Royaume-Uni" },
	{ "en_US", "Etats-Unis" }
    };
}
