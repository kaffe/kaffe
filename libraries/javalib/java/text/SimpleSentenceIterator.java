
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

class SimpleSentenceIterator extends BreakIterator {
	CharacterIterator iterator = new StringCharacterIterator("");

public int current() {
	return iterator.getIndex();
}

public int first() {
	iterator.first();
	return iterator.getIndex();
}

public int following(int offs) {
	iterator.setIndex(iterator.getBeginIndex() + offs);
	return next();		
}

public CharacterIterator getText() {
	return iterator;
}

public int last() {
	iterator.last();
	return previous();		
}

public int next() {

	if (iterator.current() == CharacterIterator.DONE)
		return DONE;

	for (char c = iterator.next(); c != CharacterIterator.DONE; ) {
		if (c == '.') {
			c = iterator.next();
			if (c != CharacterIterator.DONE && c != ' ')
				continue;
			break;
		}
	    	c = iterator.next();
	}
	return iterator.getIndex();
}

public int next( int num) {
	int max = (num > 0) ? num : -num;

	for (int idx = 0; idx < max; idx++) {
		int c = (num > 0) ? next() : previous();
		if (c == CharacterIterator.DONE)
			return (DONE);
	}

	return iterator.getIndex();
}

public int previous() {
	int nl = 0;
	
	for (char c = iterator.previous();
	    c != CharacterIterator.DONE;
	    c = iterator.previous()) {
		if ( c == '.' ) {
			if (++nl == 2) {
				iterator.next();
				return iterator.getIndex();
			}
		}
	}

	return (DONE);
}

public void setText( CharacterIterator ci) {
	iterator = ci;
}

}
