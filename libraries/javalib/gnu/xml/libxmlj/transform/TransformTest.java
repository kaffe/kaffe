/* 
 * $Id: TransformTest.java,v 1.1 2004/04/14 19:40:35 dalibor Exp $
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

package gnu.xml.libxmlj.transform;

import javax.xml.transform.Source;
import javax.xml.transform.Result;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.TransformerFactoryConfigurationError;
import javax.xml.transform.URIResolver;
import javax.xml.transform.stream.StreamSource;
import javax.xml.transform.stream.StreamResult;

/**
 *  Simple test class with command line interface.
 */
public class TransformTest
{

   /**
    *  Launches the test.
    *
    *  @param args[0] Path or URL of the source XML document
    *
    *  @param args[1] Path or URL of the XSLT sheet
    *
    *  @param args[2] Path of the the file the resulting XML document
    *  will be written to.
    *
    *  @fixme It would be nice to use 
    */
  public static void main (String[]args) throws Exception
  {

    // Force use of Libxsltj
    System.setProperty ("javax.xml.transform.TransformerFactory",
			"gnu.xml.libxmlj.transform.TransformerFactoryImpl");

    // Read arguments
    if (args.length != 3)
      {
	System.err.println ("Usage: java " + TransformTest.class.getName ()
			    + " <source> <stylesheet> <result>");
      }
    Source source = new StreamSource (args[0]);
    Source xsltSource = new StreamSource (args[1]);
    Result target = new StreamResult (args[2]);

    // Prepare stylesheet
    TransformerFactory transformerFactory = TransformerFactory.newInstance ();
    Transformer transformer = transformerFactory.newTransformer (xsltSource);

    // Set test parameters
      transformer.setParameter ("bar", "'lala'");

    // Perform transformation
      transformer.transform (source, target);
  }
}
