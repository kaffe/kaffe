/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
package java.io;

public class OptionalDataException
  extends ObjectStreamException
{
	// part of serial form
	public int length;
	public boolean eof;

	/* The JDK doc doesn't show any constructors, but javap shows
	 * two package private constructors.  This may be because Sun
	 * implements serialization within java.io.
	 * We need to be able to construct OptionalDataExceptions from
	 * kaffe.io, hence the public constructors for now.
	 *
	 * XXX fix this
	 * XXX hardcode serialVersionUID for this class
	 */
        /*package*/ OptionalDataException(boolean eof) {
		this.eof = eof;
	}

        /*package*/ OptionalDataException(int length) {
		this.length = length;
		this.eof = false; /* ??? */
	}
}
