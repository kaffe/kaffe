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

class SimpleWordIterator
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
	if ( ret == DONE ) {
		iterator.setIndex( pos);
	}
	return (ret);
}

public CharacterIterator getText() {
	return (iterator);
}

public int last() {
	iterator.last();
	return (previous());
}

public int next() {
	char c = iterator.current();
	if (c == CharacterIterator.DONE) {
		return (BreakIterator.DONE);
	}

	boolean test;
	if (Character.isSpaceChar(c)) {
		test = false;
	}
	else {
		test = true;
	}

	for (;;) {
		c = iterator.next();
		if (c == CharacterIterator.DONE) {
			iterator.previous();
			return (iterator.getIndex() + 1);
		}
		if (Character.isSpaceChar(c) == test) {
			return (iterator.getIndex());
		}
	}
}

public int next( int num) {
	int idx;
	int ret;
	int max;

	ret = current();
	if ( num == 0 ) {
	}
	else if (num > 0) {
		for ( idx = 0; idx < num; idx++ ) {
			ret = next();
			if ( ret == DONE ) {
				break;
			}
		}
	}
	else {
		for ( idx = num; idx < 0; idx++ ) {
			ret = previous();
			if ( ret == DONE ) {
				break;
			}
		}
	}
	return (ret);
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
