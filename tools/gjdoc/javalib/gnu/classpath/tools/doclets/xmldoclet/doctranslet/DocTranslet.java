/* gnu.classpath.tools.doclets.xmldoclet.doctranslet.DocTranslet
   Copyright (C) 2001 Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.
 
GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
02111-1307 USA. */

package gnu.classpath.tools.doclets.xmldoclet.doctranslet;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;

import java.net.MalformedURLException;
import java.net.URL;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.HashMap;
import java.util.Map;

import java.util.jar.JarFile;
import java.util.jar.Manifest;
import java.util.jar.Attributes;

import javax.xml.transform.ErrorListener;
import javax.xml.transform.Source;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.TransformerFactoryConfigurationError;
import javax.xml.transform.URIResolver;

import javax.xml.transform.stream.StreamResult;
import javax.xml.transform.stream.StreamSource;

import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.ParserConfigurationException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;

import org.xml.sax.SAXException;

import gnu.classpath.tools.doclets.xmldoclet.Driver;

import com.sun.javadoc.DocErrorReporter;

public class DocTranslet implements ErrorListener {

   private String mainResourceFilename;
   private ClassLoader classLoader;
   private Map transformerMap = new java.util.HashMap(); //WeakHashMap();
   
   protected DocTranslet(String mainResourceFilename,
                         ClassLoader classLoader)
      throws DocTransletConfigurationException {

      if (mainResourceFilename.length() > 0 && mainResourceFilename.charAt(0) == '/') {
         mainResourceFilename = mainResourceFilename.substring(1);
      }
      this.mainResourceFilename = mainResourceFilename;
      this.classLoader = classLoader;
   }

   private static boolean equalsFile(File file1, File file2) {
      return file1.getAbsolutePath().equals(file2.getAbsolutePath());
   }

   private static File getParentFile(File file) {
      String filename = file.getAbsolutePath();
      if (filename.endsWith(File.separator)) {
         filename = filename.substring(0, filename.length() - 1);
      }
      int lastSlash = filename.lastIndexOf(File.separatorChar);
      if (lastSlash > 0) {
         filename = filename.substring(0, lastSlash);
      }
      else {
         filename = File.separator;
      }

      return new File(filename);
   }

   private static boolean cacheXSLTSheets = true;

   public void apply(File xmlSourceDirectory, File targetDirectory,
                     DocErrorReporter reporter)
      throws DocTransletException {

      try{
         URL mainResourceURL = classLoader == null ?
	     ClassLoader.getSystemResource(mainResourceFilename):
	     classLoader.getResource(mainResourceFilename);

         if (null == mainResourceURL) {
            throw new DocTransletException("Cannot find resource '" + mainResourceFilename + "'");
         }

         List outputFileList = getOutputFileList(mainResourceURL,
                                                 xmlSourceDirectory);

         reporter.printNotice("Running DocTranslet...");
            
         TransformerFactory transformerFactory 
            = TransformerFactory.newInstance();

         transformerFactory.setErrorListener(this);

         boolean isLibxmlJ 
            = transformerFactory.getClass().getName().equals("gnu.xml.libxmlj.transform.TransformerFactoryImpl");

         for (Iterator it = outputFileList.iterator(); it.hasNext(); ) {

            if (isLibxmlJ) {
               System.gc();
               Runtime.getRuntime().runFinalization();
            }

            OutputFileInfo fileInfo = (OutputFileInfo)it.next();

            File targetFile = new File(targetDirectory, fileInfo.getName());
            File packageTargetDir = getParentFile(targetFile);

            if (!packageTargetDir.exists() && !packageTargetDir.mkdirs()) {
               throw new DocTransletException("Target directory " + packageTargetDir + " does not exist and cannot be created.");
            }

            if (null != fileInfo.getSource()) {
            
               reporter.printNotice("Copying " + fileInfo.getComment() + "...");
               InputStream in = new URL(mainResourceURL, fileInfo.getSource()).openStream();
               FileOutputStream out = new FileOutputStream(targetFile.getAbsolutePath());
               copyStream(in, out);
               in.close();
               out.close();
            }
            else {
            
               reporter.printNotice("Generating " + fileInfo.getComment() + "...");

               String pathToRoot = "";
               for (File file = getParentFile(targetFile); !equalsFile(file, targetDirectory); file = getParentFile(file)) {
                  pathToRoot += "../";
               }
            
               StreamResult out = new StreamResult(targetFile.getAbsolutePath());

               StreamSource in = new StreamSource(new File(xmlSourceDirectory, "index.xml").getAbsolutePath());
               URL resource = new URL(mainResourceURL, fileInfo.getSheet());
               if (null == resource) {
                  throw new DocTransletException("Couldn't perform XSLT transformation: stylesheet not found '" + resource + "'");
               }
               StreamSource xsltSource = new StreamSource(resource.toExternalForm());
         
               Map parameters = new HashMap();
               parameters.put("gjdoc.outputfile.info", fileInfo.getInfo());
               parameters.put("gjdoc.pathtoroot", pathToRoot);
               parameters.put("gjdoc.xmldoclet.version", Driver.XMLDOCLET_VERSION);

               Transformer transformer;
               transformer = (Transformer)transformerMap.get(xsltSource.getSystemId());
               if (null == transformer) {
                  transformer = transformerFactory.newTransformer(xsltSource);
                  if (cacheXSLTSheets) {
                     transformerMap.put(xsltSource.getSystemId(), transformer);
                  }
               }

               transformer.setErrorListener(this);


               transformer.clearParameters();
               for (Iterator pit = parameters.keySet().iterator(); pit.hasNext(); ) {
                  String key = (String)pit.next();
                  String value = (String)parameters.get(key);
                  transformer.setParameter(key, value);
               }
               transformer.transform(in, out);
            }
         }
      }
      catch (MalformedURLException e) {
	 throw new DocTransletException(e);
      }
      catch (TransformerFactoryConfigurationError e) {
	 throw new DocTransletException(e);
      }
      catch (TransformerException e) {
	 throw new DocTransletException(e.getMessageAndLocation(), e);
      }
      catch (IOException e) {
	 throw new DocTransletException(e);
      }
   }

   private List getOutputFileList(URL resource, File xmlSourceDirectory) 
      throws DocTransletException {

      try {
	 List result;

	 OutputStream out = new ByteArrayOutputStream();

         {
            StreamSource source = new StreamSource(resource.toExternalForm());

            TransformerFactory transformerFactory = TransformerFactory.newInstance();
            Transformer transformer = (Transformer)transformerFactory.newTransformer(source);
	 
            transformer.transform(new StreamSource(new File(xmlSourceDirectory, 
                                                            "index.xml").getAbsolutePath()), 
                                  new StreamResult(out));
         }

         {
            DocumentBuilderFactory documentBuilderFactory = DocumentBuilderFactory.newInstance();
            documentBuilderFactory.setValidating(false);

            DocumentBuilder documentBuilder = documentBuilderFactory.newDocumentBuilder();
            Document document = documentBuilder.parse(new ByteArrayInputStream(((ByteArrayOutputStream)out).toByteArray()));
	 
            NodeList nodeList = document.getElementsByTagName("outputfile");
            result = new ArrayList(nodeList.getLength());

            for (int i=0; i<nodeList.getLength(); ++i) {
               Element elem = (Element)nodeList.item(i);
               String name    = elem.getElementsByTagName("name").item(0).getFirstChild().getNodeValue();
               String source  
                  = (null != elem.getElementsByTagName("source").item(0))
                  ? elem.getElementsByTagName("source").item(0).getFirstChild().getNodeValue()
                  : null;
               String sheet
                  = (null != elem.getElementsByTagName("sheet").item(0))
                  ? elem.getElementsByTagName("sheet").item(0).getFirstChild().getNodeValue()
                  : null;
               String comment = elem.getElementsByTagName("comment").item(0).getFirstChild().getNodeValue();
               String info    = null;
               if (elem.getElementsByTagName("info").getLength() > 0) {
                  info = elem.getElementsByTagName("info").item(0).getFirstChild().getNodeValue();
               }
               result.add(new OutputFileInfo(name, source, sheet, comment, info));
            }
         }
         return result;
      }
      catch (TransformerFactoryConfigurationError e) {
	 throw new DocTransletException(e);
      }
      catch (TransformerException e) {
	 throw new DocTransletException(e.getMessageAndLocation(), e);
      }
      catch (ParserConfigurationException e) {
	 throw new DocTransletException(e);
      }
      catch (SAXException e) {
	 throw new DocTransletException(e);
      }
      catch (IOException e) {
	 throw new DocTransletException(e);
      }
   }

   public static DocTranslet fromClasspath(String resourceName) 
      throws DocTransletConfigurationException {

      return new DocTranslet(resourceName, 
                             DocTranslet.class.getClassLoader());
   }

   public static DocTranslet fromJarFile(File jarFile) 
      throws DocTransletConfigurationException {

      try {
         JarFile inputJarFile = new JarFile(jarFile, false, JarFile.OPEN_READ);
      
         Manifest manifest = inputJarFile.getManifest();
         
         if (null == manifest) {
         
            throw new DocTransletConfigurationException("Jar file '" + jarFile + "' doesn't contain a manifest.");
         }
         
         Attributes mainAttributes = manifest.getMainAttributes();
      
         String docTransletMainEntry = mainAttributes.getValue("doctranslet-main-entry");

         if (null == docTransletMainEntry) {
            
            throw new DocTransletConfigurationException("Manifest in Jar file '" + jarFile + "' doesn't contain a doctranslet-main-entry specification.");
         }
         
         return new DocTranslet(docTransletMainEntry, 
                                new JarClassLoader(inputJarFile));
      }
      catch (IOException e) {
         throw new DocTransletConfigurationException(e);
      }
   }

   private static void copyStream(InputStream in, OutputStream out)
      throws IOException {

      byte[] buf = new byte[256];
      int nread;

      while ((nread = in.read(buf)) >= 0) {
         out.write(buf, 0, nread);
      }
   }

  public void error (TransformerException exception)
    throws TransformerException {

     throw exception;
  }

  public void fatalError (TransformerException exception)
    throws TransformerException {

     throw exception;
  }

  public void warning (TransformerException exception)
    throws TransformerException {

     System.err.println("WWW: " + exception.getMessage());
  }

}
