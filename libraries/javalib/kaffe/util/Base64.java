
/*
 * Java core library component.
 *
 * Copyright (c) 1999
 *	Archie L. Cobbs.  All rights reserved.
 * Copyright (c) 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Author: Archie L. Cobbs <archie@whistle.com>
 */

package kaffe.util;

public final class Base64 {
	public static final String CHARSET =
	    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	public static final char END_CHAR = '=';

	private Base64() {
	}

	public static byte[] encode(String s) {
		return encode(s.getBytes());
	}

	public static byte[] encode(byte[] buf) {
		return encode(buf, 0, buf.length);
	}

	public static byte[] encode(byte[] buf, int off, int len) {
		byte[] out = new byte[((len + 2) / 3) * 4];
		int index = 0, j = off;

		for (int i = 0; i < out.length; i++) {
			if (j >= off + len) {
				out[i] = (byte)END_CHAR;
				continue;
			}
			switch (i % 4) {
			case 0:
				index = (buf[j] >> 2) & 0x3f;
				break;
			case 1:
				index = ((buf[j++] << 4) & 0x30);
				if (j < off + len) {
					index |= ((buf[j] >> 4) & 0x0f);
				}
				break;
			case 2:
				index = ((buf[j++] << 2) & 0x3c);
				if (j < off + len) {
					index |= ((buf[j] >> 6) & 0x03);
				}
				break;
			case 3:
				index = buf[j++] & 0x3f;
				break;
			}
			out[i] = (byte)CHARSET.charAt(index & 0x3f);
		}
		return out;
	}

	public static byte[] decode(String s) {
		return decode(s.getBytes());
	}

	public static byte[] decode(byte[] buf) {
		return decode(buf, 0, buf.length);
	}

	public static byte[] decode(byte[] buf, int off, int len) {
		while (len > 0 && buf[off + len - 1] == (byte)END_CHAR) {
			len--;
		}
		byte[] out = new byte[(len * 3) / 4];
		int j = 0;
		for (int i = 0; i < len; i++) {
			byte ch = buf[off + i];
			int bits;

			if ((bits = CHARSET.indexOf((char)ch)) == -1) {
				throw new IllegalArgumentException();
			}
			switch (i % 4) {
			case 0:
				out[j] = (byte)(bits << 2);
				break;
			case 1:
				out[j++] |= (bits >> 4) & 0x03;
				if (j < out.length) {
					out[j] |= (bits << 4) & 0xf0;
				}
				break;
			case 2:
				out[j++] |= (bits >> 2) & 0x0f;
				if (j < out.length) {
					out[j] |= (bits << 6) & 0xc0;
				}
				break;
			case 3:
				out[j++] |= bits & 0x3f;
				break;
			}
		}
		return out;
	}

}

