/*
 * Java core library component.
 *
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-net.h"
#include "../../../kaffe/kaffevm/classMethod.h"
#include "../../../kaffe/kaffevm/access.h"
#include "../../../kaffe/kaffevm/exception.h"
#include "java_lang_reflect_Method.h"
#include <native.h>
/* #include "kaffe_rmi_server_RMIHashes.h" */
#include "sha-1.h"

extern char* pathname2ClassnameCopy(const char*);

jlong
kaffe_rmi_server_RMIHashes_getMethodHash(struct Hjava_lang_reflect_Method* meth)
{
	SHA1_CTX c;
	unsigned char md[SHA_DIGEST_LENGTH];
	jshort len;
	Method* m;

	m = &unhand(meth)->clazz->methods[unhand(meth)->slot];

	SHA1Init(&c);

	len = strlen(m->name->data);

	/* NOTE: Despite what it may say in the documentation about the
	 * hash values for method signatures, we do not convert the '/'s
	 * into '.' as we do for serialization.  Why be consistent eh?
	 * - TIM 9/24/99
	 */
	len += strlen(METHOD_SIGD(m));

	len = htons(len);
	SHA1Update(&c, (char*)&len, sizeof(len));
	SHA1Update(&c, m->name->data, strlen(m->name->data));
	SHA1Update(&c, METHOD_SIGD(m), strlen(METHOD_SIGD(m)));

	SHA1Final(md, &c);

	return ( (jlong)md[0]        |
		((jlong)md[1] <<  8) |
		((jlong)md[2] << 16) |
		((jlong)md[3] << 24) |
		((jlong)md[4] << 32) |
		((jlong)md[5] << 40) |
		((jlong)md[6] << 48) |
		((jlong)md[7] << 56) );
}

static
void
addUTFtoSHA(SHA1_CTX* c, const char* str)
{
	uint16 len = htons(strlen(str));
	SHA1Update(c, (char*)&len, sizeof(len));
	SHA1Update(c, str, strlen(str));
}

static
int
compareMethods(const void* one, const void* two)
{
        int r;
        Method* o = *(Method**)one;
        Method* t = *(Method**)two;

        /* push null entries to the right end of the array */
        if (o == 0) {
                return (1);
        }
        if (t == 0) {
                return (-1);
        }

        r = strcmp(o->name->data, t->name->data);
        return (r);
}

static
int
compareClasses(const void* one, const void* two)
{
        int r;
        Hjava_lang_Class* o = *(Hjava_lang_Class**)one;
        Hjava_lang_Class* t = *(Hjava_lang_Class**)two;

        /* push null entries to the right end of the array */
        if (o == 0) {
                return (1);
        }
        if (t == 0) {
                return (-1);
        }

        r = strcmp(o->name->data, t->name->data);
        return (r);
}

jlong
kaffe_rmi_server_RMIHashes_getInterfaceHash(struct Hjava_lang_Class* clazz)
{
	SHA1_CTX c;
	unsigned char md[SHA_DIGEST_LENGTH];
	Method* m;
	int nm;
	Method** base;
	Hjava_lang_Class** ebase;
	char* name;
	int one;
	int i;
	int j;
	int en;
	errorInfo einfo;

	SHA1Init(&c);

	nm = CLASS_NMETHODS(clazz);
	m = CLASS_METHODS(clazz);

	base = KMALLOC(sizeof(Method*) * nm);
	for (i = 0; i < nm; i++, m++) {
		base[i] = 0;
		if ((m->accflags & (ACC_CONSTRUCTOR|ACC_PUBLIC|ACC_STATIC)) == ACC_PUBLIC) {
			base[i] = m;
		}
	}
	qsort(base, nm, sizeof(Method*), compareMethods);

	one = htonl(1);
	SHA1Update(&c, (char*)&one, sizeof(one));

	for (i = 0; i < nm; i++) {
		Method *emeth;
		
		m = base[i];
		if (m == 0) {
			continue;
		}
		addUTFtoSHA(&c, m->name->data);
		addUTFtoSHA(&c, METHOD_SIGD(m));

		if (m->ndeclared_exceptions == 0) {
			continue;
		}
		if (m->ndeclared_exceptions == -1) {
			emeth = m->declared_exceptions_u.remote_exceptions;
		} else {
			emeth = m;
		}
		en = emeth->ndeclared_exceptions;
		ebase = KMALLOC(sizeof(Hjava_lang_Class*) * en);
		for (j = 0; j < en; j++) {
			ebase[j] = getClass(emeth->declared_exceptions[j], clazz, &einfo);
		}
		qsort(ebase, en, sizeof(Hjava_lang_Class*), compareClasses);

		for (j = 0; j < en; j++) {
			name = pathname2ClassnameCopy(ebase[j]->name->data);
			addUTFtoSHA(&c, name);
			KFREE(name);
		}

		KFREE(ebase);
	}

	SHA1Final(md, &c);

	KFREE(base);

	return ( (jlong)md[0]        |
		((jlong)md[1] <<  8) |
		((jlong)md[2] << 16) |
		((jlong)md[3] << 24) |
		((jlong)md[4] << 32) |
		((jlong)md[5] << 40) |
		((jlong)md[6] << 48) |
		((jlong)md[7] << 56) );
}
