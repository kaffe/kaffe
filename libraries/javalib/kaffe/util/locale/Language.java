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

/* This is the base class for language names.
   The names are taken from ISO 639 standard.

   Only languages that have a two letter code
   are included in the list. This leaves out
   many special use languages, but that's the
   way Sun designed the API.

   The laguages are ordered by their English name.
   Withdrawn laguages remain listed, as that's what
   Sun does, according to Java Class Libraries 2nd Ed.
   Vol. 1 Supplement documentation for Locale.
 
   If you want to provide a translation of 
   language names for another locale, please take
   a look at Language_en.java.
*/

public class Language extends ListResourceBundle {
    public Object [] [] getContents() {
	return contents;
    }

    private Object [] [] contents = {
	{ "ab", "" }, // ABKHAZIAN
	{ "aa", "" }, // AFAR
	{ "af", "" }, // AFRIKAANS
	{ "sq", "" }, // ALBANIAN
	{ "am", "" }, // AMHARIC
	{ "ar", "" }, // ARABIC
	{ "hy", "" }, // ARMENIAN
	{ "as", "" }, // ASSAMESE
	{ "ae", "" }, // AVESTAN
	{ "ay", "" }, // AYMARA
	{ "az", "" }, // AZERBAIJANI

	{ "ba", "" }, // BASHKIR
	{ "eu", "" }, // BASQUE
	{ "be", "" }, // BELARUSIAN
	{ "bn", "" }, // BENGALI
	{ "bh", "" }, // BIHARI
	{ "bi", "" }, // BISLAMA
	{ "nb", "" }, // NORWEGIAN BOKMAL
	{ "bs", "" }, // BOSNIAN
	{ "br", "" }, // BRETON
	{ "bg", "" }, // BULGARIAN
	{ "my", "" }, // BURMESE

	{ "es", "" }, // CASTILLIAN; SPANISH
	{ "ca", "" }, // CATALAN
	{ "ch", "" }, // CHAMORRO
	{ "ce", "" }, // CHECHEN
	{ "ny", "" }, // CHEWA; CHICHEWA; NYANJA
	{ "zh", "" }, // CHINESE
	{ "za", "" }, // CHUANG; ZHUANG
	{ "cu", "" }, // CHURCH SLAVIC; SLAVONIC; CHURCH SLAVONIC; OLD BULGARIAN; OLD CHURCH SLAVONIC
	{ "cv", "" }, // CHUVASH
	{ "kw", "" }, // CORNISH
	{ "co", "" }, // CORSICAN
	{ "hr", "" }, // CROATIAN
	{ "cs", "" }, // CZECH

	{ "da", "" }, // DANISH
	{ "nl", "" }, // DUTCH
	{ "dz", "" }, // DZONGKHA

	{ "en", "" }, // ENGLISH
	{ "eo", "" }, // ESPERANTO
	{ "et", "" }, // ESTONIAN

	{ "fo", "" }, // FAROESE
	{ "fj", "" }, // FIJIAN
	{ "fi", "" }, // FINNISH
	{ "fr", "" }, // FRENCH
	{ "fy", "" }, // FRISIAN

	{ "gd", "" }, // GAELIC; SCOTTISH GAELIC
	{ "gl", "" }, // GALLEGAN
	{ "ka", "" }, // GEORGIAN
	{ "de", "" }, // GERMAN
	{ "ki", "" }, // GIKUYU; KIKUYU
	{ "el", "" }, // MODERN GREEK
	{ "gn", "" }, // GUARANI
	{ "gu", "" }, // GUJARATI

	{ "ha", "" }, // HAUSA
	{ "he", "" }, // HEBREW
	{ "iw", "" }, // HEBREW (WITHDRAWN)
	{ "hz", "" }, // HERERO
	{ "hi", "" }, // HINDI
	{ "ho", "" }, // HIRI MOTU
	{ "hu", "" }, // HUNGARIAN

	{ "is", "" }, // ICELANDIC
	{ "io", "" }, // IDO
	{ "id", "" }, // INDONESIAN
	{ "in", "" }, // INDONESIAN (WITHDRAWN)
	{ "ia", "" }, // INTERLINGUA
	{ "ie", "" }, // INTERLINGUE
	{ "iu", "" }, // INUKTITUT
	{ "ik", "" }, // INUPIAQ
	{ "ga", "" }, // IRISH
	{ "it", "" }, // ITALIAN

	{ "ja", "" }, // JAPANESE
	{ "jv", "" }, // JAVANESE

	{ "kl", "" }, // KALAALLISUT
	{ "kn", "" }, // KANNADA
	{ "ks", "" }, // KASHMIRI
	{ "kk", "" }, // KAZAKH
	{ "km", "" }, // KHMER
	{ "rw", "" }, // KINYARWANDA
	{ "ky", "" }, // KIRGHIZ
	{ "kv", "" }, // KOMI
	{ "ko", "" }, // KOREAN
	{ "kj", "" }, // KUANYAMA; KWANYAMA
	{ "ku", "" }, // KURDISH

	{ "lo", "" }, // LAO
	{ "la", "" }, // LATIN
	{ "lv", "" }, // LATVIAN
	{ "lb", "" }, // LETZEBURGESCH; LUXEMBOURGISH
	{ "li", "" }, // LIMBURGAN; LIMBURGER; LIMBURGISH
	{ "ln", "" }, // LINGALA
	{ "lt", "" }, // LITHUANIAN

	{ "mk", "" }, // MACEDONIAN
	{ "mg", "" }, // MALAGASY
	{ "ms", "" }, // MALAY
	{ "ml", "" }, // MALAYALAM
	{ "mt", "" }, // MALTESE
	{ "gv", "" }, // MANX
	{ "mi", "" }, // MAORI
	{ "mr", "" }, // MARATHI
	{ "mh", "" }, // MARSHALLESE
	{ "mo", "" }, // MOLDAVIAN
	{ "mn", "" }, // MONGOLIAN

	{ "na", "" }, // NAURU
	{ "nv", "" }, // NAVAHO; NAVAJO
	{ "nd", "" }, // NORTH NDEBELE
	{ "nr", "" }, // SOUTH NDEBELE
	{ "ng", "" }, // NDONGA
	{ "ne", "" }, // NEPALI
	{ "se", "" }, // NORTHERN SAMI
	{ "no", "" }, // NORWEGIAN
	{ "nn", "" }, // NORWEGIAN NYNORSK

	{ "oc", "" }, // OCCITAN; PROVENCAL
	{ "or", "" }, // ORIYA
	{ "om", "" }, // OROMO
	{ "os", "" }, // OSSETIAN; OSSETIC

	{ "pi", "" }, // PALI
	{ "pa", "" }, // PANJABI
	{ "fa", "" }, // PERSIAN
	{ "pl", "" }, // POLISH
	{ "pt", "" }, // PORTUGUESE
	{ "ps", "" }, // PUSHTO

	{ "qu", "" }, // QUECHUA
	{ "rm", "" }, // RAETO-ROMANCE
	{ "ro", "" }, // ROMANIAN
	{ "rn", "" }, // RUNDI
	{ "ru", "" }, // RUSSIAN

	{ "sm", "" }, // SAMOAN
	{ "sg", "" }, // SANGO
	{ "sa", "" }, // SANSKRIT
	{ "sc", "" }, // SARDINIAN
	{ "sr", "" }, // SERBIAN
	{ "sh", "" }, // SERBO-CROATIAN (WITHDRAWN)
	{ "sn", "" }, // SHONA
	{ "ii", "" }, // SICHUAN YI
	{ "sd", "" }, // SINDHI
	{ "si", "" }, // SINHALESE
	{ "sk", "" }, // SLOVAK
	{ "sl", "" }, // SLOVENIAN
	{ "so", "" }, // SOMALI
	{ "st", "" }, // SOUTHERN SOTHO
	{ "su", "" }, // SUDANESE
	{ "sw", "" }, // SWAHILI
	{ "ss", "" }, // SWATI
	{ "sv", "" }, // SWEDISH

	{ "tl", "" }, // TAGALOG
	{ "ty", "" }, // TAHITIAN
	{ "tg", "" }, // TAJIK
	{ "ta", "" }, // TAMIL
	{ "tt", "" }, // TATAR
	{ "te", "" }, // TELUGU
	{ "th", "" }, // THAI
	{ "bo", "" }, // TIBETAN
	{ "ti", "" }, // TIGRINYA
	{ "to", "" }, // TONGA (TONGA ISLANDS)
	{ "ts", "" }, // TSONGA
	{ "tn", "" }, // TSWANA
	{ "tr", "" }, // TURKISH
	{ "tk", "" }, // TURKMEN
	{ "tw", "" }, // TWI

	{ "ug", "" }, // UIGHUR
	{ "uk", "" }, // UKRAINIAN
	{ "ur", "" }, // URDU
	{ "uz", "" }, // UZBEK

	{ "vi", "" }, // VIETNAMESE
	{ "vo", "" }, // VOLAPUK

	{ "wa", "" }, // WALLOON
	{ "cy", "" }, // WELSH
	{ "wo", "" }, // WOLOF

	{ "xh", "" }, // XHOSA

	{ "yi", "" }, // YIDDISH
	{ "ji", "" }, // YIDDISH (WITHDRAWN)
	{ "yo", "" }, // YORUBA

	{ "zu", "" }, // ZULU
    };
}
