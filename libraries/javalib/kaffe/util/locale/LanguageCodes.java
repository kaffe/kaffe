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

/* This class assigns Alpha 3 letter codes to
   languages from ISO 639.
*/
public class LanguageCodes extends ListResourceBundle {
    public Object [] [] getContents() {
	return contents;
    }

    private Object [] [] contents = {
	{ "ab", "abk" }, // ABKHAZIAN
	{ "aa", "aar" }, // AFAR
	{ "af", "afr" }, // AFRIKAANS
	{ "sq", "sqi" }, // ALBANIAN
	{ "am", "amh" }, // AMHARIC
	{ "ar", "ara" }, // ARABIC
	{ "hy", "hye" }, // ARMENIAN
	{ "as", "asm" }, // ASSAMESE
	{ "ae", "ave" }, // AVESTAN
	{ "ay", "aym" }, // AYMARA
	{ "az", "aze" }, // AZERBAIJANI

	{ "ba", "bak" }, // BASHKIR
	{ "eu", "eus" }, // BASQUE
	{ "be", "bel" }, // BELARUSIAN
	{ "bn", "ben" }, // BENGALI
	{ "bh", "bih" }, // BIHARI
	{ "bi", "bis" }, // BISLAMA
	{ "nb", "nob" }, // NORWEGIAN BOKMAL
	{ "bs", "bos" }, // BOSNIAN
	{ "br", "bre" }, // BRETON
	{ "bg", "bul" }, // BULGARIAN
	{ "my", "mya" }, // BURMESE

	{ "es", "spa" }, // CASTILLIAN; SPANISH
	{ "ca", "cat" }, // CATALAN
	{ "ch", "cha" }, // CHAMORRO
	{ "ce", "che" }, // CHECHEN
	{ "ny", "nya" }, // CHEWA; CHICHEWA; NYANJA
	{ "zh", "zho" }, // CHINESE
	{ "za", "zha" }, // CHUANG; ZHUANG
	{ "cu", "chu" }, // CHURCH SLAVIC; SLAVONIC; CHURCH SLAVONIC; OLD BULGARIAN; OLD CHURCH SLAVONIC
	{ "cv", "chv" }, // CHUVASH
	{ "kw", "cor" }, // CORNISH
	{ "co", "cos" }, // CORSICAN
	{ "hr", "hrv" }, // CROATIAN
	{ "cs", "ces" }, // CZECH

	{ "da", "dan" }, // DANISH
	{ "nl", "nld" }, // DUTCH
	{ "dz", "dzo" }, // DZONGKHA

	{ "en", "eng" }, // ENGLISH
	{ "eo", "epo" }, // ESPERANTO
	{ "et", "est" }, // ESTONIAN

	{ "fo", "fao" }, // FAROESE
	{ "fj", "fij" }, // FIJIAN
	{ "fi", "fin" }, // FINNISH
	{ "fr", "fra" }, // FRENCH
	{ "fy", "fry" }, // FRISIAN

	{ "gd", "gla" }, // GAELIC; SCOTTISH GAELIC
	{ "gl", "glg" }, // GALLEGAN
	{ "ka", "kat" }, // GEORGIAN
	{ "de", "deu" }, // GERMAN
	{ "ki", "kik" }, // GIKUYU; KIKUYU
	{ "el", "ell" }, // MODERN GREEK
	{ "gn", "grn" }, // GUARANI
	{ "gu", "guj" }, // GUJARATI

	{ "ha", "hau" }, // HAUSA
	{ "he", "heb" }, // HEBREW
	{ "iw", "heb" }, // HEBREW (WITHDRAWN)
	{ "hz", "her" }, // HERERO
	{ "hi", "hin" }, // HINDI
	{ "ho", "hmo" }, // HIRI MOTU
	{ "hu", "hun" }, // HUNGARIAN

	{ "is", "isl" }, // ICELANDIC
	{ "io", "iso" }, // IDO
	{ "id", "ind" }, // INDONESIAN
	{ "in", "ind" }, // INDONESIAN (WITHDRAWN)
	{ "ia", "ina" }, // INTERLINGUA
	{ "ie", "ile" }, // INTERLINGUE
	{ "iu", "iku" }, // INUKTITUT
	{ "ik", "ipk" }, // INUPIAQ
	{ "ga", "gle" }, // IRISH
	{ "it", "ita" }, // ITALIAN

	{ "ja", "jpn" }, // JAPANESE
	{ "jv", "jav" }, // JAVANESE

	{ "kl", "kal" }, // KALAALLISUT
	{ "kn", "kan" }, // KANNADA
	{ "ks", "kas" }, // KASHMIRI
	{ "kk", "kaz" }, // KAZAKH
	{ "km", "khm" }, // KHMER
	{ "rw", "kin" }, // KINYARWANDA
	{ "ky", "kir" }, // KIRGHIZ
	{ "kv", "kom" }, // KOMI
	{ "ko", "kor" }, // KOREAN
	{ "kj", "kua" }, // KUANYAMA; KWANYAMA
	{ "ku", "kur" }, // KURDISH

	{ "lo", "lao" }, // LAO
	{ "la", "lat" }, // LATIN
	{ "lv", "lav" }, // LATVIAN
	{ "lb", "ltz" }, // LETZEBURGESCH; LUXEMBOURGISH
	{ "li", "lim" }, // LIMBURGAN; LIMBURGER; LIMBURGISH
	{ "ln", "lin" }, // LINGALA
	{ "lt", "lit" }, // LITHUANIAN

	{ "mk", "mkd" }, // MACEDONIAN
	{ "mg", "mlg" }, // MALAGASY
	{ "ms", "msa" }, // MALAY
	{ "ml", "mal" }, // MALAYALAM
	{ "mt", "mlt" }, // MALTESE
	{ "gv", "glv" }, // MANX
	{ "mi", "mri" }, // MAORI
	{ "mr", "mar" }, // MARATHI
	{ "mh", "mah" }, // MARSHALLESE
	{ "mo", "mol" }, // MOLDAVIAN
	{ "mn", "mon" }, // MONGOLIAN

	{ "na", "nau" }, // NAURU
	{ "nv", "nav" }, // NAVAHO; NAVAJO
	{ "nd", "nde" }, // NORTH NDEBELE
	{ "nr", "nbl" }, // SOUTH NDEBELE
	{ "ng", "ndo" }, // NDONGA
	{ "ne", "nep" }, // NEPALI
	{ "se", "sme" }, // NORTHERN SAMI
	{ "no", "nor" }, // NORWEGIAN
	{ "nn", "nno" }, // NORWEGIAN NYNORSK

	{ "oc", "oci" }, // OCCITAN; PROVENCAL
	{ "or", "ori" }, // ORIYA
	{ "om", "orm" }, // OROMO
	{ "os", "oss" }, // OSSETIAN; OSSETIC

	{ "pi", "pli" }, // PALI
	{ "pa", "pan" }, // PANJABI
	{ "fa", "fas" }, // PERSIAN
	{ "pl", "pol" }, // POLISH
	{ "pt", "por" }, // PORTUGUESE
	{ "ps", "pus" }, // PUSHTO

	{ "qu", "que" }, // QUECHUA

	{ "rm", "roh" }, // RAETO-ROMANCE
	{ "ro", "ron" }, // ROMANIAN
	{ "rn", "run" }, // RUNDI
	{ "ru", "rus" }, // RUSSIAN

	{ "sm", "smo" }, // SAMOAN
	{ "sg", "sag" }, // SANGO
	{ "sa", "san" }, // SANSKRIT
	{ "sc", "srd" }, // SARDINIAN
	{ "sr", "srp" }, // SERBIAN
	{ "sh", "srh" }, // SERBO-CROATIAN (WITHDRAWN)
	{ "sn", "sna" }, // SHONA
	{ "ii", "iii" }, // SICHUAN YI
	{ "sd", "snd" }, // SINDHI
	{ "si", "sin" }, // SINHALESE
	{ "sk", "slk" }, // SLOVAK
	{ "sl", "slv" }, // SLOVENIAN
	{ "so", "som" }, // SOMALI
	{ "st", "sot" }, // SOUTHERN SOTHO
	{ "su", "sun" }, // SUNDANESE
	{ "sw", "swa" }, // SWAHILI
	{ "ss", "ssw" }, // SWATI
	{ "sv", "swe" }, // SWEDISH

	{ "tl", "tgl" }, // TAGALOG
	{ "ty", "tah" }, // TAHITIAN
	{ "tg", "tgk" }, // TAJIK
	{ "ta", "tam" }, // TAMIL
	{ "tt", "tat" }, // TATAR
	{ "te", "tel" }, // TELUGU
	{ "th", "tha" }, // THAI
	{ "bo", "bod" }, // TIBETAN
	{ "ti", "tir" }, // TIGRINYA
	{ "to", "ton" }, // TONGA (TONGA ISLANDS)
	{ "ts", "tso" }, // TSONGA
	{ "tn", "tsn" }, // TSWANA
	{ "tr", "tur" }, // TURKISH
	{ "tk", "tuk" }, // TURKMEN
	{ "tw", "twi" }, // TWI

	{ "ug", "uig" }, // UIGHUR
	{ "uk", "ukr" }, // UKRAINIAN
	{ "ur", "urd" }, // URDU
	{ "uz", "uzb" }, // UZBEK

	{ "vi", "vie" }, // VIETNAMESE
	{ "vo", "vol" }, // VOLAPUK

	{ "wa", "wln" }, // WALLOON
	{ "cy", "cym" }, // WELSH
	{ "wo", "wol" }, // WOLOF

	{ "xh", "xho" }, // XHOSA

	{ "yi", "yid" }, // YIDDISH
	{ "ji", "yid" }, // YIDDISH (WITHDRAWN)
	{ "yo", "yor" }, // YORUBA

	{ "zu", "zul" }, // ZULU
    };
}
