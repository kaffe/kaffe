import java.beans.*;
import java.lang.*;
import java.lang.reflect.*;

public class Bean {
  public static void main(String[] args)
  {
    try {
      Class cls = Class.forName("Bean");

      BeanInfo info = Introspector.getBeanInfo(cls);
      System.out.println("BeanDescriptor:");

      System.out.println("  Class: " + info.getBeanDescriptor().getBeanClass());

      MethodDescriptor[] meths = info.getMethodDescriptors();
      for (int i = 0; i < meths.length; i++) {
        System.out.println("  Method: " + meths[i].getMethod());
      }

      PropertyDescriptor[] props = info.getPropertyDescriptors();
      for (int i = 0; i < props.length; i++) {
	PropertyDescriptor prop = props[i];
	System.out.print("  Property: type " + prop.getPropertyType());
        Method rm = prop.getReadMethod();
        Method wm = prop.getWriteMethod();
        System.out.print(", read " + (rm == null ? "<none>" : rm.toString()));
        System.out.print(", write " + (wm == null ? "<none>" : wm.toString()));
	if (prop instanceof IndexedPropertyDescriptor) {
	  IndexedPropertyDescriptor iprop = (IndexedPropertyDescriptor)prop;
          Method rim = iprop.getIndexedReadMethod();
          Method wim = iprop.getIndexedWriteMethod();
          System.out.print(", idx read " + (rim == null ? "<none>" : rim.toString()));
          System.out.print(", idx write " + (wim == null ? "<none>" : wim.toString()));
	}
        System.out.println();

	PropertyEditor editor = PropertyEditorManager.findEditor(prop.getPropertyType());
	System.out.println("  Editor: " + (editor == null ? "<unknown>" : editor.getClass().toString()));
      }

    }
    catch (Exception e) {
      if (e.getMessage() != null) {
        System.out.println(e.getMessage());
      }
      e.printStackTrace();
    }
  }

  // These are here so we can check various properties out.
  public String getValue() { return (null); }
  public void setValue(String val) { }
  public String getArray(int idx) { return (null); }
  public void setArray(int idx, String val) { }
  public String[] getArray() { return (new String[0]); }
  public void setArray(String[] val) { }
}
