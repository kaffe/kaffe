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

/* Expected Output:
BeanDescriptor:
  Class: class Bean
  Method: public void Bean.setArray(java.lang.String[])
  Method: public final void java.lang.Object.wait(long,int) throws java.lang.InterruptedException
  Method: public final void java.lang.Object.wait() throws java.lang.InterruptedException
  Method: public void Bean.setArray(int,java.lang.String)
  Method: public boolean java.lang.Object.equals(java.lang.Object)
  Method: public final native void java.lang.Object.notifyAll()
  Method: public void Bean.setValue(java.lang.String)
  Method: public java.lang.String[] Bean.getArray()
  Method: public final native java.lang.Class java.lang.Object.getClass()
  Method: public java.lang.String Bean.getValue()
  Method: public java.lang.String Bean.getArray(int)
  Method: public final void java.lang.Object.wait(long) throws java.lang.InterruptedException
  Method: public native int java.lang.Object.hashCode()
  Method: public static void Bean.main(java.lang.String[])
  Method: public java.lang.String java.lang.Object.toString()
  Method: public final native void java.lang.Object.notify()
  Property: type class java.lang.Class, read public final native java.lang.Class java.lang.Object.getClass(), write <none>
  Editor: <unknown>
  Property: type class [Ljava.lang.String;, read public java.lang.String[] Bean.getArray(), write public void Bean.setArray(java.lang.String[]), idx read public java.lang.String Bean.getArray(int), idx write public void Bean.setArray(int,java.lang.String)
  Editor: <unknown>
  Property: type class java.lang.String, read public java.lang.String Bean.getValue(), write public void Bean.setValue(java.lang.String)
  Editor: class kaffe.beans.editors.StringEditor
*/
