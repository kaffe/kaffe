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

interface ZipConstants {

  int CEN_SIGNATURE = 0;
  int CEN_VERSIONMADE = 4;
  int CEN_VERSIONEXTRACT = 6;
  int CEN_FLAGS = 8;
  int CEN_METHOD = 10;
  int CEN_TIME = 12;
  int CEN_DATE = 14;
  int CEN_CRC = 16;
  int CEN_COMPRESSEDSIZE = 20;
  int CEN_UNCOMPRESSEDSIZE = 24;
  int CEN_FILENAMELEN = 28;
  int CEN_EXTRAFIELDLEN = 30;
  int CEN_FILECOMMENTLEN = 32;
  int CEN_DISKNUMBER = 34;
  int CEN_INTERNALATTR = 36;
  int CEN_EXTERNALATTR = 38;
  int CEN_LCLOFFSET = 42;

  int CEN_RECSZ = 46;

  long CEN_HEADSIG = 0x02014b50;

  int END_SIGNATURE = 0;
  int END_DISKNUMBER = 4;
  int END_CENDISKNUMBER = 6;
  int END_TOTALCENONDISK = 8;
  int END_TOTALCEN = 10;
  int END_CENSIZE = 12;
  int END_CENOFFSET = 16;
  int END_COMMENTLEN = 20;

  int END_RECSZ = 22;

  long END_ENDSIG = 0x06054b50;

  int LOC_SIGNATURE = 0;
  int LOC_VERSIONEXTRACT = 4;
  int LOC_FLAGS = 6;
  int LOC_METHOD = 8;
  int LOC_TIME = 10;
  int LOC_DATE = 12;
  int LOC_CRC = 14;
  int LOC_COMPRESSEDSIZE = 18;
  int LOC_UNCOMPRESSEDSIZE = 22;
  int LOC_FILENAMELEN = 26;
  int LOC_EXTRAFIELDLEN = 28;

  int LOC_RECSZ = 30;

  long LOC_HEADSIG = 0x04034b50;

  long DATA_HEADSIG = 0x08074b50;
  int DATA_SIGNATURE = 0;
  int DATA_CRC = 4;
  int DATA_COMPRESSEDSIZE = 8;
  int DATA_UNCOMPRESSEDSIZE = 12;

  int DATA_RECSZ = 16;

  int DEFLATED = 8;
  int STORED = 0;
}
