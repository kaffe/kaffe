/*
 * java.lang.Object.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "lib-license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "../../../kaffe/kaffevm/classMethod.h"
#include "../../../kaffe/kaffevm/gtypes.h"
#include "../../../kaffe/kaffevm/locks.h"
#include "../../../kaffe/kaffevm/object.h"
#include "../../../kaffe/kaffevm/constants.h"
#include "../../../kaffe/kaffevm/access.h"
#include "../../../kaffe/kaffevm/soft.h"
#include <native.h>

/*
 * Generate object hash code.
 */
jint
java_lang_Object_hashCode(struct Hjava_lang_Object* o)
{
	/* Hash code is object's address */
	return ((jint)(jword)o);
}

/*
 * Return class object for this object.
 */
struct Hjava_lang_Class*
java_lang_Object_getClass(struct Hjava_lang_Object* o)
{
	return (OBJECT_CLASS(o));
}

/*
 * Notify threads waiting here.
 */
void
java_lang_Object_notifyAll(struct Hjava_lang_Object* o)
{
	broadcastCond(o);
}

/*
 * Notify a thread waiting here.
 */
void
java_lang_Object_notify(struct Hjava_lang_Object* o)
{
	signalCond(o);
}

/*
 * Clone me.
 */
struct Hjava_lang_Object*
java_lang_Object_clone(struct Hjava_lang_Object* o)
{
	Hjava_lang_Object* obj;
	Hjava_lang_Class* class;
	static Hjava_lang_Class* cloneClass;

	/* * Lookup cloneable class interface if we don't have it  */
	if (cloneClass == 0) {
		cloneClass = lookupClass("java/lang/Cloneable");
	}

	class = OBJECT_CLASS(o);

	if (!CLASS_IS_ARRAY(class)) {
		/* Check class is cloneable and throw exception if it isn't */
		if (soft_instanceof(cloneClass, o) == 0) {
			SignalError("java.lang.CloneNotSupportedException", class->name->data);
		}
		/* Clone an object */
		obj = newObject(class);
		memcpy(OBJECT_DATA(obj), OBJECT_DATA(o), CLASS_FSIZE(class) - sizeof(Hjava_lang_Object));
	}
	else {
		/* Clone an array */
		obj = newArray(CLASS_ELEMENT_TYPE(class), ARRAY_SIZE(o));
		memcpy(ARRAY_DATA(obj), ARRAY_DATA(o),
		       ARRAY_SIZE(o) * TYPE_SIZE(CLASS_ELEMENT_TYPE(class)));
	}
	return (obj);
}

/*
 * Wait for this object to be notified.
 */
void
java_lang_Object_wait0(struct Hjava_lang_Object* o, jlong timeout)
{
	waitCond(o, timeout);
}
