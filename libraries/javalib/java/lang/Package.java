/*
 * Java core library component.
 *
 * Copyright (c) 2002
 *      Dalibor Topic. All rights reserved.
 * Copyright (c) 1997, 1998, 2002
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Author: Dalibor Topic <robilad@yahoo.com>
 * Checked Spec: JDK 1.2
 */

package java.lang;

import java.net.URL;
import java.util.NoSuchElementException;
import java.util.StringTokenizer;

import kaffe.lang.ThreadStack;

public class Package {

	private final String name;
	private final String specTitle;
	private final String specVersion;
	private final String specVendor;
	private final String implTitle;
	private final String implVersion;
	private final String implVendor;
	private final URL sealBase;

	private static final String VERSION_DELIMITER = ".";

	Package(String name, String specTitle,
		String specVersion, String specVendor,
		String implTitle, String implVersion,
		String implVendor, URL sealBase) throws IllegalArgumentException {

		if (name == null) {
		    throw new IllegalArgumentException ("Package name can not be null");
		}

		this.name = name;
		this.specTitle = specTitle;
		this.specVersion = specVersion;
		this.specVendor = specVendor;
		this.implTitle = implTitle;
		this.implVersion = implVersion;
		this.implVendor = implVendor;
		this.sealBase = sealBase;
	}

	public String getImplementationTitle() {
		return implTitle;
	}

	public String getImplementationVendor() {
		return implVendor;
	}

	public String getImplementationVersion() {
		return implVersion;
	}

	public String getName() {
		return name;
	}

	public static Package getPackage(String pname) {
		return ThreadStack.getCallersClassLoader(true).getPackage(pname);
	}

	public static Package [] getPackages() {
		return ThreadStack.getCallersClassLoader(true).getPackages();
	}

	public String getSpecificationTitle() {
		return specTitle;
	}

	public String getSpecificationVendor() {
		return specVendor;
	}

	public String getSpecificationVersion() {
		return specVersion;
	}

	public int hashCode() {
		return getName().hashCode();
	}

	public boolean isCompatibleWith(String version) throws NumberFormatException {
		StringTokenizer specTokenizer
			= new StringTokenizer(getSpecificationVersion(),
					      VERSION_DELIMITER);

		StringTokenizer versionTokenizer
			= new StringTokenizer(version, VERSION_DELIMITER);

		while (specTokenizer.hasMoreTokens()
		       || versionTokenizer.hasMoreTokens()) {
			int spec;
			int ver;

			try {
			String specToken = specTokenizer.nextToken();
			spec = Integer.parseInt(specToken);
			}
			catch (NoSuchElementException e) {
				spec = 0;
			}

			try {
				String verToken = versionTokenizer.nextToken();
				ver =  Integer.parseInt(verToken);
			}
			catch (NoSuchElementException e) {
				ver = 0;
			}

			if (spec < ver) {
				return false;
			}
		}

		return true;
	}

	public boolean isSealed() {
		return sealBase != null;
	}

	public boolean isSealed(URL url) {
		return url.equals(sealBase);
	}
}
