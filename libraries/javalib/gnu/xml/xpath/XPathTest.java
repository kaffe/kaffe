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
        long t1, t2;
        
        XPathImpl xpath = new XPathImpl(null, null);
        t1 = System.currentTimeMillis();
        Expr expr = (Expr) xpath.compile(expression);
        t2 = System.currentTimeMillis();
        System.out.println("Compiled in "+(t2-t1)+"ms");
        System.out.println(">>> "+expr);
        t1 = System.currentTimeMillis();
        Object ret = expr.evaluate(doc, 1, 1);
        t2 = System.currentTimeMillis();
        System.out.println("Evaluated in "+(t2-t1)+"ms");
        if (ret instanceof Collection)
          {
            System.out.println("[");
            Collection ns = (Collection) ret;
            for (Iterator i = ns.iterator(); i.hasNext(); )
              {
                System.out.println(i.next().toString());
              }
            System.out.println("]");
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
