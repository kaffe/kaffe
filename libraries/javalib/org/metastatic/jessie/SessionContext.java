/* SessionContext.java -- Implementation of a session context.
   Copyright (C) 2003  Casey Marshall <rsdio@metastatic.org>

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the

   Free Software Foundation, Inc.,
   59 Temple Place, Suite 330,
   Boston, MA  02111-1307
   USA

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under terms
of your choice, provided that you also meet, for each linked independent
module, the terms and conditions of the license of that module.  An
independent module is a module which is not derived from or based on
this library.  If you modify this library, you may extend this exception
to your version of the library, but you are not obligated to do so.  If
you do not wish to do so, delete this exception statement from your
version.  */


package org.metastatic.jessie;

import java.util.Enumeration;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Vector;

import javax.net.ssl.SSLSession;
import javax.net.ssl.SSLSessionContext;

class SessionContext implements SSLSessionContext
{

  // Fields.
  // -------------------------------------------------------------------------

  private HashMap sessions;

  private int cacheSize;

  private int timeout;

  // Constructor.
  // -------------------------------------------------------------------------

  SessionContext()
  {
    sessions = new HashMap();
    cacheSize = 0;
    timeout = 0;
  }

  // Instance methods.
  // -------------------------------------------------------------------------

  public Enumeration getIds()
  {
    Vector ids = new Vector();
    for(Iterator i = sessions.keySet().iterator(); i.hasNext(); )
      {
        Session.ID id = (Session.ID) i.next();
        ids.add(id.getId());
      }
    return ids.elements();
  }

  public SSLSession getSession(byte[] sessionId)
  {
    Session session = (Session) sessions.get(new Session.ID(sessionId));
    if (session == null)
      return null;
    long elapsed = System.currentTimeMillis() - session.getLastAccessedTime();
    if (timeout > 0 && (int) (elapsed / 1000) > timeout)
      {
        sessions.remove(session.getIdObject());
        session.invalidate();
        return null;
      }
    if (!session.getValid())
      {
        sessions.remove(session.getIdObject());
        session.invalidate();
        return null;
      }
    return session;
  }

  public int getSessionCacheSize()
  {
    return cacheSize;
  }

  public void setSessionCacheSize(int cacheSize)
  {
    if (cacheSize < 0)
      throw new IllegalArgumentException();
    this.cacheSize = cacheSize;
  }

  public int getSessionTimeout()
  {
    return timeout;
  }

  public void setSessionTimeout(int timeout)
  {
    if (timeout < 0)
      throw new IllegalArgumentException();
    this.timeout = timeout;
  }

  // Package methods.
  // -------------------------------------------------------------------------

  void addSession(Session.ID sessionId, Session session)
  {
    if (cacheSize > 0 && sessions.size() > cacheSize)
      {
        boolean removed = false;
        for (Iterator i = sessions.values().iterator(); i.hasNext(); )
          {
            Session s = (Session) i.next();
            long elapsed = System.currentTimeMillis() - s.getLastAccessedTime();
            if (!s.getValid())
              {
                sessions.remove(session.getIdObject());
                removed = true;
              }
            else if ((int) (elapsed / 1000) > timeout)
              {
                sessions.remove(session.getIdObject());
                removed = true;
              }
          }
        if (removed)
          {
            sessions.put(sessionId, session);
            session.setSessionContext(this);
            session.setSessionId(sessionId);
            session.setLastAccessedTime(System.currentTimeMillis());
          }
      }
    else
      {
        sessions.put(sessionId, session);
        session.setSessionContext(this);
        session.setSessionId(sessionId);
        session.setLastAccessedTime(System.currentTimeMillis());
      }
  }
}
