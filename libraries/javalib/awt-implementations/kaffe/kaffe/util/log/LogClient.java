/**
 * LogClient - interface for LogStream clients
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

package kaffe.util.log;


public interface LogClient
{
void disable();

void enable();

void newLogLine ( LogStream ls );
}
