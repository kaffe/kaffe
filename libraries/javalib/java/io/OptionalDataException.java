package java.io;


/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class OptionalDataException
  extends ObjectStreamException
{
	// part of serial form
	public int length;
	public boolean eof;

	/* The JDK doc doesn't show any constructors, but javap shows
	 * two package private constructors.  We include them so that
	 * the serial version id is properly computed without having
	 * to hardcode the id
	 */
        OptionalDataException(boolean eof) {
		this.eof = eof;
	}

        OptionalDataException(int length) {
		this.length = length;
		this.eof = false; /* ??? */
	}
}
