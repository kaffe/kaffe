
package java.lang;

import java.lang.reflect.Array;

public class System
{

public static void arraycopy(Object src, int src_position, Object dst, int dst_position, int length) {
    
    if (src == null)
	throw new NullPointerException("src == null");

    if (dst == null)
	throw new NullPointerException("dst == null");

    if (length == 0)
	return; 	 

    final Class source_class = src.getClass();

    if (!source_class.isArray())
	throw new ArrayStoreException("source is not an array: " + source_class.getName());

    final Class destination_class = dst.getClass();

    if (!destination_class.isArray())
	throw new ArrayStoreException("destination is not an array: " + destination_class.getName());

    if (src_position < 0)
	throw new ArrayIndexOutOfBoundsException("src_position < 0: " + src_position);

    final int src_length = Array.getLength(src);

    if (src_position + length > src_length)
	throw new ArrayIndexOutOfBoundsException("src_position + length > src.length: " + src_position + " + " + length + " > " + src_length);

    if (dst_position < 0)
	throw new ArrayIndexOutOfBoundsException("dst_position < 0: " + dst_position);

    final int dst_length = Array.getLength(dst);

    if (dst_position + length > dst_length)
	throw new ArrayIndexOutOfBoundsException("dst_position + length > dst.length: " + dst_position + " + " + length + " > " + dst_length);

    if (length < 0)
	throw new ArrayIndexOutOfBoundsException("length < 0: " + length);

    arraycopy0(src, src_position, dst, dst_position, length);
}

private static native void arraycopy0(Object src, int src_position, Object dst, int dst_position, int length);

}
