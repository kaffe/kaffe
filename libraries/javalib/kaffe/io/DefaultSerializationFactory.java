package kaffe.io;

import java.lang.Class;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.ObjectStreamClass;
import kaffe.io.ObjectStreamClassImpl;
import kaffe.io.ObjectOutputStreamImpl;
import kaffe.io.ObjectInputStreamImpl;

public class DefaultSerializationFactory extends SerializationFactory {

public boolean hasRdWrMethods(Class cl) {
	return (ObjectStreamClassImpl.hasWriteObject(cl));
}

public ObjectStreamClass newObjectStreamClass(Class cls, int method) {
	return (new ObjectStreamClassImpl(cls, method));
}

public ObjectInputStreamImpl newObjectInputStreamImpl(InputStream inp, ObjectInputStream oinp) {
	return (new ObjectInputStreamImpl(inp, oinp));
}

public ObjectOutputStreamImpl newObjectOutputStreamImpl(OutputStream out, ObjectOutputStream oout) {
	return (new ObjectOutputStreamImpl(out, oout));
}

}
