/*
 * jitBasic.c
 *
 * Copyright (c) 2003, 2004 University of Utah and the Flux Group.
 * All rights reserved.
 *
 * This file is licensed under the terms of the GNU Public License.
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Contributed by the Flux Research Group, Department of Computer Science,
 * University of Utah, http://www.cs.utah.edu/flux/
 */

#include "config.h"
#include "object.h"
#include "support.h"
#include "utf8const.h"
#include "classMethod.h"
#include "stringParsing.h"

#if !defined(TRANSLATOR)

int internal_test(char *class_name)
{
	return( 0 );
}

#else

#include <machine.h>
#include <slots.h>
#include <seq.h>
#include <labels.h>
#include <codeproto.h>
#include <basecode.h>

Method *findMethodNoSig(Hjava_lang_Class *cl, Utf8Const *name)
{
	Method *retval = 0;
	int lpc;

	assert(cl != 0);
	assert(name != 0);

	for( lpc = 0; (lpc < CLASS_NMETHODS(cl)) && !retval; lpc++ )
	{
		if( CLASS_METHODS(cl)[lpc].name == name )
			retval = &CLASS_METHODS(cl)[lpc];
	}

	assert(retval != 0);
	
	return( retval );
}

#define MAX_TEST_FIELD_COMPONENTS 64

struct testField {
	char *tf_Name;
	int tf_ComponentCount;
	int tf_MethodIndex;
	parsedString tf_Components[MAX_TEST_FIELD_COMPONENTS];
};

static
int fieldComponentHandler(struct testField *tf)
{
	int retval = 1;

	tf->tf_ComponentCount += 1;
	tf->tf_Components[tf->tf_ComponentCount] = tf->tf_Components[0];
	if( tf->tf_Components[0].len == 0 )
		tf->tf_MethodIndex = tf->tf_ComponentCount + 1;
	return( retval );
}

int parseTestField(struct testField *tf)
{
	return parseString(
		tf->tf_Name,
		SPO_NotEmpty,
		  SPO_Do,
		    SPO_String, &tf->tf_Components[0],
		    SPO_While, "_", "",
		    SPO_Handle, fieldComponentHandler, tf,
		    SPO_End,
		  SPO_End,
		SPO_End);
}

int field2values(jvalue *dst, parsed_signature_t *ps, struct testField *tf)
{
	int lpc, retval = 1;

	for( lpc = 1; (lpc <= ps->nargs) && retval; lpc++ )
	{
		parsedString *arg;
		char *str;

		arg = &tf->tf_Components[lpc + 1];
		switch( ps->signature->data[ps->ret_and_args[lpc]] )
		{
		case 'Z':
			dst[lpc - 1].z = !cmpPStrStr(arg, "true");
			break;
		case 'B':
			dst[lpc - 1].b = (char)strtol(arg->data, 0, 0);
			break;
		case 'C':
			dst[lpc - 1].c = arg->data[0];
			break;
		case 'S':
			dst[lpc - 1].s = (short)strtol(arg->data, 0, 0);
			break;
		case 'I':
			dst[lpc - 1].i = strtoul(arg->data, 0, 0);
			break;
		case 'J':
#if defined(HAVE_STRTOULL)
			dst[lpc - 1].j = (long long)strtoull(arg->data, 0, 0);
#elif defined(HAVE_STRTOUQ)
			dst[lpc - 1].j = (long long)strtouq(arg->data, 0, 0);
#else
#error "jitBasic needs either strtoull or strtouq."
#endif
			break;
		case 'D':
		case 'F':
			if( (str = promoteParsedString(arg)) )
			{
				double value;
				char *sep;
				
				if( (sep = strchr(str, 'd')) )
				{
					*sep = '.';
				}
				value = strtod(str, 0);
				if( ps->signature->data[ps->ret_and_args[lpc]]
				    == 'D' )
				{
					dst[lpc - 1].d = value;
				}
				else
				{
					dst[lpc - 1].f = (float)value;
				}
				gc_free(str);
			}
			else
			{
				retval = 0;
			}
			break;
		default:
			assert(0);
			break;
		}
	}
	return( retval );
}

int testMethod(Hjava_lang_Class *cl, Field *field)
{
	struct testField tf;
	errorInfo einfo;
	int retval = 0;

	tf.tf_Name = (char *)field->name->data;
	tf.tf_ComponentCount = 0;
	if( parseTestField(&tf) )
	{
		Utf8Const *utf;
		Method *meth;
		
		utf = utf8ConstNew(tf.tf_Components[tf.tf_MethodIndex].data,
				   -1);
		meth = findMethodNoSig(cl, utf);
		if( translate(meth, &einfo) )
		{
			jvalue args[MAX_TEST_FIELD_COMPONENTS];
			jvalue rc, actual;
			void *methblock;

			field2values(args, METHOD_PSIG(meth), &tf);
			methblock = KGC_getObjectBase(main_collector,
						     METHOD_NATIVECODE(meth));
			memset(&rc, 0, sizeof(rc));
			memset(&actual, 0, sizeof(actual));
			KaffeVM_callMethodA(meth,
				    METHOD_NATIVECODE(meth),
				    0,
				    args,
				    &rc,
				    0);
			switch( field->bsize )
			{
			case sizeof(jbyte):
				actual.i = ((char *)field->info.addr)[0];
				break;
			case sizeof(jshort):
				actual.i = ((short *)field->info.addr)[0];
				break;
			case sizeof(jint):
				memcpy(&actual.i,
				       field->info.addr,
				       field->bsize);
				break;
			case sizeof(jdouble):
				memcpy(&actual.d,
				       field->info.addr,
				       field->bsize);
				break;
			default:
				assert(0);
				break;
			}
			if( !memcmp(&rc, &actual, sizeof(rc)) )
			{
				kaffe_dprintf("Success %08x\n", rc.i);
				retval = 1;
			}
			else
			{
				kaffe_dprintf("Failure for %s, got: %08x %f\n"
					      "  expected: %08x %f\n",
					      field->name->data,
					      rc.i, rc.d,
					      actual.i, actual.d);
			}
		}
		else
		{
			assert(0);
		}
	}
	return( retval );
}

int internal_test(parsedString *ps)
{
	char *class_name;
	int retval = 0;

	if( (class_name = promoteParsedString(ps)) )
	{
		Hjava_lang_Class *cl;
		errorInfo einfo;
		int lpc;
		
		retval = 1;
		loadStaticClass(&cl, class_name);
		processClass(cl, CSTATE_COMPLETE, &einfo);
		kaffe_dprintf("class: %s\n", cl->name->data);
		for( lpc = 0; (lpc < CLASS_NSFIELDS(cl)) && retval; lpc++ )
		{
			Field *field;
			uint32 expected_value;
			
			field = &CLASS_SFIELDS(cl)[lpc];

			switch (field->bsize) {
			case 1:
			  expected_value = ((uint8 *)field->info.addr)[0];
			  break;
			case 2:
			  expected_value = ((uint16 *)field->info.addr)[0];
			  break;
			case 4:
			  expected_value = ((uint32 *)field->info.addr)[0];
			  break;
			default:
			  expected_value = 0;
			}
                        kaffe_dprintf("  field: %s = 0x%08x\n",
                                      field->name->data,
				      expected_value);
			if( !strncmp("test_", field->name->data, 5) )
			{
				retval = testMethod(cl, field);
			}
		}
		gc_free(class_name);
	}
	return( retval );
}

#endif
