/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.util;

import java.lang.String;

public abstract class ListResourceBundle extends ResourceBundle {

  public ListResourceBundle()
  {
  }

  public final Object handleGetObject(String key)
  {
    Object[][] objs = getContents();
    for (int i = 0; i < objs.length; i++) {
      if (key.equals((String)objs[i][0])) {
	return (objs[i][1]);
      }
    }
    return (null);
  }

  public Enumeration getKeys()
  {
    Vector keys = new Vector();
    Object[][] objs = getContents();
    for (int i = 0; i < objs.length; i++) {
      keys.addElement(objs[i][0]);
    }
    return (keys.elements());
  }

  protected abstract Object[][] getContents();

}
