/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.beans;

import java.util.Hashtable;

public class PropertyEditorManager {

  private static String[] editorpath = { "kaffe.beans.editors" };
  private static Hashtable assoc = new Hashtable();

  public static void registerEditor(Class targetType, Class editorClass)
  {
    if (editorClass == null) {
      assoc.remove(targetType);
    }
    else {
      assoc.put(targetType, editorClass);
    }
  }

  public static PropertyEditor findEditor(Class targetType)
  {
    PropertyEditor editor = (PropertyEditor)assoc.get(targetType);
    if (editor == null) {
      editor = loadPropertyEditor(targetType);
    }
    return (editor);
  }

  public static String[] getEditorSearchPath()
  {
    return (editorpath);
  }

  public static void setEditorSearchPath(String path[])
  {
    editorpath = path;
  }

  private static PropertyEditor loadPropertyEditor(Class editorClass)
  {
    String editorname = editorClass.getName();

    // First try to load editor from package.
    PropertyEditor editor = loadNamedPropertyEditor(editorname + "Editor");
    if (editor != null) {
      return (editor);
    }

    // Extract the name without package information.
    // We make allowances for both '.' and '/' seperators.
    int pos = editorname.lastIndexOf('.');
    int spos = editorname.lastIndexOf('/');
    if (spos > pos) {
      pos = spos;
    }
    editorname = editorname.substring(pos+1);

    // Next try the search paths
    for (int i = 0; i < editorpath.length; i++) {
      editor = loadNamedPropertyEditor(editorpath[i] + "." + editorname + "Editor");
      if (editor != null) {
	return (editor);
      }
    }

    return (null);
  }

  private static PropertyEditor loadNamedPropertyEditor(String editorname)
  {
    try {
      return ((PropertyEditor)Class.forName(editorname).newInstance());
    }
    catch (ClassNotFoundException _) {
    }
    catch (ClassCastException _) {
    }
    catch (IllegalAccessException _) {
    }
    catch (InstantiationException _) {
    }
    return (null);
  }
}
