/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package javax.servlet;

import java.io.IOException;
import java.io.PrintWriter;

public interface ServletResponse {

    void setContentType(String type);
    ServletOutputStream getOutputStream() throws IOException;
    PrintWriter getWriter() throws IOException;
    String getCharacterEncoding();

}
