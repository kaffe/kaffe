/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.util;

import java.lang.String;
import java.io.IOException;
import java.io.InputStream;

public class PropertyResourceBundle
  extends ResourceBundle
{
	private Properties props;

public PropertyResourceBundle(InputStream stream) throws IOException {
	props = new Properties();
	props.load(stream);
}

public Enumeration getKeys() {
	return (props.propertyNames());
}

public Object handleGetObject(String key) {
	return (props.getProperty(key));
}

}
