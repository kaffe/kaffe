/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.util.zip;

// This isn't documented so I don't know what it contains .... !!!

public interface ZipConstants {

  public final static int CEN_SIGNATURE = 0;
  public final static int CEN_VERSIONMADE = 4;
  public final static int CEN_VERSIONEXTRACT = 6;
  public final static int CEN_FLAGS = 8;
  public final static int CEN_METHOD = 10;
  public final static int CEN_TIME = 12;
  public final static int CEN_DATE = 14;
  public final static int CEN_CRC = 16;
  public final static int CEN_COMPRESSEDSIZE = 20;
  public final static int CEN_UNCOMPRESSEDSIZE = 24;
  public final static int CEN_FILENAMELEN = 28;
  public final static int CEN_EXTRAFIELDLEN = 30;
  public final static int CEN_FILECOMMENTLEN = 32;
  public final static int CEN_DISKNUMBER = 34;
  public final static int CEN_INTERNALATTR = 36;
  public final static int CEN_EXTERNALATTR = 38;
  public final static int CEN_LCLOFFSET = 42;

  public final static int CEN_RECSZ = 46;

  public final static long CEN_HEADSIG = 0x02014b50;

  public final static int END_SIGNATURE = 0;
  public final static int END_DISKNUMBER = 4;
  public final static int END_CENDISKNUMBER = 6;
  public final static int END_TOTALCENONDISK = 8;
  public final static int END_TOTALCEN = 10;
  public final static int END_CENSIZE = 12;
  public final static int END_CENOFFSET = 16;
  public final static int END_COMMENTLEN = 20;

  public final static int END_RECSZ = 22;

  public final static long END_ENDSIG = 0x06054b50;

  public final static int LOC_SIGNATURE = 0;
  public final static int LOC_VERSIONEXTRACT = 4;
  public final static int LOC_FLAGS = 6;
  public final static int LOC_METHOD = 8;
  public final static int LOC_TIME = 10;
  public final static int LOC_DATE = 12;
  public final static int LOC_CRC = 14;
  public final static int LOC_COMPRESSEDSIZE = 18;
  public final static int LOC_UNCOMPRESSEDSIZE = 22;
  public final static int LOC_FILENAMELEN = 26;
  public final static int LOC_EXTRAFIELDLEN = 28;

  public final static int LOC_RECSZ = 30;

  public final static long LOC_HEADSIG = 0x04034b50;
}
