import gnu.crypto.cipher.CipherFactory;
import gnu.crypto.cipher.IBlockCipher;

import java.util.Iterator;

class TestGnuCrypto
{
  // default ctor
   
  public static final void main(String[] args)
  {
    String cipher;
    IBlockCipher algorithm;
    for (Iterator it = CipherFactory.getNames().iterator(); it.hasNext(); )
      {
        cipher = (String) it.next();
        try
          {
            algorithm = null;
            algorithm = CipherFactory.getInstance(cipher);
          } 
        catch (InternalError x) 
	  {
            x.printStackTrace(System.err);
          }
      }
  }
}