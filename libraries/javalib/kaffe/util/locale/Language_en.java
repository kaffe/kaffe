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

/* This class assigns English names to languages
   from ISO 639.

   When there is more than one English name for a
   language, I picked one that seemed more familiar
   to me.
*/
public class Language_en extends ListResourceBundle {
    public Object [] [] getContents() {
	return contents;
    }

    private Object [] [] contents = {
	{ "ab", "Abkhazian" }, // ABKHAZIAN
	{ "aa", "Afar" }, // AFAR
	{ "af", "Afrikaans" }, // AFRIKAANS
	{ "sq", "Albanian" }, // ALBANIAN
	{ "am", "Amharic" }, // AMHARIC
	{ "ar", "Arabic" }, // ARABIC
	{ "hy", "Armenian" }, // ARMENIAN
	{ "as", "Assamese" }, // ASSAMESE
	{ "ae", "Avestan" }, // AVESTAN
	{ "ay", "Aymara" }, // AYMARA
	{ "az", "Azerbaijani" }, // AZERBAIJANI

	{ "ba", "Bashkir" }, // BASHKIR
	{ "eu", "Basque" }, // BASQUE
	{ "be", "Belarusian" }, // BELARUSIAN
	{ "bn", "Bengali" }, // BENGALI
	{ "bh", "Bihari" }, // BIHARI
	{ "bi", "Bislama" }, // BISLAMA
	{ "nb", "Norwegian Bokm\u00E5l" }, // NORWEGIAN BOKMAL
	{ "bs", "Bosnian" }, // BOSNIAN
	{ "br", "Breton" }, // BRETON
	{ "bg", "Bulgarian" }, // BULGARIAN
	{ "my", "Burmese" }, // BURMESE

	{ "es", "Spanish" }, // CASTILLIAN; SPANISH
	{ "ca", "Catalan" }, // CATALAN
	{ "ch", "Chamorro" }, // CHAMORRO
	{ "ce", "Chechen" }, // CHECHEN
	{ "ny", "Nyanja" }, // CHEWA; CHICHEWA; NYANJA
	{ "zh", "Chinese" }, // CHINESE
	{ "za", "Zhuang" }, // CHUANG; ZHUANG
	{ "cu", "Church Slavic" }, // CHURCH SLAVIC; SLAVONIC; CHURCH SLAVONIC; OLD BULGARIAN; OLD CHURCH SLAVONIC
	{ "cv", "Chuvash" }, // CHUVASH
	{ "kw", "Cornish" }, // CORNISH
	{ "co", "Corsican" }, // CORSICAN
	{ "hr", "Croatian" }, // CROATIAN
	{ "cs", "Czech" }, // CZECH

	{ "da", "Danish" }, // DANISH
	{ "nl", "Dutch" }, // DUTCH
	{ "dz", "Dzongkha" }, // DZONGKHA

	{ "en", "English" }, // ENGLISH
	{ "eo", "Esperanto" }, // ESPERANTO
	{ "et", "Estonian" }, // ESTONIAN

	{ "fo", "Faroese" }, // FAROESE
	{ "fj", "Fijian" }, // FIJIAN
	{ "fi", "Finnish" }, // FINNISH
	{ "fr", "French" }, // FRENCH
	{ "fy", "Frisian" }, // FRISIAN

	{ "gd", "Gaelic" }, // GAELIC; SCOTTISH GAELIC
	{ "gl", "Gallegan" }, // GALLEGAN
	{ "ka", "Georgian" }, // GEORGIAN
	{ "de", "German" }, // GERMAN
	{ "ki", "Kikuyu" }, // GIKUYU; KIKUYU
	{ "el", "Modern Greek" }, // MODERN GREEK
	{ "gn", "Guarani" }, // GUARANI
	{ "gu", "Gujarati" }, // GUJARATI

	{ "ha", "Hausa" }, // HAUSA
	{ "he", "Hebrew" }, // HEBREW
	{ "iw", "Hebrew" }, // HEBREW (WITHDRAWN)
	{ "hz", "Herero" }, // HERERO
	{ "hi", "Hindi" }, // HINDI
	{ "ho", "Hiri Motu" }, // HIRI MOTU
	{ "hu", "Hungarian" }, // HUNGARIAN

	{ "is", "Icelandic" }, // ICELANDIC
	{ "io", "Ido" }, // IDO
	{ "id", "Indonesian" }, // INDONESIAN
	{ "in", "Indonesian" }, // INDONESIAN (WITHDRAWN)
	{ "ia", "Interlingua" }, // INTERLINGUA
	{ "ie", "Interlingue" }, // INTERLINGUE
	{ "iu", "Inuktitut" }, // INUKTITUT
	{ "ik", "Inupiaq" }, // INUPIAQ
	{ "ga", "Irish" }, // IRISH
	{ "it", "Italian" }, // ITALIAN

	{ "ja", "Japanese" }, // JAPANESE
	{ "jv", "Javanese" }, // JAVANESE

	{ "kl", "Kalaallisut" }, // KALAALLISUT
	{ "kn", "Kannada" }, // KANNADA
	{ "ks", "Kashmiri" }, // KASHMIRI
	{ "kk", "Kazakh" }, // KAZAKH
	{ "km", "Khmer" }, // KHMER
	{ "rw", "Kinyarwanda" }, // KINYARWANDA
	{ "ky", "Kirghiz" }, // KIRGHIZ
	{ "kv", "Komi" }, // KOMI
	{ "ko", "Korean" }, // KOREAN
	{ "kj", "Kuanyama" }, // KUANYAMA; KWANYAMA
	{ "ku", "Kurdish" }, // KURDISH

	{ "lo", "Lao" }, // LAO
	{ "la", "Latin" }, // LATIN
	{ "lv", "Latvian" }, // LATVIAN
	{ "lb", "Letzeburgesh" }, // LETZEBURGESCH; LUXEMBOURGISH
	{ "li", "Limburgish" }, // LIMBURGAN; LIMBURGER; LIMBURGISH
	{ "ln", "Lingala" }, // LINGALA
	{ "lt", "Lithuanian" }, // LITHUANIAN

	{ "mk", "Macedonian" }, // MACEDONIAN
	{ "mg", "Malagasy" }, // MALAGASY
	{ "ms", "Malay" }, // MALAY
	{ "ml", "Malayalam" }, // MALAYALAM
	{ "mt", "Maltese" }, // MALTESE
	{ "gv", "Manx" }, // MANX
	{ "mi", "Maori" }, // MAORI
	{ "mr", "Marathi" }, // MARATHI
	{ "mh", "Marshallese" }, // MARSHALLESE
	{ "mo", "Moldavian" }, // MOLDAVIAN
	{ "mn", "Mongolian" }, // MONGOLIAN

	{ "na", "Nauru" }, // NAURU
	{ "nv", "Navajo" }, // NAVAHO; NAVAJO
	{ "nd", "North Debele" }, // NORTH NDEBELE
	{ "nr", "South Debele" }, // SOUTH NDEBELE
	{ "ng", "Ndonga" }, // NDONGA
	{ "ne", "Nepali" }, // NEPALI
	{ "se", "Northern Sami" }, // NORTHERN SAMI
	{ "no", "Norwegian" }, // NORWEGIAN
	{ "nn", "Norwegian Nynorsk" }, // NORWEGIAN NYNORSK

	{ "oc", "Occitan" }, // OCCITAN; PROVENCAL
	{ "or", "Oriya" }, // ORIYA
	{ "om", "Oromo" }, // OROMO
	{ "os", "Ossetian" }, // OSSETIAN; OSSETIC

	{ "pi", "Pali" }, // PALI
	{ "pa", "Panjabi" }, // PANJABI
	{ "fa", "Persian" }, // PERSIAN
	{ "pl", "Polish" }, // POLISH
	{ "pt", "Portuguese" }, // PORTUGUESE
	{ "ps", "Pushto" }, // PUSHTO

	{ "qu", "Quechua" }, // QUECHUA

	{ "rm", "Raeto-Romance" }, // RAETO-ROMANCE
	{ "ro", "Romanian" }, // ROMANIAN
	{ "rn", "Rundi" }, // RUNDI
	{ "ru", "Russian" }, // RUSSIAN

	{ "sm", "Samoan" }, // SAMOAN
	{ "sg", "Sango" }, // SANGO
	{ "sa", "Sanskrit" }, // SANSKRIT
	{ "sc", "Sardinian" }, // SARDINIAN
	{ "sr", "Serbian" }, // SERBIAN
	{ "sh", "Serbo-Croatian" }, // SERBO-CROATIAN (WITHDRAWN)
	{ "sn", "Shona" }, // SHONA
	{ "ii", "Sichuan Yi" }, // SICHUAN YI
	{ "sd", "Sindhi" }, // SINDHI
	{ "si", "Sinhalese" }, // SINHALESE
	{ "sk", "Slovak" }, // SLOVAK
	{ "sl", "Slovenian" }, // SLOVENIAN
	{ "so", "Somali" }, // SOMALI
	{ "st", "Southern Sotho" }, // SOUTHERN SOTHO
	{ "su", "Sundanese" }, // SUNDANESE
	{ "sw", "Swahili" }, // SWAHILI
	{ "ss", "Swati" }, // SWATI
	{ "sv", "Swedish" }, // SWEDISH

	{ "tl", "Tagalog" }, // TAGALOG
	{ "ty", "Tahitian" }, // TAHITIAN
	{ "tg", "Tajik" }, // TAJIK
	{ "ta", "Tamil" }, // TAMIL
	{ "tt", "Tatar" }, // TATAR
	{ "te", "Telugu" }, // TELUGU
	{ "th", "Thai" }, // THAI
	{ "bo", "Tibetan" }, // TIBETAN
	{ "ti", "Tigrinya" }, // TIGRINYA
	{ "to", "Tonga (Tonga Islands)" }, // TONGA (TONGA ISLANDS)
	{ "ts", "Tsonga" }, // TSONGA
	{ "tn", "Tswana" }, // TSWANA
	{ "tr", "Turkish" }, // TURKISH
	{ "tk", "Turkmen" }, // TURKMEN
	{ "tw", "Twi" }, // TWI

	{ "ug", "Uighur" }, // UIGHUR
	{ "uk", "Ukrainian" }, // UKRAINIAN
	{ "ur", "Urdu" }, // URDU
	{ "uz", "Uzbek" }, // UZBEK

	{ "vi", "Vietnamese" }, // VIETNAMESE
	{ "vo", "Volap\u00FCk" }, // VOLAPUK

	{ "wa", "Walloon" }, // WALLOON
	{ "cy", "Welsh" }, // WELSH
	{ "wo", "Wolof" }, // WOLOF

	{ "xh", "Xhosa" }, // XHOSA

	{ "yi", "Yiddish" }, // YIDDISH
	{ "ji", "Yiddish" }, // YIDDISH (WITHDRAWN)
	{ "yo", "Yoruba" }, // YORUBA

	{ "zu", "Zulu" }, // ZULU
    };
}
