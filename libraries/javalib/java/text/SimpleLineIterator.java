package java.text;

import java.lang.String;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
class SimpleLineIterator
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
	if ( iterator.current() == CharacterIterator.DONE ) {
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
		if ( c == CharacterIterator.DONE )
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
		if ( CharacterIterator.DONE == ((num > 0) ? next() : previous()) )
			return (DONE);
	}
	
	return (iterator.getIndex());
}

public int previous() {
	int nl = 0;
	
	for ( char c = iterator.previous();;c = iterator.previous() ) {
		if ( c == CharacterIterator.DONE )
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
