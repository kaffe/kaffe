/*
 * Java core library component.
 *
 * Copyright (c) 2002
 *      Dalibor Topic <robilad@yahoo.com>.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.sql;

import java.io.InputStream;
import java.io.Reader;
import java.io.OutputStream;
import java.io.Writer;

public interface Clob {
    InputStream getAsciiStream() throws SQLException;
    Reader getCharacterStream() throws SQLException;
    String getSubString(long position, int length) throws SQLException;
    long length() throws SQLException;
    long position(Clob search_string, long start) throws SQLException;
    long position(String search_string, long start) throws SQLException;
/* this is jdbc 3.0 material
    OutputStream setAsciiStream(long position) throws SQLException;
    Writer setCharacterStream(long position) throws SQLException;
    int setString(long position, String str) throws SQLException;
    int setString(long position, String str, int offset, int len) throws SQLException;
    void truncate(long length) throws SQLException;
*/
}
