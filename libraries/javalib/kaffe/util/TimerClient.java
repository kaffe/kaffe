/*
 * interface to be used in combination with kaffe.util.Timer 
 *
 * Copyright (c) 1998
 *    Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author P.C.Mehlitz
 */

package kaffe.util;

public interface TimerClient {

void timerExpired ( Timer timer );

}
