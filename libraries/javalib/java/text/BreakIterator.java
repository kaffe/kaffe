/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.text;

import java.lang.String;
import java.io.Serializable;
import java.util.Locale;

abstract public class BreakIterator
  implements Cloneable, Serializable
{
	public static char DONE = CharacterIterator.DONE;

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
	return null;
}

public static BreakIterator getLineInstance () {
	return (getLineInstance( Locale.getDefault() ));
}

public static BreakIterator getLineInstance ( Locale loc) {
	return ( new LineIterator() );
}

public static BreakIterator getSentenceInstance () {
	return (getSentenceInstance( Locale.getDefault() ));
}

public static BreakIterator getSentenceInstance ( Locale loc) {
	return ( new SentenceIterator() );
}

abstract public CharacterIterator getText();

public static BreakIterator getWordInstance() {
	return (getWordInstance( Locale.getDefault() ));
}

public static BreakIterator getWordInstance( Locale loc) {
	return ( new WordIterator() );
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

class LineIterator
  extends BreakIterator
{
	CharacterIterator iterator = new StringCharacterIterator("");

public int current() {
	return (iterator.getIndex());
}

public int first() {
	iterator.first();
	return (iterator.getIndex());		
}

public int following( int offs) {
	int pos = iterator.getIndex();
	iterator.setIndex( iterator.getBeginIndex() + offs);
	if ( iterator.current() == DONE ) {
		iterator.setIndex( pos);
		return DONE;
	}
	return (next());		
}

public CharacterIterator getText() {
	return iterator;
}

public int last() {
	iterator.last();
	return (previous());		
}

public int next() {
	int pos = iterator.getIndex();
	boolean nl = false;
	
	for ( char c = iterator.next();;c = iterator.next() ) {
		if ( c == DONE )
			break;
		if ( Character.getType( c) == Character.LINE_SEPARATOR )
			nl = true;
		else if ( nl )
			return (iterator.getIndex());
	}
			
	iterator.setIndex( pos);
	return DONE;
}

public int next( int num) {
	int max = (num > 0) ? num : -num;
	
	for ( int idx=0; idx<max; idx++) {
		if ( DONE == ((num > 0) ? next() : previous()) )
			return (DONE);
	}
	
	return (iterator.getIndex());
}

public int previous() {
	int nl = 0;
	
	for ( char c = iterator.previous();;c = iterator.previous() ) {
		if ( c == DONE )
			break;
		if ( Character.getType( c) == Character.LINE_SEPARATOR ) {
			nl++;
			if ( nl == 2) {
				iterator.next();
				return (iterator.getIndex());
			}
		}
	}

	return (DONE);
}

public void setText( CharacterIterator ci) {
	iterator = ci;
}
}

class SentenceIterator
  extends BreakIterator
{
	CharacterIterator iterator = new StringCharacterIterator("");

public int current() {
	return (iterator.getIndex());
}

public int first() {
	iterator.first();
	return (iterator.getIndex());		
}

public int following( int offs) {
	int pos = iterator.getIndex();
	iterator.setIndex( iterator.getBeginIndex() + offs);
	if ( iterator.current() == DONE ) {
		iterator.setIndex( pos);
		return DONE;
	}
	return (next());		
}

public CharacterIterator getText() {
	return iterator;
}

public int last() {
	iterator.last();
	return (previous());		
}

public int next() {
	int pos = iterator.getIndex();
	boolean nl = false;
	
	for ( char c = iterator.next();;c = iterator.next() ) {
		if ( c == DONE )
			break;
		if ( c == '.' )
			nl = true;
		else if ( nl )
			return (iterator.getIndex());
	}
			
	iterator.setIndex( pos);
	return DONE;
}

public int next( int num) {
	int max = (num > 0) ? num : -num;
	
	for ( int idx=0; idx<max; idx++) {
		if ( DONE == ((num > 0) ? next() : previous()) )
			return (DONE);
	}
	
	return (iterator.getIndex());
}

public int previous() {
	int nl = 0;
	
	for ( char c = iterator.previous();;c = iterator.previous() ) {
		if ( c == DONE )
			break;
		if ( c == '.' ) {
			nl++;
			if ( nl == 2) {
				iterator.next();
				return (iterator.getIndex());
			}
		}
	}

	return (DONE);
}

public void setText( CharacterIterator ci) {
	iterator = ci;
}
}

class WordIterator
  extends BreakIterator
{
	CharacterIterator iterator = new StringCharacterIterator("");

public int current() {
	return iterator.getIndex();
}

public int first() {
	iterator.first();
	return (iterator.getIndex());		
}

public int following( int offs) {
	int pos = iterator.getIndex();
	int ret;
	
	iterator.setIndex( offs);
	ret = next();
	if ( ret == DONE )
		iterator.setIndex( pos);
		
	return ret;
}

public CharacterIterator getText() {
	return iterator;
}

public int last() {
	iterator.last();
	return (previous());
}

public int next() {
	char c = iterator.current();
	
	if ( Character.isSpaceChar( c) ) {
		for ( ;;c = iterator.next()) {
			if ( ( c == DONE) || ! Character.isSpaceChar( c) ) {
				return (iterator.getIndex());
			}
		}
	}
	
	for (;; c = iterator.next()){
		if ( ( c == DONE) || Character.isSpaceChar( c) )
			break;
	}
	
	return (iterator.getIndex());
}

public int next( int num) {
	int idx, ret = current();
	int max = (num > 0) ? num : -num;
	
	if ( num == 0 )
		return (ret);

	for ( idx = 0; idx < max; idx++ ) {
			ret = ( num > 0 ) ? next() : previous();
			if ( ret == DONE )
				break;
	}
	
	return ret;
}

public int previous() {
	char c;
	int pos = iterator.getIndex();
	
	for ( c = iterator.previous(); ! Character.isSpaceChar( c) ; c = iterator.previous() ){
		if ( c == DONE ) {
			iterator.setIndex( pos);
			return (DONE);
		}
	}
	for ( c = iterator.previous(); Character.isSpaceChar( c) ; c = iterator.previous() ){
		if ( c == DONE ) {
			iterator.setIndex( pos);
			return (DONE);
		}
	}
	for ( c = iterator.previous(); ! Character.isSpaceChar( c) ; c = iterator.previous() ){
		if ( c == DONE ) {
			iterator.setIndex( pos);
			return (DONE);
		}
	}
				
	iterator.next();
	return iterator.getIndex();
}

public void setText( CharacterIterator ci) {
	iterator = ci;
}
}
