/**
 * SerializerConverter - export/import clipboard data as serialized Java objects
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author P. Mehlitz
 */

package kaffe.awt;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;

public class SerializerConverter
  implements FlavorConverter
{
public byte[] exportObject ( Object data ) {
	try {
		ByteArrayOutputStream os = new ByteArrayOutputStream();
		ObjectOutputStream    oos = new ObjectOutputStream( os);
	
		oos.writeObject( data);
		oos.close();

		return os.toByteArray();
	}
	catch ( Exception x ) {
		return null;
	}
}

public Object importBytes ( byte[] data ) {
	try {
		Object ret;
		ByteArrayInputStream is = new ByteArrayInputStream( data);
		ObjectInputStream     ois = new ObjectInputStream( is);
	
		ret = ois.readObject();
		ois.close();
	
		return ret;
	}
	catch ( Exception x ) {
		return null;
	}
}
}
