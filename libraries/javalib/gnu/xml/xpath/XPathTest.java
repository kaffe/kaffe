package gnu.xml.xpath;

import java.io.FileInputStream;
import java.io.InputStream;
import java.util.Collection;
import java.util.Iterator;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.xml.sax.InputSource;

public class XPathTest
{

  public static void main(String[] args)
  {
    try
      {
        if (args.length < 1)
          {
            System.out.println("Syntax: java "+XPathTest.class.getName()+
                               " expression [file]");
          }
        String expression = args[0];
        InputStream in = System.in;
        if (args.length > 1)
          {
            in = new FileInputStream(args[1]);
          }
        
        DocumentBuilderFactory f = new gnu.xml.dom.JAXPFactory();
        f.setNamespaceAware(true);
        DocumentBuilder b = f.newDocumentBuilder();
        Document doc = b.parse(new InputSource(in));
        
        XPathImpl xpath = new XPathImpl(null, null);
        Object ret = xpath.evaluate(expression, doc, null);
        if (ret instanceof Collection)
          {
            Collection ns = (Collection) ret;
            for (Iterator i = ns.iterator(); i.hasNext(); )
              {
                System.out.println(Expr._string((Node) i.next(), null));
              }
          }
        else
          {
            System.out.println(ret);
          }
      }
    catch (Exception e)
      {
        e.printStackTrace(System.err);
      }
  }
  
}
