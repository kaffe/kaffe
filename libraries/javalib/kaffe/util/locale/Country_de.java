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

public class Country_de extends ListResourceBundle {
    public Object [] [] getContents() {
	return contents;
    }

    private Object [] [] contents = {
	{ "en_CA", "Kanada" },
	{ "fr_CA", "Kanada" },
	{ "zh_CN", "China" },
	{ "fr_FR", "Frankreich" },
	{ "de_DE", "Deutschland" },
	{ "it_IT", "Italien" },
	{ "ja_JP", "Japan" },
	{ "ko_KR", "Südkorea" },
	{ "zh_TW", "Taiwan" },
	{ "en_GB", "Vereinigtes Königreich" },
	{ "en_US", "Vereinigte Staaten von Amerika" }
    };
}
