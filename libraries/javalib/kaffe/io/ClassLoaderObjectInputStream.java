package kaffe.io;

import java.io.IOException;
import java.io.InputStream;
import java.io.ObjectInputStream;
import java.io.ObjectStreamClass;
import java.io.StreamCorruptedException;

public class ClassLoaderObjectInputStream extends ObjectInputStream {

private final static boolean DOTRACE = true;

private ClassLoader loader;

public ClassLoaderObjectInputStream(InputStream in, ClassLoader cld) throws IOException, StreamCorruptedException {
	super(in);
	loader = cld;
}

protected Class resolveClass(ObjectStreamClass desc) throws IOException, ClassNotFoundException
{
	Class cls;
	if (DOTRACE) System.out.println("Loading class " + desc.getName() + " from " + loader);
	cls = loader.loadClass(desc.getName());
	if (DOTRACE) System.out.println("  done");
	return (cls);
}

}
