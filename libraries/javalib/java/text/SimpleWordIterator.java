package java.text;


/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
class SimpleWordIterator
  extends BreakIterator
{
	CharacterIterator iterator = new StringCharacterIterator("");

public int current() {
	return (iterator.getIndex());		
}

public int first() {
	int fi = iterator.getBeginIndex();
	iterator.setIndex( fi);
	return (fi);
}

public int following( int offs) {
	char c;
	int bi = iterator.getBeginIndex();
	int ei = iterator.getEndIndex();
	
	if ( (offs < bi) || (offs > ei) )
		return (DONE);
		
	//get word end index from offset
	iterator.setIndex( offs);
	for ( c = iterator.current(); Character.isSpaceChar( c); c = iterator.next() ) {
		if ( c == CharacterIterator.DONE )
			return (ei);
	}
	for (; !Character.isSpaceChar( c); c = iterator.next() ) {
		if ( c == CharacterIterator.DONE )
			return (ei);
	}
	
	return (iterator.getIndex());
}

public CharacterIterator getText() {
	return (iterator);
}

public int last() {
	int li = iterator.getEndIndex();
	iterator.setIndex( li);
	return (li);
}

public int next() {
	int pos = iterator.getIndex();
	int ei  = iterator.getEndIndex();
	
	//already at end
	if ( pos == ei )
		return (DONE);
		
	char c = iterator.current();
	
	if ( ! Character.isSpaceChar( c ) ) {
		for ( ; !Character.isSpaceChar( c) ; c = iterator.next() ){
			if ( c == CharacterIterator.DONE )
				return (ei);
		}
	}
	
	for ( ; Character.isSpaceChar( c) ; c = iterator.next() ){
			if ( c == CharacterIterator.DONE )
				return (ei);
	}
	
	return ( iterator.getIndex() );
}

public int next( int num) {
	boolean fwd = num > 0;
	int absn = Math.abs( num);
	int ret = current();
	
	for ( int i=0; i<absn; i++) {
		if ( ( ret = (fwd ? next() : previous()) ) == DONE )
			return (DONE);
	}
	
	return ret;
}

public int previous() {
	char c;
	int pos = iterator.getIndex();
	int bi  = iterator.getBeginIndex();
	
	//already at start
	if ( pos == bi )
		return (DONE);
		
	for ( c = iterator.previous(); Character.isSpaceChar( c) ; c = iterator.previous() ){
		if ( c == CharacterIterator.DONE )
			return (bi);
	}
	for ( ; !Character.isSpaceChar( c) ; c = iterator.previous() ){
		if ( c == CharacterIterator.DONE )
			return (bi);
	}
	
	iterator.next();
	return (iterator.getIndex());
}

public void setText( CharacterIterator ci) {
	iterator = ci;
}
}
