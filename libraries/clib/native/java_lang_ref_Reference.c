#include <native.h>
#include "debug.h"
#include <jni.h>
#include "java_lang_ref_Reference.h"
#include "reference.h"
#include "soft.h"
#include "classMethod.h"
#include "baseClasses.h"
#include "gc.h"
#include "errors.h"
#include "utf8const.h"

void JNICALL Java_java_lang_ref_Reference_create(JNIEnv* env, jobject reference, jobject object)
{
  kgc_reference_type reftype;
  Hjava_lang_Object *ref_object = (Hjava_lang_Object *)reference;
  Hjava_lang_Class *ref_class = OBJECT_CLASS(ref_object);

  if (object == NULL)
    return;

  if (instanceof(javaLangRefWeakReference, ref_class))
    reftype = KGC_WEAK_REFERENCE;
  else if (instanceof(javaLangRefSoftReference, ref_class))
    reftype = KGC_SOFT_REFERENCE;
  else if (instanceof(javaLangRefPhantomReference, ref_class))
    reftype = KGC_PHANTOM_REFERENCE;
  else
    {
      (*env)->FatalError(env, "Unknown reference type");
      return;
    }

  KaffeVM_registerObjectReference(reference, object, reftype);
}
