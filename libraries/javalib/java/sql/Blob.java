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
import java.io.OutputStream;

public interface Blob {
    InputStream getBinaryStream() throws SQLException;
    byte[] getBytes(long position, int length) throws SQLException;
    long length() throws SQLException;
    long position(byte[] pattern, long start) throws SQLException;
/* this is jdbc 3.0 material
    OutputStream setBinaryStream(long position) throws SQLException;
    int setBytes(long position, byte[] bytes) throws SQLException;
    int setBytes(long position, byte[] bytes, int offset, int length) throws SQLException;
    void truncate(long length) throws SQLException;
*/
}
