
/*
 * Java core library component.
 *
 * Copyright (c) 1999
 *	Archie L. Cobbs.  All rights reserved.
 * Copyright (c) 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Author: Archie L. Cobbs <archie@whistle.com>
 */

package java.text;

public final class CollationElementIterator {
	public final static int NULLORDER = 0xffffffff;
	private CharacterIterator src;

	CollationElementIterator(String str) {
		src = new StringCharacterIterator(str);
	}

	CollationElementIterator(CharacterIterator src) {
		this.src = src;
	}

	public void reset() {
		src.setIndex(0);
	}
   
	public int next() {
		char c = src.next();
		if (c == CharacterIterator.DONE) {
			return NULLORDER;
		}
		return c << 16;
	}
   
	public int previous() {
		char c = src.previous();
		if (c == CharacterIterator.DONE) {
			return NULLORDER;
		}
		return c << 16;
	}

	public final static int primaryOrder(int order) {
		return order >> 16;
	}

	public final static short secondaryOrder(int order) {
		return (short)((order >> 8) & 0xff);
	}

	public final static short tertiaryOrder(int order) {
		return (short)(order & 0xff);
	}

	public void setOffset(int newOffset) {
		src.setIndex(newOffset);
	}

	public int getOffset() {
		return src.getIndex();
	}

	public int getMaxExpansion(int order) {
		return 1;	// XXX ??
	}

	public void setText(String str) {
		src = new StringCharacterIterator(str);
	}

	public void setText(CharacterIterator source) {
		src = source;
	}

}
