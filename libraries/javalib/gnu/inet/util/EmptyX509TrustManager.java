/*
 * $Id: EmptyX509TrustManager.java,v 1.4 2004/10/04 19:34:03 robilad Exp $
 * Copyright (C) 2004 The Free Software Foundation
 * 
 * This file is part of GNU inetlib, a library.
 * 
 * GNU inetlib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * GNU inetlib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 * As a special exception, if you link this library with other files to
 * produce an executable, this library does not by itself cause the
 * resulting executable to be covered by the GNU General Public License.
 * This exception does not however invalidate any other reasons why the
 * executable file might be covered by the GNU General Public License.
 */

package gnu.inet.util;

import java.security.cert.CertificateException;
import java.security.cert.X509Certificate;
import javax.net.ssl.X509TrustManager;

/**
 * Empty implementation of an X509 trust manager.
 * This implementation does not check any certificates in the chain.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public class EmptyX509TrustManager
implements X509TrustManager
{

  public void checkClientTrusted (X509Certificate[] chain, String authType)
    throws CertificateException
    {
    }

  public void checkServerTrusted (X509Certificate[] chain, String authType)
    throws CertificateException
    {
    }

  public X509Certificate[] getAcceptedIssuers ()
    {
      return new X509Certificate[0];
    }
  
}
