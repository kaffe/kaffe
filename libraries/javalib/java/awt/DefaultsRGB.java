/**
 * DefaultsRGB
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
 */

package java.awt;

// We put these in another class to avoid initialization hassel
class DefaultsRGB {
	static int[] RgbRequests = {
	// note that requested colors can't refer to Color objects (which
	// require an already initialized Color mapping)

	195<<16 | 195<<8 | 195,     // lightgray
	230<<16 | 230<<8 | 230,     // lighter lightGray
	163<<16 | 163<<8 | 163,     // darker lightGray
	190<<16                     // dark red
};

}
