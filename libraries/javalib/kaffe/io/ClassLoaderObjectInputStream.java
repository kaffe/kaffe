package kaffe.io;

import java.io.ObjectInputStream;
import java.io.InputStream;
import java.io.IOException;
import java.io.StreamCorruptedException;
import java.io.ObjectStreamClass;

public class ClassLoaderObjectInputStream extends ObjectInputStream {

private ClassLoader loader;

public ClassLoaderObjectInputStream(InputStream in, ClassLoader cld) throws IOException, StreamCorruptedException {
	super(in);
	loader = cld;
}

protected Class resolveClass(ObjectStreamClass desc) throws IOException, ClassNotFoundException
{
	Class cls;
//System.out.println("Loading class " + desc.getName() + " from " + loader);
	cls = loader.loadClass(desc.getName());
//System.out.println("  done");
	return (cls);
}

}
