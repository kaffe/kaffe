package java.text;

import java.io.Serializable;
import java.util.Locale;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
/* NB: BreakIterator is serializable only in JDK 1.1, not in JDK 1.2 */
abstract public class BreakIterator
  implements Cloneable, Serializable
{
	public static final int DONE = -1;

protected BreakIterator () {
}

public Object clone () {
	try {
		return (super.clone());
	}
	catch ( CloneNotSupportedException _x) {
		_x.printStackTrace();
		return (null);
	}
}

abstract public int current();

abstract public int first();

abstract public int following( int offs);

public static synchronized Locale[] getAvailableLocales () {
	return null;
}

public static BreakIterator getCharacterInstance () {
	return (getCharacterInstance( Locale.getDefault() ));
}

public static BreakIterator getCharacterInstance ( Locale loc) {
	return ( null);
}

public static BreakIterator getLineInstance () {
	return (getLineInstance( Locale.getDefault() ));
}

public static BreakIterator getLineInstance ( Locale loc) {
	return ( new SimpleLineIterator() );
}

public static BreakIterator getSentenceInstance () {
	return (getSentenceInstance( Locale.getDefault() ));
}

public static BreakIterator getSentenceInstance ( Locale loc) {
	return ( new SimpleSentenceIterator() );
}

abstract public CharacterIterator getText();

public static BreakIterator getWordInstance() {
	return (getWordInstance( Locale.getDefault() ));
}

public static BreakIterator getWordInstance( Locale loc) {
	return ( new SimpleWordIterator() );
}

abstract public int last();

abstract public int next();

abstract public int next( int num);

abstract public int previous();

abstract public void setText( CharacterIterator ci);

public void setText( String txt) {
	setText ( new StringCharacterIterator( txt) );
}
}
