/*
 * BERConstants.java
 * Copyright (C) 2004 The Free Software Foundation
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
 * Linking this library statically or dynamically with other modules is
 * making a combined work based on this library.  Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * As a special exception, the copyright holders of this library give you
 * permission to link this library with independent modules to produce an
 * executable, regardless of the license terms of these independent
 * modules, and to copy and distribute the resulting executable under
 * terms of your choice, provided that you also meet, for each linked
 * independent module, the terms and conditions of the license of that
 * module.  An independent module is a module which is not derived from
 * or based on this library.  If you modify this library, you may extend
 * this exception to your version of the library, but you are not
 * obliged to do so.  If you do not wish to do so, delete this
 * exception statement from your version.
 */

package gnu.inet.ldap;

/**
 * Constants used in BER encoding and decoding.
 * Universal types are described in ISO/IEC 8824-1:2003, section 8.4.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public interface BERConstants
{

  /* -- Tag classes -- */
  public static final int UNIVERSAL = 0x00;
  public static final int APPLICATION = 0x40; // bit 7
  public static final int CONTEXT = 0x80; // bit 8
  public static final int PRIVATE = 0xc0; // bits 7 and 8

  /* -- Universal tags -- */
  public static final int BOOLEAN = 0x01; // UNIVERSAL 1
  public static final int INTEGER = 0x02;
  public static final int BIT_STRING = 0x03;
  public static final int OCTET_STRING = 0x04;
  public static final int NULL = 0x05;
  public static final int OID = 0x06;
  public static final int DESCRIPTOR = 0x07;
  public static final int EXTERNAL = 0x08;
  public static final int REAL = 0x09;
  public static final int ENUMERATED = 0x0a;
  public static final int EMBEDDED_PDV = 0x0b;
  public static final int UTF8_STRING = 0x0c;
  public static final int RELATIVE_OID = 0x0d;
  public static final int SEQUENCE = 0x10; // UNIVERSAL 16
  public static final int SET = 0x11;

  /* -- Application tags -- */
  public static final int FILTER_PRESENT = 0x87; // APPLICATION 7
  public static final int FILTER_AND = 0xa0; // APPLICATION 32
  public static final int FILTER_OR = 0xa1;
  public static final int FILTER_NOT = 0xa2;
  public static final int FILTER_EQUAL = 0xa3;
  public static final int FILTER_SUBSTRING = 0xa4;
  public static final int FILTER_GREATER = 0xa5;
  public static final int FILTER_LESS = 0xa6;
  public static final int FILTER_APPROX = 0xa8;
  public static final int FILTER_EXTENSIBLE = 0xa9;
  
}
