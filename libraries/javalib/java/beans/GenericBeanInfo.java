/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.beans;

import java.awt.Image;

class GenericBeanInfo extends SimpleBeanInfo {

private final BeanDescriptor bean;
private final PropertyDescriptor[] properties;
private final MethodDescriptor[] methods;
private final EventSetDescriptor[] events;
private final int defaultEvent;
private final int defaultProperty;
private final BeanInfo target;

public GenericBeanInfo(BeanDescriptor bean, PropertyDescriptor[] properties, MethodDescriptor[] methods, EventSetDescriptor[] events, int defaultEvent, int defaultProperty, BeanInfo target) {
	this.bean = bean;
	this.properties = properties;
	this.methods = methods;
	this.events = events;
	this.defaultEvent = defaultEvent;
	this.defaultProperty = defaultProperty;
	this.target = target;
}

public BeanDescriptor getBeanDescriptor() {
	return (bean);
}

public int getDefaultEventIndex() {
	return (defaultEvent);
}

public int getDefaultPropertyIndex() {
	return (defaultProperty);
}

public EventSetDescriptor[] getEventSetDescriptors() {
	return (events);
}

public Image getIcon(int i) {
	if (target != null) {
		return (target.getIcon(i));
	}
	else {
		return (null);
	}
}

public MethodDescriptor[] getMethodDescriptors() {
	return (methods);
}

public PropertyDescriptor[] getPropertyDescriptors() {
	return (properties);
}

}
