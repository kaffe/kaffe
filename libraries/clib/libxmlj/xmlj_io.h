/* 
 * $Id: xmlj_io.h,v 1.1 2004/04/14 19:40:14 dalibor Exp $
 * Copyright (C) 2003 Julian Scheid
 * 
 * This file is part of GNU LibxmlJ, a JAXP-compliant Java wrapper for
 * the XML and XSLT C libraries for Gnome (libxml2/libxslt).
 * 
 * GNU LibxmlJ is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *  
 * GNU LibxmlJ is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU LibxmlJ; see the file COPYING.  If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA. 
 */

#ifndef XMLJ_IO_H
#define XMLJ_IO_H

#include <jni.h>
#include <libxml/xmlIO.h>
#include "xmlj_error.h"

xmlDocPtr 
xmljParseJavaInputStream (JNIEnv * env, jobject inputStream,
			  jstring inSystemId, jstring inPublicId,
			  jobject saxErrorAdapter);
void xmljSaveFileToJavaOutputStream (JNIEnv * env, jobject outputStream,
					xmlDocPtr tree,
					const char *outputEncoding);
xmlParserInputPtr  xmljLoadExternalEntity (const char *URL, const char *ID,
					      xmlParserCtxtPtr ctxt);
jobject xmljResolveURI (SaxErrorContext * saxErrorContext, const char *URL,
			  const char *ID);
xmlDocPtr
xmljResolveURIAndOpen (SaxErrorContext * saxErrorContext,
		       const char *URL, const char *ID);

void
xmljSetThreadContext (SaxErrorContext *ctxt);

SaxErrorContext *
xmljGetThreadContext (void);

void
xmljClearThreadContext ();


#endif	/* !defined XMLJ_IO_H */

