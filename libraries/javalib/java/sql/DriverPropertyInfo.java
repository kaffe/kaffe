/*
 * Java core library component.
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.sql;

public class DriverPropertyInfo {

public String name;
public String description;
public boolean required;
public String value;
public String choices[];

public DriverPropertyInfo(String name, String value) {
	this.name = name;
	this.value = value;
}

}
