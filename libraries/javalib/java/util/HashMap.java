package java.util;

public class HashMap extends Hashtable {

    public HashMap() {
	super();
    }
  
    public HashMap(int initialCapacity) {
	super(initialCapacity);
    }
  
    public HashMap(int initialCapacity, float loadFactor) {
	super(initialCapacity, loadFactor);
    }

    public HashMap(Map map) {
	super();
	putAll(map);
    }

}
