/* 
 * $Id: DefaultURIResolverImpl.java,v 1.1 2004/04/14 19:40:35 dalibor Exp $
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

import java.io.File;
import javax.xml.transform.Source;
import javax.xml.transform.TransformerException;
import javax.xml.transform.URIResolver;
import javax.xml.transform.stream.StreamSource;

/**
 *  A simple implementation of {@link URIResolver} which currently
 *  only works for local files.
 */
class DefaultURIResolverImpl implements URIResolver
{
  public Source resolve (String href, String base) throws TransformerException
  {
    try
      {
        File baseFile = new File (base);
        if (baseFile.exists ())
          {
            File baseDirectory = baseFile.getParentFile ();
            File resolvedFile = new File (baseDirectory, href);
            if (resolvedFile.exists ())
              {
                return new StreamSource (resolvedFile.getAbsolutePath ());
              }
            else 
              {
                 System.err.println("Not found: " + href + " (in base " + base + ")");
                 throw new TransformerException("Not found: " + href);
              }
          }

        // FIXME: Naive approach
        String prefix = base;
        int ndx = prefix.lastIndexOf ('/');
        if (ndx > 0)
          {
            prefix = prefix.substring (0, ndx);
          }
        else
          {
            prefix = "";
          }

        while (prefix.length () > 0 && href.startsWith ("../"))
          {
            ndx = prefix.lastIndexOf ('/');
            href = href.substring (3);
            if (ndx > 0)
              {
                prefix = prefix.substring (0, ndx);
              }
            else
              {
                prefix = "";
              }
          }

        return new StreamSource (prefix + "/" + href);
      }
    catch (Throwable t)
      {
        t.printStackTrace ();
        throw new TransformerException (t);
      }
  }
}
