/*
 * $Id: CompressedOutputStream.java,v 1.2 2004/03/22 11:24:07 dalibor Exp $
 * Copyright (C) 2003 The Free Software Foundation
 * 
 * This file is part of GNU inetlib, a library.
 * 
 * GNU inetlib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * GNU inetlib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 * As a special exception, if you link this library with other files to
 * produce an executable, this library does not by itself cause the
 * resulting executable to be covered by the GNU General Public License.
 * This exception does not however invalidate any other reasons why the
 * executable file might be covered by the GNU General Public License.
 */

package gnu.inet.ftp;

import java.io.IOException;
import java.io.OutputStream;

/**
 * A DTP output stream that implements the FTP compressed transfer mode.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 * @version $Revision: 1.2 $ $Date: 2004/03/22 11:24:07 $
 */
class CompressedOutputStream extends DTPOutputStream
{

    static final byte RECORD = -128;      // 0x80
    static final byte EOF = 64;   // 0x40
    
    CompressedOutputStream(DTP dtp, OutputStream out)
    {
        super(dtp, out);
    }
    
    /**
     * Just one byte cannot be compressed.
     * It takes 5 bytes to transmit - hardly very compressed!
     */
    public void write(int c) throws IOException
    {
        if (transferComplete)
            return;
        byte[] buf = new byte[]
        {
            RECORD,                   /* record descriptor */
            0x00, 0x01,             /* one byte */
            0x01,                   /* one uncompressed byte */
            (byte) c                /* the byte */
        };
        out.write(buf, 0, 5);
    }
    
    public void write(byte[] b) throws IOException
    {
        write(b, 0, b.length);
    }

    /**
     * The larger len is, the better.
     */
    public void write(byte[] b, int off, int len) throws IOException
    {
        if (transferComplete)
            return;
        byte[] buf = compress(b, off, len);
        len = buf.length;
        buf[0] = RECORD;            /* record descriptor */
        buf[1] = (byte) ((len & 0x00ff) >> 8);      /* high byte of bytecount */
        buf[2] = (byte) (len & 0xff00);     /* low byte of bytecount */
        out.write(buf, 0, len);
    }
    
    /**
     * Returns the compressed form of the given byte array.
     * The first 3 bytes are left free for header information.
     */
    byte[] compress(byte[] b, int off, int len)
    {
        byte[] buf = new byte[len];
        byte last = 0;
        int pos = 0, raw_count = 0, rep_count = 1;
        for (int i = off; i < len; i++)
        {
            byte c = b[i];
            if (i > off && c == last) // compress
            {
                if (raw_count > 0)      // flush raw bytes to buf
                {
                    // need to add raw_count+1 bytes
                    if (pos + (raw_count + 1) > buf.length)
                        buf = realloc(buf, len);
                    pos = flush_raw(buf, pos, b, (i - raw_count) - 1, raw_count);
                    raw_count = 0;
                }
                rep_count++;            // keep looking for same byte
            }
            else
            {
                if (rep_count > 1)      // flush compressed bytes to buf
                {
                    // need to add 2 bytes
                    if (pos + 2 > buf.length)
                        buf = realloc(buf, len);
                    pos = flush_compressed(buf, pos, rep_count, last);
                    rep_count = 1;
                }
                raw_count++;            // keep looking for raw bytes
            }
            if (rep_count == 127)     // flush compressed bytes
            {
                // need to add 2 bytes
                if (pos + 2 > buf.length)
                    buf = realloc(buf, len);
                pos = flush_compressed(buf, pos, rep_count, last);
                rep_count = 1;
            }
            if (raw_count == 127)     // flush raw bytes
            {
                // need to add raw_count+1 bytes
                if (pos + (raw_count + 1) > buf.length)
                    buf = realloc(buf, len);
                pos = flush_raw(buf, pos, b, (i - raw_count), raw_count);
                raw_count = 0;
            }
            last = c;
        }
        if (rep_count > 1)          // flush compressed bytes
        {
            // need to add 2 bytes
            if (pos + 2 > buf.length)
                buf = realloc(buf, len);
            pos = flush_compressed(buf, pos, rep_count, last);
            rep_count = 1;
        }
        if (raw_count > 0)          // flush raw bytes
        {
            // need to add raw_count+1 bytes
            if (pos + (raw_count + 1) > buf.length)
                buf = realloc(buf, len);
            pos = flush_raw(buf, pos, b, (len - raw_count), raw_count);
            raw_count = 0;
        }
        byte[] ret = new byte[pos + 3];
        System.arraycopy(buf, 0, ret, 3, pos);
        return ret;
    }

    int flush_compressed(byte[] buf, int pos, int count, byte c)
    {
        buf[pos++] = (byte) (0x80 | count);
        buf[pos++] = c;
        return pos;
    }
    
    int flush_raw(byte[] buf, int pos, byte[] src, int off, int len)
    {
        buf[pos++] = (byte) len;
        System.arraycopy(src, off, buf, pos, len);
        return pos + len;
    }
    
    byte[] realloc(byte[] buf, int len)
    {
        byte[] ret = new byte[buf.length + len];
        System.arraycopy(buf, 0, ret, 0, buf.length);
        return ret;
    }
    
    public void close() throws IOException
    {
        byte[] buf = new byte[]
        {
            EOF,                      /* eof descriptor */
            0x00, 0x00              /* no bytes */
        };
        out.write(buf, 0, 3);
        out.close();
    }

}
