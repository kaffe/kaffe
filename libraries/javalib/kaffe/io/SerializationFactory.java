package kaffe.io;

import java.io.InputStream;
import java.io.OutputStream;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.ObjectStreamClass;

abstract public class SerializationFactory {

abstract public ObjectStreamClass newObjectStreamClass(Class cls, int method);
abstract public ObjectInputStreamImpl newObjectInputStreamImpl(InputStream inp, ObjectInputStream oinp);
abstract public ObjectOutputStreamImpl newObjectOutputStreamImpl(OutputStream out, ObjectOutputStream oout);
abstract public boolean hasRdWrMethods(Class cls);

}
