package kaffe.lang;

import java.io.InputStream;

/**
 * This interface maps a fully qualified class name to an array of bytes.
 */
public interface ResourceReader
{
	/**
	 * Read byte code.
	 *
	 * @param name	fully qualified class name (using dots as separators
	 *		and no trailing .class suffix)
	 * @return array of bytes in class file format
	 */
	byte[] getByteCode(String name) throws Exception;

	/**
	 * Read byte code.
	 *
	 * @param name	name of the resource, leading slashes are ignored
	 *	
	 * @return array of bytes of that resource
	 */
	InputStream getResourceAsStream(String name) throws Exception;
}
