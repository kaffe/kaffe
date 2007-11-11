import gnu.javax.crypto.cipher.CipherFactory;

import java.util.Iterator;

class TestGnuCrypto
{
  // default ctor
   
  public static final void main(String[] args)
  {
    String cipher;
    for (Iterator it = CipherFactory.getNames().iterator(); it.hasNext(); )
      {
        cipher = (String) it.next();
        try
          {
            CipherFactory.getInstance(cipher);
          } 
        catch (InternalError x) 
	  {
            x.printStackTrace(System.err);
          }
      }
  }
}
