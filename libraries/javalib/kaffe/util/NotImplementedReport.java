/*
 * NotImplementedReport
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.util;


public class NotImplementedReport {

public NotImplementedReport()
{
	(new NotImplemented()).printStackTrace();
}

public NotImplementedReport(String mess)
{
	(new NotImplemented(mess)).printStackTrace();
}

}
